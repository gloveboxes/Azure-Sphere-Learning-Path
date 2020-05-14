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

#ifndef __OS_HAL_UART_H__
#define __OS_HAL_UART_H__

#include "mhal_uart.h"

/**
 * @addtogroup HAL
 * @{
 * @addtogroup UART
 * @{
 * This section describes the programming interfaces of the UART hal
 */

typedef enum {
	OS_HAL_UART_PORT0 = 0,
	OS_HAL_UART_ISU0,
	OS_HAL_UART_ISU1,
	OS_HAL_UART_ISU2,
	OS_HAL_UART_ISU3,
	OS_HAL_UART_ISU4,
	OS_HAL_UART_MAX_PORT
} UART_PORT;

/**
 * @brief  Init UART controller.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4.
 *
 *  @return -UART_EPTR means fail.
 *  @return 0 means success.
 */
int mtk_os_hal_uart_ctlr_init(UART_PORT port_num);

/**
 * @brief  Deinit UART controller.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4.
 *
 *  @return -UART_EPTR means fail.
 *  @return 0 means success.
 */
int mtk_os_hal_uart_ctlr_deinit(UART_PORT port_num);

/**
 * @brief  Dump UART register.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4.
 *
 *  @return None.
 */
void mtk_os_hal_uart_dumpreg(UART_PORT port_num);

/**
 * @brief  Set UART Baudrate.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4.
 *  @param [in] baudrate : UART Baudrate,
 *  it can be 300~3K.
 *
 *  @return None.
 */
void mtk_os_hal_uart_set_baudrate(UART_PORT port_num, u32 baudrate);

/**
 * @brief  Set UART data format.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4.
 *  @param [in] data_bit : UART data length.
 *  @param [in] parity : UART parity information.
 *  @param [in] stop_bit : UART stop_bit length.
 *
 *  @return None.
 */
void mtk_os_hal_uart_set_format(UART_PORT port_num,
		mhal_uart_data_len data_bit,
		mhal_uart_parity parity,
		mhal_uart_stop_bit stop_bit);
/**
 * @brief  Get UART one byte of data in PIO mode.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4
 *
 *  @return received data.
 */
u8 mtk_os_hal_uart_get_char(UART_PORT port_num);

/**
 * @brief  Get UART one byte of data directly in PIO mode.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4
 *
 *  @return received data.
 */
u8 mtk_os_hal_uart_get_char_nowait(UART_PORT port_num);

/**
 * @brief  Send UART one byte of data in PIO mode.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4
 *
 *  @return None.
 */
void mtk_os_hal_uart_put_char(UART_PORT port_num, u8 data);

/**
 * @brief  Set UART hardware flow control.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4
 *  @param [in] hw_fc : Hardware flow control feature,
 *  please refer to UART_EFR_HW_FC_XXX type definition
 *
 *  @return None.
 */
void mtk_os_hal_uart_set_hw_fc(UART_PORT port_num, u8 hw_fc);

/**
 * @brief  Disable UART software flow control.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4
 *
 *  @return None.
 */
void mtk_os_hal_uart_disable_sw_fc(UART_PORT port_num);

/**
 * @brief  Set UART software flow control.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4
 *  @param [in] xon1 : XON1 character for software flow control.
 *  @param [in] xoff1 : XOFF1 character for software flow control.
 *  @param [in] xon2 : XON2 character for software flow control.
 *  @param [in] xoff2 : XOFF2 character for software flow control.
 *  @param [in] escape_data : Escape character for software flow control.
 *
 *  @return None.
 */
void mtk_os_hal_uart_set_sw_fc(UART_PORT port_num,
	u8 xon1, u8 xoff1, u8 xon2, u8 xoff2, u8 escape_data);

/**
 * @brief  Get and clear UART interrupt.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4.
 *
 *  @return None.
 */
int mtk_os_hal_uart_clear_irq_status(UART_PORT port_num);

/**
 * @brief  Set UART IRQ.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4.
 *  @param [in] irq_flag : UART IRQ type,
 *  please refer to UART_INT_XXX type definition.
 *
 *  @return None.
 */
void mtk_os_hal_uart_set_irq(UART_PORT port_num, u8 irq_flag);

/**
 * @brief  Send UART data in DMA mode.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4.
 *  @param [in] data : Pointer to the data.
 *  @param [in] len : Data length.
 *  @param [in] vff_mode : true: VFF Mode; false: Half-Size Mode.
 *
 *  @return Number of bytes have been send.
 */
int mtk_os_hal_uart_dma_send_data(UART_PORT port_num,
	u8 *data, u32 len, bool vff_mode);

/**
 * @brief  Get UART data in DMA mode.
 *
 *  @param [in] bus_num : UART Port number,
 *  it can be OS_HAL_UART_PORT0~OS_HAL_UART_ISU4.
 *  @param [in] data : Pointer to the data.
 *  @param [in] len : Data length.
 *  @param [in] vff_mode : true: VFF Mode; false: Half-Size Mode.
 *
 *  @return Number of bytes have been received.
 */
int mtk_os_hal_uart_dma_get_data(UART_PORT port_num,
	u8 *data, u32 len, bool vff_mode);

#endif
