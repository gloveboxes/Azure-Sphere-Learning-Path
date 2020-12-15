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
#include "mhal_i2s.h"

enum_i2s_dl_sample_rate _mtk_mhal_i2s_tx_sample_rate(hal_i2s_sample_rate
						     sample_rate);
enum_i2s_ul_sample_rate _mtk_mhal_i2s_rx_sample_rate(hal_i2s_sample_rate
						     sample_rate);

int mtk_mhal_i2s_cfg_type(struct mtk_i2s_ctlr *ctlr,
			  hal_i2s_initial_type i2s_initial_type)
{
	struct mtk_i2s_private *p_i2s_internal_cfg;

	if (!ctlr)
		return -I2S_EPTR;
	/*----set default value----*/
	p_i2s_internal_cfg = ctlr->mdata;
	memset(p_i2s_internal_cfg, 0, sizeof(struct mtk_i2s_private));

	if (i2s_initial_type <= MHAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE)
		p_i2s_internal_cfg->i2s_initial_type = i2s_initial_type;
	else {
		i2s_err("I2S config type initial fail\n");
		return -I2S_EPTR;
	}
	return 0;
}

int mtk_mhal_i2s_reset(struct mtk_i2s_ctlr *ctlr)
{
	if (!ctlr)
		return -I2S_EPTR;
	mtk_hdl_i2s_reset(ctlr->base);
	return 0;
}

int mtk_mhal_i2s_clk_en(struct mtk_i2s_ctlr *ctlr, i2s_fnen en)
{
	if (!ctlr)
		return -I2S_EPTR;
	mtk_hdl_i2s_clk_gating(ctlr->base, en);
	return 0;
}

int mtk_mhal_i2s_set_config(struct mtk_i2s_ctlr *ctlr,
			    struct hal_i2s_config *config)
{
	struct mtk_i2s_private *p_i2s_internal_cfg;
	struct i2s_gbl_cfg i2sgblcfg;
	struct i2s_dl_cfg i2sdlcfg;
	struct i2s_ul_cfg i2sulcfg;
	enum_i2s_dl_mono_stereo_mode i2s_dl_mono_stereo;
	enum_i2s_dl_sample_rate i2s_dl_sample_rate;
	enum_i2s_ul_sample_rate i2s_ul_sample_rate;
	enum_i2s_dl_bit_per_sample_rate i2s_dl_bit_per_sample_rate;
	enum_i2s_ul_bit_per_sample_rate i2s_ul_bit_per_sample_rate;
	enum_i2s_dl_ch_per_sample i2s_dl_ch_per_sample;
	enum_i2s_ul_ch_per_sample i2s_ul_ch_per_sample;
	enum_i2s_initial_type i2s_initial_type;
	u8 i2s_dl_mono_dup_en;
	u8 i2s_down_rate_en;

	if (!ctlr || !config)
		return -I2S_EPTR;
	p_i2s_internal_cfg = ctlr->mdata;

	switch (p_i2s_internal_cfg->i2s_initial_type) {
	case MHAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE:
		i2s_initial_type = I2S_INTERNAL_LOOPBACK_MODE;
	break;
	case MHAL_I2S_TYPE_EXTERNAL_MODE:
		i2s_initial_type = I2S_EXTERNAL_MODE;
	break;
	case MHAL_I2S_TYPE_EXTERNAL_TDM_MODE:
		i2s_initial_type = I2S_EXTERNAL_TDM_MODE;
	break;
	default:
		i2s_err("I2S config type initial fail\n");
		return -I2S_EPTR;
	}

	mtk_hdl_i2s_cfg_init_setting(i2s_initial_type,
				     &i2sgblcfg,
				     &i2sdlcfg,
				     &i2sulcfg);
	/*set tx sample rate*/
	if (config->i2s_out.sample_rate > MHAL_I2S_SAMPLE_RATE_48K)
		return -I2S_EPTR;
	i2s_dl_sample_rate =
		_mtk_mhal_i2s_tx_sample_rate(config->i2s_out.sample_rate);
	/*set rx sample rate*/
	if (config->i2s_in.sample_rate > MHAL_I2S_SAMPLE_RATE_48K)
		return -I2S_EPTR;
	i2s_ul_sample_rate =
		_mtk_mhal_i2s_rx_sample_rate(config->i2s_in.sample_rate);
	if (config->i2s_out.sample_rate != config->i2s_in.sample_rate) {
		i2s_err("tx/rx sample rate different\n");
		return -I2S_EPTR;
	}
	/*set tx channel number*/
	if (config->i2s_out.channel_number != config->i2s_in.channel_number) {
		i2s_err("in/out channel num different\n");
		return -I2S_EPTR;
	}
	switch (config->i2s_out.channel_number) {
	case MHAL_I2S_MONO:
		i2s_dl_mono_stereo = I2S_DL_MONO_MODE;
		p_i2s_internal_cfg->i2s_user_config.i2s_out.channel_number =
		MHAL_I2S_MONO;
	break;
	case MHAL_I2S_STEREO:
		i2s_dl_mono_stereo = I2S_DL_STEREO_MODE;
		p_i2s_internal_cfg->i2s_user_config.i2s_out.channel_number =
		MHAL_I2S_STEREO;
	break;
	default:
		i2s_err("set tx channel number fail\n");
		return -I2S_EPTR;
	}
	/*set rx down rate*/
	switch (config->rx_down_rate) {
	case MHAL_I2S_RX_DOWN_RATE_DISABLE:
		i2s_down_rate_en = I2S_UL_DOWN_RATE_DIS;
		p_i2s_internal_cfg->i2s_user_config.rx_down_rate =
		MHAL_I2S_RX_DOWN_RATE_DISABLE;
	break;
	case MHAL_I2S_RX_DOWN_RATE_ENABLE:
		i2s_down_rate_en = I2S_UL_DOWN_RATE_EN;
		p_i2s_internal_cfg->i2s_user_config.rx_down_rate =
		MHAL_I2S_RX_DOWN_RATE_ENABLE;
	break;
	default:
		i2s_err("set rx down rate fail\n");
		return -I2S_EPTR;
	}
	/*set tx mode*/
	switch (config->tx_mode) {
	case MHAL_I2S_TX_MONO_DUPLICATE_DISABLE:
		i2s_dl_mono_dup_en = I2S_DL_MONO_DUP_DIS;
		p_i2s_internal_cfg->i2s_user_config.tx_mode =
		MHAL_I2S_TX_MONO_DUPLICATE_DISABLE;
	break;
	case MHAL_I2S_TX_MONO_DUPLICATE_ENABLE:
		i2s_dl_mono_dup_en = I2S_DL_MONO_DUP_EN;
		p_i2s_internal_cfg->i2s_user_config.tx_mode =
		MHAL_I2S_TX_MONO_DUPLICATE_ENABLE;
	break;
	default:
		i2s_err("set tx mode fail\n");
		return -I2S_EPTR;
	}
	/*set TX LR swap*/
	switch (config->i2s_out.lr_swap) {
	case MHAL_FN_DIS:
		i2sgblcfg.i2s_dl_swap_lr = 0;
		p_i2s_internal_cfg->i2s_user_config.i2s_out.lr_swap =
		MHAL_FN_DIS;
	break;
	case MHAL_FN_EN:
		i2sgblcfg.i2s_dl_swap_lr = 1;
		p_i2s_internal_cfg->i2s_user_config.i2s_out.lr_swap =
		MHAL_FN_EN;
	break;
	default:
		i2s_err("set TX LR swap fail\n");
		return -I2S_EPTR;
	}
	/*set RX LR swap*/
	switch (config->i2s_in.lr_swap) {
	case MHAL_FN_DIS:
		i2sulcfg.i2s_lr_swap = 0;
		p_i2s_internal_cfg->i2s_user_config.i2s_in.lr_swap =
		MHAL_FN_DIS;
	break;
	case MHAL_FN_EN:
		i2sulcfg.i2s_lr_swap = 1;
		p_i2s_internal_cfg->i2s_user_config.i2s_in.lr_swap =
		MHAL_FN_EN;
	break;
	default:
		i2s_err("set RX LR swap fail\n");
		return -I2S_EPTR;
	}

	if (config->i2s_out.msb_offset < 128 &&
	    config->i2s_in.msb_offset < 128) {
		/*set TX msb_offset*/
		i2sdlcfg.i2s_msbo_ffset = config->i2s_out.msb_offset;
		/*set RX msb_offset*/
		i2sulcfg.i2s_msbo_ffset = config->i2s_in.msb_offset;
	} else {
		i2s_err("msb_offset over maximum(127)\n");
		return -I2S_EPTR;
	}
	/*set TX word_select_inverse*/
	if (config->i2s_out.word_select_inverse <= MHAL_FN_EN)
		i2sdlcfg.i2s_word_sel_inv =
		config->i2s_out.word_select_inverse;
	else
		return -I2S_EPTR;
	/*set RX word_select_inverse*/
	if (config->i2s_in.word_select_inverse <= MHAL_FN_EN)
		i2sulcfg.i2s_word_sel_inv =
		config->i2s_in.word_select_inverse;
	else
		return -I2S_EPTR;
	/*set tx bit per sample*/
	switch (config->i2s_out.bits_per_sample) {
	case MHAL_I2S_BITS_PER_SAMPLE_32:
		i2s_dl_bit_per_sample_rate = I2S_DL_32BITS_PER_SAMPLE;
		p_i2s_internal_cfg->i2s_user_config.i2s_out.bits_per_sample =
		MHAL_I2S_BITS_PER_SAMPLE_32;
	break;
	case MHAL_I2S_BITS_PER_SAMPLE_64:
		i2s_dl_bit_per_sample_rate = I2S_DL_64BITS_PER_SAMPLE;
		p_i2s_internal_cfg->i2s_user_config.i2s_out.bits_per_sample =
		MHAL_I2S_BITS_PER_SAMPLE_64;
	break;
	case MHAL_I2S_BITS_PER_SAMPLE_128:
		i2s_dl_bit_per_sample_rate = I2S_DL_128BITS_PER_SAMPLE;
		p_i2s_internal_cfg->i2s_user_config.i2s_out.bits_per_sample =
		MHAL_I2S_BITS_PER_SAMPLE_128;
	break;
	default:
		i2s_err("set tx bit per sample fail\n");
		return -I2S_EPTR;
	}
	/*set rx bit per sample*/
	switch (config->i2s_in.bits_per_sample) {
	case MHAL_I2S_BITS_PER_SAMPLE_32:
		i2s_ul_bit_per_sample_rate = I2S_UL_32BITS_PER_SAMPLE;
		p_i2s_internal_cfg->i2s_user_config.i2s_in.bits_per_sample =
		MHAL_I2S_BITS_PER_SAMPLE_32;
	break;
	case MHAL_I2S_BITS_PER_SAMPLE_64:
		i2s_ul_bit_per_sample_rate = I2S_UL_64BITS_PER_SAMPLE;
		p_i2s_internal_cfg->i2s_user_config.i2s_in.bits_per_sample =
		MHAL_I2S_BITS_PER_SAMPLE_64;
	break;
	case MHAL_I2S_BITS_PER_SAMPLE_128:
		i2s_ul_bit_per_sample_rate = I2S_UL_128BITS_PER_SAMPLE;
		p_i2s_internal_cfg->i2s_user_config.i2s_in.bits_per_sample =
		MHAL_I2S_BITS_PER_SAMPLE_128;
	break;
	default:
		i2s_err("set rx bit per sample fail\n");
		return -I2S_EPTR;
	}
	if (config->i2s_out.bits_per_sample != config->i2s_in.bits_per_sample) {
		i2s_err("set tx/rx bit per sample different\n");
		return -I2S_EPTR;
	}

	if (i2s_initial_type == I2S_EXTERNAL_TDM_MODE) {
		/*set tx channel per sample*/
		switch (config->i2s_out.channels_per_sample) {
		case MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_2:
			i2s_dl_ch_per_sample = I2S_DL_2_CH_PER_SAMPLE;

			p_i2s_internal_cfg->
			i2s_user_config.i2s_out.channels_per_sample =
			MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_2;
		break;
		case MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_4:
			i2s_dl_ch_per_sample = I2S_DL_4_CH_PER_SAMPLE;

			p_i2s_internal_cfg->
			i2s_user_config.i2s_out.channels_per_sample =
			MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_4;
		break;
		default:
			i2s_err("TDM set tx channel per sample fail\n");
			return -I2S_EPTR;
		}
		/*set rx channel per sample*/
		switch (config->i2s_in.channels_per_sample) {
		case MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_2:
			i2s_ul_ch_per_sample = I2S_UL_2_CH_PER_SAMPLE;

			p_i2s_internal_cfg->
			i2s_user_config.i2s_in.channels_per_sample =
			MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_2;
		break;
		case MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_4:
			i2s_ul_ch_per_sample = I2S_UL_4_CH_PER_SAMPLE;

			p_i2s_internal_cfg->
			i2s_user_config.i2s_in.channels_per_sample =
			MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_4;
		break;
		default:
			i2s_err("TDM set rx channel per sample fail\n");
			return -I2S_EPTR;
		}
		if (config->i2s_out.channels_per_sample !=
		    config->i2s_in.channels_per_sample) {
			i2s_err("TDM set tx/rx channel per sample different\n");
			return -I2S_EPTR;
		}
	} else {
		i2s_dl_ch_per_sample = I2S_DL_2_CH_PER_SAMPLE;
		i2s_ul_ch_per_sample = I2S_UL_2_CH_PER_SAMPLE;
	}

	mtk_hdl_i2s_cfg_sample_rate(
				i2s_dl_sample_rate,
				i2s_ul_sample_rate,
				i2s_down_rate_en,
				&i2sdlcfg, &i2sulcfg);

	mtk_hdl_i2s_cfg_mono_stereo(
				i2s_dl_mono_stereo,
				i2s_dl_mono_dup_en,
				&i2sgblcfg);

	if (i2s_initial_type == I2S_EXTERNAL_TDM_MODE)
		mtk_hdl_i2s_cfg_tdm_ch_bit_per_sample(
				i2s_dl_ch_per_sample,
				i2s_ul_ch_per_sample,
				i2s_dl_bit_per_sample_rate,
				i2s_ul_bit_per_sample_rate,
				&i2sdlcfg, &i2sulcfg);

	mtk_hdl_i2s_gbl_cfg(ctlr->base, &i2sgblcfg);
	mtk_hdl_i2s_dl_cfg(ctlr->base, &i2sdlcfg);
	mtk_hdl_i2s_ul_cfg(ctlr->base, &i2sulcfg);

	return 0;
}

int mtk_mhal_i2s_enable_audio_top(struct mtk_i2s_ctlr *ctlr)
{
	/*----Enable  audio_top----*/
	if (!ctlr)
		return -I2S_EPTR;
	mtk_hdl_i2s_clk_fifo_en(ctlr->base, I2S_TRUE);
	return 0;
}

int mtk_mhal_i2s_disable_audio_top(struct mtk_i2s_ctlr *ctlr)
{
	/*----Disable audio_top----*/
	if (!ctlr)
		return -I2S_EPTR;
	mtk_hdl_i2s_clk_fifo_en(ctlr->base, I2S_FALSE);
	return 0;
}

int mtk_mhal_i2s_cfg_tx_dma_irq_enable(struct mtk_i2s_ctlr *ctlr,
				       i2s_dma_callback_func callback_func)
{
	struct mtk_i2s_private *p_i2s_internal_cfg;
	struct osai_dma_config *p_dma_cfg;

	if (!ctlr || !callback_func)
		return -I2S_EPTR;
	p_i2s_internal_cfg = ctlr->mdata;
	p_dma_cfg = &p_i2s_internal_cfg->i2s_txdma_cfg;
	/*----Enable DMA interrupt for TX----*/
	p_dma_cfg->interrupt_flag = OSAI_DMA_INT_VFIFO_THRESHOLD;
	p_dma_cfg->done_callback = callback_func;
	return 0;

}

int mtk_mhal_i2s_cfg_tx_dma_irq_disable(struct mtk_i2s_ctlr *ctlr)
{
	struct mtk_i2s_private *p_i2s_internal_cfg;
	struct osai_dma_config *p_dma_cfg;

	if (!ctlr)
		return -I2S_EPTR;
	p_i2s_internal_cfg = ctlr->mdata;
	p_dma_cfg = &p_i2s_internal_cfg->i2s_txdma_cfg;
	/*----Disable DMA interrupt for TX---- */
	p_dma_cfg->interrupt_flag = 0;
	return 0;

}

int mtk_mhal_i2s_cfg_rx_dma_irq_enable(struct mtk_i2s_ctlr *ctlr,
				       i2s_dma_callback_func callback_func)
{
	struct mtk_i2s_private *p_i2s_internal_cfg;
	struct osai_dma_config *p_dma_cfg;

	if (!ctlr || !callback_func)
		return -I2S_EPTR;
	p_i2s_internal_cfg = ctlr->mdata;
	p_dma_cfg = &p_i2s_internal_cfg->i2s_rxdma_cfg;
	/*----Enable  DMA interrupt for RX---- */
	p_dma_cfg->interrupt_flag = OSAI_DMA_INT_VFIFO_THRESHOLD;
	p_dma_cfg->done_callback = callback_func;
	return 0;
}

int mtk_mhal_i2s_cfg_rx_dma_irq_disable(struct mtk_i2s_ctlr *ctlr)
{
	struct mtk_i2s_private *p_i2s_internal_cfg;
	struct osai_dma_config *p_dma_cfg;

	if (!ctlr)
		return -I2S_EPTR;
	p_i2s_internal_cfg = ctlr->mdata;
	p_dma_cfg = &p_i2s_internal_cfg->i2s_rxdma_cfg;
	/*----Disable DMA interrupt for RX---- */
	p_dma_cfg->interrupt_flag = 0;
	return 0;
}
int mtk_mhal_i2s_move_tx_point(struct mtk_i2s_ctlr *ctlr,
			       u32 buffer_length)
{
	u8 bresut = 0;
	struct mtk_i2s_private *p_i2s_internal_cfg;
	struct osai_dma_config *p_dma_cfg;
	u8 channel_num;

	if (!ctlr)
		return -I2S_EPTR;

	channel_num = ctlr->i2s_txdma_chnum;
	p_i2s_internal_cfg = ctlr->mdata;
	p_dma_cfg = &p_i2s_internal_cfg->i2s_txdma_cfg;
	if (buffer_length > p_dma_cfg->vfifo_size ||
	    !buffer_length)
		return -I2S_EPTR;

	bresut = osai_dma_update_vfifo_swptr(channel_num, buffer_length);

	if (bresut) {
		i2s_err("TX DMA move point fail\n");
		return -I2S_EPTR;
	}
	return 0;
}

int mtk_mhal_i2s_move_rx_point(struct mtk_i2s_ctlr *ctlr,
			       u32 buffer_length)
{
	u8 bresut = 0;
	struct mtk_i2s_private *p_i2s_internal_cfg;
	struct osai_dma_config *p_dma_cfg;
	u8 channel_num;

	if (!ctlr)
		return -I2S_EPTR;

	channel_num = ctlr->i2s_rxdma_chnum;
	p_i2s_internal_cfg = ctlr->mdata;
	p_dma_cfg = &p_i2s_internal_cfg->i2s_rxdma_cfg;
	if (buffer_length > p_dma_cfg->vfifo_size ||
	    !buffer_length)
		return -I2S_EPTR;

	bresut = osai_dma_update_vfifo_swptr(channel_num, buffer_length);

	if (bresut) {
		i2s_err("RX DMA move point fail\n");
		return -I2S_EPTR;
	}
	return 0;
}

int mtk_mhal_i2s_start_tx_vfifo(struct mtk_i2s_ctlr *ctlr,
				u32 *buffer,
				u32 threshold,
				u32 buffer_length)
{
	u8 bresut = 0;
	struct mtk_i2s_private *p_i2s_internal_cfg;
	struct osai_dma_config *p_dma_cfg;
	u8 channel_num;

	if (!ctlr || buffer == NULL)
		return -I2S_EPTR;
	if (buffer_length < 1024)
		return -I2S_ELENGTH;
	if (threshold > buffer_length || threshold == 0)
		return -I2S_ELENGTH;

	channel_num = ctlr->i2s_txdma_chnum;
	p_i2s_internal_cfg = ctlr->mdata;
	p_dma_cfg = &p_i2s_internal_cfg->i2s_txdma_cfg;

	p_dma_cfg->dir = 0;
	p_dma_cfg->src_addr = (u32)buffer;
	p_dma_cfg->dst_addr = (u32)(ctlr->i2s_tx_fifo_port);
	p_dma_cfg->vfifo_thrsh = threshold;
	p_dma_cfg->vfifo_size = buffer_length;
	p_dma_cfg->transize = OSAI_DMA_SIZE_LONG;

	bresut |= osai_dma_stop(channel_num);
	bresut |= osai_dma_config(channel_num, p_dma_cfg);
	bresut |= osai_dma_clr_dreq(channel_num);
	bresut |= osai_dma_start(channel_num);

	if (bresut) {
		i2s_err("TX DMA return fail\n");
		return -I2S_EPTR;
	}
	return 0;
}

int mtk_mhal_i2s_start_rx_vfifo(struct mtk_i2s_ctlr *ctlr,
				u32 *buffer,
				u32 threshold,
				u32 buffer_length)
{
	u8 bresut = 0;
	struct mtk_i2s_private *p_i2s_internal_cfg;
	struct osai_dma_config *p_dma_cfg;
	u8 channel_num;

	if (!ctlr || buffer == NULL)
		return -I2S_EPTR;
	if (buffer_length < 1024)
		return -I2S_ELENGTH;
	if (threshold > buffer_length || threshold == 0)
		return -I2S_ELENGTH;

	channel_num = ctlr->i2s_rxdma_chnum;
	p_i2s_internal_cfg = ctlr->mdata;
	p_dma_cfg = &p_i2s_internal_cfg->i2s_rxdma_cfg;

	p_dma_cfg->dir = 1;
	p_dma_cfg->src_addr = (u32)(ctlr->i2s_rx_fifo_port);
	p_dma_cfg->dst_addr = (u32)buffer;
	p_dma_cfg->vfifo_thrsh = threshold;
	p_dma_cfg->vfifo_size = buffer_length;
	p_dma_cfg->transize = OSAI_DMA_SIZE_LONG;

	bresut |= osai_dma_stop(channel_num);
	bresut |= osai_dma_config(channel_num, p_dma_cfg);
	bresut |= osai_dma_clr_dreq(channel_num);
	bresut |= osai_dma_start(channel_num);

	if (bresut) {
		i2s_err("RX DMA return fail\n");
		return -I2S_EPTR;
	}
	return 0;
}

int mtk_mhal_i2s_stop_tx_vfifo(struct mtk_i2s_ctlr *ctlr)
{
	u8 bresut = 0;
	u8 channel_num;

	if (!ctlr)
		return -I2S_EPTR;
	channel_num = ctlr->i2s_txdma_chnum;
	bresut = osai_dma_stop(channel_num);
	if (bresut) {
		i2s_err("DMA channel stop return fail\n");
		return -I2S_EPTR;
	}
	return 0;
}

int mtk_mhal_i2s_stop_rx_vfifo(struct mtk_i2s_ctlr *ctlr)
{
	u8 bresut = 0;
	u8 channel_num;

	if (!ctlr)
		return -I2S_EPTR;
	channel_num = ctlr->i2s_rxdma_chnum;
	bresut = osai_dma_stop(channel_num);
	if (bresut) {
		i2s_err("DMA channel stop return fail\n");
		return -I2S_EPTR;
	}
	return 0;
}
int mtk_mhal_i2s_alloc_vfifo_ch(struct mtk_i2s_ctlr *ctlr)
{
	u8 bresut = 0;

	if (!ctlr)
		return -I2S_EPTR;
	bresut = osai_dma_allocate_chan(ctlr->i2s_txdma_chnum);
	if (bresut) {
		i2s_err("TX DMA channel allocate return fail\n");
		return -I2S_EPTR;
	}
	bresut = osai_dma_allocate_chan(ctlr->i2s_rxdma_chnum);
	if (bresut) {
		i2s_err("RX DMA channel allocate return fail\n");
		return -I2S_EPTR;
	}
	return 0;
}
int mtk_mhal_i2s_release_vfifo_ch(struct mtk_i2s_ctlr *ctlr)
{
	u8 bresut = 0;

	if (!ctlr)
		return -I2S_EPTR;
	bresut = osai_dma_release_chan(ctlr->i2s_txdma_chnum);
	if (bresut) {
		i2s_err("TX DMA channel release return fail\n");
		return -I2S_EPTR;
	}
	bresut = osai_dma_release_chan(ctlr->i2s_rxdma_chnum);
	if (bresut) {
		i2s_err("RX DMA channel release return fail\n");
		return -I2S_EPTR;
	}
	return 0;
}

int mtk_mhal_i2s_enable_tx(struct mtk_i2s_ctlr *ctlr)
{
	if (!ctlr)
		return -I2S_EPTR;
	mtk_hdl_i2s_dl_en(ctlr->base, true);
	return 0;
}

int mtk_mhal_i2s_enable_rx(struct mtk_i2s_ctlr *ctlr)
{
	if (!ctlr)
		return -I2S_EPTR;
	mtk_hdl_i2s_ul_en(ctlr->base, true);
	return 0;
}

int mtk_mhal_i2s_disable_tx(struct mtk_i2s_ctlr *ctlr)
{

	if (!ctlr)
		return -I2S_EPTR;
	mtk_hdl_i2s_dl_en(ctlr->base, false);
	return 0;
}

int mtk_mhal_i2s_disable_rx(struct mtk_i2s_ctlr *ctlr)
{
	if (!ctlr)
		return -I2S_EPTR;
	mtk_hdl_i2s_ul_en(ctlr->base, false);
	return 0;
}

enum_i2s_dl_sample_rate _mtk_mhal_i2s_tx_sample_rate(hal_i2s_sample_rate
						     sample_rate)
{
	enum_i2s_dl_sample_rate dl_sample_rate = I2S_DL_SAMPLE_RATE_8K;

	switch (sample_rate) {
	case MHAL_I2S_SAMPLE_RATE_8K:
		dl_sample_rate = I2S_DL_SAMPLE_RATE_8K;
		break;
	case MHAL_I2S_SAMPLE_RATE_12K:
		dl_sample_rate = I2S_DL_SAMPLE_RATE_12K;
		break;
	case MHAL_I2S_SAMPLE_RATE_16K:
		dl_sample_rate = I2S_DL_SAMPLE_RATE_16K;
		break;
	case MHAL_I2S_SAMPLE_RATE_24K:
		dl_sample_rate = I2S_DL_SAMPLE_RATE_24K;
		break;
	case MHAL_I2S_SAMPLE_RATE_32K:
		dl_sample_rate = I2S_DL_SAMPLE_RATE_32K;
		break;
	case MHAL_I2S_SAMPLE_RATE_48K:
		dl_sample_rate = I2S_DL_SAMPLE_RATE_48K;
		break;
	}
	return dl_sample_rate;
}
enum_i2s_ul_sample_rate _mtk_mhal_i2s_rx_sample_rate(hal_i2s_sample_rate
						     sample_rate)
{
	enum_i2s_ul_sample_rate ul_sample_rate = I2S_UL_SAMPLE_RATE_8K;

	switch (sample_rate) {
	case MHAL_I2S_SAMPLE_RATE_8K:
		ul_sample_rate = I2S_UL_SAMPLE_RATE_8K;
		break;
	case MHAL_I2S_SAMPLE_RATE_12K:
		ul_sample_rate = I2S_UL_SAMPLE_RATE_12K;
		break;
	case MHAL_I2S_SAMPLE_RATE_16K:
		ul_sample_rate = I2S_UL_SAMPLE_RATE_16K;
		break;
	case MHAL_I2S_SAMPLE_RATE_24K:
		ul_sample_rate = I2S_UL_SAMPLE_RATE_24K;
		break;
	case MHAL_I2S_SAMPLE_RATE_32K:
		ul_sample_rate = I2S_UL_SAMPLE_RATE_32K;
		break;
	case MHAL_I2S_SAMPLE_RATE_48K:
		ul_sample_rate = I2S_UL_SAMPLE_RATE_48K;
		break;
	}
	return ul_sample_rate;
}
