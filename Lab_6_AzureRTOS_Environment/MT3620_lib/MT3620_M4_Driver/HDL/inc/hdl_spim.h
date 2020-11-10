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

#ifndef __HDL_SPI_MASTER__H__
#define __HDL_SPI_MASTER__H__

#include "mhal_osai.h"

/* debug help MACRO */
#define MTK_SPIM_DEBUG_ENABLE 0

#define spim_err(fmt, arg...)	osai_print(fmt, ##arg)
#if MTK_SPIM_DEBUG_ENABLE
#define spim_debug(fmt, arg...)	osai_print(fmt, ##arg)
#else
#define spim_debug(fmt, arg...)
#endif

#define SPI_REG_CTL(offset)	(offset + 0x00)
#define SPI_REG_OPCODE(offset)	(offset + 0x04)
#define SPI_REG_DATA0(offset)	(offset + 0x08)
#define SPI_REG_DATA(offset, x)	(offset + 0x08 + ((x) * 4))
#define SPI_REG_MASTER(offset)	(offset + 0x28)
#define SPI_REG_MOREBUF(offset)	(offset + 0x2c)
#define SPI_REG_Q_CTL(offset)	(offset + 0x30)
#define SPI_REG_STATUS(offset)	(offset + 0x34)

#define SPI_REG_CS_POLAR(offset)	(offset + 0x38)
#define CMD_DELAY_SEL_MASK		BITS(12, 15)
#define CMD_DELAY_SEL_OFFSET		12
#define END_DELAY_SEL_MASK		BITS(16, 19)
#define END_DELAY_SEL_OFFSET		16

#define SPI_REG_DATAPORT_CR(offset)	(offset + 0x40)
#define SPI_REG_SDIR0(offset)	(offset + 0x48)
#define SPI_REG_SDIR(offset, x)	(offset + 0x48 + ((x) * 4))

#define SPI_CTL_ADDR_SIZE_16BIT	0x00080000UL
#define SPI_CTL_ADDR_SIZE_24BIT	0x00100000UL
#define SPI_CTL_ADDR_SIZE_32BIT	0x00200000UL

#define SPI_CTL_START	0x00000100
#define SPI_CTL_BUSY	0x00010000
#define SPI_CTL_TXCNT_MASK	0x0000000f
#define SPI_CTL_RXCNT_MASK	0x000000f0
#define SPI_CTL_TX_RX_CNT_MASK	0x000000ff
#define SPI_CTL_SIZE_MASK	0x00180000
#define SPI_CTL_ADDREXT_MASK	0xff000000

#define SPI_MASTER_MB_LSB_FIRST	(0x1UL << 3)
#define SPI_MASTER_MB_MSB_FIRST	(0x0UL << 3)

#define SPI_MASTER_CPOL_0	(0x0UL << 4)
#define SPI_MASTER_CPOL_1	(0x1UL << 4)

#define SPI_MASTER_CPHA_0	(0x0UL << 5)
#define SPI_MASTER_CPHA_1	(0x1UL << 5)

#define SPI_MASTER_BIDIR_MODE_DISABLE	(0x0UL << 6)
#define SPI_MASTER_BIDIR_MODE_ENABLE	(0x1UL << 6)

#define SPI_MASTER_PREFETCH_DISABLE	(0x0UL << 7)
#define SPI_MASTER_PREFETCH_ENABLE	(0x1UL << 7)

#define SPI_START_SEL_3SPIMCLK	(0x0UL << 8)
#define SPI_START_SEL_6SPIMCLK	(0x1UL << 8)

#define SPI_MASTER_INT_DISABLE	(0x0UL << 9)
#define SPI_MASTER_INT_ENABLE	(0x1UL << 9)

#define SPI_MASTER_FULL_DUPLEX	(0x1UL << 10)

#define SPI_MASTER_SCLK_LOW_LONGER	(0x0UL << 28)
#define SPI_MASTER_SCLK_HIGH_LONGER	(0x1UL << 28)

#define SPI_MASTER_SLAVE_SEL_0	(0x0UL << 29)
#define SPI_MASTER_SLAVE_SEL_1	(0x1UL << 29)

#define SPI_MASTER_SERIAL_MODE_STANDARD	(0x0UL << 0)
#define SPI_MASTER_SERIAL_MODE_DUAL	(0x1UL << 0)
#define SPI_MASTER_SERIAL_MODE_QUAD	(0x2UL << 0)

#define SPI_MASTER_MB_MODE_DISABLE	(0x0UL << 2)
#define SPI_MASTER_MB_MODE_ENABLE	(0x1UL << 2)
#define SPI_MASTER_CLOCK_DIV_SHIFT	(16)

#define SPI_MASTER_BIDIR_MODE_DISABLE	(0x0UL << 6)
#define SPI_MASTER_BIDIR_MODE_ENABLE	(0x1UL << 6)

#define HALF_DUPLEX	(0x0UL << 10)
#define FULL_DUPLEX	(0x1UL << 10)
#define DUPLEX_MASK	(0x1UL << 10)
#define SPI_OK	0x1
#define SPI_RX_OK	0x2
#define SPI_TX_OK	0x4
#define SPI_WRITE_OK	(SPI_OK|SPI_TX_OK)
#define SPI_READ_OK	(SPI_OK|SPI_TX_OK|SPI_RX_OK)
#define SPI_HALF_DMA_MODE_EN	BIT(8)
#define SPIM_HALFDMA_TEST_LENGTH	12
#define SPIM_HALFDMA_PACKAGE_SIZE	32	/* 1~32 byte */

#define SPI_M_INT_ENABLE	0x200
#define SPI_MBCTL_TXCNT_MASK	0x000001ff
#define SPI_MBCTL_RXCNT_MASK	0x001ff000
#define SPI_MBCTL_TX_RX_CNT_MASK	(SPI_MBCTL_TXCNT_MASK |\
					 SPI_MBCTL_RXCNT_MASK)
#define SPI_MBCTL_CMD_MASK	0x3f000000
#define SPI_MBCTL_TXCNT_SHIFT	0
#define SPI_MBCTL_RXCNT_SHIFT	12
#define SPI_MBCTL_CMD_SHIFT	24

#define SPI_CTL_CLK_SEL_MASK	0x03000000
#define SPI_OPCODE_MASK	0x000000ff

#define SPI_CTL_CLK_SEL_MASK	0x03000000
#define SPI_OPCODE_MASK	0x000000ff

#define MTK_SPIM_MIN_TX_OPCODE_LEN_HALF		1
#define MTK_SPIM_MIN_RX_OPCODE_LEN_HALF		0
#define MTK_SPIM_MIN_OPCODE_LEN_FULL		1
#define MTK_SPIM_MAX_OPCODE_LEN			4

/* data_bytes */
#define MTK_SPIM_MAX_LENGTH_ONE_TRANS_FULL	(16)
/* data_bytes */
#define MTK_SPIM_MAX_LENGTH_ONE_TRANS_HALF	(32)

#ifdef __cplusplus
extern "C" {
#endif

void mtk_hdl_spim_dump_reg(void __iomem *base);
void mtk_hdl_spim_print_packet(char *name, u8 *ptr, int len);
void mtk_hdl_spim_clear_irq_status(void __iomem *base);
void mtk_hdl_spim_fifo_handle_rx(void __iomem *base,
	const void *tx_buf, void *rx_buf, u32 len);
void mtk_hdl_spim_prepare_hw(void __iomem *base,
			     u32 cpol, u32 cpha,
			     u32 tx_mlsb, u32 rx_mlsb,
			     u32 slave_sel);
void mtk_hdl_spim_prepare_transfer(void __iomem *base,
				   u32 speed_khz,
				   u32 is_full_duplex);
void mtk_hdl_spim_enable_fifo_transfer(void __iomem *base,
				       int opcode,
				       u8 addr_ext,
				       int opcode_len,
				       u8 *spi_data,
				       int data_len,
				       int tx_enable,
				       int rx_enable);
void mtk_hdl_spim_enable_clk(void __iomem *cg_base);
void mtk_hdl_spim_disable_clk(void __iomem *cg_base);
void mtk_hdl_spim_sw_reset(void __iomem *cg_base);
void mtk_hdl_spim_enable_dma(void __iomem *base);
void mtk_hdl_spim_disable_dma(void __iomem *base);

#ifdef __cplusplus
}
#endif

#endif
