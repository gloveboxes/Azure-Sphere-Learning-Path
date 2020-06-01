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

#ifndef	__HDL_I2S_H__
#define	__HDL_I2S_H__

#include "mhal_osai.h"

/******************************************************************************
*			       C O N S T A N T S
*******************************************************************************
*/
#define	I2S_GLB_CONTROL_ADDR(offset)		(offset + 0x0000)
#define	I2S_DL_CONTROL_ADDR(offset)		(offset + 0x0004)
#define	I2S_UL_CONTROL_ADDR(offset)		(offset + 0x0008)
#define	I2S_SW_RESET_ADDR(offset)		(offset + 0x000C)

#define	I2S_DL_FIFO_W_CONTROL_ADDR(offset)	(offset + 0x0010)
#define	I2S_DL_FIFO_R_CONTROL_ADDR(offset)	(offset + 0x0014)
#define	I2S_UL_FIFO_W_CONTROL_ADDR(offset)	(offset + 0x0018)
#define	I2S_UL_FIFO_R_CONTROL_ADDR(offset)	(offset + 0x001C)

#define	I2S_DL_EMPTY_VALUE_LR(offset)		(offset + 0x0020)
#define	I2S_DL_EMPTY_VALUE_L1R1(offset)		(offset + 0x0024)
#define	I2S_DEBUG_CONTROL(offset)		(offset + 0x0028)
#define	I2S_DEBUG_PROBE(offset)			(offset + 0x002c)
#define	I2S_DMA_IF_CONTROL(offset)		(offset + 0x0030)

#define	I2S_GLOBAL_INT_EN(offset)		(offset + 0x0040)
#define	I2S_GLOBAL_STS_CLR(offset)		(offset + 0x0044)
#define	I2S_GLOBAL_INT_STS(offset)		(offset + 0x0048)


#define	I2S_RING_BUFFER_SIZE		256
#define	MAX_INTERNAL_USE_SIZE		(2 * I2S_RING_BUFFER_SIZE)

/*-----------I2S_GLB_CONTROL------------------------*/
#define	I2S_EN_MASK			0x00000001
#define	I2S_EN_SHFT			0

#define	I2S_DLFIFO_EN_MASK		0x00000002
#define	I2S_DLFIFO_EN_SHFT		1

#define	I2S_ULFIFO_EN_MASK		0x00000004
#define	I2S_ULFIFO_EN_SHFT		2

#define	I2S_ENGEN_EN_MASK		0x00000008
#define	I2S_ENGEN_EN_SHFT		3

#define	I2S_EXT_IO_I2SIN		1
#define	I2S_EXT_IO_I2SOUT		0
#define	I2S_EXT_IO_MASK			0x00000010
#define	I2S_EXT_IO_SHFT			4

#define	I2S_EXT_MASK			0x00000020
#define	I2S_EXT_SHFT			5

#define	I2S_DL_LRSW_MASK		0x00000080
#define	I2S_DL_LRSW_SHFT		7

#define	I2S_DL_MONO_MASK		0x00000100
#define	I2S_DL_MONO_SHFT		8

#define	I2S_DL_MONO_DUP_MASK		0x00000200
#define	I2S_DL_MONO_DUP_SHFT		9

#define	I2S_MCLK_OUT_EN			1
#define	I2S_MCLK_OUT_OFF		0
#define	I2S_MCLK_OUT_MASK		0x00000400
#define	I2S_MCLK_OUT_SHFT		10

#define	I2S_IN_CLK_EN			1
#define	I2S_IN_CLK_OFF			0
#define	I2S_IN_CLK_MASK			0x00000800
#define	I2S_IN_CLK_SHFT			11

#define	I2S_OUT_CLK_EN			1
#define	I2S_OUT_CLK_OFF			0
#define	I2S_OUT_CLK_MASK		0x00001000
#define	I2S_OUT_CLK_SHFT		12

#define	I2S_26M_SEL_MASK		0x00040000
#define	I2S_26M_SEL_SHFT		18

#define	I2S_CK_INV_MASK			0x00080000
#define	I2S_CK_INV_SHFT			19

#define	I2S_NEG_CAP_MASK		0x00100000
#define	I2S_NEG_CAP_SHFT		20

#define	I2S_DL_EMPTY_VALUE_EN		1
#define	I2S_DL_EMPTY_VALUE_DIS		0
#define	I2S_DL_EMPTY_VALUE_MASK		0x00400000
#define	I2S_DL_EMPTY_VALUE_SHFT		22

#define	I2S_UL_EMPTY_VALUE_EN		1
#define	I2S_UL_EMPTY_VALUE_DIS		0
#define	I2S_UL_EMPTY_VALUE_MASK		0x00800000
#define	I2S_UL_EMPTY_VALUE_SHFT		23

#define	I2S_IN_CLK_SEL_MASK		0x03000000
#define	I2S_IN_CLK_SEL_SHFT		24
#define	I2S_OUT_CLK_SEL_MASK		0x0C000000
#define	I2S_OUT_CLK_SEL_SHFT		26

#define	I2S_MCLK_SEL_XPLL_16M		0x0
#define	I2S_MCLK_SEL_XPLL_26M		0x1
#define	I2S_MCLK_SEL_XTAL_26M		0x2

#define	I2S_MCLK_SEL_MASK		0x30000000
#define	I2S_MCLK_SEL_SHFT		28

#define	I2S_LOOPBACK_MASK		0x80000000
#define	I2S_LOOPBACK_SHFT		31

/* ------------I2S_DL_CONTROL_ADDR------------------------ */

#define	I2S_DL_EN_MASK			0x00000001
#define	I2S_DL_EN_SHFT			0

#define	I2S_DL_WLEN_MASK		0x00000002
#define	I2S_DL_WLEN_SHFT		1

#define	I2S_DL_SRC_MASK			0x00000004
#define	I2S_DL_SRC_SHFT			2

#define	I2S_DL_FMT_MASK			0x00000008
#define	I2S_DL_FMT_SHFT			3

#define	I2S_DL_DIR_MASK			0x00000010
#define	I2S_DL_DIR_SHFT			4

#define	I2S_DL_WSINV_MASK		0x00000020
#define	I2S_DL_WSINV_SHFT		5

#define	I2S_DL_FIFO_2DEQ_EN		1
#define	I2S_DL_FIFO_2DEQ_DIS		0
#define	I2S_DL_FIFO_2DEQ_MASK		0x00000080
#define	I2S_DL_FIFO_2DEQ_SHFT		7

#define	I2S_DL_SR_MASK			0x00000F00
#define	I2S_DL_SR_SHFT			8

#define	I2S_DL_BIT_PER_S_MASK		0x00006000
#define	I2S_DL_BIT_PER_S_SHFT		13

#define	I2S_DL_WS_RESYNC_MASK		0x00008000
#define	I2S_DL_WS_RESYNC_SHFT		15

#define	I2S_DL_MSB_OFFSET_MASK		0x00FE0000
#define	I2S_DL_MSB_OFFSET_SHFT		17

#define	I2S_DL_CH_PER_S_MASK		0x60000000
#define	I2S_DL_CH_PER_S_SHFT		29

/* ------------I2S_UL_CONTROL_ADDR------------------------ */

#define	I2S_UL_EN_MASK			0x00000001
#define	I2S_UL_EN_SHFT			0

#define	I2S_UL_WLEN_MASK		0x00000002
#define	I2S_UL_WLEN_SHFT		1

#define	I2S_UL_SRC_MASK			0x00000004
#define	I2S_UL_SRC_SHFT			2

#define	I2S_UL_FMT_MASK			0x00000008
#define	I2S_UL_FMT_SHFT			3

#define	I2S_UL_DIR_MASK			0x00000010
#define	I2S_UL_DIR_SHFT			4

#define	I2S_UL_WSINV_MASK		0x00000020
#define	I2S_UL_WSINV_SHFT		5

#define	I2S_UL_SR_MASK			0x00000F00
#define	I2S_UL_SR_SHFT			8

#define	I2S_UL_BIT_PER_S_MASK		0x00006000
#define	I2S_UL_BIT_PER_S_SHFT		13

#define	I2S_UL_WS_RESYNC_MASK		0x00008000
#define	I2S_UL_WS_RESYNC_SHFT		15

#define	I2S_UL_DOWN_RATE_MASK		0x00010000
#define	I2S_UL_DOWN_RATE_SHFT		16

#define	I2S_UL_MSB_OFFSET_MASK		0x00FE0000
#define	I2S_UL_MSB_OFFSET_SHFT		17

#define	I2S_UL_UPDATE_WORD_MASK		0x1F000000
#define	I2S_UL_UPDATE_WORD_SHFT		24

#define	I2S_UL_CH_PER_S_MASK		0x60000000
#define	I2S_UL_CH_PER_S_SHFT		29

#define	I2S_UL_LR_SWAP_MASK		0x80000000
#define	I2S_UL_LR_SWAP_SHFT		31

/* ------------I2S_SW_RESET_ADDR------------------------ */
#define	I2S_SW_RST_EN_MASK		0x00000001
#define	I2S_SW_RST_EN_SHFT		0

#define	I2S_GLB_SW_RST_EN_MASK		0x00000002
#define	I2S_GLB_SW_RST_EN_SHFT		1

#define	I2S_DL_SW_RST_EN_MASK		0x00000010
#define	I2S_DL_SW_RST_EN_SHFT		4

#define	I2S_UL_SW_RST_EN_MASK		0x00000100
#define	I2S_UP_SW_RST_EN_SHFT		8

/* DL FIFO Write	Side : GDMA -->
 *DL_FIFO	(0x380x0010, DL_FIFO_W_CONTROL)
 */
#define	I2S_DL_FIFO_ALMOST_FULL		0x1
#define	I2S_DL_FIFO_NOT_ALMOST_FULL	0x0
#define	I2S_DL_FIFO_ALMOST_FULL_MASK	0x00000001
#define	I2S_DL_FIFO_ALMOST_FULL_SHFT	0

#define	I2S_DL_FIFO_FULL		0x1
#define	I2S_DL_FIFO_NOT_FULL		0x0
#define	I2S_DL_FIFO_FULL_MASK		0x00000002
#define	I2S_DL_FIFO_FULL_SHFT		1

#define	I2S_DL_W_FIFO_WCLEAR		0x1
#define	I2S_DL_W_FIFO_NOT_WCLEAR	0x0
#define	I2S_DL_W_FIFO_WCLEAR_MASK	0x00000004
#define	I2S_DL_W_FIFO_WCLEAR_SHFT	2

#define	I2S_DL_W_FIFO_CNT_MASK		0x0000FF00
#define	I2S_DL_W_FIFO_CNT_SHFT		8

#define	I2S_DL_W_FIFO_THR_MASK		0x00FF0000
#define	I2S_DL_W_FIFO_THR_SHFT		16

#define	I2S_DL_W_FIFO_W_READY		0x1
#define	I2S_DL_W_FIFO_NOT_W_READY	0x0
#define	I2S_DL_W_FIFO_W_READY_MASK	0x01000000
#define	I2S_DL_W_FIFO_W_READY_SHFT	24


/* ------------I2S_DL_FIFO_CONTROL_ADDR------------------------ */
/* DL FIFO Read Side : DL_FIFO --> I2S_OUT (0x380x0014, DL_FIFO_R_CONTROL) */
#define	I2S_DL_FIFO_ALMOST_EMPTY	0x1
#define	I2S_DL_FIFO_NOT_ALMOST_EMPTY	0x0
#define	I2S_DL_FIFO_ALMOST_EMPTY_MASK	0x00000001
#define	I2S_DL_FIFO_ALMOST_EMPTY_SHFT	0

#define	I2S_DL_FIFO_EMPTY		0x1
#define	I2S_DL_FIFO_NOT_EMPTY		0x0
#define	I2S_DL_FIFO_EMPTY_MASK		0x00000002
#define	I2S_DL_FIFO_EMPTY_SHFT		1

#define	I2S_DL_R_FIFO_RCLEAR		0x1
#define	I2S_DL_R_FIFO_NOT_RCLEAR	0x0
#define	I2S_DL_R_FIFO_RCLEAR_MASK	0x00000004
#define	I2S_DL_R_FIFO_RCLEAR_SHFT	2

#define	I2S_DL_R_FIFO_CNT_MASK		0x0000FF00
#define	I2S_DL_R_FIFO_CNT_SHFT		8

#define	I2S_DL_R_FIFO_THR_MASK		0x00FF0000
#define	I2S_DL_R_FIFO_THR_SHFT		16

#define	I2S_DL_R_FIFO_R_READY		0x1
#define	I2S_DL_R_FIFO_NOT_R_READY	0x0
#define	I2S_DL_R_FIFO_R_READY_MASK	0x01000000
#define	I2S_DL_R_FIFO_R_READY_SHFT	24

/* UL FIFO Write	Side : I2S_IN --> UL_FIFO
 *(0x380x0018, UL_FIFO_W_CONTROL)
 */
#define	I2S_UL_FIFO_ALMOST_FULL		0x1
#define	I2S_UL_FIFO_NOT_ALMOST_FULL	0x0
#define	I2S_UL_FIFO_ALMOST_FULL_MASK	0x00000001
#define	I2S_UL_FIFO_ALMOST_FULL_SHFT	0

#define	I2S_UL_FIFO_FULL		0x1
#define	I2S_UL_FIFO_NOT_FULL		0x0
#define	I2S_UL_FIFO_FULL_MASK		0x00000002
#define	I2S_UL_FIFO_FULL_SHFT		1

#define	I2S_UL_W_FIFO_WCLEAR		0x1
#define	I2S_UL_W_FIFO_NOT_WCLEAR	0x0
#define	I2S_UL_W_FIFO_WCLEAR_MASK	0x00000004
#define	I2S_UL_W_FIFO_WCLEAR_SHFT	2

#define	I2S_UL_W_FIFO_CNT_MASK		0x0000FF00
#define	I2S_UL_W_FIFO_CNT_SHFT		8

#define	I2S_UL_W_FIFO_THR_MASK		0x00FF0000
#define	I2S_UL_W_FIFO_THR_SHFT		16

#define	I2S_UL_W_FIFO_W_READY		0x1
#define	I2S_UL_W_FIFO_NOT_W_READY	0x0
#define	I2S_UL_W_FIFO_W_READY_MASK	0x01000000
#define	I2S_UL_W_FIFO_W_READY_SHFT	24

/* ------------I2S_UL_FIFO_CONTROL_ADDR------------------------ */
/* UL FIFO Read Side : UL_FIFO --> GDMA (0x380x001C, UL_FIFO_R_CONTROL) */
#define	I2S_UL_FIFO_ALMOST_EMPTY	0x1
#define	I2S_UL_FIFO_NOT_ALMOST_EMPTY	0x0
#define	I2S_UL_FIFO_ALMOST_EMPTY_MASK	0x00000001
#define	I2S_UL_FIFO_ALMOST_EMPTY_SHFT	0

#define	I2S_UL_FIFO_EMPTY		0x1
#define	I2S_UL_FIFO_NOT_EMPTY		0x0
#define	I2S_UL_FIFO_EMPTY_MASK		0x00000002
#define	I2S_UL_FIFO_EMPTY_SHFT		1

#define	I2S_UL_R_FIFO_RCLEAR		0x1
#define	I2S_UL_R_FIFO_NOT_RCLEAR	0x0
#define	I2S_UL_R_FIFO_RCLEAR_MASK	0x00000004
#define	I2S_UL_R_FIFO_RCLEAR_SHFT	2

#define	I2S_UL_R_FIFO_CNT_MASK		0x0000FF00
#define	I2S_UL_R_FIFO_CNT_SHFT		8

#define	I2S_UL_R_FIFO_THR_MASK		0x00FF0000
#define	I2S_UL_R_FIFO_THR_SHFT		16

#define	I2S_UL_R_FIFO_R_READY		0x1
#define	I2S_UL_R_FIFO_NOT_R_READY	0x0
#define	I2S_UL_R_FIFO_R_READY_MASK	0x01000000
#define	I2S_UL_R_FIFO_R_READY_SHFT	24

/* ------------I2S_DMA_IF_CONTROL-------------------------------------- */
#define	I2S_UL_DMAREQ_MI_NUM_HALT	0x0
#define	I2S_UL_DMAREQ_MI_NUM_SINGLE	0x1
#define	I2S_UL_DMAREQ_MI_NUM_MUTL_2	0x2
#define	I2S_UL_DMAREQ_MI_NUM_MUTL_3	0x3
#define	I2S_UL_DMAREQ_MI_NUM_MASK	0x00030000
#define	I2S_UL_DMAREQ_MI_NUM_SHIFT	16

#define	I2S_DL_DMAREQ_MI_NUM_HALT	0x0
#define	I2S_DL_DMAREQ_MI_NUM_SINGLE	0x1
#define	I2S_DL_DMAREQ_MI_NUM_MUTL_2	0x2
#define	I2S_DL_DMAREQ_MI_NUM_MUTL_3	0x3
#define	I2S_DL_DMAREQ_MI_NUM_MASK	0x00000003
#define	I2S_DL_DMAREQ_MI_NUM_SHIFT	0

/* ------------I2S_GLOBAL_INTERRUPT_ENABLE_ADDR------------------------ */
#define	WRITE_DL_FIFO_FULL_INT_EN	0x00000001
#define	READ_DL_FIFO_EMPTY_INT_EN	0x00000002
#define	DL_FIFO_W_READY_INT_EN		0x00000010
#define	DL_FIFO_R_READY_INT_EN		0x00000020
#define	WRITE_UL_FIFO_FULL_INT_EN	0x00000100
#define	READ_UL_FIFO_EMPTY_INT_EN	0x00000200
#define	UL_FIFO_W_READY_INT_EN		0x00001000
#define	UL_FIFO_R_READY_INT_EN		0x00002000
#define	ALL_INT_EN			0x00003333

/* ------------I2S_GLOBAL_INTERRUPT_STATUS_CLEAR_ADDR----------------- */
#define	WRITE_DL_FIFO_FULL_STS_CLR	0x00000001
#define	READ_DL_FIFO_EMPTY_STS_CLR	0x00000002
#define	DL_FIFO_W_READY_STS_CLR		0x00000010
#define	DL_FIFO_R_READY_STS_CLR		0x00000020
#define	WRITE_UL_FIFO_FULL_STS_CLR	0x00000100
#define	READ_UL_FIFO_EMPTY_STS_CLR	0x00000200
#define	UL_FIFO_W_READY_STS_CLR		0x00001000
#define	UL_FIFO_R_READY_STS_CLR		0x00002000

/* ------------I2S_GLOBAL_INTERRUPT_STATUS_ADDR------------------------ */
#define	WRITE_DL_FIFO_FULL_STS		0x00000001
#define	READ_DL_FIFO_EMPTY_STS		0x00000002
#define	DL_FIFO_W_READY_STS		0x00000010
#define	DL_FIFO_R_READY_STS		0x00000020
#define	WRITE_UL_FIFO_FULL_STS		0x00000100
#define	READ_UL_FIFO_EMPTY_STS		0x00000200
#define	UL_FIFO_W_READY_STS		0x00001000
#define	UL_FIFO_R_READY_STS		0x00002000



/* ------------------------------------------------------ */

/* ------------I2S_DMA_ADDR------------------------ */
#define	I2S_DMA_TX_FIFO		0x79000000 /* DMA12 */
#define	I2S_DMA_RX_FIFO		0x79000100 /* DMA13 */

#define	I2S_DMA_TX_FIFO_CNT	0x83010C38
#define	I2S_DMA_RX_FIFO_CNT	0x83010D38



/* ------------I2S Variable------------------------ */
#define	I2S_TRUE			1
#define	I2S_FALSE			0

#define	I2S_EN				1
#define	I2S_DIS				0
#define	I2S_DLFIFO_EN			1
#define	I2S_DLFIFO_DIS			0

#define	I2S_ULFIFO_EN			1
#define	I2S_ULFIFO_DIS			0

#define	I2S_ENGEN_EN			1
#define	I2S_ENGEN_DIS			0

#define	I2S_EXT_EN			1
#define	I2S_EXT_DIS			0

#define	I2S_DL_LRSW_EN			1
#define	I2S_DL_LRSW_DIS			0

#define	I2S_DL_MONO			1
#define	I2S_DL_STEREO			0

#define	I2S_DL_MONO_DUP_EN		1
#define	I2S_DL_MONO_DUP_DIS		0

#define	I2S_CK_INV_EN			1
#define	I2S_CK_INV_DIS			0

#define	I2S_NEG_CAP_EN			1
#define	I2S_NEG_CAP_DIS			0

#define	I2S_LOOPBACK_EN			1
#define	I2S_LOOPBACK_DIS		0

#define	I2S_DL_EN			1
#define	I2S_DL_DIS			0

#define	I2S_DL_SRC_SLAVE		1
#define	I2S_DL_SRC_MASTER		0

#define	I2S_DL_FMT_I2S			1
#define	I2S_DL_FMT_TDM			0

#define	I2S_DL_WSINV_EN			1
#define	I2S_DL_WSINV_DIS		0

#define	I2S_CLK_SEL_13M			0x0
#define	I2S_CLK_SEL_26M			0x1
#define	I2S_CLK_SEL_XPLL_16M		0x2
#define	I2S_CLK_SEL_EXT_BCLK		0x3

#define	I2S_26M_SEL_XPLL		1
#define	I2S_26M_SEL_XTAL		0
#define	I2S_26M_SEL_MASK		0x00040000
#define	I2S_26M_SEL_SHFT		18

#define	I2S_DL_SR_8K			0x0
#define	I2S_DL_SR_12K			0x2
#define	I2S_DL_SR_16K			0x4
#define	I2S_DL_SR_24K			0x6
#define	I2S_DL_SR_32K			0x8
#define	I2S_DL_SR_48K			0xA

#define	I2S_DL_BIT_PER_S_32BTIS		0x0
#define	I2S_DL_BIT_PER_S_64BTIS		0x1
#define	I2S_DL_BIT_PER_S_128BTIS	0x2

#define	I2S_DL_WS_RESYNC_EN		1
#define	I2S_DL_WS_RESYNC_DIS		0

#define	I2S_DL_CH_PER_S_2_CH		0x0
#define	I2S_DL_CH_PER_S_4_CH		0x1

#define	I2S_UL_EN			1
#define	I2S_UL_DIS			0

#define	I2S_UL_SRC_SLAVE		1
#define	I2S_UL_SRC_MASTER		0

#define	I2S_UL_FMT_I2S			1
#define	I2S_UL_FMT_TDM			0

#define	I2S_UL_WSINV_EN			1
#define	I2S_UL_WSINV_DIS		0

#define	I2S_UL_SR_8K			0x0
#define	I2S_UL_SR_12K			0x2
#define	I2S_UL_SR_16K			0x4
#define	I2S_UL_SR_24K			0x6
#define	I2S_UL_SR_32K			0x8
#define	I2S_UL_SR_48K			0xA

#define	I2S_UL_BIT_PER_S_32BTIS		0x0
#define	I2S_UL_BIT_PER_S_64BTIS		0x1
#define	I2S_UL_BIT_PER_S_128BTIS	0x2

#define	I2S_UL_WS_RESYNC_EN		1
#define	I2S_UL_WS_RESYNC_DIS		0

#define	I2S_UL_DOWN_RATE_EN		1
#define	I2S_UL_DOWN_RATE_DIS		0

#define	I2S_UL_CH_PER_S_2_CH		0x00
#define	I2S_UL_CH_PER_S_4_CH		0x01

#define	I2S_UL_LR_SWAP_EN		1
#define	I2S_UL_LR_SWAP_DIS		0

#define	I2S_SW_RST_EN			0x1
#define	I2S_SW_RST_DIS			0x0

#define	I2S_DMA_CVFF_EN			1
#define	I2S_DMA_CVFF_DIS		0

#define	I2S_DMA_INTR_EN			1
#define	I2S_DMA_INTR_DIS		0

#define	I2S_STATE_IDLE			0
#define	I2S_STATE_RX_RUNNING		1
#define	I2S_STATE_TX_RUNNING		2

#define i2s_err(fmt, args...)	osai_print(fmt, ## args)

#ifdef I2S_DUMP_DBG
#define i2s_debug(fmt, args...)	osai_print(fmt, ## args)
#else
#define i2s_debug(fmt, args...)
#endif

/** @defgroup Driver_i2s_enum Enum
 * @{
 */
/** @brief Initial register config struct type select.*/
typedef	enum {
	/**< I2S Internal loop back */
	I2S_INTERNAL_LOOPBACK_MODE	= 0,
	/**< I2S slave normal mode   */
	I2S_EXTERNAL_MODE,
	/**< TDM slave normal mode */
	I2S_EXTERNAL_TDM_MODE,
	/**< Type NUM. */
	I2S_INITIAL_TYPE_NUM
} enum_i2s_initial_type;

/** @brief Down	link sample rate select*/
typedef	enum {
	/**< Down link sample rate 8k  */
	I2S_DL_SAMPLE_RATE_8K	= I2S_DL_SR_8K,
	/**< Down link sample rate 12k */
	I2S_DL_SAMPLE_RATE_12K	= I2S_DL_SR_12K,
	/**< Down link sample rate 16k */
	I2S_DL_SAMPLE_RATE_16K	= I2S_DL_SR_16K,
	/**< Down link sample rate 24k */
	I2S_DL_SAMPLE_RATE_24K	= I2S_DL_SR_24K,
	/**< Down link sample rate 32k */
	I2S_DL_SAMPLE_RATE_32K	= I2S_DL_SR_32K,
	/**< Down link sample rate 48k */
	I2S_DL_SAMPLE_RATE_48K	= I2S_DL_SR_48K
} enum_i2s_dl_sample_rate;

/** @brief Up link sample rate select*/
typedef	enum {
	/**< Up	link sample rate 8k */
	I2S_UL_SAMPLE_RATE_8K	= I2S_UL_SR_8K,
	/**< Up link sample rate 12k */
	I2S_UL_SAMPLE_RATE_12K	= I2S_UL_SR_12K,
	/**< Up link sample rate 16k */
	I2S_UL_SAMPLE_RATE_16K	= I2S_UL_SR_16K,
	/**< Up	link sample rate 24k */
	I2S_UL_SAMPLE_RATE_24K	= I2S_UL_SR_24K,
	/**< Up link sample rate 32k */
	I2S_UL_SAMPLE_RATE_32K	= I2S_UL_SR_32K,
	/**< Up link sample rate 48k */
	I2S_UL_SAMPLE_RATE_48K	= I2S_UL_SR_48K
} enum_i2s_ul_sample_rate;

/** @brief Down	link mono/stereo select	*/
typedef	enum {
	/**< Down link mono mode */
	I2S_DL_MONO_MODE	= I2S_DL_MONO,
	/**< Down link stereo mode */
	I2S_DL_STEREO_MODE	= I2S_DL_STEREO
} enum_i2s_dl_mono_stereo_mode;

/** @brief Down	link number of bits in each FS cycle select */
typedef	enum {
	/**< number of bits is 32 bit*/
	I2S_DL_32BITS_PER_SAMPLE	= I2S_DL_BIT_PER_S_32BTIS,
	/**< number of bits is 64 bit */
	I2S_DL_64BITS_PER_SAMPLE	= I2S_DL_BIT_PER_S_64BTIS,
	/**< number of bits is 128 bit */
	I2S_DL_128BITS_PER_SAMPLE	= I2S_DL_BIT_PER_S_128BTIS
} enum_i2s_dl_bit_per_sample_rate;

/** @brief Up link number of bits in each FS cycle select */
typedef	enum {
	/**< number of bits is 32 bit */
	I2S_UL_32BITS_PER_SAMPLE	= I2S_UL_BIT_PER_S_32BTIS,
	/**< number of bits is 64	bit */
	I2S_UL_64BITS_PER_SAMPLE	= I2S_UL_BIT_PER_S_64BTIS,
	/**< number of bits is 128 bit */
	I2S_UL_128BITS_PER_SAMPLE	= I2S_UL_BIT_PER_S_128BTIS
} enum_i2s_ul_bit_per_sample_rate;

/** @brief Down	link number of channel in each FS cycle	select */
typedef	enum {
	/**< number of channel is 2 channel */
	I2S_DL_2_CH_PER_SAMPLE	= I2S_DL_CH_PER_S_2_CH,
	/**< number of channel is 4 channel */
	I2S_DL_4_CH_PER_SAMPLE	= I2S_DL_CH_PER_S_4_CH
} enum_i2s_dl_ch_per_sample;

/** @brief Up link number of channel in	each FS	cycle select */
typedef	enum {
	/**< number of channel is 2 channel */
	I2S_UL_2_CH_PER_SAMPLE	= I2S_UL_CH_PER_S_2_CH,
	/**< number of channel is 4 channel */
	I2S_UL_4_CH_PER_SAMPLE	= I2S_UL_CH_PER_S_4_CH
} enum_i2s_ul_ch_per_sample;

/**
 * @}
 */
/** @defgroup Driver_i2s_struct	Struct
 * @{
 */
/** @brief I2S GLOBAL_CONTROL register config struct  */
struct i2s_gbl_cfg {
	/**bit 31 LOOPBACK,I2S out to I2S in loopback */
	u8		i2s_loopback_en;
	/**bit 29:28 EXT_MCLK_SEL, I2S MCLK source select*/
	u32		i2s_mclk_sel;
	/**bit 27:26 LK_SEL_OUT, I2S out clock source selection*/
	u32		i2s_out_clk_sel;
	/**bit 25:24 CLK_SEL_IN, I2S in source selection*/
	u32		i2s_in_clk_sel;
	/**bit 23 UL_EMPTY_VALUE_EN, Enable to return 0 for
	 * AHB read when UL FIFO is empty.
	 */
	u8		i2s_ul_empty_val_en;
	/**bit 22 DL_EMPTY_VALUE_EN, Enable to repeat DL sample
	 *value from CR after DL FIFO is empt
	 */
	u8		i2s_dl_empty_val_en;
	/**bit 20 NEG_CAP, Negative edge capture RX data*/
	u8		i2s_negcap_en;
	/**bit 19 EXT_BCLK_INV, BCLK clock inverse*/
	u8		i2s_clk_inv_en;
	/**bit 18 X26M_SEL 26M, clock source selection*/
	u8		i2s_26m_sel;
	/**bit 12 I2S_OUT_CLK_EN, Enable I2S out clock*/
	u8		i2s_out_clk_en;
	/**bit 11 I2S_IN_CLK_EN, Enable I2S in clock*/
	u8		i2s_in_clk_en;
	/**bit 10 MCLK_OUTPUT_EN, Enable mclk output*/
	u8		i2s_mclk_en;
	/**bit 9 DL_MONO_DUP When DL_MONO=1,
	 *if right channel send duplicate data
	 */
	u8		i2s_dl_monodup_en;
	/**bit 8 DL_MONO, DL MONO mode*/
	u8		i2s_dl_mono_stereo_sel;
	/**bit 7 DL_LRSW, DL with LR switch*/
	u8		i2s_dl_swap_lr;
	/**bit 5 EXT,External codec select*/
	u8		i2s_ext_codec_sel;
};

typedef	struct i2s_gbl_cfg i2s_gbl_cfg;

/** @brief I2S DL_CONTROL register config struct  */
struct i2s_dl_cfg {
	/**bit 30:29 CH_PER_S, Number of channel in each FS cycle
	 *(just used in TDM mode)
	 */
	u32		i2s_ch_per_sample;
	/**bit 23:17 MSB_OFFSET, Delay cycle from rising edge of
	 *FS to first channel MSB
	 */
	u32		i2s_msbo_ffset;
	/**bit 14:13BIT_PER_S, Number of bits in each FS cycle */
	u32		i2s_bit_per_sample;
	/**bit 11:8SR,	Sample rate */
	u32		i2s_sample_rate;
	/**bit 7DLFIFO_2DEQ DLFIFO, dequeue 2 entries per frame
	 *for TDM64 mode
	 */
	u8		i2s_dl_fifo2deq;
	/**bit 5WSINV,WS reverse */
	u8		i2s_word_sel_inv;
	/**bit 3FMT,Data Format (I2S or TDM) */
	u8		i2s_format;
	/**bit 2SRC,Master/Slave mode */
	u8		i2s_src;
};
typedef	struct i2s_dl_cfg i2s_dl_cfg;

/** @brief I2S UL_CONTROL register config struct  */
struct i2s_ul_cfg {
	/**bit 31 LR_SWAP, Swap the data of Right and Left channel */
	u8		i2s_lr_swap;
	/**bit 30:29 CH_PER_S,Number of channel in each FS cycle
	 *(just used in TDM mode)
	 */
	u32		i2s_ch_per_sample;
	/**bit 28:24 UPDATE_WORD, Which cycle will I2S_IN update
	 *data to UL_FIFO
	 */
	u32		i2s_update_word;
	/**bit 23:17 MSB_OFFSET, Delay cycle from rising edge of
	 *FS to first channel MSB
	 */
	u32		i2s_msbo_ffset;
	/**bit 16 DOWN_RATE,Will UL real sample rate is 1/2 of SR */
	u8		i2s_down_rate_en;
	/**14:13BIT_PER_S, Number of bits in each FS cycle */
	u32		i2s_bit_per_sample;
	/**bit 11:8SR,Sample rate */
	u32		i2s_sample_rate;
	/**bit5	WSINV, WS reverse */
	u8		i2s_word_sel_inv;
	/**bit 3FMT, Data Format (I2S or TDM) */
	u8		i2s_format;
	/**bit 2SRC, Master/Slave mode */
	u8		i2s_src;
};
typedef	struct i2s_ul_cfg i2s_ul_cfg;

/**
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

void mtk_hdl_i2s_gbl_cfg(void __iomem *base_address,
			 struct i2s_gbl_cfg *i2s_gbl_cfg);

void mtk_hdl_i2s_dl_cfg(void __iomem *base_address,
			struct i2s_dl_cfg *i2s_dl_cfg);

void mtk_hdl_i2s_ul_cfg(void __iomem *base_address,
			struct i2s_ul_cfg *i2s_ul_cfg);

void mtk_hdl_i2s_clk_gating(void __iomem *base_address, u8 en);

void mtk_hdl_i2s_clk_fifo_en(void __iomem *base_address, u8 en);

void mtk_hdl_i2s_dl_en(void __iomem *base_address, u8 en);

void mtk_hdl_i2s_ul_en(void __iomem *base_address, u8 en);

void mtk_hdl_i2s_reset(void __iomem *base_address);

void mtk_hdl_i2s_cfg_tdm_ch_bit_per_sample(
		enum_i2s_dl_ch_per_sample dl_ch_per_sample,
		enum_i2s_ul_ch_per_sample ul_ch_per_sample,
		enum_i2s_dl_bit_per_sample_rate dl_bit_per_sample,
		enum_i2s_ul_bit_per_sample_rate ul_bit_per_sample,
		struct i2s_dl_cfg *i2s_dl_cfg,
		struct i2s_ul_cfg *i2s_ul_cfg);

void mtk_hdl_i2s_cfg_mono_stereo(
			enum_i2s_dl_mono_stereo_mode dl_mono_stereo,
			u8 i2s_dl_monodup_en,
			struct i2s_gbl_cfg *i2s_gbl_cfg);

void mtk_hdl_i2s_cfg_sample_rate(
			enum_i2s_dl_sample_rate dl_sample_rate,
			enum_i2s_ul_sample_rate ul_sample_rate,
			u8  ul_down_rate_en,
			struct i2s_dl_cfg *i2s_dl_cfg,
			struct i2s_ul_cfg *i2s_ul_cfg);

void mtk_hdl_i2s_cfg_init_setting(enum_i2s_initial_type initial_type,
				  struct i2s_gbl_cfg *i2s_gbl_cfg,
				  struct i2s_dl_cfg *i2s_dl_cfg,
				  struct i2s_ul_cfg *i2s_ul_cfg);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __HDL_I2S_H__ */
