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

#ifndef __HDL_GPIOIF__H__
#define __HDL_GPIOIF__H__

#include "mhal_osai.h"

/* debug help MACRO */
#define MTK_GPIOIF_DEBUG_ENABLE 0

#define gpioif_err(fmt, arg...)	osai_print(fmt, ##arg)
#if MTK_GPIOIF_DEBUG_ENABLE
#define gpioif_debug(fmt, arg...)	osai_print(fmt, ##arg)
#else
#define gpioif_debug(fmt, arg...)
#endif

/** This defines control edge type number of GPIOIF Event Counter
  *  to identify GPIOIF Direction Mode(1~3) and GPIOIF Up Down mode(4~6)
  */
#define DIR_UP_DOWN_CTRL_SETTING		3

#define GPIO_Counter_Clock_Switch_ADDR		(0x210)
#define GPIO_PAD_Control_OFFSET_ADDR		(0x10000)

/* Legacy Input/Output GPIO 00~90 */
#define GPIO_PWM_GRP_GLOBAL_CTRL		(0x0000)
#define GPIO_PWM_GRP_DIN				(0x0004)
#define GPIO_PWM_GRP_DOUT			(0x0010)
#define GPIO_PWM_GRP_DOUT_SET		(0x0014)
#define GPIO_PWM_GRP_DOUT_RESET		(0x0018)
#define GPIO_PWM_GRP_OE				(0x0020)
#define GPIO_PWM_GRP_OE_SET			(0x0024)
#define GPIO_PWM_GRP_OE_RESET		(0x0028)
#define GPIO_PWM_GRP_PU				(0x0030)
#define GPIO_PWM_GRP_PU_SET			(0x0034)
#define GPIO_PWM_GRP_PU_RESET		(0x0038)
#define GPIO_PWM_GRP_PD				(0x0040)
#define GPIO_PWM_GRP_PD_SET			(0x0044)
#define GPIO_PWM_GRP_PD_RESET		(0x0048)
#define GPIO_PWM_GRP_SR				(0x0050)
#define GPIO_PWM_GRP_SR_SET			(0x0054)
#define GPIO_PWM_GRP_SR_RESET		(0x0058)
#define GPIO_PWM_GRP_IES				(0X0060)
#define GPIO_PWM_GRP_IES_SET			(0x0064)
#define GPIO_PWM_GRP_IES_RESET		(0x0068)
#define GPIO_PWM_GRP_PADDRV			(0x0070)
#define GPIO_PWM_GRP_RDSEL			(0x0080)
#define GPIO_PWM_GRP_TDSEL			(0x0090)
/* GPIO Counter 100~128 */
#define GPIO_CNT_GRP_STATIC_CTRL_0	(0x0100)
#define GPIO_CNT_GRP_STATIC_CTRL_1	(0x0104)
#define GPIO_CNT_GRP_STATIC_CTRL_2	(0x0108)
#define GPIO_CNT_GRP_STATIC_CTRL_3	(0x010C)
#define GPIO_CNT_GRP_DYNAMIC_CTRL_0	(0x0110)
#define GPIO_CNT_GRP_RST_VAL			(0x0120)
#define GPIO_CNT_GRP_LOW_LIMIT		(0x0124)
#define GPIO_CNT_GRP_HIGH_LIMIT		(0x0128)
/* Interrupt Control 130~138 */
#define GPIO_CNT_GRP_INT_EN			(0x0130)
#define GPIO_CNT_GRP_INT_STS_CLR		(0x0134)
#define GPIO_CNT_GRP_INT_STS			(0x0138)
/* Capture FIFO Read 140~154 */
#define GPIO_CNT_GRP_CAP_FIFO_STS		(0x0140)
#define GPIO_CNT_GRP_CAP_VALUE_0		(0x0144)
#define GPIO_CNT_GRP_CAP_VALUE_1		(0x0148)
#define GPIO_CNT_GRP_COUNTER_VALUE	(0x014c)
#define GPIO_CNT_GRP_DBG_CTRL			(0x0150)
#define GPIO_CNT_GRP_DBG_OUT			(0x0154)

#define GPIO_Counter_Clock_Switch_MASK		BITS(0, 1)
#define GPIO_Counter_Clock_Switch_SHFT		0

#define GPIO_PWM_GRP_GLOBAL_CTRL_CNT_MODE_EN_MASK	BIT(0)
#define GPIO_PWM_GRP_GLOBAL_CTRL_CNT_MODE_EN_SHFT	0

#define GPIO_PWM_GRP_GLOBAL_CTRL_CR_SW_RST_MASK		BIT(1)
#define GPIO_PWM_GRP_GLOBAL_CTRL_CR_SW_RST_SHFT		1

#define GPIO_PWM_GRP_GLOBAL_CTRL_CNT_CLK_EN_MASK		BIT(3)
#define GPIO_PWM_GRP_GLOBAL_CTRL_CNT_CLK_EN_SHFT		0

#define GPIO_0_DGLT_MIN_PULSE_WID_MASK		BITS(0, 13)
#define GPIO_0_DGLT_MIN_PULSE_WID_SHFT		0
#define GPIO_0_DGLT_EN_MASK					BIT(14)
#define GPIO_0_DGLT_EN_SHFT					14
#define GPIO_0_DGLT_INIT_VAL_MASK				BIT(15)
#define GPIO_0_DGLT_INIT_VAL_SHFT				15
#define GPIO_CNT_STATIC_CTRL0_RSV_B31_B1_MASK	BITS(26, 27)
#define GPIO_CNT_STATIC_CTRL0_RSV_B31_B1_SHFT	26

#define GPIO_CNT_GPIO2_RST_LVL_MASK		BIT(0)
#define GPIO_CNT_GPIO2_RST_LVL_SHFT		0
#define GPIO_CAP_CNT_GPIO2_RST_EN_MASK	BIT(1)
#define GPIO_CAP_CNT_GPIO2_RST_EN_SHFT	1
#define GPIO_EVENT_CNT_GPIO2_RST_EN_MASK	BIT(2)
#define GPIO_EVENT_CNT_GPIO2_RST_EN_SHFT	2
#define GPIO_CNT_STATIC_CTRL3_RSV_B7_B3_MASK	BITS(3, 7)
#define GPIO_CNT_STATIC_CTRL3_RSV_B7_B3_SHFT	3
#define GPIO_CAP_CNT_GPIO0_EDGE_TYPE_MASK	BITS(8, 9)
#define GPIO_CAP_CNT_GPIO0_EDGE_TYPE_SHFT	8
#define GPIO_CAP_CNT_GPIO1_EDGE_TYPE_MASK	BITS(10, 11)
#define GPIO_CAP_CNT_GPIO1_EDGE_TYPE_SHFT	10
#define GPIO_CNT_STATIC_CTRL3_RSV_B15_B1_MASK	BITS(12, 15)
#define GPIO_CNT_STATIC_CTRL3_RSV_B15_B1_SHFT	12
#define GPIO_EVENT_CNT_EDGE_TYPE_MASK	BITS(16, 17)
#define GPIO_EVENT_CNT_EDGE_TYPE_SHFT	16
#define GPIO_EVENT_CNT_MODE_MASK			BITS(18, 19)
#define GPIO_EVENT_CNT_MODE_SHFT			18
#define GPIO_EVENT_CNT_DIR_DEF_MASK		BIT(20)
#define GPIO_EVENT_CNT_DIR_DEF_SHFT		20
#define GPIO_EVENT_CNT_UPDOWN_DEF_MASK		BIT(21)
#define GPIO_EVENT_CNT_UPDOWN_DEF_SHFT		21
#define GPIO_EVENT_CNT_QUAD_DEF_MASK		BIT(22)
#define GPIO_EVENT_CNT_QUAD_DEF_SHFT			22
#define GPIO_CNT_STATIC_CTRL3_RSV_B23_MASK	BIT(23)
#define GPIO_CNT_STATIC_CTRL3_RSV_B23_SHFT	23
#define GPIO_EVENT_CNT_INT_MODE_MASK	BITS(24, 25)
#define GPIO_EVENT_CNT_INT_MODE_SHFT		24
#define GPIO_CNT_STATIC_CTRL3_RSV_B27_B2_MASK	BITS(26, 27)
#define GPIO_CNT_STATIC_CTRL3_RSV_B27_B2_SHFT	26
#define GPIO_EVENT_CNT_SAT_MODE_MASK		BITS(28, 30)
#define GPIO_EVENT_CNT_SAT_MODE_SHFT		28
#define GPIO_CNT_STATIC_CTRL3_RSV_B31_MASK	BIT(31)
#define GPIO_CNT_STATIC_CTRL3_RSV_B31_SHFT	31

#define GPIO_EVENT_CNT_EN_MASK	BIT(0)
#define GPIO_EVENT_CNT_EN_SHFT	0
#define GPIO_CAP_CNT_EN_MASK		BIT(1)
#define GPIO_CAP_CNT_EN_SHFT		1


#define GPIO_EVENT_CNT_SW_RST_MASK	BIT(16)
#define GPIO_EVENT_CNT_SW_RST_SHFT	16
#define GPIO_CAP_CNT_SW_RST_MASK	BIT(17)
#define GPIO_CAP_CNT_SW_RST_SHFT		17

#define GPIO_EVENT_CNT_RST_VAL_MASK	BITS(0, 31)
#define GPIO_EVENT_CNT_RST_VAL_SHFT	0

#define GPIO_EVENT_CNT_LOW_LIMIT_MASK	BITS(0, 31)
#define GPIO_EVENT_CNT_LOW_LIMIT_SHFT	0

#define GPIO_EVENT_CNT_HIGH_LIMIT_MASK	BITS(0, 31)
#define GPIO_EVENT_CNT_HIGH_LIMIT_SHFT	0

#define GPIO_CNT_INT_EN_MASK	BITS(0, 31)
#define GPIO_CNT_INT_EN_SHFT	0

#define GPIO_CNT_INT_STS_CLR_MASK	BITS(0, 31)
#define GPIO_CNT_INT_STS_CLR_SHFT		0

#define GPIO_CNT_INT_STS_MASK		BITS(0, 31)
#define GPIO_CNT_INT_STS_SHFT		0
#define GPIO_CNT_INT_STS_EVENT_LOW_MASK		BIT(0)
#define GPIO_CNT_INT_STS_EVENT_LOW_SHFT		0
#define GPIO_CNT_INT_STS_EVENT_HIGH_MASK	BIT(1)
#define GPIO_CNT_INT_STS_EVENT_HIGH_SHFT		1
#define GPIO_CNT_INT_STS_GPIO2_RST_DONE_MASK	BIT(2)
#define GPIO_CNT_INT_STS_GPIO2_RST_DONE_SHFT	2
#define GPIO_CNT_INT_STS_EVENT_OVER_MASK	BIT(8)
#define GPIO_CNT_INT_STS_EVENT_OVER_SHFT	8
#define GPIO_CNT_INT_STS_EVENT_UF_MASK		BIT(9)
#define GPIO_CNT_INT_STS_EVENT_UF_SHFT		9
#define GPIO_CNT_INT_STS_CAP_F0_FULL_MASK	BIT(12)
#define GPIO_CNT_INT_STS_CAP_F0_FULL_SHFT	12
#define GPIO_CNT_INT_STS_CAP_F1_FULL_MASK	BIT(13)
#define GPIO_CNT_INT_STS_CAP_F1_FULL_SHFT	13
#define GPIO_CNT_INT_STS_RESET_CAP_F0_FULL_MASK		BIT(14)
#define GPIO_CNT_INT_STS_RESET_CAP_F0_FULL_SHFT		14
#define GPIO_CNT_INT_STS_RESET_CAP_F1_FULL_MASK		BIT(15)
#define GPIO_CNT_INT_STS_RESET_CAP_F1_FULL_SHFT		15

#define GPIO_CNT_INT_STS_CAP_F0_NP_MASK		BIT(16)
#define GPIO_CNT_INT_STS_CAP_F0_NP_SHFT		16
#define GPIO_CNT_INT_STS_CAP_F1_NP_MASK		BIT(17)
#define GPIO_CNT_INT_STS_CAP_F1_NP_SHFT		17
#define GPIO_CNT_INT_STS_CAP_F0_P_MASK		BIT(24)
#define GPIO_CNT_INT_STS_CAP_F0_P_SHFT		24
#define GPIO_CNT_INT_STS_CAP_F1_P_MASK		BIT(25)
#define GPIO_CNT_INT_STS_CAP_F1_P_SHFT		25

#define GPIO_CAP_FIFO_0_CNT_MASK		BITS(0, 3)
#define GPIO_CAP_FIFO_0_CNT_SHFT		0
#define GPIO_CAP_FIFO_1_CNT_MASK		BITS(4, 7)
#define GPIO_CAP_FIFO_1_CNT_SHFT		4

#define GPIO_CAP_VALUE_0_MASK		BITS(0, 31)
#define GPIO_CAP_VALUE_0_SHFT		0

#define GPIO_CAP_VALUE_1_MASK		BITS(0, 31)
#define GPIO_CAP_VALUE_1_SHFT		0

#ifdef __cplusplus
extern "C" {
#endif

void mtk_hdl_gpioif_select_clock_source(
	void __iomem *reg_clk_base, u32 clk);
void mtk_hdl_gpioif_event_counter_setting(void __iomem *base_addr,
	u32 mode, u8 control_setting, u8 updown,
	u32 reset_value, u32 low_limit, u32 high_limit);
void mtk_hdl_gpioif_capture_counter_setting(void __iomem *base_addr,
	u32 edge_type_gpio_0, u32 edge_type_gpio_1);
void mtk_hdl_gpioif_enable_event_counter(void __iomem *base_addr);
void mtk_mhdl_gpioif_disable_event_counter(void __iomem *base_addr);
void mtk_hdl_gpioif_enable_capture_counter(void __iomem *base_addr);
void mtk_hdl_gpioif_disable_capture_counter(void __iomem *base_addr);
void mtk_hdl_gpioif_software_reset(void __iomem *base_addr, u32 mode);
void mtk_hdl_gpioif_interrupt_bit_wise(void __iomem *base_addr,
	u32 bit, u8 enable);
void mtk_hdl_gpioif_clear_interrupt_status(void __iomem *base_addr, u32 bit);
void mtk_hdl_gpioif_hardware_reset_by_gpio_2(
	void __iomem *base_addr, u32 mode, int active_reset);
void mtk_hdl_gpioif_read_gpio_event_count(void __iomem *base_addr, u32 *pvalue);
void mtk_hdl_gpioif_read_gpio_cap_fifo0_value(
	void __iomem *base_addr, u32 *pvalue);
void mtk_hdl_gpioif_read_gpio_cap_fifo1_value(
	void __iomem *base_addr, u32 *pvalue);
void mtk_hdl_gpioif_de_glitch(void __iomem *base_addr,
	int gpio, int enable, u32 min_p, u32 init_v);
void mtk_hdl_gpioif_limit_comparator(void __iomem *base_addr,
	u32 sa_limit_v, u32 interrupt_limit_v);
void mtk_hdl_gpioif_global_reset(void __iomem *base_addr);
void mtk_hdl_gpioif_counter_clock_setting(
	void __iomem *base_addr, int enable);
void  mtk_hdl_gpioif_read_cap_fifo0_count(
	void __iomem *base_addr, u32 *pvalue);
void  mtk_hdl_gpioif_read_cap_fifo1_count(
	void __iomem *base_addr, u32 *pvalue);
void mtk_hdl_gpioif_read_reset_val(void __iomem *base_addr, u32 *pvalue);
void mtk_hdl_gpioif_read_low_limit_val(
	void __iomem *base_addr, u32 *pvalue);
void mtk_hdl_gpioif_read_high_limit_val(
	void __iomem *base_addr, u32 *pvalue);
void mtk_hdl_gpioif_clear_int_sts_by_bit(
	void __iomem *base_addr, u32 int_bit, u32 shift, u32 mask);
void	mtk_hdl_gpioif_read_int_val(void __iomem *base_addr, u32 *pvalue);

#ifdef __cplusplus
}
#endif

#endif
