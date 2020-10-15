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

#include "hdl_uart.h"
#include "mhal_uart.h"

int mtk_mhal_uart_enable_clk(struct mtk_uart_controller *ctlr)
{
	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_cg_config(ctlr->cg_base, true);

	return 0;
}

int mtk_mhal_uart_disable_clk(struct mtk_uart_controller *ctlr)
{
	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_cg_config(ctlr->cg_base, false);

	return 0;
}

int mtk_mhal_uart_sw_reset(struct mtk_uart_controller *ctlr)
{
	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_reset(ctlr->cg_base);
	return 0;
}

int mtk_mhal_uart_dumpreg(struct mtk_uart_controller *ctlr)
{
	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_dmreg(ctlr->base);
	return 0;
}

int mtk_mhal_uart_hw_init(struct mtk_uart_controller *ctlr)
{
	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_init(ctlr->base);
	mtk_hdl_uart_set_baudrate(ctlr->base, ctlr->baudrate);
	mtk_hdl_uart_set_format(ctlr->base,
				ctlr->data_bit,
				ctlr->parity,
				ctlr->stop_bit);

	return 0;
}

int mtk_mhal_uart_set_baudrate(struct mtk_uart_controller *ctlr)
{
	if (!ctlr)
		return -UART_EPTR;

	if ((ctlr->baudrate > MAX_UART_BAUD) ||
		(ctlr->baudrate < MIN_UART_BAUD))
		return -UART_EINVAL;

	mtk_hdl_uart_set_baudrate(ctlr->base, ctlr->baudrate);
	return 0;
}

int mtk_mhal_uart_set_format(struct mtk_uart_controller *ctlr)
{
	if (!ctlr)
		return -UART_EPTR;

	if (ctlr->data_bit > UART_DATA_8_BITS)
		return -UART_EINVAL;

	if ((ctlr->parity != UART_NONE_PARITY) &&
		(ctlr->parity != UART_ODD_PARITY) &&
		(ctlr->parity != UART_EVEN_PARITY) &&
		(ctlr->parity != UART_MARK_PARITY) &&
		(ctlr->parity != UART_SPACE_PARITY))
		return -UART_EINVAL;

	if ((ctlr->stop_bit != UART_STOP_1_BIT) &&
		(ctlr->stop_bit != UART_STOP_2_BIT))
		return -UART_EINVAL;

	mtk_hdl_uart_set_format(ctlr->base,
					ctlr->data_bit,
					ctlr->parity,
					ctlr->stop_bit);
	return 0;
}

int mtk_mhal_uart_set_hw_fc(struct mtk_uart_controller *ctlr, u8 hw_fc)
{
	if (!ctlr)
		return -UART_EPTR;

	if ((hw_fc != UART_EFR_HW_FC_DISABLE) &&
		(hw_fc != UART_EFR_HW_FC_RTS) &&
		(hw_fc != UART_EFR_HW_FC_CTS) &&
		(hw_fc != UART_EFR_HW_FC_RTS_CTS))
		return -UART_EINVAL;

	mtk_hdl_uart_set_hw_fc(ctlr->base, hw_fc);

	return 0;
}

int mtk_mhal_uart_disable_sw_fc(struct mtk_uart_controller *ctlr)
{
	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_disable_sw_fc(ctlr->base);

	return 0;
}

int mtk_mhal_uart_set_sw_fc(struct mtk_uart_controller *ctlr,
		u8 xon1, u8 xoff1, u8 xon2, u8 xoff2, u8 escape_data)
{
	if (!ctlr)
		return -UART_EPTR;

	if ((xon1 == 0) && (xoff1 == 0) && (xon2 == 0) && (xoff2 == 0))
		return -UART_EINVAL;

	mtk_hdl_uart_set_sw_fc(ctlr->base, xon1, xoff1,
		xon2, xoff2, escape_data);

	return 0;
}

int mtk_mhal_uart_set_dma(struct mtk_uart_controller *ctlr, bool enable_dma)
{
	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_dma_en(ctlr->base, enable_dma);

	return 0;
}

int mtk_mhal_uart_clear_irq_status(struct mtk_uart_controller *ctlr)
{
	int status;

	if (!ctlr)
		return -UART_EPTR;

	status = mtk_hdl_uart_clear_irq_status(ctlr->base);

	return status;
}

int mtk_mhal_uart_set_irq(struct mtk_uart_controller *ctlr, u8 int_flag)
{
	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_set_irq(ctlr->base, int_flag);

	return 0;
}

int mtk_mhal_uart_putc(struct mtk_uart_controller *ctlr, u8 data)
{
	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_output_char(ctlr->base, data);

	return 0;
}

int mtk_mhal_uart_getc(struct mtk_uart_controller *ctlr)
{
	int ch;

	if (!ctlr)
		return -UART_EPTR;

	do {
		ch = mtk_hdl_uart_input_char(ctlr->base);
	} while (ch == 0xFF);

	return ch;
}

int mtk_mhal_uart_getc_nowait(struct mtk_uart_controller *ctlr)
{
	u8 ch;

	if (!ctlr)
		return -UART_EPTR;

	ch = mtk_hdl_uart_input_char(ctlr->base);
	return ch;
}

/********************************** DMA *********************************/
/** This function is used to register user's done callback to OS-HAL layer */
int mtk_mhal_uart_dma_tx_callback_register(struct mtk_uart_controller *ctlr,
						uart_dma_done_callback callback,
						void *user_data)
{
	struct mtk_uart_private *mdata;

	if (!ctlr || !callback || !user_data)
		return -UART_EPTR;

	mdata = ctlr->mdata;
	mdata->user_data = user_data;
	mdata->uart_tx_dma_callback = callback;

	return 0;
}

int mtk_mhal_uart_dma_rx_callback_register(struct mtk_uart_controller *ctlr,
						uart_dma_done_callback callback,
						void *user_data)
{
	struct mtk_uart_private *mdata;

	if (!ctlr || !callback || !user_data)
		return -UART_EPTR;

	mdata = ctlr->mdata;
	mdata->user_data = user_data;
	mdata->uart_rx_dma_callback = callback;

	return 0;
}

static void _mtk_mhal_uart_dma_tx_callback(void *data)
{
	struct mtk_uart_controller *ctlr = data;
	struct mtk_uart_private *mdata = ctlr->mdata;

	uart_debug("_mtk_mhal_uart_dma_tx_callback\n");

	mtk_mhal_uart_stop_dma_tx(ctlr);
	mdata->uart_tx_dma_callback(mdata->user_data);
}

static void _mtk_mhal_uart_dma_rx_callback(void *data)
{
	struct mtk_uart_controller *ctlr = data;
	struct mtk_uart_private *mdata = ctlr->mdata;

	uart_debug("_mtk_mhal_uart_dma_rx_callback\n");

	mtk_mhal_uart_stop_dma_rx(ctlr);
	mdata->uart_rx_dma_callback(mdata->user_data);
}

int mtk_mhal_uart_allocate_dma_tx_ch(struct mtk_uart_controller *ctlr)
{
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	ret = osai_dma_allocate_chan(ctlr->mdata->dma_tx_ch);
	if (ret)
		return -UART_ENXIO;

	return 0;
}

int mtk_mhal_uart_allocate_dma_rx_ch(struct mtk_uart_controller *ctlr)
{
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	ret = osai_dma_allocate_chan(ctlr->mdata->dma_rx_ch);
	if (ret)
		return -UART_ENXIO;

	return 0;
}

int mtk_mhal_uart_release_dma_tx_ch(struct mtk_uart_controller *ctlr)
{
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	ret = osai_dma_release_chan(ctlr->mdata->dma_tx_ch);
	if (ret)
		return -UART_ENXIO;

	return 0;
}

int mtk_mhal_uart_release_dma_rx_ch(struct mtk_uart_controller *ctlr)
{
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	ret = osai_dma_release_chan(ctlr->mdata->dma_rx_ch);
	if (ret)
		return -UART_ENXIO;

	return 0;
}

int mtk_mhal_uart_dma_tx_config(struct mtk_uart_controller *ctlr)
{
	struct osai_dma_config tx_config;
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	uart_debug("mtk_mhal_uart_dma_tx_config\n");

	memset(&tx_config, 0, sizeof(tx_config));

	ctlr->mdata->tx_dma = osai_get_phyaddr(ctlr->mdata->tx_buf);

	if (ctlr->vff_dma_mode) {
		/** for Virtual FIFO DMA */
		tx_config.vfifo_thrsh = 1;
		tx_config.vfifo_size = 0x4000;
		tx_config.interrupt_flag = OSAI_DMA_INT_VFIFO_THRESHOLD;
	} else {
		/** for Half-size DMA */
		tx_config.interrupt_flag = OSAI_DMA_INT_COMPLETION;
		tx_config.count = ctlr->mdata->tx_len;
	}

	tx_config.transize = OSAI_DMA_SIZE_BYTE;
	tx_config.dir = 0;

	tx_config.src_addr = ctlr->mdata->tx_dma;
	tx_config.dst_addr = (u32)(ctlr->base);

	tx_config.done_callback_data = ctlr;
	tx_config.done_callback = _mtk_mhal_uart_dma_tx_callback;

	ret = osai_dma_config(ctlr->mdata->dma_tx_ch, &tx_config);
	if (ret)
		return -UART_ENXIO;

	uart_debug("tx_ch: %d, src_addr: 0x%x, dst_addr: 0x%x\n",
		ctlr->mdata->dma_tx_ch, tx_config.src_addr, tx_config.dst_addr);

	return 0;
}

int mtk_mhal_uart_dma_rx_config(struct mtk_uart_controller *ctlr)
{
	struct osai_dma_config rx_config;
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	uart_debug("mtk_mhal_uart_dma_rx_config\n");

	memset(&rx_config, 0, sizeof(rx_config));

	ctlr->mdata->rx_dma = osai_get_phyaddr(ctlr->mdata->rx_buf);

	if (ctlr->vff_dma_mode) {
		/** for Virtual FIFO DMA */
		rx_config.interrupt_flag = OSAI_DMA_INT_VFIFO_THRESHOLD;
		rx_config.vfifo_thrsh = ctlr->mdata->rx_len;
		rx_config.vfifo_size = 0x4000;
	} else {
		/** for Half-size DMA */
		rx_config.interrupt_flag = OSAI_DMA_INT_COMPLETION;
		rx_config.count = ctlr->mdata->rx_len;
	}

	rx_config.transize = OSAI_DMA_SIZE_BYTE;
	rx_config.dir = 1;

	rx_config.src_addr = (u32)(ctlr->base);
	rx_config.dst_addr = ctlr->mdata->rx_dma;

	rx_config.done_callback_data = ctlr;
	rx_config.done_callback = _mtk_mhal_uart_dma_rx_callback;

	ret = osai_dma_config(ctlr->mdata->dma_rx_ch, &rx_config);
	if (ret)
		return -UART_ENXIO;

	uart_debug("rx_ch: %d, src_addr: 0x%x, dst_addr: 0x%x\n",
		ctlr->mdata->dma_rx_ch, rx_config.src_addr, rx_config.dst_addr);

	return 0;
}

int mtk_mhal_uart_start_dma_tx(struct mtk_uart_controller *ctlr)
{
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	uart_debug("mtk_mhal_uart_start_dma_tx\n");

	if (ctlr->vff_dma_mode) {
		ret = osai_dma_update_vfifo_swptr(
				ctlr->mdata->dma_tx_ch,
				ctlr->mdata->tx_len);
		if (ret)
			return -UART_ENXIO;
	}

	ret = osai_dma_clr_dreq(ctlr->mdata->dma_tx_ch);
	if (ret)
		return -UART_ENXIO;

	ret = osai_dma_start(ctlr->mdata->dma_tx_ch);
	if (ret)
		return -UART_ENXIO;

	mtk_hdl_uart_tx_dma_handshake(ctlr->base, true);

	return 0;
}

int mtk_mhal_uart_start_dma_rx(struct mtk_uart_controller *ctlr)
{
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	uart_debug("mtk_mhal_uart_start_dma_rx\n");

	ret = osai_dma_clr_dreq(ctlr->mdata->dma_rx_ch);
	if (ret)
		return -UART_ENXIO;


	ret = osai_dma_start(ctlr->mdata->dma_rx_ch);
	if (ret)
		return -UART_ENXIO;

	mtk_hdl_uart_rx_dma_handshake(ctlr->base, true);

	return 0;
}

int mtk_mhal_uart_stop_dma_tx(struct mtk_uart_controller *ctlr)
{
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_tx_dma_handshake(ctlr->base, false);

	ret = osai_dma_stop(ctlr->mdata->dma_tx_ch);
	if (ret)
		return -UART_ENXIO;

	return 0;
}

int mtk_mhal_uart_stop_dma_rx(struct mtk_uart_controller *ctlr)
{
	int ret;

	if (!ctlr)
		return -UART_EPTR;

	mtk_hdl_uart_rx_dma_handshake(ctlr->base, false);

	ret = osai_dma_stop(ctlr->mdata->dma_rx_ch);
	if (ret)
		return -UART_ENXIO;

	return 0;
}

int mtk_mhal_uart_update_dma_tx_info(struct mtk_uart_controller *ctlr)
{
	u32 fifo_cnt;

	if (!ctlr)
		return -UART_EPTR;

	if (ctlr->vff_dma_mode) {
		fifo_cnt = osai_dma_get_param(ctlr->mdata->dma_tx_ch,
					OSAI_DMA_PARAM_VFF_FIFO_CNT);
		ctlr->mdata->tx_size = ctlr->mdata->tx_len - fifo_cnt;

	} else
		fifo_cnt = osai_dma_get_param(ctlr->mdata->dma_tx_ch,
					OSAI_DMA_PARAM_RLCT);

	ctlr->mdata->tx_size = ctlr->mdata->tx_len - fifo_cnt;

	return 0;
}

int mtk_mhal_uart_update_dma_rx_info(struct mtk_uart_controller *ctlr)
{
	u32 fifo_cnt;

	if (!ctlr)
		return -UART_EPTR;

	if (ctlr->vff_dma_mode) {
		fifo_cnt = osai_dma_get_param(ctlr->mdata->dma_rx_ch,
					OSAI_DMA_PARAM_VFF_FIFO_CNT);
		ctlr->mdata->rx_size = fifo_cnt;
	} else {
		fifo_cnt = osai_dma_get_param(ctlr->mdata->dma_rx_ch,
					OSAI_DMA_PARAM_RLCT);
		ctlr->mdata->rx_size = ctlr->mdata->rx_len - fifo_cnt;
	}

	return 0;
}

