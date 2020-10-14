/*
 * (C) 2005-2019 MediaTek Inc. All rights reserved.
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

#include "hdl_spim.h"
#include "mhal_spim.h"
#include "mhal_osai.h"

/*
  * mt3620 spim hw the max transfer len is 32bytes in half duplex,
  * 16bytes in full duplex.
  * But SW can workaround:support hw multi-transfer in one sw spi_transfer
  * by dma mode.
  * MAX_SW_LOOP_CNT is the config that SW can support how many multi-transfer
  * in one sw spi_transfer.
  * Why we alloc mdata->tx(rx)_buf here?
  * The reason is:
  * when can osai_mem_alloc in _mtk_mhal_spim_fill_dma_buffer,
  * osai_mem_free should be called in dma_rx_callback. But osai_mem_free is not
  * safety in freeRTos, so SW malloc memory firstly
  * in mtk_mhal_spim_alloc_controller.
*/
#define MAX_SW_LOOP_CNT		1

enum spi_dma_transfer_direction {
	DMA_MEM_TO_DEV = 0,
	DMA_DEV_TO_MEM = 1,
};

int mtk_mhal_spim_dump_reg(struct mtk_spi_controller *ctlr)
{
	if (!ctlr) {
		spim_err("%s ctlr is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!ctlr->base) {
		spim_err("%s ctlr->base is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	mtk_hdl_spim_dump_reg(ctlr->base);

	return 0;
}

int mtk_mhal_spim_clear_irq_status(struct mtk_spi_controller *ctlr)
{
	if (!ctlr) {
		spim_err("%s ctlr is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!ctlr->base) {
		spim_err("%s ctlr->base is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	mtk_hdl_spim_clear_irq_status(ctlr->base);

	return 0;
}

int mtk_mhal_spim_fifo_handle_rx(struct mtk_spi_controller *ctlr,
				  struct mtk_spi_transfer *xfer)
{
		if (!ctlr || !xfer) {
			spim_err("%s ctlr or xfer is NULL\n", __func__);
			return -SPIM_EPTR;
		}

		if (!ctlr->base) {
			spim_err("%s ctlr->base is NULL\n", __func__);
			return -SPIM_EPTR;
		}

		if (!xfer->tx_buf && !xfer->rx_buf) {
			spim_err("%s xfer->tx(rx)_buf is NULL\n", __func__);
			return -SPIM_EPTR;
		}

		if ((!xfer->tx_buf) && xfer->rx_buf) {	/* half duplex */
			if ((xfer->len > MTK_SPIM_MAX_LEN) ||
			    (xfer->len <= 1)) {
				spim_err(
				"FIFO half duplex support 2~33bytes(opcode+data), now %dbytes.\n",
					 xfer->len);
				return -SPIM_ELENGTH;
			}
		}

		if (xfer->tx_buf && xfer->rx_buf) { /* full duplex */
			if ((xfer->len > (MTK_SPIM_MAX_LEN / 2 + 1)) ||
				(xfer->len <= 1)) {
				spim_err(
				"FIFO full duplex support 2~17bytes(opcode+data), now %dbytes.\n",
					 xfer->len);
				return -SPIM_ELENGTH;
			}
		}

	mtk_hdl_spim_fifo_handle_rx(ctlr->base, xfer->tx_buf,
				    xfer->rx_buf, xfer->len);

	return 0;
}

int mtk_mhal_spim_prepare_hw(struct mtk_spi_controller *ctlr,
			      struct mtk_spi_config *config)
{
	if (!ctlr) {
		spim_err("%s ctlr is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!ctlr->base) {
		spim_err("%s ctlr->base is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!config) {
		spim_err("%s config is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	mtk_hdl_spim_sw_reset(ctlr->cg_base);
	mtk_hdl_spim_prepare_hw(ctlr->base,
				config->cpol, config->cpha,
				config->tx_mlsb, config->rx_mlsb,
				config->slave_sel);

	return 0;
}

int mtk_mhal_spim_prepare_transfer(struct mtk_spi_controller *ctlr,
				    struct mtk_spi_transfer *xfer)
{
	u32 is_full_duplex;

	if (!ctlr || !xfer) {
		spim_err("%s ctlr or xfer is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!ctlr->base) {
		spim_err("%s ctlr->base is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!xfer->tx_buf && !xfer->rx_buf) {
		spim_err("%s xfer->tx(rx)_buf is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	ctlr->current_xfer = xfer;

	if (xfer->speed_khz > 40 * 1000) {
		spim_err("spim support bus CLK <= 40Mhz, now %d khz.\n",
			xfer->speed_khz);
		return -SPIM_EPTR;
	}

	if ((xfer->use_dma != 0) && (xfer->use_dma != 1)) {
		spim_err("spim xfer->use_dma setting wrong(%d)\n",
			   xfer->use_dma);
		return -SPIM_EPTR;
	}

	if (xfer->tx_buf && xfer->rx_buf)
		is_full_duplex = 1;
	else
		is_full_duplex = 0;

	mtk_hdl_spim_prepare_transfer(ctlr->base, xfer->speed_khz,
				      is_full_duplex);

	return 0;
}

int mtk_mhal_spim_fifo_transfer_one(struct mtk_spi_controller *ctlr,
				    struct mtk_spi_transfer *xfer)
{
	u32 opcode, tx_enable, rx_enable;

	if (!ctlr || !xfer) {
		spim_err("%s ctlr or xfer is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!ctlr->base) {
		spim_err("%s ctlr->base is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!xfer->tx_buf && !xfer->rx_buf) {
		spim_err("%s xfer->tx(rx)_buf is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (xfer->tx_buf && xfer->rx_buf) {
		if ((xfer->len > (MTK_SPIM_MAX_LEN / 2 + 1)) ||
		    (xfer->len <= 1)) {
			spim_err(
			"FIFO full duplex support 2~17bytes(opcode+data), now %dbytes.\n",
			     xfer->len);
			return -SPIM_ELENGTH;
		}
	} else {
		if ((xfer->len > MTK_SPIM_MAX_LEN) || (xfer->len <= 1)) {
			spim_err(
			"FIFO half duplex support 2~33bytes(opcode+data), now %dbytes.\n",
			     xfer->len);
			return -SPIM_ELENGTH;
		}
	}

	mtk_hdl_spim_disable_dma(ctlr->base);

	if (xfer->tx_buf)
		tx_enable = 1;
	else
		tx_enable = 0;
	if (xfer->rx_buf)
		rx_enable = 1;
	else
		rx_enable = 0;

	if (xfer->tx_buf)
		mtk_hdl_spim_print_packet("tx_buf",
					  (u8 *) xfer->tx_buf, xfer->len);
	if (xfer->rx_buf)
		mtk_hdl_spim_print_packet("rx_buf",
					  (u8 *) xfer->rx_buf, xfer->len);

	if (xfer->tx_buf) {
		memcpy(&opcode, xfer->tx_buf, 1);
		opcode &= 0xff;
		spim_debug("tx opcode(0x%x)\n", opcode);
		mtk_hdl_spim_enable_fifo_transfer(ctlr->base, opcode, 0, 1,
						  (u8 *) (xfer->tx_buf + 1),
						  xfer->len - 1, tx_enable,
						  rx_enable);
	}

	if ((!xfer->tx_buf) && xfer->rx_buf) {
		memcpy(&opcode, xfer->rx_buf, 1);
		opcode &= 0xff;
		spim_debug("rx opcode(0x%x)\n", opcode);
		mtk_hdl_spim_enable_fifo_transfer(ctlr->base, opcode, 0, 1,
						  xfer->rx_buf + 1,
						  xfer->len - 1, tx_enable,
						  rx_enable);
	}

	return 0;
}

static void _mtk_mhal_spim_dma_rx_callback(void *data)
{
	int i = 0, cnt = 0, remainder = 0, max_len = 0;
	struct mtk_spi_controller *ctlr = data;
	struct mtk_spi_transfer *xfer = ctlr->current_xfer;
	struct mtk_spi_private *mdata = ctlr->mdata;

	if (!xfer->rx_buf)
		return;

	if (xfer->tx_buf && xfer->rx_buf)
		max_len = MTK_SPIM_MAX_LENGTH_ONE_TRANS_FULL;
	else
		max_len = MTK_SPIM_MAX_LENGTH_ONE_TRANS_HALF;

	cnt = xfer->len / max_len;
	if (xfer->len % max_len)
		remainder = 1;

	osai_invalid_cache(mdata->rx_buf, xfer->len);

	i = 0;
	while (cnt > 0) {
		memcpy(xfer->rx_buf + 1 + i * max_len,
		       mdata->rx_buf + i * (max_len - 1), max_len - 1);
		i++;
		cnt--;
	}
	if (remainder)
		memcpy(xfer->rx_buf + 1 + i * max_len,
		       mdata->rx_buf + i * (max_len - 1),
		       xfer->len % max_len - 1);

	/* call OS-HAL done callback */
	mdata->dma_done_callback(mdata->user_data);
}

/** This function is used to register user's done callback to OS-HAL layer */
int mtk_mhal_spim_dma_done_callback_register(struct mtk_spi_controller *ctlr,
					     spi_dma_done_callback callback,
					     void *user_data)
{
	struct mtk_spi_private *mdata;

	if (!ctlr) {
		spim_err("%s ctlr is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!callback) {
		spim_err("%s callback is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!user_data) {
		spim_err("%s user_data is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	mdata = ctlr->mdata;
	mdata->user_data = user_data;
	mdata->dma_done_callback = callback;

	return 0;
}

int mtk_mhal_spim_allocate_dma_chan(struct mtk_spi_controller *ctlr)
{
	int ret;

	if (!ctlr) {
		spim_err("%s ctlr is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	ret = osai_dma_allocate_chan(ctlr->dma_tx_chan);
	if (ret) {
		spim_err("%s:dma_tx_chan fail.\n", __func__);
		return -SPIM_EBUSY;
	}

	ret = osai_dma_allocate_chan(ctlr->dma_rx_chan);
	if (ret) {
		spim_err("%s:dma_rx_chan fail.\n", __func__);
		return -SPIM_EBUSY;
	}

	return 0;
}

int mtk_mhal_spim_release_dma_chan(struct mtk_spi_controller *ctlr)
{
	int ret;

	if (!ctlr) {
		spim_err("%s ctlr is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	ret = osai_dma_release_chan(ctlr->dma_tx_chan);
	if (ret) {
		spim_err("%s:dma_tx_chan fail.\n", __func__);
		return -SPIM_EBUSY;
	}

	ret = osai_dma_release_chan(ctlr->dma_rx_chan);
	if (ret) {
		spim_err("%s:dma_rx_chan fail.\n", __func__);
		return -SPIM_EBUSY;
	}

	return 0;
}

static int _mtk_mhal_spim_dma_config(struct mtk_spi_controller *ctlr,
				     dma_addr_t dma_addr,
				     u32 dma_len,
				     enum spi_dma_transfer_direction dir)
{
	int is_dma_tx, chan_num, ret;
	struct osai_dma_config config = {0};

	/* 1: spim tx; 0: spim rx */
	is_dma_tx = (dir == DMA_MEM_TO_DEV) ? 1 : 0;

	config.interrupt_flag = OSAI_DMA_INT_COMPLETION;
	config.transize = OSAI_DMA_SIZE_LONG;
	config.dir = dir;
	config.count = dma_len;
	config.done_callback_data = ctlr;

	if (is_dma_tx) {
		/* spi tx */
		chan_num = ctlr->dma_tx_chan;
		config.src_addr = dma_addr;
		config.dst_addr = (u32)SPI_REG_DATAPORT_CR(ctlr->base);
		config.done_callback = NULL;
	} else {
		/* spi rx */
		chan_num = ctlr->dma_rx_chan;
		config.src_addr = (u32)SPI_REG_DATAPORT_CR(ctlr->base);
		config.dst_addr = dma_addr;
		config.done_callback = _mtk_mhal_spim_dma_rx_callback;
	}

	ret = osai_dma_config(chan_num, &config);
	if (ret)
		spim_err("%s:osai_dma_config fail.\n", __func__);

	return ret;
}

static int _mtk_mhal_spim_fill_dma_buffer(struct mtk_spi_controller *ctlr,
					  struct mtk_spi_transfer *xfer)
{
	u32 i, reg_val, cnt = 0, remainder = 0, total_cnt, tmp_len, max_len = 0;
	struct mtk_spi_private *mdata = ctlr->mdata;
	int ret = 0;

	/* tmp buffer init */
	mdata->tx_buf = ctlr->dma_tmp_tx_buf;
	mdata->rx_buf = ctlr->dma_tmp_rx_buf;

	ctlr->current_xfer = xfer;
	mdata->xfer_len = xfer->len;

	if (xfer->tx_buf && xfer->rx_buf)
		max_len = MTK_SPIM_MAX_LENGTH_ONE_TRANS_FULL;
	else
		max_len = MTK_SPIM_MAX_LENGTH_ONE_TRANS_HALF;

	if (xfer->len <= 1) {
		spim_err("xfer len should be opcode[1byte]+data[1~%dbytes]\n",
			max_len);
		return -SPIM_ELENGTH;
	}

	cnt = xfer->len / max_len;
	if (xfer->len % max_len)
		remainder = 1;

	total_cnt = cnt + remainder;

	if (total_cnt > MAX_SW_LOOP_CNT) {
		spim_err("spim support len=%dBytes(<%d)\n",
			max_len * MAX_SW_LOOP_CNT, xfer->len);
		return -SPIM_ELENGTH;
	}

	memset(mdata->tx_buf, 0, MTK_SPIM_DMA_BUFFER_BYTES * total_cnt);

	for (i = 0; i < total_cnt; i++) {
		/* data should be like this:
		 * [1]  opcode,byte1,byte2,byte3...byte16...bytes32
		 * [2]  opcode,byte1,byte2,byte3...byte16...bytes32
		 * [3]  opcode,byte1,byte2,byte3...
		 * and so on
		 */

		/*  get this transfer len */
		if ((i == cnt) && (remainder == 1))
			tmp_len = xfer->len % max_len - 1;
		else
			tmp_len = max_len - 1;

		/* config opcode: 0DW=opcode */
		if (xfer->tx_buf)
			memcpy(mdata->tx_buf + i * MTK_SPIM_DMA_BUFFER_BYTES,
			       xfer->tx_buf + i * max_len, 1);
		else /* rx also need send opcode */
			memcpy(mdata->tx_buf + i * MTK_SPIM_DMA_BUFFER_BYTES,
			       xfer->rx_buf + i * max_len, 1);

		/* config tx data: 1~8DW=tx data */
		if (xfer->tx_buf) {
			memcpy(mdata->tx_buf + 4 +
			       i * MTK_SPIM_DMA_BUFFER_BYTES,
			       xfer->tx_buf + 1 + i * max_len, tmp_len);
		}

		/* config 0x28 reg */
		reg_val = osai_readl(SPI_REG_MASTER(ctlr->base));
		memcpy(mdata->tx_buf + 36 + i * MTK_SPIM_DMA_BUFFER_BYTES,
		       &reg_val, 4);

		/* config  0x2c reg: cmd&mosi_bit_cnt&miso_bit_cnt */
		reg_val = (1 * 8) << SPI_MBCTL_CMD_SHIFT;
		if (xfer->tx_buf)
			reg_val |= (tmp_len * 8) << SPI_MBCTL_TXCNT_SHIFT;
		if (xfer->rx_buf)
			reg_val |= (tmp_len * 8) << SPI_MBCTL_RXCNT_SHIFT;
		memcpy(mdata->tx_buf + 40 + i * MTK_SPIM_DMA_BUFFER_BYTES,
		       &reg_val, 4);

		/* config 0x0 reg */
		reg_val = SPI_CTL_ADDR_SIZE_24BIT | SPI_CTL_START;
		memcpy(mdata->tx_buf + 44 + i * MTK_SPIM_DMA_BUFFER_BYTES,
		       &reg_val, 4);
	}

	mdata->tx_dma = osai_get_phyaddr(mdata->tx_buf);
	osai_clean_cache(mdata->tx_buf,
			 MTK_SPIM_DMA_BUFFER_BYTES * MAX_SW_LOOP_CNT);

	if (xfer->rx_buf) {
		memset(mdata->rx_buf, 0, xfer->len - total_cnt);
		mdata->rx_dma = osai_get_phyaddr(mdata->rx_buf);

		ret = _mtk_mhal_spim_dma_config(ctlr, mdata->rx_dma,
					  xfer->len - total_cnt,
					  DMA_DEV_TO_MEM);
		if (ret) {
			spim_err("%s:rx_dma_config fail.\n", __func__);
			goto err_exit;
		}
	}

	ret = _mtk_mhal_spim_dma_config(ctlr, mdata->tx_dma,
				  MTK_SPIM_DMA_BUFFER_BYTES * total_cnt,
				  DMA_MEM_TO_DEV);
	if (ret) {
		spim_err("%s:tx_dma_config fail.\n", __func__);
		goto err_exit;
	}

	ret = osai_dma_start(ctlr->dma_tx_chan);
	if (ret) {
		spim_err("%s:osai_dma_start tx fail.\n", __func__);
		goto err_exit;
	}

	if (xfer->rx_buf) {
		ret = osai_dma_start(ctlr->dma_rx_chan);
		if (ret) {
			spim_err("%s:osai_dma_start rx fail.\n", __func__);
			goto err_exit;
		}
	}

err_exit:
	return ret;
}

int mtk_mhal_spim_dma_transfer_one(struct mtk_spi_controller *ctlr,
				   struct mtk_spi_transfer *xfer)
{
	int ret;

	if (!ctlr || !xfer) {
		spim_err("%s ctlr or xfer is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!ctlr->base) {
		spim_err("%s ctlr->base is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!xfer->tx_buf && !xfer->rx_buf) {
		spim_err("%s xfer->tx(rx)_buf is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	mtk_hdl_spim_enable_dma(ctlr->base);
	ret = _mtk_mhal_spim_fill_dma_buffer(ctlr, xfer);

	return ret;
}

int mtk_mhal_spim_enable_clk(struct mtk_spi_controller *ctlr)
{
	if (!ctlr) {
		spim_err("%s ctlr is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!ctlr->cg_base) {
		spim_err("%s ctlr->cg_base is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	mtk_hdl_spim_enable_clk(ctlr->cg_base);

	return 0;
}

int mtk_mhal_spim_disable_clk(struct mtk_spi_controller *ctlr)
{
	if (!ctlr) {
		spim_err("%s ctlr is NULL\n", __func__);
		return -SPIM_EPTR;
	}

	if (!ctlr->cg_base) {
		spim_err("%s ctlr->cg_base is NULL\n", __func__);
		return -SPIM_EPTR;
	}
	mtk_hdl_spim_disable_clk(ctlr->cg_base);

	return 0;
}
