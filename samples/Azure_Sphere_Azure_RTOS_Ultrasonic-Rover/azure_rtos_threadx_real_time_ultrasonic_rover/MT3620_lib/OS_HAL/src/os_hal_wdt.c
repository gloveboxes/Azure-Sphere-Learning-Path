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

#include <mhal_wdt.h>
#include <os_hal_wdt.h>
#include <timer.h>
#include <nvic.h>
#include <irq.h>

#define OS_WDT_LOG(string, args...)	printf("[os_wdt]"string, ##args)

struct os_wdt_dev {
	struct hal_wdt_dev	hal_dev;
	struct os_wdt_int	int_info;
	IRQn_Type		irq_id;
	enum os_wdt_rst_sta	reset_status;
	bool			inited;
};

static struct os_wdt_dev wdt_dev;

static bool _mtk_os_hal_wdt_is_inited(struct os_wdt_dev *dev)
{
	if (!dev->inited) {
		OS_WDT_LOG("wdt device initialization failed\n");
		return false;
	}
	return true;
}

/* internal functions for interrupt */

static void _mtk_os_hal_wdt_default_irq_hdl(void *unused)
{
	unsigned int rst_sta = 0;

	mtk_mhal_wdt_get_status(&(wdt_dev.hal_dev), &rst_sta);
	OS_WDT_LOG("default_irq_hdl: sta %x\n", rst_sta);

	mtk_mhal_wdt_enable(&(wdt_dev.hal_dev), 0);
	/* some information can be dumped in here... */
	mtk_mhal_wdt_hwrst(&(wdt_dev.hal_dev));
}

static void _mtk_os_hal_wdt_isr(void)
{
	if (wdt_dev.int_info.wdt_cb_hdl)
		wdt_dev.int_info.wdt_cb_hdl(wdt_dev.int_info.wdt_cb_data);
}

/* external functions */

void mtk_os_hal_wdt_enable(void)
{
	if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
		return;

	mtk_mhal_wdt_restart(&(wdt_dev.hal_dev));
	mtk_mhal_wdt_enable(&(wdt_dev.hal_dev), 1);
}

void mtk_os_hal_wdt_disable(void)
{
	if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
		return;

	mtk_mhal_wdt_enable(&(wdt_dev.hal_dev), 0);
}

int mtk_os_hal_wdt_set_timeout(unsigned int sec)
{
	if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
		return -WDT_EPTR;

	return mtk_mhal_wdt_set_timeout(&(wdt_dev.hal_dev), sec);
}

void mtk_os_hal_wdt_restart(void)
{
	if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
		return;

	mtk_mhal_wdt_restart(&(wdt_dev.hal_dev));
}

void mtk_os_hal_wdt_sw_reset(void)
{
	if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
		return;

	mtk_mhal_wdt_config(&(wdt_dev.hal_dev), 0);
	mtk_mhal_wdt_swrst(&(wdt_dev.hal_dev));
}

void mtk_os_hal_wdt_hw_reset(void)
{
	if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
		return;

	mtk_mhal_wdt_config(&(wdt_dev.hal_dev), 0);
	mtk_mhal_wdt_hwrst(&(wdt_dev.hal_dev));
}

enum os_wdt_rst_sta mtk_os_hal_wdt_get_reset_status(void)
{
	return wdt_dev.reset_status;
}

void mtk_os_hal_wdt_config(enum os_wdt_mode mode)
{
	if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
		return;

	mtk_mhal_wdt_config(&(wdt_dev.hal_dev), mode);
}

void mtk_os_hal_wdt_register_irq(struct os_wdt_int *wdt_int)
{
	if (!wdt_int || !(wdt_int->wdt_cb_hdl)) {
		wdt_dev.int_info.wdt_cb_hdl = _mtk_os_hal_wdt_default_irq_hdl;
		wdt_dev.int_info.wdt_cb_data = NULL;
	} else {
		wdt_dev.int_info.wdt_cb_hdl = wdt_int->wdt_cb_hdl;
		wdt_dev.int_info.wdt_cb_data = wdt_int->wdt_cb_data;
	}
}

/* device initialization */

void mtk_os_hal_wdt_init(void)
{
	unsigned int rst_sta;

	if (wdt_dev.inited)
		return;

	/* initialize WDT devices */
	wdt_dev.hal_dev.cm4_wdt_base = (void __iomem *)(0x21020000);

	wdt_dev.irq_id = (IRQn_Type)CM4_IRQ_WDT_M4_IO;

	/* get WDT reset status */
	mtk_mhal_wdt_get_status(&(wdt_dev.hal_dev), &rst_sta);
	wdt_dev.reset_status = (enum os_wdt_rst_sta)rst_sta;

	/* disable WDT */
	mtk_mhal_wdt_enable(&(wdt_dev.hal_dev), 0);

	/* register default interrupt handle */
	mtk_os_hal_wdt_register_irq(NULL);
	/* request irq from system */
	NVIC_Register(wdt_dev.irq_id, _mtk_os_hal_wdt_isr);
	NVIC_SetPriority(wdt_dev.irq_id, CM4_WDT_PRI);
	NVIC_EnableIRQ(wdt_dev.irq_id);

	wdt_dev.inited = true;
}


