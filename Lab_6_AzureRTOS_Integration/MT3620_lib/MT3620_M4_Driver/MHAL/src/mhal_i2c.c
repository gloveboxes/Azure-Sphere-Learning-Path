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

#include "mhal_i2c.h"
#include "hdl_i2c.h"

enum i2c_dma_tran_dir {
	DMA_MEM_TO_DEV = 0,
	DMA_DEV_TO_MEM = 1,
};

int mtk_mhal_i2c_init_hw(struct mtk_i2c_controller *i2c)
{
	if (!i2c)
		return -I2C_EPTR;

	if (i2c->cg_base == 0x00 || i2c->base == 0x00)
		return -I2C_EINVAL;

	if ((i2c->i2c_mode != I2C_MASTER_MODE) &&
		(i2c->i2c_mode != I2C_SLAVE_MODE))
		return -I2C_EINVAL;

	mtk_hdl_i2c_init_hw(i2c->base, i2c->cg_base);

	if (i2c->i2c_mode == I2C_MASTER_MODE) {
		if (i2c->i2c_speed)
			mtk_mhal_i2c_init_speed(i2c, i2c->i2c_speed);
		else
			mtk_mhal_i2c_init_speed(i2c, I2C_SCL_100kHz);
	} else {
		if (i2c->slave_addr)
			mtk_hdl_i2c_init_slave_addr(i2c->base, i2c->slave_addr);
		else
			mtk_hdl_i2c_init_slave_addr(i2c->base, 0x20);
	}

	osai_dma_reset(i2c->dma_rx_chan);
	osai_dma_reset(i2c->dma_tx_chan);
	return I2C_OK;
}

int mtk_mhal_i2c_enable_clk(struct mtk_i2c_controller *i2c)
{
	if (!i2c)
		return -I2C_EPTR;

	if (i2c->cg_base == 0x00 || i2c->base == 0x00)
		return -I2C_EINVAL;

	mtk_hdl_i2c_enable_clk(i2c->cg_base);

	return I2C_OK;
}

int mtk_mhal_i2c_disable_clk(struct mtk_i2c_controller *i2c)
{
	if (!i2c)
		return -I2C_EPTR;

	if (i2c->cg_base == 0x00 || i2c->base == 0x00)
		return -I2C_EINVAL;

	mtk_hdl_i2c_disable_clk(i2c->cg_base);

	return I2C_OK;
}

int mtk_mhal_i2c_dump_register(struct mtk_i2c_controller *i2c)
{
	if (!i2c) {
		i2c_err("mtk_i2c_controller pointer is NULL\n");
		return -I2C_EPTR;
	}

	if (i2c->cg_base == 0x00 || i2c->base == 0x00)
		return -I2C_EINVAL;

	mtk_hdl_i2c_dump(i2c->base);

	return I2C_OK;
}

static int _mtk_mhal_scl_freq_to_phase(enum i2c_speed_kHz eFreq, u32 oper_freq)
{
	u16 phase = 0;
	u16 remainder = 0;

	switch (eFreq) {
	case I2C_SCL_50kHz:
		phase = ((oper_freq / I2C_50kHz) / 4) - 1;
		remainder = ((oper_freq / I2C_50kHz) % 4);
		break;

	case I2C_SCL_100kHz:
		phase = ((oper_freq / I2C_100kHz) / 4) - 1;
		remainder = ((oper_freq / I2C_100kHz) % 4);
		break;

	case I2C_SCL_200kHz:
		phase = ((oper_freq / I2C_200kHz) / 4) - 1;
		remainder = ((oper_freq / I2C_200kHz) % 4);
		break;

	case I2C_SCL_400kHz:
		phase = ((oper_freq / I2C_400kHz) / 4) - 1;
		remainder = ((oper_freq / I2C_400kHz) % 4);
		break;

	case I2C_SCL_1000kHz:
		phase = ((oper_freq / I2C_1000kHz) / 4) - 1;
		remainder = ((oper_freq / I2C_1000kHz) % 4);
		break;

	default:
		return -I2C_EINVAL;
	}

	return ((remainder << 8) + phase);
}

int _mtk_mhal_i2c_init_speed(struct mtk_i2c_controller *i2c,
			   enum i2c_speed_kHz speed_kHz)
{
	int phase_val = 0;
	u16 phase_val0 = 0;
	u16 phase_val1 = 0;
	u16 phase_val2 = 0;
	u16 phase_val3 = 0;
	u16 phase_temp = 0;
	u8 fd_mode = 0;
	u8 ft0 = 0;
	u8 ft1 = 0;
	u8 ft2 = 0;
	u8 ft3 = 0;
	u16 phase1_adj = 0;
	u16 reg_val;

	phase_val = _mtk_mhal_scl_freq_to_phase(speed_kHz,
						I2C_SOURCE_CLK_40MHZ);
	if (phase_val < 0)
		return -I2C_EINVAL;

	fd_mode = phase_val >> 8;
	phase_val = phase_val & 0xff;

	switch (fd_mode) {
	case 1:
		ft0 = 1;
		break;
	case 2:
		ft0 = ft2 = 1;
		break;
	case 3:
		ft0 = ft2 = ft3 = 1;
		break;
	default:
		break;
	}

	if (speed_kHz == I2C_SCL_400kHz) {
		phase_val0 = phase_val + ft0 + 5;
		phase_val2 = phase_val + ft2 - 5;
	} else {
		phase_val0 = phase_val + ft0 + 3;
		phase_val2 = phase_val + ft2 - 3;
	}

	phase_val1 = phase_val + ft1;
	phase_val3 = phase_val + ft3;

	reg_val = mtk_hdl_i2c_get_pad_con0(i2c->base);

	if (reg_val & I2C_CLK_SYNC_EN)
		phase1_adj += 2;

	if (reg_val & I2C_DE_CNT_MASK)
		phase1_adj += ((reg_val & I2C_DE_CNT_MASK) + 4);

	if (phase_val1 <= phase1_adj) {
		phase_temp = phase1_adj - (phase_val1 - 1);
		phase_val1 = 1;

		if (phase_val2 <= phase_temp) {
			phase_temp -= (phase_val2 - 1);
			phase_val2 = 1;
			phase_val0 -= phase_temp;
		} else
			phase_val2 -= phase_temp;
	} else
		phase_val1 -= phase1_adj;

	mtk_hdl_i2c_init_speed(i2c->base, phase_val0,
			phase_val1, phase_val2, phase_val3);

	return I2C_OK;
}


int mtk_mhal_i2c_init_speed(struct mtk_i2c_controller *i2c,
			    enum i2c_speed_kHz speed_khz)
{
	int ret;

	if (!i2c) {
		i2c_err("mtk_i2c_controller pointer is NULL\n");
		return -I2C_EPTR;
	}

	if (i2c->cg_base == 0x00 || i2c->base == 0x00)
		return -I2C_EINVAL;

	ret = _mtk_mhal_i2c_init_speed(i2c, speed_khz);
	if (ret)
		i2c_err("i2c speed initialization failed\n");
	else
		i2c->i2c_speed = speed_khz;

	return ret;
}

void _mtk_mhal_i2c_push_data_to_fifo(struct mtk_i2c_controller *i2c)
{
	u16 len;
	u8 *buf;
	struct i2c_msg *msgs;
	enum i2c_trans_mode i2c_mode;

	msgs = i2c->msg;
	len = msgs->len;
	buf = msgs->buf;
	i2c_mode = i2c->i2c_mode;

	if ((i2c_mode == I2C_MASTER_MODE) && (i2c->op != I2C_RD))
		mtk_hdl_i2c_push_data_to_fifo(i2c->base, buf, len,
					      I2C_MASTER_MODE);
	else if ((i2c_mode == I2C_SLAVE_MODE) && (msgs->flags == I2C_SLAVE_TX))
		mtk_hdl_i2c_push_data_to_fifo(i2c->base, buf, len,
					      I2C_SLAVE_TX);
}

int mtk_mhal_i2c_irq_handle(struct mtk_i2c_controller *i2c)
{
	u16 ack_len;

	if (!i2c) {
		i2c_err("mtk_i2c_controller pointer is NULL\n");
		return -I2C_EPTR;
	}

	if (i2c->cg_base == 0x00 || i2c->base == 0x00)
		return -I2C_EINVAL;

	if (i2c->i2c_mode == I2C_MASTER_MODE) {
		if (i2c->op == I2C_RD)
			ack_len = i2c->msg_num;
		else
			ack_len = i2c->msg_num + i2c->msg->len;

		if (ack_len > I2C_FIFO_MAX_LEN)
			ack_len = I2C_FIFO_MAX_LEN;

		mtk_hdl_i2c_master_irq_handle(i2c->base, &i2c->irq_stat,
					      i2c->msg_num, ack_len);
		if (((!i2c->dma_en) || (i2c->op == I2C_WR) || (i2c->irq_stat &
		(I2C_ARB_LOSE | I2C_ID_ACKERR | I2C_DATA_ACKERR))) &&
		(i2c->irq_stat & I2C_MM_INT_STA))
			return I2C_TRANS_DONE;
		else
			return I2C_WAIT_DMA;
	} else if (i2c->i2c_mode == I2C_SLAVE_MODE) {
		mtk_hdl_i2c_slave_irq_handle(i2c->base, &i2c->irq_stat);
		if (((!i2c->dma_en) || (i2c->msg->flags == I2C_SLAVE_TX)) &&
			(i2c->irq_stat & I2C_S_INT_STA))
			return I2C_TRANS_DONE;
		else
			return I2C_WAIT_DMA;
	} else
		return -I2C_EINVAL;
}

int mtk_mhal_i2c_result_handle(struct mtk_i2c_controller *i2c)
{
	struct i2c_msg *msgs = NULL;
	struct mtk_i2c_private *mdata = NULL;
	u8 *data_buf;
	u16 data_len;
	int ret = I2C_OK;

	if (!i2c) {
		i2c_err("mtk_i2c_controller pointer is NULL\n");
		return -I2C_EPTR;
	}

	if (i2c->cg_base == 0x00 || i2c->base == 0x00)
		return -I2C_EINVAL;

	msgs = i2c->msg;
	mdata = i2c->mdata;
	if ((msgs == NULL) || (mdata == NULL)) {
		i2c_err("msgs or mdata parameter is NULL\n");
		return -I2C_EPTR;
	}

	if (i2c->i2c_mode == I2C_MASTER_MODE) {
		if (i2c->irq_stat & I2C_ARB_LOSE) {
			i2c_err("addr: %x, transfer ARB_LOSE error\n",
				 msgs->addr);
			ret = -I2C_EBUSY;
			goto err_exit;
		}

		if (i2c->irq_stat & I2C_ID_ACKERR) {
			/* i2c_err("addr: %x, slave id ACK error\n", msgs->addr); */

			ret = -I2C_ENXIO;
			goto err_exit;
		}

		if (i2c->irq_stat & I2C_DATA_ACKERR) {
			i2c_err("addr: %x, data ACK error\n", msgs->addr);

			ret = -I2C_ENXIO;
			goto err_exit;
		}

		if (!i2c->dma_en && i2c->op != I2C_WR) {
			if (i2c->op == I2C_RD) {
				data_buf = msgs->buf;
				data_len = msgs->len;
			} else {
				data_buf = (msgs + 1)->buf;
				data_len = (msgs + 1)->len;
			}

			ret = mtk_hdl_i2c_master_get_data(i2c->base, data_buf,
							   data_len);
			if (ret < 0)
				goto err_exit;
		}

		ret = mtk_hdl_i2c_master_check_fifo_sta(i2c->base);
		if (ret < 0)
			goto err_exit;

	} else if (i2c->i2c_mode == I2C_SLAVE_MODE) {
		if (i2c->irq_stat & I2C_S_FIFO_CHECK) {
			i2c_err("i2c slave fifo abnormal, fifo_status: 0x%x\n",
				i2c->irq_stat);
			return -I2C_EFIFO;
		}

		if (!i2c->dma_en && msgs->flags == I2C_SLAVE_RX) {
			ret = mtk_hdl_i2c_slave_get_data(i2c->base, msgs->buf,
							 msgs->len);
			if (ret < 0)
				return ret;
		}

		ret = mtk_hdl_i2c_slave_check_fifo_sta(i2c->base);
		if (ret < 0)
			return ret;
	} else
		ret = -I2C_EINVAL;

err_exit:

	return ret;
}

/** This function is used to register user's done callback to OS-HAL layer */
int mtk_mhal_i2c_dma_done_callback_register(struct mtk_i2c_controller *i2c,
					    i2c_dma_done_callback callback,
					    void *user_data)
{
	struct mtk_i2c_private *mdata;

	if (!i2c) {
		osai_print("%s ctlr is NULL\n", __func__);
		return -I2C_EPTR;
	}

	if (!callback) {
		osai_print("%s callback is NULL\n", __func__);
		return -I2C_EPTR;
	}

	if (!user_data) {
		osai_print("%s user_data is NULL\n", __func__);
		return -I2C_EPTR;
	}

	mdata = i2c->mdata;
	mdata->user_data = user_data;
	mdata->dma_done_callback = callback;

	return 0;
}

void _mtk_mhal_i2c_dma_callback(void *data)
{
	struct mtk_i2c_controller *i2c;
	struct mtk_i2c_private *mdata;

	i2c = (struct mtk_i2c_controller *)data;
	mdata = i2c->mdata;

	mdata->dma_done_callback(mdata->user_data);
}

int _mtk_mhal_i2c_fill_mdata(struct mtk_i2c_controller *i2c)
{
	struct mtk_i2c_private *mdata = i2c->mdata;
	struct i2c_msg *msg = i2c->msg;

	if (i2c->op == I2C_WR || msg->flags == I2C_SLAVE_TX) {
		mdata->tx_buff = msg->buf;
		if (mdata->tx_buff == NULL)
			return -I2C_EPTR;

		mdata->tx_len = msg->len;
		mdata->tx_dma_addr = osai_get_phyaddr(mdata->tx_buff);
	} else if (i2c->op == I2C_RD || msg->flags == I2C_SLAVE_RX) {
		mdata->rx_buff = msg->buf;
		if (mdata->rx_buff == NULL)
			return -I2C_EPTR;

		memset(mdata->rx_buff, 0, msg->len);

		mdata->rx_len = msg->len;
		mdata->rx_dma_addr = osai_get_phyaddr(mdata->rx_buff);
	} else if (i2c->op == I2C_WRRD) {
		mdata->tx_buff = msg->buf;
		mdata->rx_buff = (msg+1)->buf;
		if (!mdata->tx_buff || !mdata->rx_buff)
			return -I2C_EPTR;

		memset(mdata->rx_buff, 0, (msg+1)->len);

		mdata->tx_len = msg->len;
		mdata->rx_len = (msg+1)->len;
		mdata->tx_dma_addr = osai_get_phyaddr(mdata->tx_buff);
		mdata->rx_dma_addr = osai_get_phyaddr(mdata->rx_buff);
	}

	return I2C_OK;
}

int mtk_mhal_i2c_request_dma(struct mtk_i2c_controller *i2c)
{
	int ret = I2C_OK;

	if (!i2c) {
		i2c_err("mtk_i2c_controller pointer is NULL\n");
		return -I2C_EPTR;
	}

	ret = osai_dma_allocate_chan(i2c->dma_tx_chan);
	if (ret)
		return -I2C_EBUSY;

	ret = osai_dma_allocate_chan(i2c->dma_rx_chan);
	if (ret)
		return -I2C_EBUSY;

	return I2C_OK;
}

int mtk_mhal_i2c_release_dma(struct mtk_i2c_controller *i2c)
{
	int ret = I2C_OK;

	if (!i2c) {
		i2c_err("mtk_i2c_controller pointer is NULL\n");
		return -I2C_EPTR;
	}

	ret = osai_dma_release_chan(i2c->dma_tx_chan);
	if (ret)
		return ret;

	ret = osai_dma_release_chan(i2c->dma_rx_chan);
	if (ret)
		return ret;

	return I2C_OK;
}

int _mtk_mhal_i2c_dma(struct mtk_i2c_controller *i2c)
{
	struct osai_dma_config i2c_dma;
	struct mtk_i2c_private *mdata = i2c->mdata;
	struct i2c_msg *msgs = i2c->msg;
	int size = 0;
	int ret = I2C_OK;

	ret = _mtk_mhal_i2c_fill_mdata(i2c);
	if (ret < I2C_OK)
		return ret;

	size = sizeof(struct osai_dma_config);
	memset(&i2c_dma, 0, size);

	if (i2c->op == I2C_WR || msgs->flags == I2C_SLAVE_TX) {
		/*Data from memory to peripheral*/
		i2c_dma.interrupt_flag = OSAI_DMA_INT_COMPLETION;
		i2c_dma.dir = DMA_MEM_TO_DEV;
		i2c_dma.src_addr = (u32)mdata->tx_dma_addr;
		i2c_dma.count = mdata->tx_len;
		i2c_dma.transize = OSAI_DMA_SIZE_BYTE;
		i2c_dma.done_callback = NULL;
		i2c_dma.done_callback_data = NULL;
		if (i2c->op == I2C_WR)
			i2c_dma.dst_addr = (u32)i2c->base +
					   OFFSET_MM_FIFO_DATA;
		else
			i2c_dma.dst_addr = (u32)i2c->base +
					   OFFSET_S_FIFO_DATA;

		osai_clean_cache(mdata->tx_buff, mdata->tx_len);

		ret = osai_dma_config(i2c->dma_tx_chan, &i2c_dma);
		if (ret) {
			i2c_err("i2c config TX dma fail, ret = %d\n", ret);
			return ret;
		}

		ret = osai_dma_start(i2c->dma_tx_chan);
		if (ret) {
			i2c_err("i2c start TX dma fail, ret = %d\n", ret);
			return ret;
		}
	} else if (i2c->op == I2C_RD || msgs->flags == I2C_SLAVE_RX) {
		/*Data from peripheral to memory*/
		i2c_dma.interrupt_flag = OSAI_DMA_INT_COMPLETION;
		i2c_dma.dir = DMA_DEV_TO_MEM;
		i2c_dma.dst_addr = (u32)mdata->rx_dma_addr;
		i2c_dma.count = mdata->rx_len;
		i2c_dma.transize = OSAI_DMA_SIZE_BYTE;
		i2c_dma.done_callback = _mtk_mhal_i2c_dma_callback;
		i2c_dma.done_callback_data = (void *)i2c;

		if (i2c->op == I2C_RD)
			i2c_dma.src_addr = (u32)i2c->base +
					   OFFSET_MM_FIFO_DATA;
		else
			i2c_dma.src_addr = (u32)i2c->base +
					   OFFSET_S_FIFO_DATA;

		osai_clean_cache(mdata->rx_buff, mdata->rx_len);

		ret = osai_dma_config(i2c->dma_rx_chan, &i2c_dma);
		if (ret) {
			i2c_err("i2c config RX dma fail, ret = %d\n", ret);
			return ret;
		}

		ret = osai_dma_start(i2c->dma_rx_chan);
		if (ret) {
			i2c_err("i2c start RX dma fail, ret = %d\n", ret);
			return ret;
		}

		osai_invalid_cache(mdata->rx_buff, mdata->rx_len);
	} else if (i2c->op == I2C_WRRD) {
		i2c_dma.interrupt_flag = OSAI_DMA_INT_COMPLETION;
		i2c_dma.dir = DMA_MEM_TO_DEV;
		i2c_dma.src_addr = (u32)mdata->tx_dma_addr;
		i2c_dma.dst_addr = (u32)i2c->base + OFFSET_MM_FIFO_DATA;
		i2c_dma.count = mdata->tx_len;
		i2c_dma.transize = OSAI_DMA_SIZE_BYTE;
		i2c_dma.done_callback = NULL;
		i2c_dma.done_callback_data = NULL;

		osai_clean_cache(mdata->tx_buff, mdata->tx_len);

		ret = osai_dma_config(i2c->dma_tx_chan, &i2c_dma);
		if (ret) {
			i2c_err("i2c WRRD config TX dma fail, ret = %d\n", ret);
			return ret;
		}

		ret = osai_dma_start(i2c->dma_tx_chan);
		if (ret) {
			i2c_err("i2c WRRD start TX dma fail, ret = %d\n", ret);
			return ret;
		}

		i2c_dma.interrupt_flag = OSAI_DMA_INT_COMPLETION;
		i2c_dma.dir = DMA_DEV_TO_MEM;
		i2c_dma.src_addr = (u32)i2c->base + OFFSET_MM_FIFO_DATA;
		i2c_dma.dst_addr = (u32)mdata->rx_dma_addr;
		i2c_dma.count = mdata->rx_len;
		i2c_dma.transize = OSAI_DMA_SIZE_BYTE;
		i2c_dma.done_callback = _mtk_mhal_i2c_dma_callback;
		i2c_dma.done_callback_data = (void *)i2c;

		osai_clean_cache(mdata->rx_buff, mdata->rx_len);
		ret = osai_dma_config(i2c->dma_rx_chan, &i2c_dma);
		if (ret) {
			i2c_err("i2c WRRD config RX dma fail, ret = %d\n", ret);
			return ret;
		}

		ret = osai_dma_start(i2c->dma_rx_chan);
		if (ret) {
			i2c_err("i2c WRRD start RX dma fail, ret = %d\n", ret);
			return ret;
		}

		osai_invalid_cache(mdata->rx_buff, mdata->rx_len);
	}

	return I2C_OK;
}

int mtk_mhal_i2c_trigger_transfer(struct mtk_i2c_controller *i2c)
{
	int ret = I2C_OK;
	int num_cnt;
	struct i2c_msg *msgs;

	if (!i2c) {
		i2c_err("mtk_i2c_controller pointer is NULL\n");
		return -I2C_EPTR;
	}

	msgs = i2c->msg;
	if (!msgs) {
		i2c_err("I2C msg pointer is NULL\n");
		return -I2C_EPTR;
	}

	if (i2c->cg_base == 0x00 || i2c->base == 0x00)
		return -I2C_EINVAL;

	mtk_mhal_i2c_init_hw(i2c);

	/*I2C Master mode*/
	if (i2c->i2c_mode == I2C_MASTER_MODE) {
		if ((msgs->flags != I2C_MASTER_RD) &&
			(msgs->flags != I2C_MASTER_WR)) {
			i2c_err("i2c master not support msg->flag = %d.\n",
				msgs->flags);
			return -I2C_EINVAL;
		}

		for (num_cnt = 0; num_cnt < i2c->msg_num; num_cnt++) {
			if (!msgs[num_cnt].buf) {
				i2c_err("msgs[%d].buf is NULL.\n", num_cnt);
				return -I2C_EPTR;
			}
		}

		if (msgs->addr >= 0x80) {
			i2c_err("Invaild slave address, msgs->addr: 0x%x",
				msgs->addr);
			return -I2C_EINVAL;
		}

		if ((i2c->msg_num == 2) && (msgs[0].addr == msgs[1].addr) &&
			(!(msgs[0].flags & I2C_MASTER_RD) &&
			(msgs[1].flags & I2C_MASTER_RD))) {
			i2c->op = I2C_WRRD;
		} else if (i2c->msg_num == 1) {
			if (msgs->flags & I2C_MASTER_RD)
				i2c->op = I2C_RD;
			else if (msgs->flags & I2C_MASTER_WR)
				i2c->op = I2C_WR;
		} else {
			i2c_err("I2C master unsupport transfer mode!\n");
			return -I2C_EINVAL;
		}

		if ((msgs->len == 0) || ((i2c->op == I2C_WRRD) &&
			(msgs + 1)->len == 0))
			return -I2C_EINVAL;

		if ((msgs->len > I2C_FIFO_MAX_LEN) || ((i2c->op == I2C_WRRD) &&
			((msgs + 1)->len > I2C_FIFO_MAX_LEN)))
			i2c->dma_en = true;

		ret = mtk_hdl_i2c_master_trigger_transfer(i2c->base, i2c->op,
			i2c->msg_num, msgs->addr, msgs->len, (msgs+1)->len);
		if (ret < I2C_OK)
			return ret;

		if (i2c->dma_en)
			_mtk_mhal_i2c_dma(i2c);
		else
			_mtk_mhal_i2c_push_data_to_fifo(i2c);

		mtk_hdl_i2c_master_start(i2c->base, i2c->dma_en);
	/*I2C slave mode*/
	} else if (i2c->i2c_mode == I2C_SLAVE_MODE) {
		if ((msgs->flags != I2C_SLAVE_RX) &&
			(msgs->flags != I2C_SLAVE_TX)) {
			i2c_err("i2c master not support msg->flag = %d.\n",
				msgs->flags);
			return -I2C_EINVAL;
		}

		if (msgs->len == 0)
			return -I2C_EINVAL;

		if (msgs->buf == NULL)
			return -I2C_EPTR;

		if (msgs->len > I2C_FIFO_MAX_LEN)
			i2c->dma_en = true;

		mtk_hdl_i2c_slave_trigger_transfer(i2c->base, i2c->dma_en);

		if (i2c->dma_en)
			_mtk_mhal_i2c_dma(i2c);
		else
			_mtk_mhal_i2c_push_data_to_fifo(i2c);
	} else
		return -I2C_EINVAL;

	return ret;
}

int mtk_mhal_i2c_init_slv_addr(struct mtk_i2c_controller *i2c, u8 slv_addr)
{
	int ret = I2C_OK;

	if (!i2c) {
		i2c_err("mtk_i2c_controller pointer is NULL\n");
		return -I2C_EPTR;
	}

	if (i2c->cg_base == 0x00 || i2c->base == 0x00)
	return -I2C_EINVAL;

	if (i2c->i2c_mode != I2C_SLAVE_MODE) {
		i2c_err("I2C IP isn't SLAVE mode, no need to set slave id\n");
		return -I2C_EINVAL;
	}

	if (slv_addr >= 0x80 || slv_addr == 0x0) {
		i2c_err("Invaild slave address slv_addr = 0x%x\n", slv_addr);
		return -I2C_EINVAL;
	}

	i2c->slave_addr = slv_addr;

	mtk_hdl_i2c_init_slave_addr(i2c->base, slv_addr);

	return ret;
}
