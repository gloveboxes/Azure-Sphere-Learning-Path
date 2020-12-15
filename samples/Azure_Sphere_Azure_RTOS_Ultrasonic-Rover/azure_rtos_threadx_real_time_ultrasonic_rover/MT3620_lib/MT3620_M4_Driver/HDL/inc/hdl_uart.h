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

#ifndef __HDL_UART_H__
#define __HDL_UART_H__

#include "mhal_osai.h"

/* debug help MACRO */
#define MTK_UART_DEBUG_ENABLE 0

#define uart_err(fmt, arg...)	osai_print(fmt, ##arg)
#if MTK_UART_DEBUG_ENABLE
#define uart_debug(fmt, arg...)	osai_print(fmt, ##arg)
#else
#define uart_debug(fmt, arg...)
#endif

#define UART_CLOCK		(26000000UL)

#define MAX_UART_BAUD	3*1024*1024
#define MIN_UART_BAUD	300

#define UART_RBR				(0x0)
#define UART_THR				(0x0)
#define UART_IER				(0x4)
#define UART_IIR				(0x8)
#define UART_FCR				(0x8)
#define UART_LCR				(0xc)
#define UART_MCR				(0x10)
#define UART_LSR				(0x14)
#define UART_MSR				(0x18)
#define UART_SCR				(0x1c)
#define UART_DLL				(0x0)
#define UART_DLH				(0x4)
#define UART_EFR				(0x8)
#define UART_XON1				(0x10)
#define UART_XON2				(0x14)
#define UART_XOFF1				(0x18)
#define UART_XOFF2				(0x1c)

#define UART_RX_LEVEL_CTL		(0x20)
#define UART_RATE_STEP			(0x24)
#define UART_STEP_COUNT			(0x28)
#define UART_SAMPLE_COUNT		(0x2c)
#define UART_AUTOBAUD_REG		(0x30)
#define UART_RATE_FIX_REG		(0x34)
#define UART_ESCAPE_DATA		(0x40)
#define UART_ESCAPE_EN			(0x44)
#define UART_SW_RTO_LIMIT		(0x48)
#define UART_VFIFO_EN_REG		(0x4C)
#define UART_RXTRIG_REG			(0x50)
#define UART_FRACDIV_L			(0x54)
#define UART_FRACDIV_M			(0x58)
#define UART_FCR_RO			(0x5c)
#define EXTEND_ADD			(0x64)
#define RX_OFFSET			(0x68)
#define TX_OFFSET			(0x6C)
#define INTERRUPT_STA			(0x70)

/* FCR */
#define UART_FCR_FIFOE			(1 << 0)
#define UART_FCR_CLRR			(1 << 1)
#define UART_FCR_CLRT			(1 << 2)
#define UART_FCR_RXFIFO_1B_TRI		(0 << 6)
#define UART_FCR_RXFIFO_6B_TRI		(1 << 6)
#define UART_FCR_RXFIFO_12B_TRI		(2 << 6)
#define UART_FCR_RXFIFO_RX_TRI		(3 << 6)
#define UART_FCR_TXFIFO_1B_TRI		(0 << 4)
#define UART_FCR_TXFIFO_4B_TRI		(1 << 4)
#define UART_FCR_TXFIFO_8B_TRI		(2 << 4)
#define UART_FCR_TXFIFO_14B_TRI		(3 << 4)
#define UART_FCR_FIFO_INIT		(UART_FCR_FIFOE | \
					UART_FCR_CLRR | UART_FCR_CLRT)
#define UART_FCR_NORMAL			(UART_FCR_FIFO_INIT | \
					UART_FCR_RXFIFO_12B_TRI)

/* LCR */
#define UART_LCR_BREAK			(1 << 6)
#define UART_LCR_DLAB			(1 << 7)
#define UART_WLS_5			(0 << 0)
#define UART_WLS_6			(1 << 0)
#define UART_WLS_7			(2 << 0)
#define UART_WLS_8			(3 << 0)
#define UART_WLS_MASK			(3 << 0)
#define UART_1_STOP			(0 << 2)
#define UART_2_STOP			(1 << 2)
#define UART_STOP_MASK			(1 << 2)
#define UART_PARITY_NONE		(0 << 3)
#define UART_PARITY_ODD			(0x1 << 3)
#define UART_PARITY_EVEN		(0x3 << 3)
#define UART_PARITY_MARK		(0x5 << 3)
#define UART_PARITY_SPACE		(0x7 << 3)
#define UART_PARITY_MASK		(0x7 << 3)

/* MCR */
#define UART_MCR_DTR			(1 << 0)
#define UART_MCR_RTS			(1 << 1)
#define UART_MCR_LOOP			(1 << 4)
#define UART_MCR_NORMAL			(UART_MCR_DTR|UART_MCR_RTS)

/* LSR */
#define UART_LSR_DR			(0x01)
#define UART_LSR_THRE			(0x20)

#define UART_IER_ALLOFF			(0x00)
#define TXRX_DMA_HSK_EN			(0x03)
#define SW_FLOW_INT_W1C			(0x20)
#define HW_FLOW_INT_W1C			(0x40)

#ifdef __cplusplus
extern "C" {
#endif

void mtk_hdl_uart_cg_config(void __iomem *uart_cg_base, bool enable_clk);
void mtk_hdl_uart_reset(void __iomem *uart_cg_base);

void mtk_hdl_uart_dmreg(void __iomem *uart_base);
void mtk_hdl_uart_init(void __iomem *uart_base);

void mtk_hdl_uart_set_baudrate(void __iomem *uart_base, u32 baudrate);
void mtk_hdl_uart_set_format(void __iomem *uart_base, u8 databit,
	u8 parity, u8 stopbit);

void mtk_hdl_uart_dma_en(void __iomem *uart_base, bool dma_en);
void mtk_hdl_uart_tx_dma_handshake(void __iomem *uart_base, bool hsk_en);
void mtk_hdl_uart_rx_dma_handshake(void __iomem *uart_base, bool hsk_en);

u8 mtk_hdl_uart_clear_irq_status(void __iomem *uart_base);
void mtk_hdl_uart_set_irq(void __iomem *uart_base, u8 int_flag);

void mtk_hdl_uart_set_hw_fc(void __iomem *uart_base, u8 hw_fc);
void mtk_hdl_uart_disable_sw_fc(void __iomem *uart_base);
void mtk_hdl_uart_set_sw_fc(void __iomem *uart_base,
		u8 xon1, u8 xoff1, u8 xon2, u8 xoff2, u8 escape_data);


/* PIO mode */
void mtk_hdl_uart_output_char(void __iomem *uart_base, u8 c);
u8 mtk_hdl_uart_input_char(void __iomem *uart_base);

#ifdef __cplusplus
}
#endif

#endif /*_HDL_UART_H_*/

