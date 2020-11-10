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

#include "hdl_adc.h"

void mtk_hdl_adc_start(void __iomem *base)
{
	adc_debug("\tmtk hdl_adc_start.\n");

	/* Enable ADC */
	adc_mod_reg(base, ADC_FSM_EN, ADC_FSM_ENABLE);
}

void mtk_hdl_adc_stop(void __iomem *base)
{
	/* Disable ADC */
	adc_mod_reg(base, ADC_FSM_EN, ADC_FSM_DISABLE);
}

void mtk_hal_adc_init(void __iomem *base)
{
	/* Restore as default values */
	adc_mod_reg(base, ADC_REG_CH_MAP, CH_BIT_MAP);
	adc_mod_reg(base, ADC_REG_AUXADC_COMP_DELAY_TIME, ADC_DELAY_6NS);
	adc_mod_reg(base, ADC_REG_AUXADC_COMP_PREAMP_CURRENT, ADC_CURRENT_80UA);
	adc_mod_reg(base, ADC_REG_AUXADC_COMP_PREAMP_EN,
		ADC_COMP_PREAMP_ENABLE);

	adc_mod_reg(base, ADC_REG_AUXADC_DITHERING_EN, ADC_DITHERING_ENABLE);
	adc_mod_reg(base, ADC_REG_AUXADC_DITHERING_STEPS, ADC_DITHER_8STEPS);
	adc_mod_reg(base, ADC_REG_AUXADC_INVERT_PMU_CLK,
		ADC_INVERT_PMU_CLK_DISABLE);
	adc_mod_reg(base, ADC_REG_AUXADC_ADC_CLK_SRC, ADC_CLK_SRC_BUCK);
	adc_mod_reg(base, ADC_REG_AUXADC_ADC_VCM_VREF, ADC_VREF25_SUPPORT);
}

void mtk_hal_adc_uninit(void __iomem *base)
{
	adc_mod_reg(base, ADC_FSM_EN, 0);
}

void mtk_hdl_adc_enable_clk(void __iomem *cg_base)
{
	u32 reg_val;

	reg_val = osai_readl(cg_base);
	reg_val |= 0x80000000;
	osai_writel(reg_val, cg_base);
}

void mtk_hdl_adc_disable_clk(void __iomem *cg_base)
{
	u32 reg_val;

	reg_val = osai_readl(cg_base);
	reg_val &= ~0x80000000;
	osai_writel(reg_val, cg_base);
}

void mtk_hdl_adc_fsm_param_set(void __iomem *base,
	u16 pmode, u16 avg_mode, u16 channel_map, u32 period)
{

	/* Set ADC parameters */
	adc_mod_reg(base, ADC_PMODE_EN, pmode);
	adc_mod_reg(base, ADC_REG_AVG_MODE, avg_mode);
	adc_mod_reg(base, ADC_REG_T_INIT, ADC_INIT_TIME);
	adc_mod_reg(base, ADC_REG_T_CH, ADC_CH_STABLE_TIME);
	adc_mod_reg(base, ADC_REG_CH_MAP, channel_map);
	osai_writel(period, base + ADC_REG_PERIOD_ADDR);
	adc_mod_reg(base, ADC_REG_ADC_DATA_SYNC_MODE,
		ADC_DATA_SYNC_POSITIVE_MODE);
	/*Positive edge trigger*/
	adc_mod_reg(base, ADC_REG_ADC_TIMESTAMP_EN,
		ADC_TIME_STAMP_DISABLE);
	/*Timestamp enable*/

	adc_debug("\t[ADC] Fsm param setting OK!\n");
}

void mtk_hdl_adc_fsm_param_get(void __iomem *base,
	u16 *pmode, u16 *avg_mode, u16 *channel_map, u32 *period)
{
	/* Set ADC parameters */
	*pmode =	adc_get_reg(base, ADC_PMODE_EN);
	*avg_mode =	adc_get_reg(base, ADC_REG_AVG_MODE);
	*channel_map = adc_get_reg(base, ADC_REG_CH_MAP);
	*period = adc_get_reg(base, ADC_REG_PERIOD);

	adc_debug("\t[ADC] Fsm param setting OK!\n");
}

void mtk_hdl_adc_reset(void __iomem *base)
{
	u32 value = 0;

	adc_debug("hal_adc_reset.\n");
	value = osai_readl(base);
	value = value & (~ADC_CR_SW_RST_MASK);
	osai_writel(value, base);
	value |= ADC_CR_SW_RST_MASK;
	osai_writel(value, base);
}

void mtk_hdl_adc_dma_enable(void __iomem *base)
{
	adc_mod_reg(base, ADC_RX_DMA_EN, ADC_RX_DMA_ENABLE);
	adc_mod_reg(base, ADC_RXFEN, ADC_RX_FULL_DISABLE);
	adc_mod_reg(base, ADC_RXTIMEFEN, ADC_RX_TIME_OUT_DISABLE);
}

void mtk_hdl_adc_fifo_rx_full_enable(void __iomem *base)
{
	adc_mod_reg(base, ADC_RX_DMA_EN, ADC_RX_DMA_DISABLE);
	adc_mod_reg(base, ADC_RXFEN, ADC_RX_FULL_ENABLE);
}

void mtk_hdl_adc_trigger_level_set(void __iomem *base,
		u32 trigger_level)
{
	adc_mod_reg(base, ADC_RX_TRI_LVL, trigger_level);
}

void mtk_hdl_adc_periodic_mode_set(void __iomem *base,
		u32 periodic_mode)
{
	adc_mod_reg(base, ADC_PMODE_EN, periodic_mode);
}

void mtk_hdl_adc_fifo_get_length(void __iomem *base,
		u32 *length)
{
	u32 read_ptr = 0, write_ptr = 0;

	/* Get the length from read/write pointer */
	read_ptr = adc_get_reg(base, ADC_RX_PTR_READ);
	write_ptr = adc_get_reg(base, ADC_RX_PTR_WRITE);

	if (write_ptr == read_ptr)
		*length = 0;
	else if (write_ptr > read_ptr)
		*length = (write_ptr - read_ptr);
	else
		*length = (write_ptr + ADC_FIFO_SIZE - read_ptr);
		/* write_ptr < read_ptr */
}

int mtk_hdl_adc_fifo_read_data(void __iomem *base)
{
	u32 samples_data = 0;

	samples_data = (u32)adc_get_reg(base, ADC_RBR);

	return samples_data;
}

