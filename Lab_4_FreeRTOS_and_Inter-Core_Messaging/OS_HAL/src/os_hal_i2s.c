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

#include "os_hal_i2s.h"
#include "os_hal_dma.h"


#define MTK_I2S_MAX_PORT_NUMBER	2
#define I2S0_BASE_ADDR	0x380D0000
#define I2S1_BASE_ADDR	0x380E0000
#define I2S0_FIFO_PORT_ADDR	0x380F0000
#define I2S1_FIFO_PORT_ADDR	0x38100000

static unsigned long i2s_base_addr[MTK_I2S_MAX_PORT_NUMBER] = {
	I2S0_BASE_ADDR,
	I2S1_BASE_ADDR,
};
static unsigned long i2s_fifo_port_addr[MTK_I2S_MAX_PORT_NUMBER][2] = {
	/* [0]:tx, [1]:rx */
	{I2S0_FIFO_PORT_ADDR, I2S0_FIFO_PORT_ADDR},
	{I2S1_FIFO_PORT_ADDR, I2S1_FIFO_PORT_ADDR},
};
static unsigned long i2s_dma_ch_num[MTK_I2S_MAX_PORT_NUMBER][2] = {
	/* [0]:tx, [1]:rx */
	{VDMA_I2S0_TX_CH25, VDMA_I2S0_RX_CH26},
	{VDMA_I2S1_TX_CH27, VDMA_I2S1_RX_CH28},
};

struct mtk_i2s_ctlr_cfg {
	struct mtk_i2s_ctlr i2s_ctrl;
	unsigned int tx_dma_ch_num;
	unsigned int rx_dma_ch_num;
	unsigned int tx_period_len;
	unsigned int rx_period_len;
	i2s_dma_callback_func tx_callback_func;
	void *tx_callback_data;
	i2s_dma_callback_func rx_callback_func;
	void *rx_callback_data;
};

static struct mtk_i2s_ctlr_cfg i2s0_ctlr_cfg;
static struct mtk_i2s_ctlr_cfg i2s1_ctlr_cfg;
static struct mtk_i2s_private	i2s0_mdata;
static struct mtk_i2s_private	i2s1_mdata;

void _mtk_os_hal_i2s0_tx_callback(void *data)
{
	mtk_mhal_i2s_move_tx_point(&i2s0_ctlr_cfg.i2s_ctrl,
				   i2s0_ctlr_cfg.tx_period_len);
	if (*i2s0_ctlr_cfg.tx_callback_func != NULL)
		i2s0_ctlr_cfg.tx_callback_func(
				i2s0_ctlr_cfg.tx_callback_data);
}
void _mtk_os_hal_i2s0_rx_callback(void *data)
{
	mtk_mhal_i2s_move_rx_point(&i2s0_ctlr_cfg.i2s_ctrl,
				   i2s0_ctlr_cfg.rx_period_len);
	if (*i2s0_ctlr_cfg.rx_callback_func != NULL)
		i2s0_ctlr_cfg.rx_callback_func(
				i2s0_ctlr_cfg.rx_callback_data);
}
void _mtk_os_hal_i2s1_tx_callback(void *data)
{
	mtk_mhal_i2s_move_tx_point(&i2s1_ctlr_cfg.i2s_ctrl,
				   i2s1_ctlr_cfg.tx_period_len);
	if (*i2s1_ctlr_cfg.tx_callback_func != NULL)
		i2s1_ctlr_cfg.tx_callback_func(
				i2s1_ctlr_cfg.tx_callback_data);
}
void _mtk_os_hal_i2s1_rx_callback(void *data)
{
	mtk_mhal_i2s_move_rx_point(&i2s1_ctlr_cfg.i2s_ctrl,
				   i2s1_ctlr_cfg.rx_period_len);
	if (*i2s1_ctlr_cfg.rx_callback_func != NULL)
		i2s1_ctlr_cfg.rx_callback_func(
				i2s1_ctlr_cfg.rx_callback_data);
}

int mtk_os_hal_request_i2s(i2s_no i2s_port)
{
	struct mtk_i2s_ctlr_cfg *i2s_ctrl_cfg;
	int result = 0;

	if (i2s_port >= MTK_I2S_MAX_PORT_NUMBER)
		return -I2S_EPTR;
	if (i2s_port == MHAL_I2S0) {
		i2s_ctrl_cfg = &i2s0_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s0_mdata;
	} else {
		i2s_ctrl_cfg = &i2s1_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s1_mdata;
	}
	i2s_ctrl_cfg->i2s_ctrl.base = (void __iomem *)i2s_base_addr[i2s_port];
	i2s_ctrl_cfg->i2s_ctrl.i2s_port = i2s_port;
	i2s_ctrl_cfg->i2s_ctrl.i2s_txdma_chnum = i2s_dma_ch_num[i2s_port][0];
	i2s_ctrl_cfg->i2s_ctrl.i2s_rxdma_chnum = i2s_dma_ch_num[i2s_port][1];
	result = mtk_mhal_i2s_reset(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s reset fail :\n");
		return -I2S_EPTR;
	}
	result = mtk_mhal_i2s_alloc_vfifo_ch(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s allocate DMA fail :\n");/* error handle */
		return -I2S_EPTR;
	}

	return 0;
}

int mtk_os_hal_free_i2s(i2s_no i2s_port)
{
	struct mtk_i2s_ctlr_cfg *i2s_ctrl_cfg;
	int result = 0;

	if (i2s_port >= MTK_I2S_MAX_PORT_NUMBER)
		return -I2S_EPTR;
	if (i2s_port == MHAL_I2S0) {
		i2s_ctrl_cfg = &i2s0_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s0_mdata;
	} else {
		i2s_ctrl_cfg = &i2s1_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s1_mdata;
	}
	i2s_ctrl_cfg->i2s_ctrl.i2s_txdma_chnum = i2s_dma_ch_num[i2s_port][0];
	i2s_ctrl_cfg->i2s_ctrl.i2s_rxdma_chnum = i2s_dma_ch_num[i2s_port][1];
	result = mtk_mhal_i2s_release_vfifo_ch(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s release DMA fail :\n");/* error handle */
		return -I2S_EPTR;
	}

	return 0;
}

int mtk_os_hal_config_i2s(i2s_no i2s_port, audio_parameter *parameter)
{
	struct hal_i2s_config i2s_config;
	struct mtk_i2s_ctlr_cfg *i2s_ctrl_cfg;
	int result = 0;

	if (i2s_port >= MTK_I2S_MAX_PORT_NUMBER || !parameter)
		return -I2S_EPTR;
	if (i2s_port == MHAL_I2S0) {
		i2s_ctrl_cfg = &i2s0_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s0_mdata;
	} else {
		i2s_ctrl_cfg = &i2s1_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s1_mdata;
	}
	i2s_ctrl_cfg->i2s_ctrl.base = (void __iomem *)i2s_base_addr[i2s_port];
	i2s_ctrl_cfg->i2s_ctrl.i2s_port = i2s_port;
	i2s_ctrl_cfg->i2s_ctrl.i2s_txdma_chnum = i2s_dma_ch_num[i2s_port][0];
	i2s_ctrl_cfg->i2s_ctrl.i2s_rxdma_chnum = i2s_dma_ch_num[i2s_port][1];
	i2s_ctrl_cfg->i2s_ctrl.i2s_tx_fifo_port =
			 (void __iomem *)i2s_fifo_port_addr[i2s_port][0];
	i2s_ctrl_cfg->i2s_ctrl.i2s_rx_fifo_port =
			 (void __iomem *)i2s_fifo_port_addr[i2s_port][1];
	result = mtk_mhal_i2s_cfg_type(&i2s_ctrl_cfg->i2s_ctrl,
				       parameter->i2s_initial_type);
	if (result != 0) {
		printf("i2s config type fail :\n");/* error handle */
		return result;
	}

	/*configure RX settings*/
	i2s_config.i2s_in.sample_rate = parameter->sample_rate;
	i2s_config.i2s_in.bits_per_sample = parameter->bits_per_sample;
	i2s_config.i2s_in.channel_number = parameter->channel_number;
	i2s_config.i2s_in.channels_per_sample = parameter->channels_per_sample;
	i2s_config.i2s_in.msb_offset = parameter->msb_offset;
	i2s_config.i2s_in.word_select_inverse = parameter->word_select_inverse;
	i2s_config.i2s_in.lr_swap = parameter->lr_swap;
	i2s_config.rx_down_rate = parameter->rx_down_rate;

	/*configure TX settings*/
	i2s_config.i2s_out.sample_rate = parameter->sample_rate;
	i2s_config.i2s_out.bits_per_sample = parameter->bits_per_sample;
	i2s_config.i2s_out.channel_number = parameter->channel_number;
	i2s_config.i2s_out.channels_per_sample
			= parameter->channels_per_sample;
	i2s_config.i2s_out.msb_offset = parameter->msb_offset;
	i2s_config.i2s_out.word_select_inverse = parameter->word_select_inverse;
	i2s_config.i2s_out.lr_swap = parameter->lr_swap;
	i2s_config.tx_mode = parameter->tx_mode;

	result = mtk_mhal_i2s_set_config(&i2s_ctrl_cfg->i2s_ctrl, &i2s_config);
	if (result != 0) {
		printf("i2s set config fail :\n");/* error handle */
		return result;
	}
	/*configure VFIFO*/

	i2s_ctrl_cfg->tx_period_len = parameter->tx_period_len;
	i2s_ctrl_cfg->rx_period_len = parameter->rx_period_len;
	if (*parameter->tx_callback_func == NULL ||
	    *parameter->rx_callback_func == NULL) {
		printf("callback function is NULL:\n");/* error handle */
		return -I2S_EPTR;
	}
	i2s_ctrl_cfg->tx_callback_func = parameter->tx_callback_func;
	i2s_ctrl_cfg->rx_callback_func = parameter->rx_callback_func;
	i2s_ctrl_cfg->tx_callback_data = parameter->tx_callback_data;
	i2s_ctrl_cfg->rx_callback_data = parameter->rx_callback_data;

	if (i2s_port == MHAL_I2S0)
		result = mtk_mhal_i2s_cfg_tx_dma_irq_enable(
					&i2s_ctrl_cfg->i2s_ctrl,
					_mtk_os_hal_i2s0_tx_callback);
	else
		result = mtk_mhal_i2s_cfg_tx_dma_irq_enable(
					&i2s_ctrl_cfg->i2s_ctrl,
					_mtk_os_hal_i2s1_tx_callback);

	if (result != 0) {
		printf("i2s config tx irq enable fail :\n");
		/* error handle */
		return result;
	}
	if (i2s_port == MHAL_I2S0)
		result = mtk_mhal_i2s_cfg_rx_dma_irq_enable(
						&i2s_ctrl_cfg->i2s_ctrl,
						_mtk_os_hal_i2s0_rx_callback);
	else
		result = mtk_mhal_i2s_cfg_rx_dma_irq_enable(
					&i2s_ctrl_cfg->i2s_ctrl,
					_mtk_os_hal_i2s1_rx_callback);
	if (result != 0) {
		printf("i2s config rx irq enable fail :\n");
		/* error handle */
		return result;
	}
	result = mtk_mhal_i2s_start_tx_vfifo(&i2s_ctrl_cfg->i2s_ctrl,
					     parameter->tx_buffer_addr,
					     1,
					     parameter->tx_buffer_len);
	if (result != 0) {
		printf("i2s tx vfifo setup fail :\n");/* error handle */
		return result;
	}

	result = mtk_mhal_i2s_start_rx_vfifo(&i2s_ctrl_cfg->i2s_ctrl,
					     parameter->rx_buffer_addr,
					     parameter->rx_period_len - 1,
					     parameter->rx_buffer_len);
	if (result != 0) {
		printf("i2s rx vfifo setup fail :\n");/* error handle */
		return result;
	}

	return 0;
}

int mtk_os_hal_enable_i2s(i2s_no i2s_port)
{
	struct mtk_i2s_ctlr_cfg *i2s_ctrl_cfg;
	int result = 0;

	if (i2s_port >= MTK_I2S_MAX_PORT_NUMBER)
		return -I2S_EPTR;
	if (i2s_port == MHAL_I2S0) {
		i2s_ctrl_cfg = &i2s0_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s0_mdata;
	} else {
		i2s_ctrl_cfg = &i2s1_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s1_mdata;
	}
	i2s_ctrl_cfg->i2s_ctrl.base = (void __iomem *)i2s_base_addr[i2s_port];
	i2s_ctrl_cfg->i2s_ctrl.i2s_port = i2s_port;

	result = mtk_mhal_i2s_enable_audio_top(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s audio top enable fail :\n");
		/* error handle */
		return result;
	}
	result = mtk_mhal_i2s_enable_tx(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s tx enable fail :\n");
		/* error handle */
		return result;
	}
	result = mtk_mhal_i2s_enable_rx(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s rx enable fail :\n");
		/* error handle */
		return result;
	}
	return 0;
}

int mtk_os_hal_disable_i2s(i2s_no i2s_port)
{
	struct mtk_i2s_ctlr_cfg *i2s_ctrl_cfg;
	int result = 0;

	if (i2s_port >= MTK_I2S_MAX_PORT_NUMBER)
		return -I2S_EPTR;
	if (i2s_port == MHAL_I2S0) {
		i2s_ctrl_cfg = &i2s0_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s0_mdata;
	} else {
		i2s_ctrl_cfg = &i2s1_ctlr_cfg;
		i2s_ctrl_cfg->i2s_ctrl.mdata = &i2s1_mdata;
	}
	i2s_ctrl_cfg->i2s_ctrl.base = (void __iomem *)i2s_base_addr[i2s_port];
	i2s_ctrl_cfg->i2s_ctrl.i2s_port = i2s_port;
	result = mtk_mhal_i2s_stop_tx_vfifo(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s tx dma stop fail :\n");
		/* error handle */
		return result;
	}
	result = mtk_mhal_i2s_stop_rx_vfifo(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s rx dma stop fail :\n");
		/* error handle */
		return result;
	}
	result = mtk_mhal_i2s_disable_tx(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s tx disable fail :\n");
		/* error handle */
		return result;
	}
	result = mtk_mhal_i2s_disable_rx(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s rx disable fail :\n");
		/* error handle */
		return result;
	}
	result = mtk_mhal_i2s_disable_audio_top(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s audio top disable fail :\n");
		/* error handle */
		return result;
	}
	result = mtk_mhal_i2s_reset(&i2s_ctrl_cfg->i2s_ctrl);
	if (result != 0) {
		printf("i2s reset fail :\n");
		/* error handle */
		return result;
	}
	return 0;
}
