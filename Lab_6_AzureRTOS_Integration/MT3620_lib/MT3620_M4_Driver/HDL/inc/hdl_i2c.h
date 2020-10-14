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

#ifndef __HDL_I2C_H__
#define __HDL_I2C_H__

#include "mhal_osai.h"

#define i2c_err(fmt, arg...) osai_print(fmt, ##arg)

#define ENABLE_I2C_DEBUG 0
#if ENABLE_I2C_DEBUG
#define i2c_debug(fmt, arg...) osai_print(fmt, ##arg)
#else
#define i2c_debug(fmt, arg...)
#endif

/* 0x00 ISU_GLOBAL_CRTL */
#define OFFSET_ISU_GLOBAL_CRTL		(0x00)
#define ISU_CR_SW_RST			BIT(0)
#define ISU_I2C_CLK_EN			BIT(8)

#define OFFSET_ISU_EA			(0xA0)
#define PAD_I2C_CLK			BIT(1)
#define PAD_I2C_SDA			BIT(2)

#define I2C_FIFO_MAX_LEN		(8)
#define I2C_PKT_MAX_NUM			(3)

#define I2C_SOURCE_CLK_26MHZ		(26000000)
#define I2C_SOURCE_CLK_40MHZ		(40000000)

#define I2C_50kHz			(50000)
#define I2C_100kHz			(100000)
#define I2C_200kHz			(200000)
#define I2C_400kHz			(400000)
#define I2C_1000kHz			(1000000)

#define I2C_ARB_LOSE			BIT(4)
#define I2C_ID_ACKERR			BIT(5)
#define I2C_DATA_ACKERR			BIT(6)
#define I2C_S_RX_FIFO_UDR		BIT(7)
#define I2C_S_RX_FIFO_OVF		BIT(8)
#define I2C_S_TX_FIFO_UDR		BIT(9)
#define I2C_S_TX_FIFO_OVF		BIT(10)
#define I2C_S_FIFO_CHECK		BITS(7, 10)

enum mtk_trans_op {
	I2C_WR = 1,
	I2C_RD,
	I2C_WRRD,
};

/**0x00 I2C_INT_CTRL*/
#define OFFSET_I2C_INT_CTRL		(0x00)
#define I2C_MM_INT_STA			BIT(0)
#define I2C_MM_INT_EN			BIT(1)
#define I2C_MM_INT_MSK			BIT(2)
#define I2C_S_INT_STA			BIT(4)
#define I2C_S_INT_EN			BIT(5)
#define I2C_S_INT_MSK			BIT(6)

/** 0x40 MM_PAD_CON0 */
#define OFFSET_MM_PAD_CON0		(0x40)
#define I2C_DE_CNT_OFFSET		(0)
#define I2C_DE_CNT_MASK			BITS(0, 4)
#define I2C_SCL_DRVH_EN			BIT(5)
#define I2C_SDA_DRVH_EN			BIT(6)
#define I2C_CLK_SYNC_EN			BIT(7)

/** 0x44 MM_CNT_VAL_PHL */
#define OFFSET_MM_CNT_VAL_PHL		0x44
#define I2C_MM_CNTPHASE_VAL1_OFFSET	(8)
#define I2C_MM_CNTPHASE_VAL1_MASK	BITS(8, 15)
#define I2C_MM_CNTPHASE_VAL0_OFFSET	(0)
#define I2C_MM_CNTPHASE_VAL0_MASK	BITS(0, 7)

/** 0x48 MM_CNT_VAL_PHH */
#define OFFSET_MM_CNT_VAL_PHH		0x48
#define I2C_MM_CNTPHASE_VAL3_OFFSET	(8)
#define I2C_MM_CNTPHASE_VAL3_MASK	BITS(8, 15)
#define I2C_MM_CNTPHASE_VAL2_OFFSET	(0)
#define I2C_MM_CNTPHASE_VAL2_MASK	BITS(0, 7)

/** 0x54 MM_CNT_BYTE_VAL_PK0*/
#define OFFSET_MM_CNT_BYTE_VAL_PK0	0x54
#define I2C_MM_CNT_BYTE_VAL_PK0_OFFSET	BIT(0)
#define I2C_MM_CNT_BYTE_VAL_PK0_MASK	BITS(0, 15)

/** 0x58 MM_CNT_BYTE_VAL_PK1*/
#define OFFSET_MM_CNT_BYTE_VAL_PK1	0x58
#define I2C_MM_CNT_BYTE_VAL_PK1_OFFSET	BIT(0)
#define I2C_MM_CNT_BYTE_VAL_PK1_MASK	BITS(0, 15)

/** 0x5C MM_CNT_BYTE_VAL_PK2*/
#define OFFSET_MM_CNT_BYTE_VAL_PK2	0x58
#define I2C_MM_CNT_BYTE_VAL_PK2_OFFSET	BIT(0)
#define I2C_MM_CNT_BYTE_VAL_PK2_MASK	BITS(0, 15)

/** 0x60 MM_ID_CON0 */
#define OFFSET_MM_ID_CON0		0x60
#define I2C_MM_SLAVE_ID_OFFSET		BIT(0)
#define I2C_MM_SLAVE_ID_MASK		(BITS(0, 6))

/** 0x68 MM_PACK_CON0 */
#define OFFSET_MM_PACK_CON0		0x68
#define I2C_MM_PACK_RW0			(0)
#define I2C_MM_PACK_RW1			(1)
#define I2C_MM_PACK_RW2			(2)
#define I2C_MM_PACK_RW3			(3)
#define I2C_MM_PACK_VAL_OFFSET		(4)
#define I2C_MM_PACK_VAL_MASK		BITS(4, 5)

/** 0x6C MM_ACK_VAL */
#define OFFSET_MM_ACK_VAL		0x6c
#define I2C_ACK_PKT0_OFFSET		(8)
#define I2C_ACK_PKT1_OFFSET		(9)
#define I2C_ACK_PKT2_OFFSET		(10)
#define I2C_MM_ACK_ID_MASK		BITS(8, 10)
#define I2C_MM_ACK_DATA_MASK		BITS(0, 7)

/** 0x70 MM_CON0 */
#define OFFSET_MM_CON0			0x70
#define I2C_MM_START_EN			BIT(0)
#define I2C_MM_GMODE			BIT(14)
#define I2C_MASTER_EN			BIT(15)

/** 0x74 MM_STATUS */
#define OFFSET_MM_STATUS		0x74
#define I2C_BUS_BUSY			BIT(0)
#define I2C_MM_ARB_LOSE			BIT(1)
#define I2C_MM_START_READY		BIT(2)

/** 0x78 MM_FIFO_CON0 */
#define OFFSET_MM_FIFO_CON0		0x78
#define I2C_MM_TX_FIFO_CLR		BIT(1)
#define I2C_MM_RX_FIFO_CLR		BIT(0)
#define I2C_MM_FIFO_CLR			BITS(0, 1)

/** 0x80 MM_FIFO_STATUS */
#define OFFSET_MM_FIFO_STATUS		0x80
#define MM_TX_FIFO_OVF			BIT(7)
#define MM_TX_FIFO_UDR			BIT(6)
#define MM_TX_FIFO_FUL			BIT(5)
#define MM_TX_FIFO_EMP			BIT(4)
#define MM_RX_FIFO_OVF			BIT(3)
#define MM_RX_FIFO_UDR			BIT(2)
#define MM_RX_FIFO_FUL			BIT(1)
#define MM_RX_FIFO_EMP			BIT(0)
#define MM_FIFO_STATUS_INIT		(MM_TX_FIFO_EMP | MM_RX_FIFO_EMP)

/** 0x84 MM_FIFO_PTR */
#define OFFSET_MM_FIFO_PTR		0x84
#define I2C_MM_RX_FIFO_RPTR_OFFSET	(0)
#define I2C_MM_RX_FIFO_RPTR_MASK	BITS(0, 3)
#define I2C_MM_RX_FIFO_WPTR_OFFSET	(4)
#define I2C_MM_RX_FIFO_WPTR_MASK	BITS(4, 7)
#define I2C_MM_TX_FIFO_RPTR_OFFSET	BIT(8)
#define I2C_MM_TX_FIFO_RPTR_MASK	BITS(8, 11)
#define I2C_MM_TX_FIFO_WPTR_OFFSET	BIT(12)
#define I2C_MM_TX_FIFO_WPTR_MASK	BITS(12, 15)

/** 0x90 MM_FIFO_DATA */
#define OFFSET_MM_FIFO_DATA		0x90
#define I2C_MM_FIFO_DATA		BITS(0, 7)

/** 0xC0 DMA_CON0 */
#define OFFSET_DMA_CON0			0xC0
#define I2C_DMA_SLAVE_SELECT		BIT(5)
#define I2C_DMA_HANDSHAKE_EN		BIT(4)
#define I2C_DMA_HANDSHAKE_MASK		BITS(4, 5)


/** 0xC4 S_CON0 */
#define OFFSET_S_CON0			0xC4
#define I2C_SLAVE_EN			BIT(15)
#define I2C_S_MUTE_MODE			BIT(14)
#define I2C_S_IND_EN			BIT(11)

/** 0xC8 S_ID_CON0 */
#define OFFSET_S_ID_CON0		0xC8
#define I2C_SLAVE_ID_OFFSET		BIT(0)
#define I2C_SLAVE_ID_MASK		BITS(0, 6)

/** 0xCC S_ID_RECEIVED0 */
#define OFFSET_S_ID_RECEIVED0		0xCC
#define I2C_S_RECEIVED_ID_OFFSET	BIT(0)
#define I2C_S_RECEIVED_ID_MASK		BITS(0, 6)
#define I2C_S_RECEIVED_READ		BIT(7)

/** 0xD8 S_FIFO_CON0 */
#define OFFSET_S_FIFO_CON0		0xD8
#define I2C_S_TX_FIFO_CLR		BIT(1)
#define I2C_S_RX_FIFO_CLR		BIT(0)
#define I2C_S_FIFO_CLR			BITS(0, 1)

/** 0xE0 S_FIFO_STATUS */
#define OFFSET_S_FIFO_STATUS		0xE0
#define S_TX_FIFO_OVF			BIT(7)
#define S_TX_FIFO_UDR			BIT(6)
#define S_TX_FIFO_FUL			BIT(5)
#define S_TX_FIFO_EMP			BIT(4)
#define S_RX_FIFO_OVF			BIT(3)
#define S_RX_FIFO_UDR			BIT(2)
#define S_RX_FIFO_FUL			BIT(1)
#define S_RX_FIFO_EMP			BIT(0)
#define S_FIFO_STATUS_INIT		(S_TX_FIFO_EMP | S_RX_FIFO_EMP)

/** 0xE4 S_TX_FIFO_PTR*/
#define OFFSET_S_FIFO_PTR		0xE4
#define I2C_S_RX_FIFO_RPTR_OFFSET	(0)
#define I2C_S_RX_FIFO_RPTR_MASK		BITS(0, 3)
#define I2C_S_RX_FIFO_WPTR_OFFSET	(4)
#define I2C_S_RX_FIFO_WPTR_MASK		BITS(4, 7)
#define I2C_S_TX_FIFO_RPTR_OFFSET	(8)
#define I2C_S_TX_FIFO_RPTR_MASK		BITS(8, 11)
#define I2C_S_TX_FIFO_WPTR_OFFSET	(12)
#define I2C_S_TX_FIFO_WPTR_MASK		BITS(12, 15)

#define OFFSET_S_FIFO_DATA		0xF0
#define I2C_S_DATA_WR_REG_MASK		BITS(0, 7)

#ifdef __cplusplus
extern "C" {
#endif

u32 mtk_hdl_i2c_get_pad_con0(void __iomem *i2c_base);
void mtk_hdl_i2c_enable_clk(void __iomem *clk_base);
void mtk_hdl_i2c_disable_clk(void __iomem *clk_base);
void mtk_hdl_i2c_dump(void __iomem *i2c_base);
void mtk_hdl_i2c_push_data_to_fifo(void __iomem *i2c_base, u8 *buf,
				   int len, int i2c_mode);
void mtk_hdl_i2c_init_hw(void __iomem *i2c_base, void __iomem *clk_base);
void mtk_hdl_i2c_init_speed(void __iomem *i2c_base, u16 phase_val0,
			    u16 phase_val1, u16 phase_val2, u16 phase_val3);
void mtk_hdl_i2c_master_irq_handle(void __iomem *i2c_base, u32 *irq_stat,
				   u8 msg_num, u8 ack_len);
int mtk_hdl_i2c_master_trigger_transfer(void __iomem *i2c_base, u8 op,
					u8 msg_num, u8 dev_addr,
					u16 msgs0_len, u16 msgs1_len);
void mtk_hdl_i2c_master_start(void __iomem *i2c_base, bool dma_en);
int mtk_hdl_i2c_master_get_data(void __iomem *i2c_base, u8 *buf, u16 data_len);
int mtk_hdl_i2c_master_check_fifo_sta(void __iomem *i2c_base);

void mtk_hdl_i2c_init_slave_addr(void __iomem *i2c_base, u8 slv_addr);
void mtk_hdl_i2c_slave_irq_handle(void __iomem *i2c_base, u32 *irq_stat);
void mtk_hdl_i2c_slave_trigger_transfer(void __iomem *i2c_base, bool dma_en);
int mtk_hdl_i2c_slave_get_data(void __iomem *i2c_base, u8 *buf, u16 len);
int mtk_hdl_i2c_slave_check_fifo_sta(void __iomem *i2c_base);

#ifdef __cplusplus
}
#endif

#endif
