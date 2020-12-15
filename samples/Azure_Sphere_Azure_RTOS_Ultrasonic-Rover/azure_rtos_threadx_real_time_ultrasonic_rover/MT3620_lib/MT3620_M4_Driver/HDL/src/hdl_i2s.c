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

#include "hdl_i2s.h"

void mtk_hdl_i2s_gbl_cfg(void __iomem *base_address,
			 struct i2s_gbl_cfg *i2s_gbl_cfg)
{
	u32 reg = 0;

	if (i2s_gbl_cfg->i2s_ext_codec_sel == I2S_EXT_EN)
		reg |= BIT(I2S_EXT_SHFT);
	if (i2s_gbl_cfg->i2s_dl_swap_lr == I2S_DL_LRSW_EN)
		reg |= BIT(I2S_DL_LRSW_SHFT);
	if (i2s_gbl_cfg->i2s_dl_mono_stereo_sel == I2S_DL_MONO)
		reg |= BIT(I2S_DL_MONO_SHFT);
	if (i2s_gbl_cfg->i2s_dl_monodup_en == I2S_DL_MONO_DUP_EN)
		reg |= BIT(I2S_DL_MONO_DUP_SHFT);
	if (i2s_gbl_cfg->i2s_mclk_en == I2S_MCLK_OUT_EN)
		reg |= BIT(I2S_MCLK_OUT_SHFT);
	if (i2s_gbl_cfg->i2s_in_clk_en == I2S_IN_CLK_EN)
		reg |= BIT(I2S_IN_CLK_SHFT);
	if (i2s_gbl_cfg->i2s_out_clk_en == I2S_OUT_CLK_EN)
		reg |= BIT(I2S_OUT_CLK_SHFT);
	if (i2s_gbl_cfg->i2s_26m_sel == I2S_26M_SEL_XPLL)
		reg |= BIT(I2S_26M_SEL_SHFT);
	if (i2s_gbl_cfg->i2s_ul_empty_val_en == I2S_UL_EMPTY_VALUE_EN)
		reg |= BIT(I2S_UL_EMPTY_VALUE_SHFT);
	if (i2s_gbl_cfg->i2s_dl_empty_val_en == I2S_DL_EMPTY_VALUE_EN)
		reg |= BIT(I2S_DL_EMPTY_VALUE_SHFT);
	if (i2s_gbl_cfg->i2s_clk_inv_en == I2S_CK_INV_EN)
		reg |= BIT(I2S_CK_INV_SHFT);
	if (i2s_gbl_cfg->i2s_negcap_en == I2S_NEG_CAP_EN)
		reg |= BIT(I2S_NEG_CAP_SHFT);

	reg |= (i2s_gbl_cfg->i2s_mclk_sel << I2S_MCLK_SEL_SHFT)
		    & I2S_MCLK_SEL_MASK;
	reg |= (i2s_gbl_cfg->i2s_in_clk_sel << I2S_IN_CLK_SEL_SHFT)
		    & I2S_IN_CLK_SEL_MASK;
	reg |= (i2s_gbl_cfg->i2s_out_clk_sel << I2S_OUT_CLK_SEL_SHFT)
		    & I2S_OUT_CLK_SEL_MASK;

	if (i2s_gbl_cfg->i2s_loopback_en  == I2S_LOOPBACK_EN)
		reg |= BIT(I2S_LOOPBACK_SHFT);

	osai_writel(reg, I2S_GLB_CONTROL_ADDR(base_address));
}

void mtk_hdl_i2s_dl_cfg(void __iomem *base_address,
			struct i2s_dl_cfg *i2s_dl_cfg)
{
	u32 reg = 0;

	if (i2s_dl_cfg->i2s_src == I2S_DL_SRC_SLAVE)
		reg |= BIT(I2S_DL_SRC_SHFT);
	if (i2s_dl_cfg->i2s_format == I2S_DL_FMT_I2S)
		reg |= BIT(I2S_DL_FMT_SHFT);
	if (i2s_dl_cfg->i2s_word_sel_inv == I2S_DL_WSINV_EN)
		reg |= BIT(I2S_DL_WSINV_SHFT);
	if (i2s_dl_cfg->i2s_dl_fifo2deq == I2S_DL_FIFO_2DEQ_EN)
		reg |= BIT(I2S_DL_FIFO_2DEQ_SHFT);

	reg |= (i2s_dl_cfg->i2s_sample_rate << I2S_DL_SR_SHFT)
		    & I2S_DL_SR_MASK;
	reg |= (i2s_dl_cfg->i2s_bit_per_sample << I2S_DL_BIT_PER_S_SHFT)
		    & I2S_DL_BIT_PER_S_MASK;
	reg |= (i2s_dl_cfg->i2s_msbo_ffset << I2S_DL_MSB_OFFSET_SHFT)
		    & I2S_DL_MSB_OFFSET_MASK;
	reg |= (i2s_dl_cfg->i2s_ch_per_sample << I2S_DL_CH_PER_S_SHFT)
		    & I2S_DL_CH_PER_S_MASK;
	reg |= BIT(I2S_DL_WS_RESYNC_SHFT);

	osai_writel(reg, I2S_DL_CONTROL_ADDR(base_address));
}


void mtk_hdl_i2s_ul_cfg(void __iomem *base_address,
			struct i2s_ul_cfg *i2s_ul_cfg)
{
	u32 reg = 0;

	if (i2s_ul_cfg->i2s_src == I2S_UL_SRC_SLAVE)
		reg |= BIT(I2S_UL_SRC_SHFT);
	if (i2s_ul_cfg->i2s_format == I2S_UL_FMT_I2S)
		reg |= BIT(I2S_UL_FMT_SHFT);
	if (i2s_ul_cfg->i2s_word_sel_inv == I2S_UL_WSINV_EN)
		reg |= BIT(I2S_UL_WSINV_SHFT);
	reg |= (i2s_ul_cfg->i2s_sample_rate << I2S_UL_SR_SHFT)
		    & I2S_UL_SR_MASK;
	reg |= (i2s_ul_cfg->i2s_bit_per_sample << I2S_UL_BIT_PER_S_SHFT)
		    & I2S_UL_BIT_PER_S_MASK;
	if (i2s_ul_cfg->i2s_down_rate_en == I2S_UL_DOWN_RATE_EN)
		reg |= BIT(I2S_UL_DOWN_RATE_SHFT);
	if (i2s_ul_cfg->i2s_lr_swap == I2S_UL_LR_SWAP_EN)
		reg |= BIT(I2S_UL_LR_SWAP_SHFT);
	reg |= (i2s_ul_cfg->i2s_msbo_ffset << I2S_UL_MSB_OFFSET_SHFT)
		    & I2S_UL_MSB_OFFSET_MASK;
	reg |= (i2s_ul_cfg->i2s_update_word << I2S_UL_UPDATE_WORD_SHFT)
		    & I2S_UL_UPDATE_WORD_MASK;
	reg |= (i2s_ul_cfg->i2s_ch_per_sample << I2S_UL_CH_PER_S_SHFT)
		    & I2S_UL_CH_PER_S_MASK;
	reg |= BIT(I2S_UL_WS_RESYNC_SHFT);

	osai_writel(reg, I2S_UL_CONTROL_ADDR(base_address));
}
void mtk_hdl_i2s_clk_gating(void __iomem *base_address, u8 en)
{
	u32 reg = 0;

	reg = osai_readl(I2S_GLB_CONTROL_ADDR(base_address));
	if (en == I2S_EN) {
		reg |= BIT(I2S_MCLK_OUT_SHFT);
		reg |= BIT(I2S_IN_CLK_SHFT);
		reg |= BIT(I2S_OUT_CLK_SHFT);
	} else {
		reg &= ~(I2S_MCLK_OUT_MASK);
		reg &= ~(I2S_IN_CLK_MASK);
		reg &= ~(I2S_OUT_CLK_MASK);
	}
	osai_writel(reg, I2S_GLB_CONTROL_ADDR(base_address));
}

void mtk_hdl_i2s_clk_fifo_en(void __iomem *base_address, u8 en)
{
	u32 reg = 0;

	reg = osai_readl(I2S_GLB_CONTROL_ADDR(base_address));
	if (en == I2S_EN)
		reg |= BIT(I2S_EN_SHFT);
	else {
		reg &= ~(I2S_EN_MASK);
		reg &= ~(I2S_ULFIFO_EN_MASK);
		reg &= ~(I2S_DLFIFO_EN_MASK);
		reg &= ~(I2S_ENGEN_EN_MASK);
	}
	osai_writel(reg, I2S_GLB_CONTROL_ADDR(base_address));
}

void mtk_hdl_i2s_dl_en(void __iomem *base_address, u8 en)
{
	u32 reg = 0;

	reg = osai_readl(I2S_DL_CONTROL_ADDR(base_address));
	if (en == I2S_EN)
		reg |= BIT(I2S_DL_EN_SHFT);
	else
		reg &= ~(I2S_DL_EN_MASK);
	osai_writel(reg, I2S_DL_CONTROL_ADDR(base_address));
}


void mtk_hdl_i2s_ul_en(void __iomem *base_address, u8 en)
{
	u32 reg = 0;

	reg = osai_readl(I2S_UL_CONTROL_ADDR(base_address));
	if (en == I2S_EN)
		reg |= BIT(I2S_UL_EN_SHFT);
	else
		reg &= ~(I2S_UL_EN_MASK);
	osai_writel(reg, I2S_UL_CONTROL_ADDR(base_address));
}

void mtk_hdl_i2s_reset(void __iomem *base_address)
{
	u32 reg = 0;

	reg = osai_readl(I2S_SW_RESET_ADDR(base_address));
	reg |= (BIT(I2S_SW_RST_EN_SHFT) | BIT(I2S_GLB_SW_RST_EN_SHFT));
	osai_writel(reg, I2S_SW_RESET_ADDR(base_address));
	osai_delay_us(1);
	reg = osai_readl(I2S_SW_RESET_ADDR(base_address));
	reg &= ~(BIT(I2S_SW_RST_EN_SHFT) | BIT(I2S_GLB_SW_RST_EN_SHFT));
	osai_writel(reg, I2S_SW_RESET_ADDR(base_address));
	osai_delay_us(1);
}

void mtk_hdl_i2s_cfg_tdm_ch_bit_per_sample(enum_i2s_dl_ch_per_sample
			dl_ch_per_sample,
			enum_i2s_ul_ch_per_sample ul_ch_per_sample,
			enum_i2s_dl_bit_per_sample_rate dl_bit_per_sample,
			enum_i2s_ul_bit_per_sample_rate ul_bit_per_sample,
			struct i2s_dl_cfg *i2s_dl_cfg,
			struct i2s_ul_cfg *i2s_ul_cfg)
{
	i2s_dl_cfg->i2s_ch_per_sample = dl_ch_per_sample;
	i2s_ul_cfg->i2s_ch_per_sample = ul_ch_per_sample;
	i2s_dl_cfg->i2s_bit_per_sample = dl_bit_per_sample;
	i2s_ul_cfg->i2s_bit_per_sample = ul_bit_per_sample;
}

void mtk_hdl_i2s_cfg_mono_stereo(enum_i2s_dl_mono_stereo_mode dl_mono_stereo,
				u8 i2s_dl_monodup_en,
				struct i2s_gbl_cfg *i2s_gbl_cfg)
{

	if ((dl_mono_stereo == I2S_DL_STEREO_MODE)
	     && (i2s_dl_monodup_en == true))
		i2s_gbl_cfg->i2s_dl_monodup_en = I2S_DL_MONO_DUP_DIS;

	i2s_gbl_cfg->i2s_dl_mono_stereo_sel = dl_mono_stereo;
	i2s_gbl_cfg->i2s_dl_monodup_en = i2s_dl_monodup_en;
}

void mtk_hdl_i2s_cfg_sample_rate(enum_i2s_dl_sample_rate dl_sample_rate,
				 enum_i2s_ul_sample_rate ul_sample_rate,
				 u8  ul_down_rate_en,
				 struct i2s_dl_cfg *i2s_dl_cfg,
				 struct i2s_ul_cfg *i2s_ul_cfg)
{
	i2s_dl_cfg->i2s_sample_rate = dl_sample_rate;
	i2s_ul_cfg->i2s_sample_rate = ul_sample_rate;
	i2s_ul_cfg->i2s_down_rate_en = ul_down_rate_en;
}


void mtk_hdl_i2s_cfg_init_setting(enum_i2s_initial_type initial_type,
				  struct i2s_gbl_cfg *i2s_gbl_cfg,
				  struct i2s_dl_cfg *i2s_dl_cfg,
				  struct i2s_ul_cfg *i2s_ul_cfg)
{

	switch (initial_type) {
	case I2S_INTERNAL_LOOPBACK_MODE:/*just for test case use*/
		i2s_gbl_cfg->i2s_loopback_en = I2S_LOOPBACK_EN;
		i2s_gbl_cfg->i2s_mclk_sel = I2S_MCLK_SEL_XPLL_16M;
		i2s_gbl_cfg->i2s_in_clk_sel = I2S_CLK_SEL_13M;
		i2s_gbl_cfg->i2s_out_clk_sel = I2S_CLK_SEL_13M;
		i2s_gbl_cfg->i2s_ul_empty_val_en = I2S_UL_EMPTY_VALUE_DIS;
		i2s_gbl_cfg->i2s_dl_empty_val_en = I2S_DL_EMPTY_VALUE_DIS;
		i2s_gbl_cfg->i2s_clk_inv_en = I2S_CK_INV_DIS;
		i2s_gbl_cfg->i2s_negcap_en = I2S_NEG_CAP_DIS;
		i2s_gbl_cfg->i2s_out_clk_en = I2S_OUT_CLK_EN;
		i2s_gbl_cfg->i2s_in_clk_en	= I2S_IN_CLK_EN;
		i2s_gbl_cfg->i2s_mclk_en = I2S_MCLK_OUT_EN;
		i2s_gbl_cfg->i2s_26m_sel = I2S_26M_SEL_XTAL;
		i2s_gbl_cfg->i2s_dl_swap_lr = I2S_DL_LRSW_DIS;
		i2s_gbl_cfg->i2s_ext_codec_sel = I2S_EXT_DIS;
		i2s_dl_cfg->i2s_ch_per_sample = I2S_DL_CH_PER_S_2_CH;
		i2s_dl_cfg->i2s_msbo_ffset = 0;
		i2s_dl_cfg->i2s_bit_per_sample = I2S_DL_BIT_PER_S_32BTIS;
		i2s_dl_cfg->i2s_dl_fifo2deq = I2S_DL_FIFO_2DEQ_DIS;
		i2s_dl_cfg->i2s_word_sel_inv = I2S_DL_WSINV_DIS;
		i2s_dl_cfg->i2s_format = I2S_DL_FMT_I2S;
		i2s_dl_cfg->i2s_src = I2S_DL_SRC_MASTER;
		i2s_ul_cfg->i2s_lr_swap = I2S_UL_LR_SWAP_DIS;
		i2s_ul_cfg->i2s_ch_per_sample = I2S_UL_CH_PER_S_2_CH;
		i2s_ul_cfg->i2s_update_word = 8;
		i2s_ul_cfg->i2s_msbo_ffset = 0;
		i2s_ul_cfg->i2s_bit_per_sample = I2S_UL_BIT_PER_S_32BTIS;
		i2s_ul_cfg->i2s_word_sel_inv = I2S_UL_WSINV_DIS;
		i2s_ul_cfg->i2s_format = I2S_UL_FMT_I2S;
		i2s_ul_cfg->i2s_src = I2S_UL_SRC_MASTER;
	break;
	case I2S_EXTERNAL_MODE:
		i2s_gbl_cfg->i2s_loopback_en = I2S_LOOPBACK_DIS;
		i2s_gbl_cfg->i2s_mclk_sel = I2S_MCLK_SEL_XPLL_16M;
		i2s_gbl_cfg->i2s_in_clk_sel = I2S_CLK_SEL_EXT_BCLK;
		i2s_gbl_cfg->i2s_out_clk_sel = I2S_CLK_SEL_EXT_BCLK;
		i2s_gbl_cfg->i2s_ul_empty_val_en = I2S_UL_EMPTY_VALUE_DIS;
		i2s_gbl_cfg->i2s_dl_empty_val_en = I2S_DL_EMPTY_VALUE_DIS;
		i2s_gbl_cfg->i2s_clk_inv_en = I2S_CK_INV_EN;
		i2s_gbl_cfg->i2s_negcap_en = I2S_NEG_CAP_EN;
		i2s_gbl_cfg->i2s_out_clk_en = I2S_OUT_CLK_EN;
		i2s_gbl_cfg->i2s_in_clk_en	= I2S_IN_CLK_EN;
		i2s_gbl_cfg->i2s_mclk_en = I2S_MCLK_OUT_EN;
		i2s_gbl_cfg->i2s_26m_sel = I2S_26M_SEL_XTAL;
		i2s_gbl_cfg->i2s_dl_swap_lr = I2S_DL_LRSW_DIS;
		i2s_gbl_cfg->i2s_ext_codec_sel = I2S_EXT_EN;
		i2s_dl_cfg->i2s_ch_per_sample = I2S_DL_CH_PER_S_2_CH;
		i2s_dl_cfg->i2s_msbo_ffset = 0;
		i2s_dl_cfg->i2s_bit_per_sample = I2S_DL_BIT_PER_S_32BTIS;
		i2s_dl_cfg->i2s_dl_fifo2deq = I2S_DL_FIFO_2DEQ_DIS;
		i2s_dl_cfg->i2s_word_sel_inv = I2S_DL_WSINV_DIS;
		i2s_dl_cfg->i2s_format = I2S_DL_FMT_I2S;
		i2s_dl_cfg->i2s_src = I2S_DL_SRC_SLAVE;
		i2s_ul_cfg->i2s_lr_swap = I2S_UL_LR_SWAP_DIS;
		i2s_ul_cfg->i2s_ch_per_sample = I2S_UL_CH_PER_S_2_CH;
		i2s_ul_cfg->i2s_update_word = 8;
		i2s_ul_cfg->i2s_msbo_ffset = 0;
		i2s_ul_cfg->i2s_bit_per_sample = I2S_UL_BIT_PER_S_32BTIS;
		i2s_ul_cfg->i2s_word_sel_inv = I2S_UL_WSINV_DIS;
		i2s_ul_cfg->i2s_format = I2S_UL_FMT_I2S;
		i2s_ul_cfg->i2s_src = I2S_UL_SRC_SLAVE;
	break;
	case I2S_EXTERNAL_TDM_MODE:/* only for DA7212 codec*/
		i2s_gbl_cfg->i2s_loopback_en = I2S_LOOPBACK_DIS;
		i2s_gbl_cfg->i2s_mclk_sel = I2S_MCLK_SEL_XPLL_16M;
		i2s_gbl_cfg->i2s_in_clk_sel = I2S_CLK_SEL_EXT_BCLK;
		i2s_gbl_cfg->i2s_out_clk_sel = I2S_CLK_SEL_EXT_BCLK;
		i2s_gbl_cfg->i2s_ul_empty_val_en = I2S_UL_EMPTY_VALUE_DIS;
		i2s_gbl_cfg->i2s_dl_empty_val_en = I2S_DL_EMPTY_VALUE_DIS;
		i2s_gbl_cfg->i2s_negcap_en = I2S_NEG_CAP_EN;
		i2s_gbl_cfg->i2s_clk_inv_en = I2S_CK_INV_DIS;
		i2s_gbl_cfg->i2s_out_clk_en = I2S_OUT_CLK_EN;
		i2s_gbl_cfg->i2s_in_clk_en	= I2S_IN_CLK_EN;
		i2s_gbl_cfg->i2s_mclk_en = I2S_MCLK_OUT_EN;
		i2s_gbl_cfg->i2s_26m_sel = I2S_26M_SEL_XTAL;
		i2s_gbl_cfg->i2s_dl_swap_lr = I2S_DL_LRSW_DIS;
		i2s_gbl_cfg->i2s_ext_codec_sel = I2S_EXT_EN;
		i2s_dl_cfg->i2s_msbo_ffset = 0;
		i2s_dl_cfg->i2s_dl_fifo2deq = I2S_DL_FIFO_2DEQ_DIS;
		i2s_dl_cfg->i2s_word_sel_inv = I2S_DL_WSINV_EN;
		i2s_dl_cfg->i2s_format = I2S_DL_FMT_TDM;
		i2s_dl_cfg->i2s_src = I2S_DL_SRC_SLAVE;
		i2s_ul_cfg->i2s_lr_swap = I2S_UL_LR_SWAP_DIS;
		i2s_ul_cfg->i2s_update_word = 8;
		i2s_ul_cfg->i2s_msbo_ffset = 0;
		i2s_ul_cfg->i2s_word_sel_inv = I2S_UL_WSINV_EN;
		i2s_ul_cfg->i2s_format = I2S_UL_FMT_TDM;
		i2s_ul_cfg->i2s_src = I2S_UL_SRC_SLAVE;
	break;
	default:
	break;
	}
}
