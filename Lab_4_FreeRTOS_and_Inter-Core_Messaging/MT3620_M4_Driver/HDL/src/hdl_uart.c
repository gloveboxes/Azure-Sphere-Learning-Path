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

void mtk_hdl_uart_cg_config(void __iomem *uart_cg_base, bool enable_clk)
{
	u32 reg_val;

	reg_val = osai_readl(uart_cg_base);

	if (enable_clk)
		reg_val |= 0x00006000;
	else
		reg_val &= ~0x00006000;

	osai_writel(reg_val, uart_cg_base);
}

void mtk_hdl_uart_reset(void __iomem *uart_cg_base)
{
	u32 reg_val;

	reg_val = osai_readl(uart_cg_base);

	osai_writel(reg_val & (~0x00000001), uart_cg_base);
	osai_writel(reg_val | 0x00000001, uart_cg_base);
}

void mtk_hdl_uart_dmreg(void __iomem *uart_base)
{
	u32 i;

	for (i = 0; i < 0x80; i += 0x10) {
		uart_debug("addr: 0x%02x: %08X %08X %08X %08X\n", i,
			osai_readl(uart_base + i),
			osai_readl(uart_base + i + 4),
			osai_readl(uart_base + i + 8),
			osai_readl(uart_base + i + 0x0C));
	}
}

void mtk_hdl_uart_init(void __iomem *uart_base)
{
	/* FIFO init */
	osai_writel(UART_FCR_NORMAL, uart_base + UART_FCR);

	/* Disbale CTS/RTS in loopback mode */
	osai_writel(0x00, uart_base + UART_MCR);

	/* Disbale all interrupt */
	osai_writel(UART_IER_ALLOFF, uart_base + UART_IER);
}

void mtk_hdl_uart_set_baudrate(void __iomem *uart_base, u32 baudrate)
{
	u8 uart_lcr, fraction;
	u32 data, high_speed_div, sample_count, sample_point;
	u8 fraction_L_mapping[] = {0x00, 0x10, 0x44, 0x92, 0x59,
			0xab, 0xb7, 0xdf, 0xff, 0xff, 0xff};
	u8 fraction_M_mapping[] = {0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x01, 0x03};

	/* Clear fraction */
	osai_writel(0x00, uart_base + UART_FRACDIV_L);
	osai_writel(0x00, uart_base + UART_FRACDIV_M);

	/* High speed mode */
	osai_writel(0x03, uart_base + UART_RATE_STEP);

	/* DLAB start */
	uart_lcr = osai_readl(uart_base + UART_LCR);
	osai_writel(uart_lcr | UART_LCR_DLAB, uart_base + UART_LCR);

	data = UART_CLOCK / baudrate;
	/* divided by 256 */
	high_speed_div = (data >> 8) + 1;

	sample_count = data/high_speed_div - 1;
	/* threshold value */
	if (sample_count == 3)
		sample_point = 0;
	else
		sample_point = (sample_count + 1) / 2 - 2;

	/* check uart_clock, prevent calculation overflow */
	fraction = ((UART_CLOCK * 10 / baudrate * 10 / high_speed_div -
		(sample_count + 1) * 100) * 10 + 55) / 100;

	osai_writel((high_speed_div & 0x00ff), uart_base + UART_DLL);
	osai_writel((high_speed_div >> 8) & 0x00ff, uart_base + UART_DLH);
	osai_writel(sample_count, uart_base + UART_STEP_COUNT);
	osai_writel(sample_point, uart_base + UART_SAMPLE_COUNT);
	osai_writel(fraction_M_mapping[fraction], uart_base + UART_FRACDIV_M);
	osai_writel(fraction_L_mapping[fraction], uart_base + UART_FRACDIV_L);

	/* DLAB end */
	osai_writel(uart_lcr, uart_base + UART_LCR);
}

void mtk_hdl_uart_set_format(void __iomem *uart_base, u8 databit,
	u8 parity, u8 stopbit)
{
	u8 control_word;

	/* DLAB start */
	control_word = osai_readl(uart_base + UART_LCR);

	control_word &= ~UART_WLS_MASK;
	control_word |= databit;
	control_word &= ~UART_STOP_MASK;
	control_word |= stopbit;
	control_word &= ~UART_PARITY_MASK;
	control_word |= parity;

	/* DLAB End */
	osai_writel(control_word, uart_base + UART_LCR);
}

void mtk_hdl_uart_dma_en(void __iomem *uart_base, bool dma_en)
{
	u8 reg_val;

	if (dma_en) {
		/* Clear TX and RX FIFO */
		reg_val = osai_readl(uart_base + UART_FCR_RO);
		reg_val |= UART_FCR_FIFO_INIT;
		osai_writel(reg_val, uart_base + UART_FCR);

		reg_val = 0x01;
	}
	else
		reg_val = 0x00;
	osai_writel(reg_val, uart_base + UART_VFIFO_EN_REG);
}

void mtk_hdl_uart_tx_dma_handshake(void __iomem *uart_base, bool hsk_en)
{
	u8 reg_val;

	reg_val = osai_readl(uart_base + EXTEND_ADD);

	if (hsk_en)
		reg_val |= 0x02;
	else
		reg_val &= ~0x02;

	osai_writel(reg_val, uart_base + EXTEND_ADD);
}

void mtk_hdl_uart_rx_dma_handshake(void __iomem *uart_base, bool hsk_en)
{
	u8 reg_val;

	reg_val = osai_readl(uart_base + EXTEND_ADD);

	if (hsk_en)
		reg_val |= 0x01;
	else
		reg_val &= ~0x01;

	osai_writel(reg_val, uart_base + EXTEND_ADD);
}

u8 mtk_hdl_uart_clear_irq_status(void __iomem *uart_base)
{
	u8 reg_val;

	reg_val = osai_readl(uart_base + UART_IIR);

	if (reg_val & 0x10)
		osai_writel(SW_FLOW_INT_W1C, uart_base + INTERRUPT_STA);
	else if (reg_val & 0x20)
		osai_writel(HW_FLOW_INT_W1C, uart_base + INTERRUPT_STA);

	return reg_val;
}

void mtk_hdl_uart_set_irq(void __iomem *uart_base, u8 int_flag)
{
	osai_writel(int_flag, uart_base + UART_IER);
}

void mtk_hdl_uart_set_hw_fc(void __iomem *uart_base, u8 hw_fc)
{
	u8 EFR, LCR;

	LCR = osai_readl(uart_base + UART_LCR);
	osai_writel(0xBF, uart_base + UART_LCR);

	EFR = osai_readl(uart_base + UART_EFR);
	if (hw_fc == 0)
		EFR &= 0x1F;
	else
		EFR |= (hw_fc | 0x10);
	osai_writel(EFR, uart_base + UART_EFR);

	osai_writel(0x00, uart_base + UART_ESCAPE_EN);
	osai_writel(0x00, uart_base + UART_LCR);
	osai_writel(0x02, uart_base + UART_MCR);

	osai_writel(LCR, uart_base + UART_LCR);

}

void mtk_hdl_uart_disable_sw_fc(void __iomem *uart_base)
{
	u8 EFR, LCR;

	LCR = osai_readl(uart_base + UART_LCR);

	EFR = osai_readl(uart_base + UART_EFR);
	EFR &= 0xC0;
	osai_writel(EFR, uart_base + UART_EFR);

	osai_writel(LCR, uart_base + UART_LCR);
}

void mtk_hdl_uart_set_sw_fc(void __iomem *uart_base,
		u8 xon1, u8 xoff1, u8 xon2, u8 xoff2, u8 escape_data)
{
	u8 EFR, LCR;

	LCR = osai_readl(uart_base + UART_LCR);

	osai_writel(0xBF, uart_base + UART_LCR);
	osai_writel(xon1, uart_base + UART_XON1);
	osai_writel(xon2, uart_base + UART_XON2);
	osai_writel(xoff1, uart_base + UART_XOFF1);
	osai_writel(xoff2, uart_base + UART_XOFF2);

	if ((xon2 == 0) || (xoff2 == 0))
		EFR = 0x1A;
	else if ((xon1 == 0) || (xoff1 == 0))
		EFR = 0x15;
	else
		EFR = 0x1F;
	osai_writel(EFR, uart_base + UART_EFR);

	osai_writel(LCR, uart_base + UART_LCR);

	if (escape_data) {
		osai_writel(escape_data, uart_base + UART_ESCAPE_DATA);
		osai_writel(0x01, uart_base + UART_ESCAPE_EN);
	}
}

void mtk_hdl_uart_output_char(void __iomem *uart_base, u8 c)
{
	/* THRE bit */
	while (!(osai_readl(uart_base + UART_LSR) & UART_LSR_THRE))
	;
	osai_writel(c, uart_base + UART_RBR);
}

u8 mtk_hdl_uart_input_char(void __iomem *uart_base)
{
	u8 c = 0xFF;

	/* DataReady bit */
	if (osai_readl(uart_base + UART_LSR) & UART_LSR_DR)
		c = osai_readl(uart_base + UART_RBR);

	return c;
}

