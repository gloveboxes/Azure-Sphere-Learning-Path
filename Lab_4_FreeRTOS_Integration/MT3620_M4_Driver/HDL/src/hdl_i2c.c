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

#include "hdl_i2c.h"

/**I2C execute OK*/
#define I2C_OK				0
/**It means the pointer is NULL*/
#define I2C_EPTR			1
/**FIFO status abnormal*/
#define I2C_EFIFO			2
/**No such device or address*/
#define I2C_ENXIO			6
/**Device busy*/
#define I2C_EBUSY			16
/**Invalid argument*/
#define I2C_EINVAL			22
/**Connection timed out*/
#define I2C_ETIMEDOUT			110

static void _mtk_hdl_i2c_writel(void __iomem *i2c_base,
				u32 data, u32 offset)
{
	osai_writel(data, i2c_base + offset);
}

static u32 _mtk_hdl_i2c_readl(void __iomem *i2c_base, u32 offset)
{
	return osai_readl(i2c_base + offset);
}

static void _mtk_hdl_isu_writel(void __iomem *clk_base,
				u32 data, u32 offset)
{
	osai_writel(data, clk_base + offset);
}

static u32 _mtk_hdl_isu_readl(void __iomem *clk_base, u32 offset)
{
	return osai_readl(clk_base + offset);
}

u32 mtk_hdl_i2c_get_pad_con0(void __iomem *i2c_base)
{
	return _mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_PAD_CON0);
}

void mtk_hdl_i2c_enable_clk(void __iomem *clk_base)
{
	u32 reg_val = 0;

	reg_val = osai_readl(clk_base);
	reg_val |= BIT(8);

	osai_writel(reg_val, clk_base);
}

void mtk_hdl_i2c_disable_clk(void __iomem *clk_base)
{
	u32 reg_val;

	reg_val = osai_readl(clk_base);

	reg_val &= (~BIT(8));

	osai_writel(reg_val, clk_base);
}

void mtk_hdl_i2c_dump(void __iomem *i2c_base)
{
	i2c_debug("I2C_INT_CRTL:[0x%04x] MM_PAD_CON0:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_I2C_INT_CTRL),
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_PAD_CON0));
	i2c_debug("MM_CNT_VAL_PHL:[0x%04x] MM_CNT_VAL_PHH:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_CNT_VAL_PHL),
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_CNT_VAL_PHH));
	i2c_debug("MM_CNT_VAL_PK0:[0x%04x] MM_CNT_VAL_PK1:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_CNT_BYTE_VAL_PK0),
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_CNT_BYTE_VAL_PK1));
	i2c_debug("MM_ID_CON0:[0x%04x] MM_PACK_CON0:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_ID_CON0),
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_PACK_CON0));
	i2c_debug("MM_ACK_VAL:[0x%04x] MM_CON0:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_ACK_VAL),
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_CON0));
	i2c_debug("MM_STATUS:[0x%04x] MM_FIFO_CON0:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_STATUS),
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_FIFO_CON0));
	i2c_debug("MM_FIFO_STATUS:[0x%04x] MM_FIFO_PTR:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_FIFO_STATUS),
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_FIFO_PTR));
	i2c_debug("DMA_CON0:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_DMA_CON0));
	i2c_debug("OFFSET_S_CON0:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_S_CON0));
	i2c_debug("OFFSET_S_ID_CON0:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_S_ID_CON0));
	i2c_debug("OFFSET_S_ID_RECEIVED0:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_S_ID_RECEIVED0));
	i2c_debug("OFFSET_S_FIFO_CON0:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_S_FIFO_CON0));
	i2c_debug("OFFSET_S_FIFO_STATUS:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_S_FIFO_STATUS));
	i2c_debug("OFFSET_S_FIFO_PTR:[0x%04x], OFFSET_S_FIFO_DATA:[0x%04x]\n",
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_S_FIFO_PTR),
		_mtk_hdl_i2c_readl(i2c_base, OFFSET_S_FIFO_DATA));
}

void mtk_hdl_i2c_init_speed(void __iomem *i2c_base, u16 phase_val0,
		u16 phase_val1, u16 phase_val2, u16 phase_val3)
{
	u16 reg_val;

	reg_val = (phase_val1 << I2C_MM_CNTPHASE_VAL1_OFFSET) |
			(phase_val0 << I2C_MM_CNTPHASE_VAL0_OFFSET);

	_mtk_hdl_i2c_writel(i2c_base, reg_val, OFFSET_MM_CNT_VAL_PHL);

	reg_val = (phase_val3 << I2C_MM_CNTPHASE_VAL3_OFFSET) |
			(phase_val2 << I2C_MM_CNTPHASE_VAL2_OFFSET);

	_mtk_hdl_i2c_writel(i2c_base, reg_val, OFFSET_MM_CNT_VAL_PHH);
}

void mtk_hdl_i2c_push_data_to_fifo(void __iomem *i2c_base,
				u8 *buf, int len, int i2c_mode)
{
	while (len--) {
		if (i2c_mode)
			osai_writel(*(buf++), i2c_base + OFFSET_S_FIFO_DATA);
		else
			osai_writel(*(buf++), i2c_base + OFFSET_MM_FIFO_DATA);
	}
}

void mtk_hdl_i2c_init_hw(void __iomem *i2c_base, void __iomem *clk_base)
{
	u32 reg_val;
	u32 reg_val_tmp;

	/*I2C software reset*/
	reg_val = _mtk_hdl_isu_readl(clk_base, OFFSET_ISU_GLOBAL_CRTL);
	reg_val_tmp = reg_val & (~ISU_CR_SW_RST);
	_mtk_hdl_isu_writel(clk_base, reg_val_tmp, OFFSET_ISU_GLOBAL_CRTL);

	reg_val_tmp = reg_val | ISU_CR_SW_RST;
	_mtk_hdl_isu_writel(clk_base, reg_val_tmp, OFFSET_ISU_GLOBAL_CRTL);

	/*I2C clock enable*/
	reg_val = _mtk_hdl_isu_readl(clk_base, OFFSET_ISU_GLOBAL_CRTL);
	reg_val |= ISU_I2C_CLK_EN;
	_mtk_hdl_isu_writel(clk_base, reg_val, OFFSET_ISU_GLOBAL_CRTL);

	/*SDA/SCL EA enable*/
	reg_val = _mtk_hdl_isu_readl(clk_base, OFFSET_ISU_EA);
	reg_val = reg_val | PAD_I2C_CLK | PAD_I2C_SDA;
	_mtk_hdl_isu_writel(clk_base, reg_val, OFFSET_ISU_EA);

	/*Clear I2C FIFO status*/
	_mtk_hdl_i2c_writel(i2c_base, I2C_MM_FIFO_CLR, OFFSET_MM_FIFO_CON0);
	_mtk_hdl_i2c_writel(i2c_base, I2C_S_FIFO_CLR, OFFSET_S_FIFO_CON0);

	/*Clear I2C interrupt status*/
	_mtk_hdl_i2c_writel(i2c_base, I2C_MM_INT_STA, OFFSET_I2C_INT_CTRL);
	_mtk_hdl_i2c_writel(i2c_base, I2C_S_INT_STA, OFFSET_I2C_INT_CTRL);

	reg_val = I2C_CLK_SYNC_EN | (0x1 << I2C_DE_CNT_OFFSET);
	_mtk_hdl_i2c_writel(i2c_base, reg_val, OFFSET_MM_PAD_CON0);

	_mtk_hdl_i2c_writel(i2c_base, 0, OFFSET_MM_CON0);
	_mtk_hdl_i2c_writel(i2c_base, 0, OFFSET_S_CON0);

}

static bool _mtk_hdl_i2c_id_ack_error(void __iomem *i2c_base, int num)
{
	int num_cnt;
	u16 ack_val;

	ack_val = (_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_ACK_VAL) &
		  I2C_MM_ACK_ID_MASK) >> I2C_ACK_PKT0_OFFSET;

	for (num_cnt = 0; num_cnt < num; num_cnt++) {
		if (ack_val & (0x1 << num_cnt))
			return true;
	}

	return false;
}

static bool _mtk_hdl_i2c_data_ack_error(void __iomem *i2c_base, u8 ack_len)
{
	u8 num_cnt;
	u16 ack_val;

	ack_val = _mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_ACK_VAL) &
				     I2C_MM_ACK_DATA_MASK;

	for (num_cnt = 0; num_cnt < ack_len; num_cnt++) {
		if (ack_val & (0x1 << num_cnt))
			return true;
	}

	return false;
}

void mtk_hdl_i2c_master_irq_handle(void __iomem *i2c_base, u32 *irq_stat,
				   u8 msg_num, u8 ack_len)
{
	u16 intr_stat;

	intr_stat = _mtk_hdl_i2c_readl(i2c_base, OFFSET_I2C_INT_CTRL);
	_mtk_hdl_i2c_writel(i2c_base, intr_stat, OFFSET_I2C_INT_CTRL);

	*irq_stat |= intr_stat;

	if (_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_STATUS) & I2C_MM_ARB_LOSE)
		*irq_stat |= I2C_ARB_LOSE;
	if (_mtk_hdl_i2c_id_ack_error(i2c_base, msg_num))
		*irq_stat |= I2C_ID_ACKERR;
	if (_mtk_hdl_i2c_data_ack_error(i2c_base, ack_len))
		*irq_stat |= I2C_DATA_ACKERR;
}

int mtk_hdl_i2c_master_trigger_transfer(void __iomem *i2c_base,
	u8 op, u8 msg_num, u8 dev_addr, u16 msgs0_len, u16 msgs1_len)
{
	u16 reg_val;

	/* Check if master is ready */
	reg_val = _mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_STATUS);
	if (!(reg_val & I2C_MM_START_READY)) {
		i2c_err("master is busy, not ready.\n");
		mtk_hdl_i2c_dump(i2c_base);
		return -I2C_EBUSY;
	}

	_mtk_hdl_i2c_writel(i2c_base, (I2C_MASTER_EN | I2C_MM_GMODE),
			    OFFSET_MM_CON0);
	_mtk_hdl_i2c_writel(i2c_base, dev_addr, OFFSET_MM_ID_CON0);

	/* Set transfer mode and length */
	if (op == I2C_WRRD) {
		reg_val = ((msg_num - 1) << I2C_MM_PACK_VAL_OFFSET) |
			  (0x1 << I2C_MM_PACK_RW1) | (0x0 << I2C_MM_PACK_RW0);

		_mtk_hdl_i2c_writel(i2c_base, reg_val, OFFSET_MM_PACK_CON0);
		_mtk_hdl_i2c_writel(i2c_base, msgs0_len,
				    OFFSET_MM_CNT_BYTE_VAL_PK0);
		_mtk_hdl_i2c_writel(i2c_base, msgs1_len,
				    OFFSET_MM_CNT_BYTE_VAL_PK1);
	} else {
		reg_val = (((msg_num - 1) << I2C_MM_PACK_VAL_OFFSET) |
			  (((op == I2C_RD) ? 1 : 0) << I2C_MM_PACK_RW0));
		_mtk_hdl_i2c_writel(i2c_base, reg_val, OFFSET_MM_PACK_CON0);
		_mtk_hdl_i2c_writel(i2c_base, msgs0_len,
				    OFFSET_MM_CNT_BYTE_VAL_PK0);
	}

	/* Enable master interrupt */
	_mtk_hdl_i2c_writel(i2c_base, I2C_MM_INT_EN, OFFSET_I2C_INT_CTRL);

	return I2C_OK;
}

void mtk_hdl_i2c_master_start(void __iomem *i2c_base, bool dma_en)
{
	u16 reg_val;

	if (dma_en)
		_mtk_hdl_i2c_writel(i2c_base, I2C_DMA_HANDSHAKE_EN,
				    OFFSET_DMA_CON0);

	reg_val = _mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_CON0) |
				     I2C_MM_START_EN;
	_mtk_hdl_i2c_writel(i2c_base, reg_val, OFFSET_MM_CON0);
}

static u8 _mtk_hdl_i2c_master_rx_fifo_size(void __iomem *i2c_base)
{
	u8 rx_wptr = 0;
	u8 rx_rptr = 0;
	u8 rx_data_size = 0;

	rx_wptr = (_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_FIFO_PTR) &
		   I2C_MM_RX_FIFO_WPTR_MASK) >> I2C_MM_RX_FIFO_WPTR_OFFSET;
	rx_rptr = (_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_FIFO_PTR) &
		   I2C_MM_RX_FIFO_RPTR_MASK) >> I2C_MM_RX_FIFO_RPTR_OFFSET;

	if (rx_wptr >= rx_rptr)
		rx_data_size = rx_wptr - rx_rptr;
	else
		rx_data_size = I2C_FIFO_MAX_LEN - (rx_rptr - rx_wptr);

	return rx_data_size;
}

int mtk_hdl_i2c_master_get_data(void __iomem *i2c_base, u8 *buf, u16 data_len)
{
	u16 read_len;
	int ret = 0;

	_mtk_hdl_i2c_writel(i2c_base, 0, OFFSET_I2C_INT_CTRL);

	read_len = _mtk_hdl_i2c_master_rx_fifo_size(i2c_base) & 0xf;
	if (read_len == data_len) {
		while (data_len--)
			*(buf++) = _mtk_hdl_i2c_readl(i2c_base,
						      OFFSET_MM_FIFO_DATA);
	} else {
		i2c_err("data_len %x, read_len %x, fifo read error\n",
			data_len, read_len);
		ret = -I2C_EFIFO;
	}

	return ret;
}

int mtk_hdl_i2c_master_check_fifo_sta(void __iomem *i2c_base)
{
	int ret = 0;

	if (_mtk_hdl_i2c_readl(i2c_base, OFFSET_MM_FIFO_STATUS) !=
	    MM_FIFO_STATUS_INIT) {
		i2c_err("i2c master fifo status error\n");
		ret = -I2C_EFIFO;
	}

	return ret;
}

void mtk_hdl_i2c_init_slave_addr(void __iomem *i2c_base, u8 slv_addr)
{
	_mtk_hdl_i2c_writel(i2c_base, slv_addr, OFFSET_S_ID_CON0);
}

void mtk_hdl_i2c_slave_irq_handle(void __iomem *i2c_base, u32 *irq_stat)
{
	u16 intr_stat;

	intr_stat = _mtk_hdl_i2c_readl(i2c_base, OFFSET_I2C_INT_CTRL);
	_mtk_hdl_i2c_writel(i2c_base, intr_stat, OFFSET_I2C_INT_CTRL);

	*irq_stat |= intr_stat;

	intr_stat = _mtk_hdl_i2c_readl(i2c_base, OFFSET_S_FIFO_STATUS);

	if (intr_stat & S_RX_FIFO_UDR)
		*irq_stat |= I2C_S_RX_FIFO_UDR;
	else if (intr_stat & S_RX_FIFO_OVF)
		*irq_stat |= I2C_S_RX_FIFO_OVF;

	if (intr_stat & S_TX_FIFO_UDR)
		*irq_stat |= I2C_S_TX_FIFO_UDR;
	else if (intr_stat & S_TX_FIFO_OVF)
		*irq_stat |= I2C_S_TX_FIFO_OVF;
}

void mtk_hdl_i2c_slave_trigger_transfer(void __iomem *i2c_base, bool dma_en)
{

	/*Enable i2c slave mode*/
	_mtk_hdl_i2c_writel(i2c_base, I2C_SLAVE_EN, OFFSET_S_CON0);

	/* Enable slave interrupt */
	_mtk_hdl_i2c_writel(i2c_base, I2C_S_INT_EN, OFFSET_I2C_INT_CTRL);

	if (dma_en)
		_mtk_hdl_i2c_writel(i2c_base, I2C_DMA_HANDSHAKE_EN |
				    I2C_DMA_SLAVE_SELECT, OFFSET_DMA_CON0);
}

static u8 _mtk_hdl_i2c_slave_rx_fifo_size(void __iomem *i2c_base)
{
	u8 rx_wptr = 0;
	u8 rx_rptr = 0;
	u8 rx_data_size = 0;

	rx_wptr = (_mtk_hdl_i2c_readl(i2c_base, OFFSET_S_FIFO_PTR) &
		   I2C_S_RX_FIFO_WPTR_MASK) >> I2C_S_RX_FIFO_WPTR_OFFSET;
	rx_rptr = (_mtk_hdl_i2c_readl(i2c_base, OFFSET_S_FIFO_PTR) &
		   I2C_S_RX_FIFO_RPTR_MASK) >> I2C_S_RX_FIFO_RPTR_OFFSET;

	if (rx_wptr >= rx_rptr)
		rx_data_size = rx_wptr - rx_rptr;
	else
		rx_data_size = I2C_FIFO_MAX_LEN - (rx_rptr - rx_wptr);

	return rx_data_size;
}

int mtk_hdl_i2c_slave_get_data(void __iomem *i2c_base, u8 *buf, u16 len)
{
	int ret = I2C_OK;
	u16 data_len;

	_mtk_hdl_i2c_writel(i2c_base, 0, OFFSET_I2C_INT_CTRL);

	data_len = _mtk_hdl_i2c_slave_rx_fifo_size(i2c_base) & 0xf;
	if (data_len == len) {
		while (data_len--)
			*(buf++) = _mtk_hdl_i2c_readl(i2c_base,
						      OFFSET_S_FIFO_DATA);
	} else {
		i2c_err("data_len %x, len %x, fifo read error\n",
			data_len, len);
		ret = -I2C_EFIFO;
	}

	return ret;
}

int mtk_hdl_i2c_slave_check_fifo_sta(void __iomem *i2c_base)
{
	int ret = I2C_OK;

	ret = _mtk_hdl_i2c_readl(i2c_base, OFFSET_S_FIFO_STATUS);
	if (ret != S_FIFO_STATUS_INIT) {
		i2c_err("I2C slave mode fifo status error\n");
		ret = -I2C_EFIFO;
	} else
		ret = I2C_OK;

	return ret;
}

