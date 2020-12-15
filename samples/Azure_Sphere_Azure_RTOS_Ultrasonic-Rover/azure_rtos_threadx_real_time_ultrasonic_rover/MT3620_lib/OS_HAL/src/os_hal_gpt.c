/*
 * (C) 2005-2020 MediaTek Inc. All rights reserved.
 *
 * Copyright Statement:
 *
 * This MT3620 driver software/firmware and related documentation
 * ("MediaTek Software") are protected under relevant copyright laws.
 * The information contained herein is confidential and proprietary to
 * MediaTek Inc. ("MediaTek"). You may only use, reproduce, modify, or
 * distribute (as applicable) MediaTek Software if you have agreed to and been
 * bound by this Statement and the applicable license agreement with MediaTek
 * ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
 * PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS
 * ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO
 * LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED
 * HEREUNDER WILL BE ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
 * RECEIVER TO MEDIATEK DURING THE PRECEDING TWELVE (12) MONTHS FOR SUCH
 * MEDIATEK SOFTWARE AT ISSUE.
 */

#include "os_hal_gpt.h"
#include "nvic.h"
#include "irq.h"

#define OS_GPT_AVAILABLE_MASK		(0x0000001f)

#define OS_GPT_LOG(string, args...)	printf("[os_gpt]"string, ##args)

struct os_gpt_dev {
	struct hal_gpt_dev	hal_dev;
	struct os_gpt_int	gpt_int[3];
	IRQn_Type		irq_id[2];
	unsigned int		holden_bitmap;
	bool			inited;
};

static struct os_gpt_dev gpt_dev;

static inline bool _mtk_os_hal_gpt_is_inited(struct os_gpt_dev *dev)
{
	if (!dev->inited) {
		OS_GPT_LOG("gpt device initialization failed\n");
		return false;
	}
	return true;
}

static bool _mtk_os_hal_gpt_check_timer_available(struct os_gpt_dev *dev,
						  enum gpt_num timer_id)
{
	if (!_mtk_os_hal_gpt_is_inited(dev))
		return false;

	if (dev->holden_bitmap & BIT(timer_id)) {
		OS_GPT_LOG("timer %u is unavailable\n", timer_id);
		return false;
	}

	return true;
}

/* internal functions for interrupt */

static void _mtk_os_hal_gpt_enable_irq(struct os_gpt_dev *dev,
				       enum gpt_num timer_id)
{
	if (timer_id <= GPT1 || timer_id == GPT3) {
		mtk_mhal_gpt_clear_irq_status(&(dev->hal_dev), timer_id);
		if (timer_id <= GPT1)
			mtk_mhal_gpt_enable_irq(&(dev->hal_dev), timer_id);
		else
			NVIC_EnableIRQ(dev->irq_id[1]);
	}
}

static void _mtk_os_gpt_disable_irq(struct os_gpt_dev *dev,
				    enum gpt_num timer_id)
{
	if (timer_id <= GPT1 || timer_id == GPT3) {
		if (timer_id <= GPT1)
			mtk_mhal_gpt_disable_irq(&(dev->hal_dev), timer_id);
		else
			NVIC_DisableIRQ(dev->irq_id[1]);
		mtk_mhal_gpt_clear_irq_status(&(dev->hal_dev), timer_id);
	}
}

static void _mtk_os_hal_gpt_default_cb(void *data)
{
	OS_GPT_LOG("default cb: timer %u\n", (unsigned int)data);
}

static void _mtk_os_hal_gpt_register_int(struct os_gpt_dev *dev,
					 enum gpt_num timer_id,
					 struct os_gpt_int *int_info)
{
	unsigned int tmrint_id;

	if (timer_id >= 2 && timer_id != 3)
		return;

	tmrint_id = (timer_id == 3) ? 2 : timer_id;

	if (!int_info || !(int_info->gpt_cb_hdl)) {
		dev->gpt_int[tmrint_id].gpt_cb_hdl = _mtk_os_hal_gpt_default_cb;
		dev->gpt_int[tmrint_id].gpt_cb_data = (void *)timer_id;
	} else {
		dev->gpt_int[tmrint_id].gpt_cb_hdl = int_info->gpt_cb_hdl;
		dev->gpt_int[tmrint_id].gpt_cb_data = int_info->gpt_cb_data;
	}
}

/* gpt0 & gpt1 */
static void _mtk_os_hal_gpt_gpt_isr(void)
{
	enum gpt_num timer_id;
	unsigned int int_sta;

	for (timer_id = GPT0; timer_id <= GPT1; timer_id++) {
		mtk_mhal_gpt_get_irq_status(&(gpt_dev.hal_dev), timer_id,
					    &int_sta);
		if (!int_sta)
			continue;

		mtk_mhal_gpt_clear_irq_status(&(gpt_dev.hal_dev), timer_id);
		if (gpt_dev.gpt_int[timer_id].gpt_cb_hdl)
			gpt_dev.gpt_int[timer_id].gpt_cb_hdl(
					gpt_dev.gpt_int[timer_id].gpt_cb_data);
		break;
	}
}

/* gpt3 */
static void _mtk_os_hal_gpt_gpt3_isr(void)
{
	mtk_mhal_gpt_clear_irq_status(&(gpt_dev.hal_dev), GPT3);

	if (gpt_dev.gpt_int[2].gpt_cb_hdl)
		gpt_dev.gpt_int[2].gpt_cb_hdl(gpt_dev.gpt_int[2].gpt_cb_data);
}

/* external functions */

int mtk_os_hal_gpt_start(enum gpt_num timer_id)
{
	if (!_mtk_os_hal_gpt_check_timer_available(&gpt_dev, timer_id))
		return -GPT_EACCES;

	_mtk_os_hal_gpt_enable_irq(&gpt_dev, timer_id);

	mtk_mhal_gpt_start(&(gpt_dev.hal_dev), timer_id);

	gpt_dev.holden_bitmap |= BIT(timer_id);
	return 0;
}

int mtk_os_hal_gpt_stop(enum gpt_num timer_id)
{
	int ret = 0;

	if (!_mtk_os_hal_gpt_is_inited(&gpt_dev) ||
	    !(gpt_dev.holden_bitmap & BIT(timer_id)))
		return -GPT_EACCES;

	ret = mtk_mhal_gpt_stop(&(gpt_dev.hal_dev), timer_id);
	if (ret)
		return ret;

	_mtk_os_gpt_disable_irq(&gpt_dev, timer_id);

	gpt_dev.holden_bitmap &= ~BIT(timer_id);
	return ret;
}

unsigned int mtk_os_hal_gpt_get_cur_count(enum gpt_num timer_id)
{
	unsigned int cnt_val = 0;

	if (_mtk_os_hal_gpt_is_inited(&gpt_dev))
		mtk_mhal_gpt_get_count(&(gpt_dev.hal_dev), timer_id, &cnt_val);

	return cnt_val;
}

int mtk_os_hal_gpt_restart(enum gpt_num timer_id)
{
	if (!_mtk_os_hal_gpt_is_inited(&gpt_dev))
		return -GPT_EACCES;

	return mtk_mhal_gpt_restart_count(&(gpt_dev.hal_dev), timer_id);
}

int mtk_os_hal_gpt_reset_timer(enum gpt_num timer_id, unsigned int count_val,
			       bool auto_repeat)
{
	int ret = 0;

	if (!_mtk_os_hal_gpt_check_timer_available(&gpt_dev, timer_id))
		return -GPT_EACCES;

	ret = mtk_mhal_gpt_set_compare(&(gpt_dev.hal_dev), timer_id, count_val);
	if (ret)
		return ret;

	return mtk_mhal_gpt_config_mode(&(gpt_dev.hal_dev), timer_id,
					(auto_repeat) ?
					GPT_REPEAT : GPT_ONE_SHOT);
}

int mtk_os_hal_gpt_config(enum gpt_num timer_id, unsigned char speed_32us,
			  struct os_gpt_int *gpt_int)
{
	if (!_mtk_os_hal_gpt_check_timer_available(&gpt_dev, timer_id))
		return -1;

	_mtk_os_hal_gpt_register_int(&gpt_dev, timer_id, gpt_int);

	return mtk_mhal_gpt_config_clk(&(gpt_dev.hal_dev), timer_id,
				       (speed_32us) ?
				       GPT_SPEED_32K : GPT_SPEED_1K);
}

/* device initialization */

void mtk_os_hal_gpt_register_irq(void)
{
	/* the common int pin for gpt0 & gpt1 is always on */
	CM4_Install_NVIC(gpt_dev.irq_id[0], CM4_GPT_PRI, IRQ_LEVEL_TRIGGER,
			 _mtk_os_hal_gpt_gpt_isr, true);

	/* config GPT3 irq */
	CM4_Install_NVIC(gpt_dev.irq_id[1], CM4_GPT_PRI, IRQ_LEVEL_TRIGGER,
			 _mtk_os_hal_gpt_gpt3_isr, false);
}

void mtk_os_hal_gpt_init(void)
{
	if (gpt_dev.inited)
		return;

	/* initialize GPT devices */
	gpt_dev.hal_dev.cm4_gpt_base = (void __iomem *)(0x21030000);
	gpt_dev.irq_id[0] = (IRQn_Type)CM4_IRQ_GPT;
	gpt_dev.irq_id[1] = (IRQn_Type)CM4_IRQ_GPT3;
	gpt_dev.holden_bitmap = ~OS_GPT_AVAILABLE_MASK;

	/* request irq from system */
	mtk_os_hal_gpt_register_irq();

	gpt_dev.inited = true;
}
