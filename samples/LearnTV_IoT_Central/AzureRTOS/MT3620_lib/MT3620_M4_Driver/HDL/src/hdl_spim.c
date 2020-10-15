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

#include "hdl_spim.h"
#include "mhal_osai.h"

void mtk_hdl_spim_enable_clk(void __iomem *cg_base)
{
	int reg_val;

	reg_val = osai_readl(cg_base);
	reg_val |= 0x00001e00;
	osai_writel(reg_val, cg_base);
}

void mtk_hdl_spim_disable_clk(void __iomem *cg_base)
{
	int reg_val;

	reg_val = osai_readl(cg_base);
	reg_val &= ~0x00001e00;
	osai_writel(reg_val, cg_base);
}

void mtk_hdl_spim_sw_reset(void __iomem *cg_base)
{
	u32 reg_val = 0;

	spim_debug("spim sw reset\n");
	reg_val = osai_readl(cg_base);
	reg_val &= ~0x1;
	osai_writel(reg_val, cg_base);

	reg_val = osai_readl(cg_base);
	reg_val |= 0x1;
	osai_writel(reg_val, cg_base);

	reg_val = osai_readl(cg_base);
	reg_val |= 0x00001e00;
	osai_writel(reg_val, cg_base);
}

void mtk_hdl_spim_dump_reg(void __iomem *base)
{
	int i;


	spim_err("STCSR(0x0)=0x%x\n",
		   osai_readl(SPI_REG_CTL(base)));
	spim_err("SOAR(0x4)=0x%x\n",
		   osai_readl(SPI_REG_OPCODE(base)));
	spim_err("SMMR(0x28)=0x%x\n",
		   osai_readl(SPI_REG_MASTER(base)));
	spim_err("SMBCR(0x2c)=0x%x\n",
		   osai_readl(SPI_REG_MOREBUF(base)));
	spim_err("CSPOL(0x38)=0x%x\n",
		   osai_readl(SPI_REG_CS_POLAR(base)));
	spim_err("DATAPORT(0x40)=0x%x\n",
		   osai_readl(SPI_REG_DATAPORT_CR(base)));
	for (i = 0; i < 8; i++)
		spim_err("SDOR%d=0x%x\n",
			   i, osai_readl(SPI_REG_DATA(base, i)));
	for (i = 0; i < 8; i++)
		spim_err("SDIR%d=0x%x\n",
			   i, osai_readl(SPI_REG_SDIR(base, i)));
}

void mtk_hdl_spim_print_packet(char *name, u8 *ptr, int len)
{
	int i;

	spim_debug("%s: ", name);
	for (i = 0; i < len; i++) {
		if (i % 4 == 0)
			spim_debug("\n");
		spim_debug(" %02x", ptr[i]);
	}
	spim_debug("\n");
}

void mtk_hdl_spim_enable_fifo_transfer(void __iomem *base,
				       int opcode,
				       u8 addr_ext,
				       int opcode_len,
				       u8 *spi_data,
				       int data_len,
				       int tx_enable,
				       int rx_enable)
{
	int i, reg_val, tmp_data;

	spim_debug("[%s]opcode(0x%x), opcode_len(%d)\n", __func__,
		   opcode, opcode_len);

	/* tx only: HW can send opcode only, then data_len = 0 */
	if (data_len > 0)
		mtk_hdl_spim_print_packet("spi_data", spi_data, data_len);

	/* 1. OPCODE first */
	osai_writel(opcode, SPI_REG_OPCODE(base));

	/* 2. Data[n] then */
	if (tx_enable) {
		for (i = 0; i < (data_len / 4 + 1); i++) {
			if ((data_len == 0) || (i == 8))
				break;
			tmp_data = spi_data[i * 4] |
			    spi_data[i * 4 + 1] << 8 |
			    spi_data[i * 4 + 2] << 16 |
			    spi_data[i * 4 + 3] << 24;
			osai_writel(tmp_data, SPI_REG_DATA(base, i));
		}
	}

	/* 3. Master register */
	reg_val = osai_readl(SPI_REG_MASTER(base));
	reg_val |= SPI_MASTER_MB_MODE_ENABLE;
	osai_writel(reg_val, SPI_REG_MASTER(base));

	/* 4. More Buffer mode1 register */
	reg_val = (opcode_len * 8) << 24;
	reg_val &= ~(SPI_MBCTL_TX_RX_CNT_MASK);
	if (tx_enable)
		reg_val |= (data_len * 8);
	/* miso_bit_cnt */
	if (rx_enable)
		reg_val |= ((data_len * 8) << 12);
	osai_writel(reg_val, SPI_REG_MOREBUF(base));

	reg_val = SPI_CTL_ADDR_SIZE_24BIT | SPI_CTL_START;
	osai_writel(reg_val, SPI_REG_CTL(base));
}

void mtk_hdl_spim_clear_irq_status(void __iomem *base)
{
	/* clear irq status */
	osai_readl(SPI_REG_STATUS(base));
}

/* fifo mode and rx only, afer transfer sw should copy rx data to rx_buf */
void mtk_hdl_spim_fifo_handle_rx(void __iomem *base,
	const void *tx_buf, void *rx_buf, u32 len)
{
	int i, reg_val, q, r, val_tmp;

	if ((!tx_buf) && rx_buf) {	/* half duplex */
		for (i = 0; i < len; i++) {
			q = i / 4;
			r = i % 4;
			reg_val = osai_readl(SPI_REG_SDIR(base, q));
			val_tmp = (u8) (reg_val >> (r * 8));

			spim_debug("i:%d,rx_data:0x%x,val_tmp:0x%x\n",
				   i, reg_val, val_tmp);

			*((char *)rx_buf + i) = val_tmp;
		}
	}

	if (tx_buf && rx_buf) {	/* full duplex */
		for (i = 0; i < len; i++) {
			q = i / 4;
			r = i % 4;
			reg_val = osai_readl(SPI_REG_SDIR(base, 4 + q));
			val_tmp = (u8) (reg_val >> (r * 8));

			spim_debug("i:%d,rx_data:0x%x,val_tmp:0x%x\n",
				   i, reg_val, val_tmp);

			*((char *)rx_buf + i) = val_tmp;
		}
	}
}

void mtk_hdl_spim_prepare_hw(void __iomem *base,
			     u32 cpol, u32 cpha,
			     u32 tx_mlsb, u32 rx_mlsb,
			     u32 slave_sel)
{
	u32 reg_val;

	reg_val = osai_readl(SPI_REG_MASTER(base));

	/* config rs_slave_sel */
	reg_val &= ~(0x7 << 29);
	if (slave_sel == 0)
		reg_val |= SPI_MASTER_SLAVE_SEL_0;
	if (slave_sel == 1)
		reg_val |= SPI_MASTER_SLAVE_SEL_1;

	/* config clk_mode */
	reg_val &= ~(0x1 << 28);
	reg_val |= SPI_MASTER_SCLK_LOW_LONGER;

	/* config CPHA */
	reg_val &= ~(0x1 << 5);
	if (cpha)
		reg_val |= SPI_MASTER_CPHA_1;
	else
		reg_val &= ~SPI_MASTER_CPHA_0;

	/* config CPOL */
	reg_val &= ~(0x1 << 4);
	if (cpol)
		reg_val |= SPI_MASTER_CPOL_1;
	else
		reg_val &= ~SPI_MASTER_CPOL_0;

	/* config lsb_first */
	reg_val &= ~(0x1 << 3);
	if (tx_mlsb && rx_mlsb)
		reg_val |= SPI_MASTER_MB_MSB_FIRST;	/* msb first */
	else
		reg_val |= SPI_MASTER_MB_LSB_FIRST;	/* lsb first */

	/* config more_buf_mode */
	reg_val &= ~(0x1 << 2);
	reg_val |= SPI_MASTER_MB_MODE_ENABLE;

	osai_writel(reg_val, SPI_REG_MASTER(base));

	/* config cs_setup and cs_hold delay to maximum  */
	reg_val = osai_readl(SPI_REG_CS_POLAR(base));
	reg_val |= 0xf << CMD_DELAY_SEL_OFFSET; /* cs_setup */
	reg_val |= 0xf << END_DELAY_SEL_OFFSET; /* cs_hold */
	osai_writel(reg_val, SPI_REG_CS_POLAR(base));
}

void mtk_hdl_spim_prepare_transfer(void __iomem *base,
				   u32 speed_khz,
				   u32 is_full_duplex)
{
	u32 reg_val;

	/* if xfer not config speed, default setting to 1Mhz */
	if (speed_khz == 0)
		speed_khz = 1 * 1000;

	reg_val = osai_readl(SPI_REG_MASTER(base));

	/* config spi bus clk freq */
	reg_val &= ~(0xfff << SPI_MASTER_CLOCK_DIV_SHIFT);
	reg_val |=
	    ((80 * 1000 / speed_khz) - 2) << SPI_MASTER_CLOCK_DIV_SHIFT;

	/* config duplex */
	if (is_full_duplex == 1) {
		reg_val &= ~DUPLEX_MASK;
		reg_val |= FULL_DUPLEX;
	} else {
		reg_val &= ~DUPLEX_MASK;
		reg_val |= HALF_DUPLEX;
	}

	/* config int_en */
	reg_val |= SPI_MASTER_INT_ENABLE;

	osai_writel(reg_val, SPI_REG_MASTER(base));
}

void mtk_hdl_spim_enable_dma(void __iomem *base)
{
	int reg_val;

	/* enable spim dma mode */
	spim_debug("enable spim dma mode\n");
	reg_val = osai_readl(SPI_REG_CS_POLAR(base));
	reg_val |= SPI_HALF_DMA_MODE_EN;
	osai_writel(reg_val, SPI_REG_CS_POLAR(base));
}

void mtk_hdl_spim_disable_dma(void __iomem *base)
{
	int reg_val;

	/* disable dma mode */
	spim_debug("disable spim dma mode\n");
	reg_val = osai_readl(SPI_REG_CS_POLAR(base));
	reg_val &= ~SPI_HALF_DMA_MODE_EN;
	osai_writel(reg_val, SPI_REG_CS_POLAR(base));
}
