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

/**
 * @brief This enum defines GPIOIF Event Counter Mode.\n
 */
typedef enum {
	/** GPIOIF Direction mode */
	GPIOIF_DIRECTION_MODE = 0,
	/** GPIOIF Up Down mode */
	GPIOIF_UP_DOWN_MODE = 1,
	/** GPIOIF Quadrature mode */
	GPIOIF_QUA_MODE = 2,
	/**  GPIOIF maximum Event Counter Mode <invalid> */
	GPIOIF_EVNET_COUNTER_MODE_MAX
} gpioif_event_counter_mode;

typedef enum {
	/** GPIOIF Event Counter Mode */
	GPIOIF_EVENT_COUNTER_MODE = 0,
	/** GPIOIF Capture Counter Mode */
	GPIOIF_CAP_COUNTER_MODE = 1,
	/** GPIOIF Both Event Counter Mode and Capture Counter Mode */
	GPIOIF_EVENT_CAP_COUNTER_MODE = 2,
	/**  GPIOIF maximum supported Mode  <invalid> */
	GPIOIF_MODE_MAX
} gpioif_mode;

void mtk_hdl_gpioif_select_clock_source(
	void __iomem *reg_clk_base, u32 clk)
{
	u32 reg_val = 0;

	/* 0:XTAL KHz(26MHz), 1:32KHz, 2:AXIL MHz(125MHz), 3:200MHz */
	gpioif_debug("gpio_clock_source clock %x\n", clk);
	reg_val = osai_readl(reg_clk_base
		+ GPIO_Counter_Clock_Switch_ADDR);
	reg_val &= (~GPIO_Counter_Clock_Switch_MASK);
	reg_val |= (clk << GPIO_Counter_Clock_Switch_SHFT);
	osai_writel(reg_val, reg_clk_base + GPIO_Counter_Clock_Switch_ADDR);
}

void mtk_hdl_gpioif_event_counter_setting(void __iomem *base_addr,
	u32 mode, u8 control_setting, u8 updown,
	u32 reset_value, u32 low_limit, u32 high_limit)
{
	u32 reg_val = 0;

	if (control_setting <= DIR_UP_DOWN_CTRL_SETTING)
		updown = 0;
	else
		updown = 1;

	/* mode : 0:direction, 1:up/down, 2.quadrature*/
	gpioif_debug("%s mode %x control_setting %x updown %x\n",
		__func__, mode, control_setting,
		updown);

	gpioif_debug("reset_value %u ow_limit 0x%x high_limit 0x%x\n",
		reset_value, low_limit,
		high_limit);

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
	reg_val &= (~GPIO_EVENT_CNT_MODE_MASK);
	reg_val |= (mode << GPIO_EVENT_CNT_MODE_SHFT);
	if (mode == GPIOIF_DIRECTION_MODE) {
		reg_val = reg_val & (~GPIO_EVENT_CNT_DIR_DEF_MASK);
		reg_val = reg_val |
			(updown << GPIO_EVENT_CNT_DIR_DEF_SHFT);
	} else if (mode == GPIOIF_UP_DOWN_MODE) {
		reg_val &= (~GPIO_EVENT_CNT_UPDOWN_DEF_MASK);
		reg_val |= (updown << GPIO_EVENT_CNT_UPDOWN_DEF_SHFT);
	} else if (mode == GPIOIF_QUA_MODE) {
		reg_val &= (~GPIO_EVENT_CNT_QUAD_DEF_MASK);
		reg_val |= (updown << GPIO_EVENT_CNT_QUAD_DEF_SHFT);
	}

	switch (control_setting) {
	case 1:
		reg_val &= (~GPIO_EVENT_CNT_EDGE_TYPE_MASK);
		reg_val |= (0x0 << GPIO_EVENT_CNT_EDGE_TYPE_SHFT);
		break;
	case 2:
		reg_val &= (~GPIO_EVENT_CNT_EDGE_TYPE_MASK);
		reg_val |= (0x1 << GPIO_EVENT_CNT_EDGE_TYPE_SHFT);
		break;
	case 3:
		reg_val &= (~GPIO_EVENT_CNT_EDGE_TYPE_MASK);
		reg_val |= (0x2 << GPIO_EVENT_CNT_EDGE_TYPE_SHFT);
		break;
	case 4:
		reg_val &= (~GPIO_EVENT_CNT_EDGE_TYPE_MASK);
		reg_val |= (0x0 << GPIO_EVENT_CNT_EDGE_TYPE_SHFT);
		break;
	case 5:
		reg_val &= (~GPIO_EVENT_CNT_EDGE_TYPE_MASK);
		reg_val |= (0x1 << GPIO_EVENT_CNT_EDGE_TYPE_SHFT);
		break;
	case 6:
		reg_val &= (~GPIO_EVENT_CNT_EDGE_TYPE_MASK);
		reg_val |= (0x2 << GPIO_EVENT_CNT_EDGE_TYPE_SHFT);
		break;
	}
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_RST_VAL);
	reg_val = (~GPIO_EVENT_CNT_RST_VAL_MASK);
	reg_val |= (reset_value << GPIO_EVENT_CNT_RST_VAL_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_RST_VAL);

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_LOW_LIMIT);
	reg_val = (~GPIO_EVENT_CNT_LOW_LIMIT_MASK);
	reg_val |= (low_limit << GPIO_EVENT_CNT_LOW_LIMIT_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_LOW_LIMIT);

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_HIGH_LIMIT);
	reg_val = (~GPIO_EVENT_CNT_HIGH_LIMIT_MASK);
	reg_val |= (high_limit << GPIO_EVENT_CNT_HIGH_LIMIT_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_HIGH_LIMIT);
}

void mtk_hdl_gpioif_capture_counter_setting(void __iomem *base_addr,
	u32 edge_type_gpio_0, u32 edge_type_gpio_1)
{
	u32 reg_val = 0;

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
	reg_val &= (~GPIO_CAP_CNT_GPIO0_EDGE_TYPE_MASK);
	reg_val |= (edge_type_gpio_0 << GPIO_CAP_CNT_GPIO0_EDGE_TYPE_SHFT);
	reg_val &= (~GPIO_CAP_CNT_GPIO1_EDGE_TYPE_MASK);
	reg_val |= (edge_type_gpio_1 << GPIO_CAP_CNT_GPIO1_EDGE_TYPE_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
}

void mtk_hdl_gpioif_enable_event_counter(void __iomem *base_addr)
{
	u32 reg_val = 0;

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
	reg_val &= (~GPIO_EVENT_CNT_EN_MASK);
	reg_val |= (1 << GPIO_EVENT_CNT_EN_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
}

void mtk_mhdl_gpioif_disable_event_counter(void __iomem *base_addr)
{
	u32 reg_val = 0;

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
	reg_val &= (~GPIO_EVENT_CNT_EN_MASK);
	reg_val |= (0 << GPIO_EVENT_CNT_EN_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
}

void mtk_hdl_gpioif_enable_capture_counter(void __iomem *base_addr)
{
	u32 reg_val = 0;

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
	reg_val &= (~GPIO_CAP_CNT_EN_MASK);
	reg_val |= (1 << GPIO_CAP_CNT_EN_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
}

void mtk_hdl_gpioif_disable_capture_counter(void __iomem *base_addr)
{
	u32 reg_val = 0;

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
	reg_val &= (~GPIO_CAP_CNT_EN_MASK);
	reg_val |= (0 << GPIO_CAP_CNT_EN_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
}

void mtk_hdl_gpioif_software_reset(void __iomem *base_addr, u32 mode)
{
	u32 reg_val = 0;

	/* mode 0: event counter ,1: capture counter */
	gpioif_debug("%s mode %x\n", __func__, mode);
	if (mode == 0) {
		reg_val = osai_readl(base_addr +
			GPIO_CNT_GRP_DYNAMIC_CTRL_0);
		reg_val &= (~GPIO_EVENT_CNT_SW_RST_MASK);
		reg_val |= (1 << GPIO_EVENT_CNT_SW_RST_SHFT);
		osai_writel(reg_val,
			base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
		reg_val &= (~GPIO_EVENT_CNT_SW_RST_MASK);
		osai_writel(reg_val,
			base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
	} else if (mode == 1) {
		reg_val = osai_readl(base_addr +
			GPIO_CNT_GRP_DYNAMIC_CTRL_0);
		reg_val &= (~GPIO_CAP_CNT_SW_RST_MASK);
		reg_val |= (1 << GPIO_CAP_CNT_SW_RST_SHFT);
		osai_writel(reg_val,
			base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
		reg_val &= (~GPIO_CAP_CNT_SW_RST_MASK);
		osai_writel(reg_val,
			base_addr + GPIO_CNT_GRP_DYNAMIC_CTRL_0);
	}
}

void mtk_hdl_gpioif_interrupt_bit_wise(
	void __iomem *base_addr, u32 bit, u8 enable)
{
	u32 reg_val = 0;

	gpioif_debug("%s bit 0x%x enable %x\n", __func__, bit, enable);

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_INT_EN);
	reg_val = (~GPIO_CNT_INT_EN_MASK);

	if (enable)
		reg_val |= (bit << GPIO_CNT_INT_EN_SHFT);
	else
		reg_val |= (0X0 << GPIO_CNT_INT_EN_SHFT);

	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_INT_EN);

	/*need to clear interrupt 0x138 bit[2] GPIO2_Reset*/
	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_INT_STS_CLR);
	reg_val |= (0X1 << GPIO_CNT_INT_STS_GPIO2_RST_DONE_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_INT_STS_CLR);
	reg_val &= (~GPIO_CNT_INT_STS_GPIO2_RST_DONE_MASK);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_INT_STS_CLR);
}

void mtk_hdl_gpioif_clear_interrupt_status(
	void __iomem *base_addr, u32 bit)
{
	u32 reg_val = 0;

	gpioif_debug("%s bit 0x%x\n", __func__, bit);
	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_INT_STS_CLR);
	reg_val = (~GPIO_CNT_INT_STS_CLR_MASK);
	/* Bit-wisely write 1 to clear interrupt */
	reg_val |= (bit << GPIO_CNT_INT_STS_CLR_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_INT_STS_CLR);
	reg_val = (~GPIO_CNT_INT_STS_CLR_MASK);
	/* Bit-wisely write 0 to accept subsequent interrupt */
	reg_val |= (0X0 << GPIO_CNT_INT_STS_CLR_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_INT_STS_CLR);
}

void mtk_hdl_gpioif_hardware_reset_by_gpio_2(
	void __iomem *base_addr, u32 mode, int active_reset)
{
	u32 reg_val = 0;

	/* active_reset 0: low , 1: hihg */
	/* mode 0: event counter ,1: capture counter 2. both reset*/
	gpioif_debug("%s mode %x active_reset %x\n",
		__func__, mode, active_reset);
	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
	reg_val &= (~GPIO_CNT_GPIO2_RST_LVL_MASK);
	reg_val |= (active_reset << GPIO_CNT_GPIO2_RST_LVL_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
	if (mode == GPIOIF_EVENT_COUNTER_MODE) {
		reg_val &= (~GPIO_EVENT_CNT_GPIO2_RST_EN_MASK);
		reg_val |= (1 << GPIO_EVENT_CNT_GPIO2_RST_EN_SHFT);
	} else if (mode == GPIOIF_CAP_COUNTER_MODE) {
		reg_val &= (~GPIO_CAP_CNT_GPIO2_RST_EN_MASK);
		reg_val |= (1 << GPIO_CAP_CNT_GPIO2_RST_EN_SHFT);
	} else if (mode == GPIOIF_EVENT_CAP_COUNTER_MODE) {
		reg_val &= (~GPIO_EVENT_CNT_GPIO2_RST_EN_MASK);
		reg_val &= (~GPIO_CAP_CNT_GPIO2_RST_EN_MASK);
		reg_val |= (1 << GPIO_EVENT_CNT_GPIO2_RST_EN_SHFT);
		reg_val |= (1 << GPIO_CAP_CNT_GPIO2_RST_EN_SHFT);
	}
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
}

void mtk_hdl_gpioif_read_gpio_event_count(
	void __iomem *base_addr, u32 *pvalue)
{
	*pvalue = osai_readl(base_addr + GPIO_CNT_GRP_COUNTER_VALUE);

	gpioif_debug("event counter 0x%x\n", *pvalue);
}

void mtk_hdl_gpioif_read_reset_val(
	void __iomem *base_addr, u32 *pvalue)
{
	*pvalue = osai_readl(base_addr + GPIO_CNT_GRP_RST_VAL);

	gpioif_debug("reset val 0x%x\n", *pvalue);
}

void mtk_hdl_gpioif_read_low_limit_val(
	void __iomem *base_addr, u32 *pvalue)
{
	*pvalue = osai_readl(base_addr + GPIO_CNT_GRP_LOW_LIMIT);

	gpioif_debug("reset val 0x%x\n", *pvalue);
}

void mtk_hdl_gpioif_read_high_limit_val(
	void __iomem *base_addr, u32 *pvalue)
{
	*pvalue = osai_readl(base_addr + GPIO_CNT_GRP_HIGH_LIMIT);

	gpioif_debug("reset val 0x%x\n", *pvalue);
}

void mtk_hdl_gpioif_read_gpio_cap_fifo0_value(
	void __iomem *base_addr, u32 *pvalue)
{
	*pvalue = osai_readl(base_addr + GPIO_CNT_GRP_CAP_VALUE_0);
}

void mtk_hdl_gpioif_read_gpio_cap_fifo1_value(
	void __iomem *base_addr, u32 *pvalue)
{
	*pvalue = osai_readl(base_addr + GPIO_CNT_GRP_CAP_VALUE_1);
}

void  mtk_hdl_gpioif_read_cap_fifo0_count(
	void __iomem *base_addr, u32 *pvalue)
{
	(*pvalue) = osai_readl(base_addr + GPIO_CNT_GRP_CAP_FIFO_STS)
		& GPIO_CAP_FIFO_0_CNT_MASK;
	gpioif_debug("cap fifo0 counter 0x%x\n", *pvalue);
}

void  mtk_hdl_gpioif_read_cap_fifo1_count(
	void __iomem *base_addr, u32 *pvalue)
{
	(*pvalue) = (osai_readl(base_addr + GPIO_CNT_GRP_CAP_FIFO_STS)
		& GPIO_CAP_FIFO_1_CNT_MASK) >> GPIO_CAP_FIFO_1_CNT_SHFT;
	gpioif_debug("cap fifo1 counter 0x%x\n", *pvalue);
}

void mtk_hdl_gpioif_de_glitch(void __iomem *base_addr,
	int gpio, int enable, u32 min_p, u32 init_v)
{
	u32 reg_val = 0;

	gpioif_debug("%s gpio %x enable %x min_p %u init_v %u\n",
		__func__, gpio, enable, min_p, init_v);
	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_STATIC_CTRL_0);
	reg_val &= (~GPIO_0_DGLT_INIT_VAL_MASK);
	reg_val |= (init_v << GPIO_0_DGLT_INIT_VAL_SHFT);
	reg_val &= (~GPIO_0_DGLT_MIN_PULSE_WID_MASK);
	reg_val |= (min_p << GPIO_0_DGLT_MIN_PULSE_WID_SHFT);
	reg_val &= (~GPIO_0_DGLT_EN_MASK);
	reg_val |= (enable << GPIO_0_DGLT_EN_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_STATIC_CTRL_0);
}

void mtk_hdl_gpioif_limit_comparator(void __iomem *base_addr,
	u32 sa_limit_v, u32 interrupt_limit_v)
{
	u32 reg_val = 0;

	gpioif_debug("%s sa_limit_v %x interrupt_limit_v %x\n",
		__func__, sa_limit_v, interrupt_limit_v);
	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
	reg_val &= (~GPIO_EVENT_CNT_SAT_MODE_MASK);
	reg_val |= (sa_limit_v << GPIO_EVENT_CNT_SAT_MODE_SHFT);
	reg_val &= (~GPIO_EVENT_CNT_INT_MODE_MASK);
	reg_val |= (interrupt_limit_v << GPIO_EVENT_CNT_INT_MODE_SHFT);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
}

void mtk_hdl_gpioif_global_reset(void __iomem *base_addr)
{
	u32 reg_val = 0;

	gpioif_debug("%s\n", __func__);
	reg_val = osai_readl(base_addr + GPIO_PWM_GRP_GLOBAL_CTRL);
	reg_val &= (~GPIO_PWM_GRP_GLOBAL_CTRL_CR_SW_RST_MASK);
	osai_writel(reg_val, base_addr + GPIO_PWM_GRP_GLOBAL_CTRL);
	reg_val |= GPIO_PWM_GRP_GLOBAL_CTRL_CR_SW_RST_MASK;
	osai_writel(reg_val, base_addr + GPIO_PWM_GRP_GLOBAL_CTRL);
}

void mtk_hdl_gpioif_counter_clock_setting(
	void __iomem *base_addr, int enable)
{
	u32 reg_val = 0;

	gpioif_debug("%s enable %x\n", __func__, enable);

	reg_val = osai_readl(base_addr + GPIO_PWM_GRP_GLOBAL_CTRL);
	if (enable) {
		reg_val |= GPIO_PWM_GRP_GLOBAL_CTRL_CNT_MODE_EN_MASK;
		reg_val |= GPIO_PWM_GRP_GLOBAL_CTRL_CNT_CLK_EN_MASK;
	} else {
		reg_val &= ~GPIO_PWM_GRP_GLOBAL_CTRL_CNT_MODE_EN_MASK;
		reg_val &= ~GPIO_PWM_GRP_GLOBAL_CTRL_CNT_CLK_EN_MASK;
	}
	osai_writel(reg_val, base_addr + GPIO_PWM_GRP_GLOBAL_CTRL);

	/* GPIO_2 reset level is 1'b1 */
	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
	reg_val |= GPIO_CNT_GPIO2_RST_LVL_MASK;
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_STATIC_CTRL_3);
}

void mtk_hdl_gpioif_clear_int_sts_by_bit(
	void __iomem *base_addr, u32 int_bit, u32 shift, u32 mask)
{
	u32 reg_val = 0;

	reg_val = osai_readl(base_addr + GPIO_CNT_GRP_INT_STS_CLR);
	reg_val |= (0X1 << shift);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_INT_STS_CLR);
	reg_val &= (~mask);
	osai_writel(reg_val, base_addr + GPIO_CNT_GRP_INT_STS_CLR);

	int_bit &= (~mask);
	osai_writel(int_bit, base_addr + GPIO_CNT_GRP_INT_EN);
}

void mtk_hdl_gpioif_read_int_val(
	void __iomem *base_addr, u32 *pvalue)
{
	*pvalue = osai_readl(base_addr + GPIO_CNT_GRP_INT_STS);
}
