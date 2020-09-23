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

#include "nvic.h"
#include "os_hal_gpioif.h"
#include "os_hal_gpio.h"

#define CM4_GPIOIF_GRP0_BASE		0x38010000
#define CM4_GPIOIF_GRP1_BASE		0x38020000
#define CM4_GPIOIF_GRP2_BASE		0x38030000
#define CM4_GPIOIF_GRP3_BASE		0x38040000
#define CM4_GPIOIF_GRP4_BASE		0x38050000
#define CM4_GPIOIF_GRP5_BASE		0x38060000

#define CM4_GPIOIF_CG_BASE		0x30030000

#define MTK_GPIOIF_MAX_VAL	0xffffffff

static unsigned long gpioif_base_addr[MTK_GPIOIF_MAX_GRP_NUM] = {
	CM4_GPIOIF_GRP0_BASE,
	CM4_GPIOIF_GRP1_BASE,
	CM4_GPIOIF_GRP2_BASE,
	CM4_GPIOIF_GRP3_BASE,
	CM4_GPIOIF_GRP4_BASE,
	CM4_GPIOIF_GRP5_BASE,
};

/**
 * this os special gpioif structure, need mapping it to mtk_gpioif_controller
 */
struct mtk_gpioif_controller_rtos {
	struct mtk_gpioif_controller *ctlr;
	/** user_data is a OS-HAL defined parameter provided
	* by #mtk_os_hal_gpioif_int_callback_register().
	*/
	void *user_data;
	/** This function is used to register user's interrupt callback
	* to test layer
	*/
	gpioif_int_callback int_callback;
};

static struct mtk_gpioif_controller_rtos
	g_gpioif_ctlr_rtos[MTK_GPIOIF_MAX_GRP_NUM];

static struct mtk_gpioif_config g_gpioif_mconfig[MTK_GPIOIF_MAX_GRP_NUM];
static struct mtk_gpioif_int_count g_gpioif_int_cnt[MTK_GPIOIF_MAX_GRP_NUM];
static struct mtk_gpioif_controller g_gpioif_ctlr[MTK_GPIOIF_MAX_GRP_NUM];

static struct mtk_gpioif_controller_rtos *_mtk_os_hal_gpioif_get_ctlr(
	gpioif_group group)
{
	if (group >= MTK_GPIOIF_MAX_GRP_NUM) {
		printf("gpioif get group[%d] fail\n", (u32)group);
		return NULL;
	}

	return &g_gpioif_ctlr_rtos[group];
}

/** This function is used to register user's interrupt callback
  *   to OS-HAL layer
  */
int mtk_os_hal_gpioif_int_callback_register(gpioif_group group,
	gpioif_int_callback callback, void *user_data)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);

	if (!ctlr_rtos || !callback || !user_data)
		return -GPIOIF_EINVAL;

	ctlr_rtos->user_data = user_data;
	ctlr_rtos->int_callback = callback;

	return 0;
}

static int _mtk_os_hal_gpioif_irq_handler(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	mtk_mhal_gpioif_isr_handle(ctlr);
	ctlr_rtos->int_callback(ctlr_rtos->user_data);

	return 0;
}

static void _mtk_os_hal_gpioif_grp0_irq_event(void)
{
	_mtk_os_hal_gpioif_irq_handler(GPIOIF_GROUP_0);
}

static void _mtk_os_hal_gpioif_grp1_irq_event(void)
{
	_mtk_os_hal_gpioif_irq_handler(GPIOIF_GROUP_1);
}

static void _mtk_os_hal_gpioif_grp2_irq_event(void)
{
	_mtk_os_hal_gpioif_irq_handler(GPIOIF_GROUP_2);
}

static void _mtk_os_hal_gpioif_grp3_irq_event(void)
{
	_mtk_os_hal_gpioif_irq_handler(GPIOIF_GROUP_3);
}

static void _mtk_os_hal_gpioif_grp4_irq_event(void)
{
	_mtk_os_hal_gpioif_irq_handler(GPIOIF_GROUP_4);
}

static void _mtk_os_hal_gpioif_grp5_irq_event(void)
{
	_mtk_os_hal_gpioif_irq_handler(GPIOIF_GROUP_5);
}

static int _mtk_os_hal_gpioif_request_irq(gpioif_group group)
{
	if (group >= MTK_GPIOIF_MAX_GRP_NUM)
		return -GPIOIF_EINVAL;

	switch (group) {
	case GPIOIF_GROUP_0:
		CM4_Install_NVIC(CM4_IRQ_GPIO_G0_CNT, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER,
				 _mtk_os_hal_gpioif_grp0_irq_event,
				 TRUE);
		break;
	case GPIOIF_GROUP_1:
		CM4_Install_NVIC(CM4_IRQ_GPIO_G1_CNT, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER,
				 _mtk_os_hal_gpioif_grp1_irq_event,
				 TRUE);
		break;
	case GPIOIF_GROUP_2:
		CM4_Install_NVIC(CM4_IRQ_GPIO_G2_CNT, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER,
				 _mtk_os_hal_gpioif_grp2_irq_event,
				 TRUE);
		break;
	case GPIOIF_GROUP_3:
		CM4_Install_NVIC(CM4_IRQ_GPIO_G3_CNT, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER,
				 _mtk_os_hal_gpioif_grp3_irq_event,
				 TRUE);
		break;
	case GPIOIF_GROUP_4:
		CM4_Install_NVIC(CM4_IRQ_GPIO_G4_CNT, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER,
				 _mtk_os_hal_gpioif_grp4_irq_event,
				 TRUE);
		break;
	case GPIOIF_GROUP_5:
		CM4_Install_NVIC(CM4_IRQ_GPIO_G5_CNT, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER,
				 _mtk_os_hal_gpioif_grp5_irq_event,
				 TRUE);
		break;
	}

	return 0;
}

static int _mtk_os_hal_gpioif_free_irq(gpioif_group group)
{
	if (group >= MTK_GPIOIF_MAX_GRP_NUM)
		return -GPIOIF_EINVAL;

	switch (group) {
	case GPIOIF_GROUP_0:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G0_CNT);
		break;
	case GPIOIF_GROUP_1:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G1_CNT);
		break;
	case GPIOIF_GROUP_2:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G2_CNT);
		break;
	case GPIOIF_GROUP_3:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G3_CNT);
		break;
	case GPIOIF_GROUP_4:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G4_CNT);
		break;
	case GPIOIF_GROUP_5:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G5_CNT);
		break;
	}

	return 0;
}

int mtk_os_hal_gpioif_read_gpio_event_count(gpioif_group group, u32 *pvalue)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	mtk_mhal_gpioif_read_gpio_event_count(ctlr, pvalue);

	return 0;
}

int mtk_os_hal_gpioif_read_reset_val(gpioif_group group, u32 *pvalue)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	mtk_mhal_gpioif_read_reset_val(ctlr, pvalue);

	return 0;
}

int mtk_os_hal_gpioif_read_low_limit_val(gpioif_group group, u32 *pvalue)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	mtk_mhal_gpioif_read_low_limit_val(ctlr, pvalue);

	return 0;
}

int mtk_os_hal_gpioif_read_high_limit_val(gpioif_group group, u32 *pvalue)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	mtk_mhal_gpioif_read_high_limit_val(ctlr, pvalue);

	return 0;
}

int mtk_os_hal_gpioif_capture_fifo_init(gpioif_group group)
{
	u32 i = 0;
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	for (i = 0; i < MAX_FIFO_ENTRY_CNT ; i++) {
		ctlr->int_cnt->cap_fifo0[i] = 0;
		ctlr->int_cnt->cap_fifo1[i] = 0;
	}
	ctlr->int_cnt->cap_fifo0_count = 0;
	ctlr->int_cnt->cap_fifo1_count = 0;

	return 0;
}

int mtk_os_hal_gpioif_interrupt_count_init(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->int_cnt->int_bit = IRQ_GPIOIF_ALL_ENABLE;
	ctlr->int_cnt->int_event_low_count = 0;
	ctlr->int_cnt->int_event_high_count = 0;
	ctlr->int_cnt->int_gpio2_rst_done_count = 0;
	ctlr->int_cnt->int_event_over_count = 0;
	ctlr->int_cnt->int_event_uf_count = 0;
	ctlr->int_cnt->int_cap_f0_full_count = 0;
	ctlr->int_cnt->int_cap_f1_full_count = 0;
	ctlr->int_cnt->int_reset_cap_f0_full_count = 0;
	ctlr->int_cnt->int_reset_cap_f1_full_count = 0;
	ctlr->int_cnt->int_cap_f0_np_count = 0;
	ctlr->int_cnt->int_cap_f1_np_count = 0;
	ctlr->int_cnt->int_cap_f0_p_count = 0;
	ctlr->int_cnt->int_cap_f1_p_count = 0;

	return 0;
}

int mtk_os_hal_gpioif_set_direction_mode(
	gpioif_group group, u8 control_setting, u32 low_limit, u32 high_limit,
	u32 reset_value, gpioif_source_clock clock_source)
{
	mhal_gpioif_event_capture_setting setting;
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((control_setting > MAX_CTRL_SETTING)
		|| (clock_source >= GPIOIF_CLOCK_MAX)
		|| (low_limit >= MTK_GPIOIF_MAX_VAL)
		|| (high_limit >= MTK_GPIOIF_MAX_VAL)
		|| (reset_value >= MTK_GPIOIF_MAX_VAL))
		return -GPIOIF_EINVAL;

	setting.control_setting = control_setting;
	setting.reset_value = reset_value;
	setting.low_limit = low_limit;
	setting.high_limit = high_limit;
	ctlr->mconfig->setting = &setting;
	ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
	ctlr->mconfig->event_cnt_mode = MHAL_GPIOIF_DIRECTION_MODE;
	ctlr->mconfig->group = (mhal_gpioif_group)group;
	ctlr->mconfig->mode = MHAL_GPIOIF_EVENT_COUNTER_MODE;

	mtk_mhal_gpioif_select_clock_source(ctlr);
	/* enable clock source*/
	mtk_mhal_gpioif_counter_clock_setting(ctlr, 0x1);
	/* set dir mode setting*/
	mtk_mhal_gpioif_event_counter_setting(ctlr);
	mtk_mhal_gpioif_enable_event_counter(ctlr);
	/* enable reset event mode */
	mtk_mhal_gpioif_software_reset(ctlr);

	return 0;
}

int mtk_os_hal_gpioif_set_updown_mode(
	gpioif_group group, u8 control_setting, u32 low_limit, u32 high_limit,
	u32 reset_value, gpioif_source_clock clock_source)
{
	mhal_gpioif_event_capture_setting setting;
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((control_setting > MAX_CTRL_SETTING)
		|| (clock_source >= GPIOIF_CLOCK_MAX)
		|| (low_limit >= MTK_GPIOIF_MAX_VAL)
		|| (high_limit >= MTK_GPIOIF_MAX_VAL)
		|| (reset_value >= MTK_GPIOIF_MAX_VAL))
		return -GPIOIF_EINVAL;

	setting.control_setting = control_setting;
	setting.reset_value = reset_value;
	setting.low_limit = low_limit;
	setting.high_limit = high_limit;
	ctlr->mconfig->setting = &setting;
	ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
	ctlr->mconfig->event_cnt_mode = MHAL_GPIOIF_UP_DOWN_MODE;
	ctlr->mconfig->group = (mhal_gpioif_group)group;
	ctlr->mconfig->mode = MHAL_GPIOIF_EVENT_COUNTER_MODE;

	mtk_mhal_gpioif_select_clock_source(ctlr);
	mtk_mhal_gpioif_counter_clock_setting(ctlr, 0x1);
	mtk_mhal_gpioif_event_counter_setting(ctlr);
	mtk_mhal_gpioif_enable_event_counter(ctlr);
	mtk_mhal_gpioif_software_reset(ctlr);

	return 0;
}
int mtk_os_hal_gpioif_set_quadrature_mode(
	gpioif_group group, u8 control_setting, u32 low_limit,
	u32 high_limit, u32 reset_value, gpioif_source_clock clock_source)
{
	mhal_gpioif_event_capture_setting setting;
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((control_setting > MAX_CTRL_SETTING)
		|| (clock_source >= GPIOIF_CLOCK_MAX)
		|| (low_limit > MTK_GPIOIF_MAX_VAL)
		|| (high_limit > MTK_GPIOIF_MAX_VAL)
		|| (reset_value > MTK_GPIOIF_MAX_VAL))
		return -GPIOIF_EINVAL;

	setting.control_setting = control_setting;
	setting.reset_value = reset_value;
	setting.low_limit = low_limit;
	setting.high_limit = high_limit;
	ctlr->mconfig->setting = &setting;
	ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
	ctlr->mconfig->event_cnt_mode = MHAL_GPIOIF_QUA_MODE;
	ctlr->mconfig->group = (mhal_gpioif_group)group;
	ctlr->mconfig->mode = MHAL_GPIOIF_EVENT_COUNTER_MODE;

	mtk_mhal_gpioif_select_clock_source(ctlr);
	mtk_mhal_gpioif_counter_clock_setting(ctlr, 0x1);
	mtk_mhal_gpioif_event_counter_setting(ctlr);
	mtk_mhal_gpioif_enable_event_counter(ctlr);
	mtk_mhal_gpioif_software_reset(ctlr);

	return 0;
}
int mtk_os_hal_gpioif_set_capture_mode(
	gpioif_group group, u8 edge_type_gpio_0, u8 edge_type_gpio_1,
	gpioif_source_clock clock_source)
{
	mhal_gpioif_event_capture_setting setting;
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((clock_source >= GPIOIF_CLOCK_MAX)
		|| (edge_type_gpio_0 >= MHAL_GPIOIF_GPIO_0_EDGE_TYPE_MAX)
		|| (edge_type_gpio_1 >= MHAL_GPIOIF_GPIO_1_EDGE_TYPE_MAX))
		return -GPIOIF_EINVAL;

	setting.edge_type_gpio_0 = edge_type_gpio_0;
	setting.edge_type_gpio_1 = edge_type_gpio_1;
	ctlr->mconfig->setting = &setting;
	ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
	ctlr->mconfig->group = (mhal_gpioif_group)group;
	ctlr->mconfig->mode = MHAL_GPIOIF_CAP_COUNTER_MODE;

	mtk_mhal_gpioif_select_clock_source(ctlr);
	mtk_mhal_gpioif_counter_clock_setting(ctlr, 0x1);
	mtk_mhal_gpioif_capture_counter_setting(ctlr);
	mtk_mhal_gpioif_enable_capture_counter(ctlr);
	mtk_mhal_gpioif_software_reset(ctlr);

	return 0;
}
int mtk_os_hal_gpioif_interrupt_control(
	gpioif_group group, u8 enable, u8 clear, u32 bit)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((enable > 1) || (clear > 1) || (bit > MTK_GPIOIF_MAX_VAL))
		return -GPIOIF_EINVAL;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	if (enable)
		mtk_mhal_gpioif_interrupt_bit_wise(ctlr,
			bit, 0x1);
	else
		mtk_mhal_gpioif_interrupt_bit_wise(ctlr,
			0x0, 0x0);

	if (clear) {
		mtk_mhal_gpioif_interrupt_bit_wise(ctlr,
			bit, 0x1);
		mtk_mhal_gpioif_interrupt_bit_wise(ctlr,
			0x0, 0x0);
	}

	return 0;
}

int mtk_os_hal_gpioif_interrupt_bit_wise(gpioif_group group,
	u32 bit, u8 enable)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((enable > 1) || (bit > MTK_GPIOIF_MAX_VAL))
		return -GPIOIF_EINVAL;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	return mtk_mhal_gpioif_interrupt_bit_wise(ctlr, bit, enable);
}


int mtk_os_hal_gpioif_limit_comparator(
	gpioif_group group, gpioif_sa_mode_limit_select sa_limit_v,
	gpioif_interrupt_limit_select interrupt_limit_v)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((sa_limit_v >= GPIOIF_SA_MAX)
		|| (interrupt_limit_v >= GPIOIF_INTERRUPT_MAX))
		return -GPIOIF_EINVAL;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	ctlr->mconfig->sa_mode_lim =
		(mhal_gpioif_sa_mode_limit_select)sa_limit_v;
	ctlr->mconfig->inter_lim =
		(mhal_gpioif_interrupt_limit_select)interrupt_limit_v;

	mtk_mhal_gpioif_limit_comparator(ctlr);

	return 0;
}

int mtk_os_hal_gpioif_de_glitch(
	gpioif_group group, u8 gpio, u8 enable, u32 min_p, u32 init_v)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((gpio > 2)	|| (enable > 1) || (init_v > 1)
		|| (min_p > MAX_MIN_PURSE_WIDTH))
		return -GPIOIF_EINVAL;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	mtk_mhal_gpioif_de_glitch(ctlr, gpio,
		enable, min_p, init_v);

	return 0;
}

int mtk_os_hal_gpioif_global_cr_reset(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	mtk_mhal_gpioif_global_reset(ctlr);

	return 0;
}

int mtk_os_hal_gpioif_hardware_reset(gpioif_group group,
	mtk_os_gpioif_mode mode, u8 active_reset)
{
	u32 value = 0;
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((active_reset > 1) || (mode >= MTK_OS_GPIOIF_MODE_MAX))
		return -GPIOIF_EINVAL;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	ctlr->mconfig->mode = (mhal_gpioif_mode)mode;
	mtk_mhal_gpioif_hardware_reset_by_gpio_2(ctlr, active_reset);

	if (mode == MTK_OS_GPIOIF_EVENT_COUNTER_MODE) {
		mtk_mhal_gpioif_read_gpio_event_count(ctlr,
		&value);
	} else if (mode == MTK_OS_GPIOIF_CAP_COUNTER_MODE) {
		mtk_mhal_gpioif_read_gpio_cap_fifo0_value(ctlr,
		&value);
	} else if (mode == MTK_OS_GPIOIF_EVENT_CAP_COUNTER_MODE) {
		mtk_mhal_gpioif_read_gpio_event_count(ctlr,
		&value);
		mtk_mhal_gpioif_read_gpio_cap_fifo0_value(ctlr,
		&value);
		mtk_mhal_gpioif_read_gpio_cap_fifo1_value(ctlr,
		&value);
	}

	return 0;
}

int mtk_os_hal_gpioif_select_clock_source(gpioif_group group,
	gpioif_source_clock clock_source)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if (clock_source >= GPIOIF_CLOCK_MAX)
		return -GPIOIF_EINVAL;

	ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
	mtk_mhal_gpioif_select_clock_source(ctlr);

	return 0;
}

int mtk_os_hal_gpioif_counter_clock_setting(gpioif_group group, u8 enable)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if (enable > 1)
		return -GPIOIF_EINVAL;

	ctlr->mconfig->group = (mhal_gpioif_group)group;

	mtk_mhal_gpioif_counter_clock_setting(ctlr, enable);

	return 0;
}

int mtk_os_hal_gpioif_software_reset(gpioif_group group,
	mtk_os_gpioif_mode mode)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if (mode >= MTK_OS_GPIOIF_MODE_MAX)
		return -GPIOIF_EINVAL;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	ctlr->mconfig->mode = (mhal_gpioif_mode)mode;

	mtk_mhal_gpioif_software_reset(ctlr);

	return 0;
}

int mtk_os_hal_gpioif_enable_event_counter(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;

	mtk_mhal_gpioif_enable_event_counter(ctlr);

	return 0;
}

int mtk_os_hal_gpioif_disable_event_counter(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;

	mtk_mhal_gpioif_disable_event_counter(ctlr);

	return 0;
}

int mtk_os_hal_gpioif_disable_capture_counter(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;

	mtk_mhal_gpioif_disable_capture_counter(ctlr);

	return 0;
}

int mtk_os_hal_gpioif_event_counter_setting(
	gpioif_group group, u8 control_setting, u32 low_limit, u32 high_limit,
	u32 reset_value, u8 event_cnt_mode)
{
	mhal_gpioif_event_capture_setting setting;
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if ((control_setting > MAX_CTRL_SETTING)
		|| (event_cnt_mode >= MHAL_GPIOIF_EVNET_COUNTER_MODE_MAX)
		|| (low_limit > MTK_GPIOIF_MAX_VAL)
		|| (high_limit > MTK_GPIOIF_MAX_VAL)
		|| (reset_value > MTK_GPIOIF_MAX_VAL))
		return -GPIOIF_EINVAL;

	setting.control_setting = control_setting;
	setting.reset_value = reset_value;
	setting.low_limit = low_limit;
	setting.high_limit = high_limit;
	ctlr->mconfig->setting = &setting;
	ctlr->mconfig->event_cnt_mode =
		(mhal_gpioif_event_counter_mode)event_cnt_mode;
	ctlr->mconfig->group = (mhal_gpioif_group)group;

	mtk_mhal_gpioif_event_counter_setting(ctlr);

	return 0;
}

int mtk_os_hal_gpioif_read_gpio_cap_fifo0_value(
	gpioif_group group, u32 *pvalue)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	ctlr->mconfig->group = (mhal_gpioif_group)group;
	return mtk_mhal_gpioif_read_gpio_cap_fifo0_value(ctlr, pvalue);
}

int mtk_os_hal_gpioif_read_gpio_cap_fifo1_value(
	gpioif_group group, u32 *pvalue)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;
	ctlr->mconfig->group = (mhal_gpioif_group)group;
	return mtk_mhal_gpioif_read_gpio_cap_fifo1_value(ctlr, pvalue);
}

int mtk_os_hal_gpioif_dump_int(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	printf("int_cnt=0x%x\n", ctlr->int_cnt->int_bit);
	printf("int_event_low_count=%d\n",
		ctlr->int_cnt->int_event_low_count);
	printf("int_event_high_count=%d\n",
		ctlr->int_cnt->int_event_high_count);
	printf("int_gpio2_rst_done_count=%d\n",
		ctlr->int_cnt->int_gpio2_rst_done_count);
	printf("int_event_over_count=%d\n",
		ctlr->int_cnt->int_event_over_count);
	printf("int_event_uf_count=%d\n",
		ctlr->int_cnt->int_event_uf_count);
	printf("int_cap_f0_full_count=%d\n",
		ctlr->int_cnt->int_cap_f0_full_count);
	printf("int_cap_f1_full_count=%d\n",
		ctlr->int_cnt->int_cap_f1_full_count);
	printf("int_reset_cap_f0_full_count=%d\n",
		ctlr->int_cnt->int_reset_cap_f0_full_count);
	printf("int_reset_cap_f1_full_count=%d\n",
		ctlr->int_cnt->int_reset_cap_f1_full_count);
	printf("int_cap_f0_np_count=%d\n",
		ctlr->int_cnt->int_cap_f0_np_count);
	printf("int_cap_f1_np_count=%d\n",
		ctlr->int_cnt->int_cap_f1_np_count);
	printf("int_cap_f0_p_count=%d\n",
		ctlr->int_cnt->int_cap_f0_p_count);
	printf("int_cap_f1_p_count =%d\n",
		ctlr->int_cnt->int_cap_f1_p_count);

	return 0;
}

int mtk_os_hal_gpioif_get_int_event_low_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_event_low_count;
}

int mtk_os_hal_gpioif_get_int_event_high_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_event_high_count;
}

int mtk_os_hal_gpioif_get_int_gpio2_rst_done_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_gpio2_rst_done_count;
}

int mtk_os_hal_gpioif_get_int_event_over_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_event_over_count;
}

int mtk_os_hal_gpioif_get_int_event_uf_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_event_uf_count;
}

int mtk_os_hal_gpioif_get_int_cap_f0_full_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_cap_f0_full_count;
}

int mtk_os_hal_gpioif_get_int_cap_f1_full_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_cap_f1_full_count;
}

int mtk_os_hal_gpioif_get_int_reset_cap_f0_full_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_reset_cap_f0_full_count;
}

int mtk_os_hal_gpioif_get_int_reset_cap_f1_full_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_reset_cap_f1_full_count;
}

int mtk_os_hal_gpioif_get_int_cap_f0_np_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_cap_f0_np_count;
}

int mtk_os_hal_gpioif_get_int_cap_f1_np_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_cap_f1_np_count;
}

int mtk_os_hal_gpioif_get_int_cap_f0_p_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_cap_f0_p_count;
}

int mtk_os_hal_gpioif_get_int_cap_f1_p_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->int_cap_f1_p_count;
}

int mtk_os_hal_gpioif_get_cap_fifo0_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->cap_fifo0_count;
}

int mtk_os_hal_gpioif_get_cap_fifo1_count(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	return ctlr->int_cnt->cap_fifo1_count;
}

int mtk_os_hal_gpioif_get_cap_fifo0_val(gpioif_group group, u32 idex)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if (idex >= MAX_FIFO_ENTRY_CNT)
		return -GPIOIF_EINVAL;

	return ctlr->int_cnt->cap_fifo0[idex];
}

int mtk_os_hal_gpioif_get_cap_fifo1_val(gpioif_group group, u32 idex)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if (idex >= MAX_FIFO_ENTRY_CNT)
		return -GPIOIF_EINVAL;

	return ctlr->int_cnt->cap_fifo1[idex];
}

int mtk_os_hal_gpioif_ctlr_init(gpioif_group group)
{
	struct mtk_gpioif_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_gpioif_get_ctlr(group);
	struct mtk_gpioif_controller *ctlr;

	if (!ctlr_rtos)
		return -GPIOIF_EINVAL;
	ctlr = ctlr_rtos->ctlr;

	if (group >= MTK_GPIOIF_MAX_GRP_NUM)
		return -GPIOIF_EINVAL;

	ctlr_rtos->ctlr = &g_gpioif_ctlr[(u8)group];
	ctlr = ctlr_rtos->ctlr;
	ctlr->base[(u8)group] = (void __iomem *)gpioif_base_addr[(u8)group];
	ctlr->reg_clk_base = (void __iomem *)CM4_GPIOIF_CG_BASE;
	ctlr->mconfig = &g_gpioif_mconfig[group];
	ctlr->int_cnt = &g_gpioif_int_cnt[group];
	ctlr->mconfig->group = (mhal_gpioif_group)group;

	mtk_os_hal_gpioif_interrupt_count_init(group);
	mtk_os_hal_gpioif_capture_fifo_init(group);

	_mtk_os_hal_gpioif_request_irq(group);

	return 0;
}

int mtk_os_hal_gpioif_ctlr_deinit(gpioif_group group)
{

	_mtk_os_hal_gpioif_free_irq(group);

	return 0;
}
