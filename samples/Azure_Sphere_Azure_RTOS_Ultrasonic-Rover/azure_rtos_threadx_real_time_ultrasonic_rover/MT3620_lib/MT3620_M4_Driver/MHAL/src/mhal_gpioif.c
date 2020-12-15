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

#include "hdl_gpioif.h"
#include "mhal_gpioif.h"

int mtk_mhal_gpioif_select_clock_source(struct mtk_gpioif_controller *ctlr)
{
	if ((!ctlr) || (!ctlr->mconfig) || (!ctlr->reg_clk_base))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->clk >= MHAL_GPIOIF_CLOCK_MAX)
		return -GPIOIF_EINVAL;

	mtk_hdl_gpioif_select_clock_source(
		ctlr->reg_clk_base, ctlr->mconfig->clk);
	return 0;
}

int mtk_mhal_gpioif_event_counter_setting(struct mtk_gpioif_controller *ctlr)
{
	mhal_gpioif_event_capture_setting *psetting;
	u8 group = 0;
	int mode = 0;

	if ((!ctlr) || (!ctlr->mconfig) || (!ctlr->mconfig->setting)
		|| (!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;

	group = ctlr->mconfig->group;
	mode = ctlr->mconfig->event_cnt_mode;
	psetting = ctlr->mconfig->setting;
	if ((group >= MHAL_GPIOIF_GROUP_MAX) ||
		(mode >= MHAL_GPIOIF_EVNET_COUNTER_MODE_MAX))
		return -GPIOIF_EINVAL;

	if (psetting->control_setting > MAX_CTRL_SETTING)
		return -GPIOIF_EINVAL;

	mtk_hdl_gpioif_event_counter_setting(ctlr->base[group],
		mode, psetting->control_setting, psetting->updown,
		psetting->reset_value, psetting->low_limit,
		psetting->high_limit);
	return 0;
}

int mtk_mhal_gpioif_capture_counter_setting(struct mtk_gpioif_controller *ctlr)
{
	u8 group = 0;
	int edge_type_gpio_0 = 0;
	int edge_type_gpio_1 = 0;

	if ((!ctlr) || (!ctlr->mconfig) || (!ctlr->mconfig->setting)
		|| (!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;

	group = ctlr->mconfig->group;
	edge_type_gpio_0 = ctlr->mconfig->setting->edge_type_gpio_0;
	edge_type_gpio_1 = ctlr->mconfig->setting->edge_type_gpio_1;
	if ((group >= MHAL_GPIOIF_GROUP_MAX) ||
		(edge_type_gpio_0 >= MHAL_GPIOIF_GPIO_0_EDGE_TYPE_MAX) ||
		(edge_type_gpio_1 >= MHAL_GPIOIF_GPIO_1_EDGE_TYPE_MAX))
		return -GPIOIF_EINVAL;

	mtk_hdl_gpioif_capture_counter_setting(ctlr->base[group],
		ctlr->mconfig->setting->edge_type_gpio_0,
		ctlr->mconfig->setting->edge_type_gpio_1);
	return 0;
}

int mtk_mhal_gpioif_enable_event_counter(struct mtk_gpioif_controller *ctlr)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_enable_event_counter(ctlr->base[group]);
	return 0;
}

int mtk_mhal_gpioif_disable_event_counter(struct mtk_gpioif_controller *ctlr)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_mhdl_gpioif_disable_event_counter(ctlr->base[group]);
	return 0;
}

int mtk_mhal_gpioif_enable_capture_counter(struct mtk_gpioif_controller *ctlr)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_enable_capture_counter(ctlr->base[group]);
	return 0;
}

int mtk_mhal_gpioif_disable_capture_counter(struct mtk_gpioif_controller *ctlr)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_disable_capture_counter(ctlr->base[group]);
	return 0;
}

int mtk_mhal_gpioif_software_reset(struct mtk_gpioif_controller *ctlr)
{
	u8 group = 0;
	u32 mode = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;

	group = ctlr->mconfig->group;
	mode = ctlr->mconfig->mode;

	if ((group >= MHAL_GPIOIF_GROUP_MAX) ||
		(mode >= MHAL_GPIOIF_MODE_MAX))
		return -GPIOIF_EINVAL;

	mtk_hdl_gpioif_software_reset(ctlr->base[group], mode);
	return 0;
}

int mtk_mhal_gpioif_interrupt_bit_wise(
	struct mtk_gpioif_controller *ctlr, u32 bit, u8 enable)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) || (!ctlr->int_cnt)
		|| (!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	if (enable > 1)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	ctlr->int_cnt->int_bit = bit;
	mtk_hdl_gpioif_interrupt_bit_wise(ctlr->base[group], bit, enable);
	return 0;
}

int mtk_mhal_gpioif_hardware_reset_by_gpio_2(
	struct mtk_gpioif_controller *ctlr, int active_reset)
{
	u8 group = 0;
	u32 mode = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;

	if ((active_reset > 1) || (active_reset < 0))
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mode = ctlr->mconfig->mode;
	if ((group >= MHAL_GPIOIF_GROUP_MAX) ||
		(mode >= MHAL_GPIOIF_EVNET_COUNTER_MODE_MAX))
		return -GPIOIF_EINVAL;

	mtk_hdl_gpioif_hardware_reset_by_gpio_2(
		ctlr->base[group], mode, active_reset);
	return 0;
}

int mtk_mhal_gpioif_read_gpio_event_count(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)])
		|| (pvalue == NULL))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_read_gpio_event_count(ctlr->base[group], pvalue);
	return 0;
}

int mtk_mhal_gpioif_read_reset_val(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)])
		|| (pvalue == NULL))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_read_reset_val(ctlr->base[group], pvalue);
	return 0;
}

int mtk_mhal_gpioif_read_low_limit_val(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)])
		|| (pvalue == NULL))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_read_low_limit_val(ctlr->base[group], pvalue);
	return 0;
}

int mtk_mhal_gpioif_read_high_limit_val(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)])
		|| (pvalue == NULL))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_read_high_limit_val(ctlr->base[group], pvalue);
	return 0;
}

int mtk_mhal_gpioif_read_gpio_cap_fifo0_value(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)])
		|| (pvalue == NULL))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_read_gpio_cap_fifo0_value(
		ctlr->base[group], pvalue);
	return 0;
}

int mtk_mhal_gpioif_read_gpio_cap_fifo1_value(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)])
		|| (pvalue == NULL))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_read_gpio_cap_fifo1_value(
		ctlr->base[group], pvalue);
	return 0;
}

int mtk_mhal_gpioif_de_glitch(
	struct mtk_gpioif_controller *ctlr, int gpio, int enable,
	unsigned int min_p, unsigned int init_v)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	if ((gpio < 0) || (gpio > 2))
		return -GPIOIF_EINVAL;

	if ((enable < 0) || (enable > 1))
		return -GPIOIF_EINVAL;

	if (min_p > MAX_MIN_PURSE_WIDTH)
		return -GPIOIF_EINVAL;

	if ((init_v != 0) && (init_v != 1))
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_de_glitch(ctlr->base[group],
		gpio, enable, min_p, init_v);
	return 0;
}

int mtk_mhal_gpioif_limit_comparator(struct mtk_gpioif_controller *ctlr)
{
	u8 group = 0;
	int sa_limit_v = 0;
	int interrupt_limit_v = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;

	group = ctlr->mconfig->group;
	sa_limit_v = ctlr->mconfig->sa_mode_lim;
	interrupt_limit_v = ctlr->mconfig->inter_lim;
	if ((group >= MHAL_GPIOIF_GROUP_MAX) ||
		(sa_limit_v >= MHAL_GPIOIF_SA_MAX) ||
		(interrupt_limit_v >= MHAL_GPIOIF_INTERRUPT_MAX))
		return -GPIOIF_EINVAL;

	mtk_hdl_gpioif_limit_comparator(ctlr->base[group],
		sa_limit_v, interrupt_limit_v);
	return 0;
}

int mtk_mhal_gpioif_global_reset(struct mtk_gpioif_controller *ctlr)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_global_reset(ctlr->base[group]);
	return 0;
}

int mtk_mhal_gpioif_counter_clock_setting(
	struct mtk_gpioif_controller *ctlr, int enable)
{
	u8 group = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;
	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	if ((enable != 0) && (enable != 1))
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_counter_clock_setting(ctlr->base[group], enable);
	return 0;
}

int mtk_mhal_gpioif_isr_handle(struct mtk_gpioif_controller *ctlr)
{
	u32 value = 0;
	u32 value1 = 0;
	u32 count = 0;
	u8 group = 0;
	u32 cap_f0_cnt, cap_f1_cnt;
	u32 count_fifo0 = 0;
	u32 count_fifo1 = 0;

	if ((!ctlr) || (!ctlr->mconfig) ||
		(!ctlr->base[(u8)(ctlr->mconfig->group)]))
		return -GPIOIF_EPTR;

	if (ctlr->mconfig->group >= MHAL_GPIOIF_GROUP_MAX)
		return -GPIOIF_EINVAL;

	group = ctlr->mconfig->group;
	mtk_hdl_gpioif_read_int_val(ctlr->base[group], &value1);
	value = (value1 & GPIO_CNT_INT_STS_EVENT_LOW_MASK);

	gpioif_debug("%s group %d interrupt_bit 0x%x, value1=0x%x\n",
		__func__, group, ctlr->int_cnt->int_bit, value1);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_EVENT_C_R_LOW\n");
		ctlr->int_cnt->int_event_low_count++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_EVENT_LOW_SHFT,
			GPIO_CNT_INT_STS_EVENT_LOW_MASK);
	}

	value = (value1 & GPIO_CNT_INT_STS_EVENT_HIGH_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_EVENT_C_R_HIGH\n");
		ctlr->int_cnt->int_event_high_count++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_EVENT_HIGH_SHFT,
			GPIO_CNT_INT_STS_EVENT_HIGH_MASK);
	}

	value = (value1 & GPIO_CNT_INT_STS_GPIO2_RST_DONE_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_RESET_DONE\n");
		ctlr->int_cnt->int_gpio2_rst_done_count++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_GPIO2_RST_DONE_SHFT,
			GPIO_CNT_INT_STS_GPIO2_RST_DONE_MASK);
	}

	value = (value1 & GPIO_CNT_INT_STS_EVENT_OVER_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_EVENT_C_OVERFLOW\n");
		ctlr->int_cnt->int_event_over_count++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_EVENT_OVER_SHFT,
			GPIO_CNT_INT_STS_EVENT_OVER_MASK);
	}

	value = (value1 & GPIO_CNT_INT_STS_EVENT_UF_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_EVENT_C_UNDERFLOW\n");
		ctlr->int_cnt->int_event_uf_count++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_EVENT_UF_SHFT,
			GPIO_CNT_INT_STS_EVENT_UF_MASK);
	}

	value = (value1 & GPIO_CNT_INT_STS_CAP_F0_FULL_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_W_CFIFO0_WHENFULL\n");
		ctlr->int_cnt->int_cap_f0_full_count++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_CAP_F0_FULL_SHFT,
			GPIO_CNT_INT_STS_CAP_F0_FULL_MASK);
	}
	value = (value1 & GPIO_CNT_INT_STS_CAP_F1_FULL_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_W_CFIFO1_WHENFULL\n");
		ctlr->int_cnt->int_cap_f1_full_count++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_CAP_F1_FULL_SHFT,
			GPIO_CNT_INT_STS_CAP_F1_FULL_MASK);
	}

	value = (value1 & GPIO_CNT_INT_STS_RESET_CAP_F0_FULL_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_W__RESET_CFIFO0_WHENFULL\n");
		ctlr->int_cnt->int_reset_cap_f0_full_count++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_RESET_CAP_F0_FULL_SHFT,
			GPIO_CNT_INT_STS_RESET_CAP_F0_FULL_MASK);
	}
	value = (value1 & GPIO_CNT_INT_STS_RESET_CAP_F1_FULL_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_W_RESET_CFIFO1_WHENFULL\n");
		ctlr->int_cnt->int_reset_cap_f1_full_count++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_RESET_CAP_F1_FULL_SHFT,
			GPIO_CNT_INT_STS_RESET_CAP_F1_FULL_MASK);
	}

	value = (value1 & GPIO_CNT_INT_STS_CAP_F0_NP_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_CFIFO0_NOT_EMPTY\n");
		ctlr->int_cnt->int_cap_f0_np_count++;

		/* Set [16]= 1'b0 */
		ctlr->int_cnt->int_bit &= ~(1 << IRQ_GPIOIF_CFIFO0_NOT_EMPTY);
		mtk_hdl_gpioif_interrupt_bit_wise(ctlr->base[group],
			ctlr->int_cnt->int_bit, 0x1);

		/* FIFO count of Capture FIFO 0 = 4'h1 */
		mtk_hdl_gpioif_read_cap_fifo0_count(ctlr->base[group], &count);
		gpioif_debug("count %u\n", count);
		for (; count > 0; count--) {
		/* Read Capture FIFO 0 Once */
			mtk_hdl_gpioif_read_gpio_cap_fifo0_value(
				ctlr->base[group], &value);
			gpioif_debug("Read Capture FIFO 0  %u\n", value);
			mtk_hdl_gpioif_read_cap_fifo0_count(
				ctlr->base[group], &count_fifo0);
			gpioif_debug("count_fifo0 %u\n", count_fifo0);
			if (ctlr->int_cnt->cap_fifo0_count < 5) {
				cap_f0_cnt = ctlr->int_cnt->cap_fifo0_count;
				ctlr->int_cnt->cap_fifo0[cap_f0_cnt] = value;
				ctlr->int_cnt->cap_fifo0_count++;
			}
		}

		value =  (1 << IRQ_GPIOIF_CFIFO0_NOT_EMPTY);
		mtk_hdl_gpioif_clear_interrupt_status(ctlr->base[group], value);
		/* Bit-wisely write 1 to enable interrupt */
		if (ctlr->int_cnt->cap_fifo0_count < 5)
			ctlr->int_cnt->int_bit |=
				(1 << IRQ_GPIOIF_CFIFO0_NOT_EMPTY);

		mtk_hdl_gpioif_interrupt_bit_wise(ctlr->base[group],
			ctlr->int_cnt->int_bit, 0x1);

	}

	value = (value1 & GPIO_CNT_INT_STS_CAP_F1_NP_MASK);
	if (value) {

		gpioif_debug("IRQ_GPIOIF_CFIFO1_NOT_EMPTY\n");
		ctlr->int_cnt->int_cap_f1_np_count++;

		/* Set [16]= 1'b0 */
		ctlr->int_cnt->int_bit &= ~(1 << IRQ_GPIOIF_CFIFO1_NOT_EMPTY);
		mtk_hdl_gpioif_interrupt_bit_wise(ctlr->base[group],
			ctlr->int_cnt->int_bit, 0x1);

		/* FIFO count of Capture FIFO 0 = 4'h1 */
		mtk_hdl_gpioif_read_cap_fifo1_count(ctlr->base[group], &count);
		gpioif_debug("count %u\n", count);
		for (; count > 0; count--) {
		/* Read Capture FIFO 1 Once */
			mtk_hdl_gpioif_read_gpio_cap_fifo1_value(
				ctlr->base[group], &value);
			gpioif_debug("Read Capture FIFO 1  %u\n", value);
			mtk_hdl_gpioif_read_cap_fifo1_count(
				ctlr->base[group], &count_fifo1);
			gpioif_debug("count_fifo1 %u\n", count_fifo1);
			if (ctlr->int_cnt->cap_fifo1_count < 5) {
				cap_f1_cnt = ctlr->int_cnt->cap_fifo1_count;
				ctlr->int_cnt->cap_fifo1[cap_f1_cnt] = value;
				ctlr->int_cnt->cap_fifo1_count++;
			}
		}

		value =  (1 << IRQ_GPIOIF_CFIFO1_NOT_EMPTY);
		mtk_hdl_gpioif_clear_interrupt_status(ctlr->base[group], value);

		/* Bit-wisely write 1 to enable interrupt */
		if (ctlr->int_cnt->cap_fifo1_count < 5)
			ctlr->int_cnt->int_bit |=
				(1 << IRQ_GPIOIF_CFIFO1_NOT_EMPTY);

		mtk_hdl_gpioif_interrupt_bit_wise(ctlr,
			ctlr->int_cnt->int_bit, 0x1);
	}

	value = (value1 & GPIO_CNT_INT_STS_CAP_F0_P_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_R_CFIFO0_WHENEMPTY\n");
		(ctlr->int_cnt->int_cap_f0_p_count)++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_CAP_F0_P_SHFT,
			GPIO_CNT_INT_STS_CAP_F0_P_MASK);
	}
	value = (value1 & GPIO_CNT_INT_STS_CAP_F1_P_MASK);
	if (value) {
		gpioif_debug("IRQ_GPIOIF_R_CFIFO1_WHENEMPTY\n");
		(ctlr->int_cnt->int_cap_f1_p_count)++;

		mtk_hdl_gpioif_clear_int_sts_by_bit(ctlr->base[group],
			ctlr->int_cnt->int_bit,
			GPIO_CNT_INT_STS_CAP_F1_P_SHFT,
			GPIO_CNT_INT_STS_CAP_F1_P_MASK);
	}

	return 0;
}
