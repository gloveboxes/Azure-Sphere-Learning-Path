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

#ifndef __MHAL_UART_H__
#define __MHAL_UART_H__

#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup UART
 * @{
 * This section introduces the Universal Asynchronous Receiver/Transmitter
 * (UART) APIs including terms and acronyms, supported features, software
 * architecture, details on how to use this driver, enums,
 * structures and functions.
 *
 * @section MHAL_UART_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b DMA              | Direct Memory Access.|
 * |\b FIFO             | First In, First Out.|
 * |\b PIO              | Programming Input/Out Model.|
 * |\b UART             | Universal Asynchronous  Receiver/Transmitter.|
 * @section MHAL_UART_Features_Chapter Supported Features
 *
 * This controller supported features of UART are as follows.
 * 1. Word legth from five to eight bits
 * 2. Optional parity bit
 * 3. One or two Stop bits
 * 4. Hardward and software flow control
 * 5. Maximal baudrate is up to 3Mbps when UART system clock is in 26MHz
 * 6. TX FIFO depth: 16 bytes
 * 7. RX FIFO depth: 32 bytes
 * 8. PIO transfer mode
 * 9. DMA transfer mode
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup UART
 * @{
 * @section MHAL_UART_Driver_Usage_Chapter How to Use This Driver
 *
 * - \b SW \b Architecture: \n
 * See @ref MHAL_Overview_2_Chapter for  details of the SW architecture.
 *
 *  This section describes the definition of APIs and provides
 *  an example on FreeRTOS about how to use these APIs to develop
 *  an OS-related UART driver.\n
 *
 * - \b The \b OS-HAL \b FreeRTOS \b driver\n
 * \b sample \b code \b is \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_uart.c"> freeRTos UART sample code on github </a>
 *
 * @}
 * @}
 */

/**
* @addtogroup M-HAL
* @{
* @addtogroup UART
* @{
*/

/** @defgroup driver_uart_enum Enum
  * @{
  * This section introduces the enumerations
  * that UART should configure before transfer.
  */
/** @brief This enum defines UART data length.
 */
typedef enum {
	/** 5 bits word length */
	UART_DATA_5_BITS = 0x00,
	/** 6 bits word length */
	UART_DATA_6_BITS = 0x01,
	/** 7 bits word length */
	UART_DATA_7_BITS = 0x02,
	/** 8 bits word length */
	UART_DATA_8_BITS = 0x03,
} mhal_uart_data_len;

/** @brief This enum defines UART parity parameter.
 */
typedef enum {
	/** Disbale parity */
	UART_NONE_PARITY = 0x00,
	/** Enable odd parity */
	UART_ODD_PARITY = 0x08,
	/** Enable even parity */
	UART_EVEN_PARITY = 0x18,
	/** Enable mark parity */
	UART_MARK_PARITY = 0x28,
	/** Enable space parity */
	UART_SPACE_PARITY = 0x38,
} mhal_uart_parity;

/** @brief This enum defines stop bit length.
 */
typedef enum {
	/** 1 stop bit */
	UART_STOP_1_BIT = 0x00,
	/** 2 stop bits */
	UART_STOP_2_BIT = 0x04,
} mhal_uart_stop_bit;
/**
 * @}
 */

/** @defgroup driver_uart_typedef Typedef
  * @{
  * This section introduces the typedef that UART M-HAL used.
  */

/** @brief This defines the callback function prototype.
 * It's used for DMA mode transaction.\n
 * Users should register a callback function when using DMA mode.\n
 * In DMA mode, once UART and DMA hardware have completed the transaction,
 * a DMA interrupt is triggered and a related user callback
 * is called in the DMA interrupt service routine.\n
 * This typedef is used to describe the callback function
 * that the user wants to call.
 *
 * @param [in] user_data: An OS-HAL defined parameter is private
 * by #mtk_mhal_uart_dma_done_callback_register().
 * @sa #mtk_mhal_uart_dma_done_callback_register()
 *
 * @return
 * Return "0" if callback registers success;\n
 * otherwise, return "-1".
 */
typedef int (*uart_dma_done_callback) (void *user_data);

/**
  * @}
  */

/** @defgroup driver_uart_struct Struct
  * @{
  * This section introduces the structure used by UART OS-HAL/M-HAL/HDL.
  */
/** @brief M-HAL private structure.
 * It's only used by DMA mode to program M-HAL.
 * OS-HAL does not need to care this structure.
 */
struct mtk_uart_private {
	/** temp tx transfer len */
	u8 dma_tx_ch;
	/** temp rx transfer len */
	u8 dma_rx_ch;

	/** used for tx temp buf */
	unsigned char *tx_buf;
	/** used for rx temp buf  */
	unsigned char *rx_buf;
	/** temp tx DMA physical addr */
	dma_addr_t tx_dma;
	/** temp rx DMA physical addr */
	dma_addr_t rx_dma;

	/** temp tx transfer len */
	u32 tx_len;
	/** temp rx transfer len */
	u32 rx_len;
	/** tx transfer len */
	u32 tx_size;
	/** tx transfer len */
	u32 rx_size;

	/** user_data is a OS-HAL defined parameter provided
	* by #mtk_mhal_uart_dma_done_callback_register().
	*/
	void *user_data;
	/** This function is used to register user's TX DMA done callback
	* to OS-HAL layer
	*/
	uart_dma_done_callback uart_tx_dma_callback;
	/** This function is used to register user's RX DMA done callback
	* to OS-HAL layer
	*/
	uart_dma_done_callback uart_rx_dma_callback;
};

/** @brief Interface to UART.
 * It's used to pass arguments among OS-HAL/M-HAL/HDL.
 */
struct mtk_uart_controller {
	/** UART controller port num */
	u8 port_num;
	/** UART controller base address */
	void __iomem *base;
	/** Clock gate base address */
	void __iomem *cg_base;
	/** UART baudrate */
	u32 baudrate;
	/** UART data bit */
	mhal_uart_data_len data_bit;
	/** UART parity */
	mhal_uart_parity parity;
	/** UART stop bit */
	mhal_uart_stop_bit stop_bit;
	/** UART DMA mode. 0: half-size; 1: VFF*/
	u8 vff_dma_mode;
	/** M-HAL privite structure, used by M-HAL only */
	struct mtk_uart_private *mdata;
};
/**
 * @}
 */

/** @defgroup driver_uart_define Define
  * @{
  * This section introduces the Macro definition
  * which is used by UART M-HAL.
  */

/**It means the pointer is NULL*/
#define UART_EPTR			(1)
/**No such device or address*/
#define UART_ENXIO			(6)
/**Invalid argument*/
#define UART_EINVAL			(22)
/**Transfer timed out*/
#define UART_ETIMEDOUT		(110)

/** Disable all interrupts */
#define UART_INT_DISABLE		0x00
/** Enable Rx buffer full interrupt */
#define UART_INT_RX_BUFFER_FULL		0x01
/** Enable Tx buffer empty interrupt */
#define UART_INT_TX_BUFFER_EMPTY	0x02
/** Enable Line Status interrupt */
#define UART_INT_LINE_STATUS		0x04
/** Enable software flow control interrupt */
#define UART_INT_SW_FLOW_CONTROL	0x20
/** Enable hardware flow control RTS interrupt */
#define UART_INT_HW_FLOW_CONTROL_RTS	0x40
/** Enable hardware flow control CTS interrupt */
#define UART_INT_HW_FLOW_CONTROL_CTS	0x80

/** Disable hardware flow control */
#define UART_EFR_HW_FC_DISABLE		0x00
/** RTS hardware flow control */
#define UART_EFR_HW_FC_RTS		0x40
/** CTS fardware flow control */
#define UART_EFR_HW_FC_CTS		0x80
/** Disable hardware flow control */
#define UART_EFR_HW_FC_RTS_CTS	0xC0
/**
 * @}
 */

/** @defgroup driver_uart_function Function
  * @{
  * This section provides Fixed APIs(defined as Common Interface)
  * to fully control the MediaTek UART HW.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function is used to enable UART clock.
 * @brief Usage : This function is used to enable UART clock.
 * OS-HAL must enable clock before accessing UART regsiter;
 * otherwise, accessing UART register will fail.
 * @param [in] ctlr : UART controller used with the device.
 * @return
 * Return "0" if enable UART clock success.\n
 * Return -#UART_EPTR if ctlr is NULL.
 */
int mtk_mhal_uart_enable_clk(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to disable UART clock.
 * @brief Usage : It needs to be called to disable clock.
 * @param [in] ctlr : UART controller used with the device.
 * @return
 * Return "0" if disable UART clock success.\n
 * Return -#UART_EPTR if ctlr is NULL.
 */
int mtk_mhal_uart_disable_clk(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to reset UART.
 * @brief Usage : It needs to be called to reset UART.
 * @param [in] ctlr : UART controller used with the device.
 * @return
 * Return "0" if reset UART success.\n
 * Return -#UART_EPTR if ctlr is NULL.
 */
int mtk_mhal_uart_sw_reset(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to dump UART register.
 * @brief Usage : It needs to be called to dump UART register value.
 * @param [in] ctlr : UART controller used with the device.
 * @return
 * Return "0" if dump UART register success.\n
 * Return -#UART_EPTR if ctlr is NULL.
 */
int mtk_mhal_uart_dumpreg(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to initialize UART port.
 * @brief Usage: OS-HAL driver should call it when opening UART.
 * OS-HAL must ensure UART clock is enabled before calling this function.
 * @param [in] ctlr : UART controller used with the device.
 * @return To indicate whether UART initialize success or not.\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    if the return value is 0, it means success.\n
 */
int mtk_mhal_uart_hw_init(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to change UART baudrate.
 * @brief Usage: OS-HAL driver should call it
 * when only change UART baudrate.
 * @param [in] ctlr : UART controller used with the device.
 * @return To indicate whether UART config success or not.\n
 *    If the return value is -#UART_EINVAL, it means argument is invalid;\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    if the return value is 0, it means success.\n
 */
int mtk_mhal_uart_set_baudrate(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to set UART data format.
 * @brief Usage: OS-HAL driver should call it
 * when only configuring UART data format.
 * @param [in] ctlr : UART controller used with the device.
 * @return To indicate whether UART config success or not.\n
 *    If the return value is -#UART_EINVAL, it means argument is invalid;\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    if the return value is 0, it means success.\n
 */
int mtk_mhal_uart_set_format(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to enable UART hardware flow control.
 * @brief Usage: OS-HAL driver should call it
 * when setting hardware flow control.
 * @param [in] ctlr : UART controller used with the device.
 * @param [in] hw_fc : Hardware flow control feature;
 * please refer to UART_EFR_HW_FC_XXX type definition.
 * @return To indicate whether setting hardware flow is successfull or not.\n
 *    If the return value is -#UART_EINVAL, it means argument is invalid;\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    if the return value is 0, it means success.\n
 */
int mtk_mhal_uart_set_hw_fc(struct mtk_uart_controller *ctlr, u8 hw_fc);

/**
 * @brief This function is used to disable UART software flow control.
 * @brief Usage: OS-HAL driver should call it
 * when disabling software flow control.
 * @param [in] ctlr : UART controller used with the device.
 * @return To indicate whether disable software flow is successfull or not.\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    if the return value is 0, it means success.\n
 */
int mtk_mhal_uart_disable_sw_fc(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to configure UART software flow control.
 * @brief Usage: OS-HAL driver should call it
 * when configuring software flow control.
 * @param [in] ctlr : UART controller used with the device.
 * @param [in] xon1 : XON1 character for software flow control.
 * @param [in] xoff1 : XOFF1 character for software flow control.
 * @param [in] xon2 : XON2 character for software flow control.
 * @param [in] xoff2 : XOFF2 character for software flow control.
 * @param [in] escape_data : Escape character for software flow control.
 * @return To indicate whether set software flow is successfull or not.\n
 *    If the return value is -#UART_EINVAL, it means argument is invalid;\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    if the return value is 0, it means success.\n
 */
int mtk_mhal_uart_set_sw_fc(struct mtk_uart_controller *ctlr,
		u8 xon1, u8 xoff1, u8 xon2, u8 xoff2, u8 escape_data);

/**
 * @brief This function is used to set the DMA setting relative to the UART.
 * @brief Usage: OS-HAL driver should call it when enabling the UART DMA mode.
 * @param [in] ctlr : UART controller used with the device.
 * @param [in] enable_dma : enable or disable UART DMA mode.
 * @return To indicate whether set dma and handshake successfull or not.\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    if the return value is 0, it means success.\n
 */
int mtk_mhal_uart_set_dma(struct mtk_uart_controller *ctlr, bool enable_dma);

/**
 * @brief This function is used to get and clear UART IRQ.
 * @brief Usage: OS-HAL driver should call it when get and clear UART IRQ.
 * @param [in] ctlr : UART controller used with the device.
 * @return To indicate whether set IRQ successfull or not.\n
 *    if the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    otherwise, it means success and return IRQ status.\n
 */
int mtk_mhal_uart_clear_irq_status(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to set UART IRQ function.
 * @brief Usage: OS-HAL driver should call it when configuring UART IRQ.
 * @param [in] ctlr : UART controller used with the device.
 * @param [in] int_flag : Set IRQ type or disable IRQ;
 * please refer to UART_INT_XXX type definition.
 * @return To indicate whether set IRQ successfull or not.\n
 *    if the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    if the return value is 0, it means success.\n
 */
int mtk_mhal_uart_set_irq(struct mtk_uart_controller *ctlr, u8 int_flag);

/**
 * @brief This function is used to send one byte of data in PIO mode.
 * @brief Usage: OS-HAL driver should call it when transmitting one byte.
 * @param [in] ctlr : UART controller used with the device.
 * @param [in] data : Output data.
 * @return To indicate send data successfull or not.\n
 *    If the return value is -#EINVAL, it means argument is invalid;\n
 *    if the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    if the return value is 0, it means success.\n
 */
int mtk_mhal_uart_putc(struct mtk_uart_controller *ctlr, u8 data);

/**
 * @brief This function is used to get data in PIO mode.
 * @brief Usage: OS-HAL driver should call it when receiving one byte of data.
 * @param [in] ctlr : UART controller used with the device.
 * @return To indicate get data successfull or not.\n
 *    If the return value is -#EINVAL, it means argument is invalid;\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    otherwise, it means success and return received data.\n
 */
int mtk_mhal_uart_getc(struct mtk_uart_controller *ctlr);

/**
 * @brief This function is used to get data directly in PIO mode.
 * @brief Usage: OS-HAL driver should call it when receiving one byte of data.
 * @param [in] ctlr : UART controller used with the device.
 * @return To indicate get data successfull or not.\n
 *    If the return value is -#EINVAL, it means argument is invalid;\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    otherwise, it means success and return received data.\n
 */
int mtk_mhal_uart_getc_nowait(struct mtk_uart_controller *ctlr);

 /**
 * @brief This function is used to allocate UART DMA TX channel.
 * @brief Usage: OS-HAL driver should call it to allocate DMA TX channel.
 * @param [in] ctlr : UART controller used with the device.
 * @return To indicate allocate DMA channel successfull or not.\n
 *    If the return value is -#UART_ENXIO, it means argument is invalid;\n
 *    If the return value is -#UART_EPTR, it means ctlr is NULL;\n
 *    otherwise, it means success.\n
 */
int mtk_mhal_uart_allocate_dma_tx_ch(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to allocate UART DMA RX channel.
* @brief Usage: OS-HAL driver should call it to allocate DMA RX channel.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate allocate DMA channel successfull or not.\n
*	 If the return value is -#UART_ENXIO, it means argument is invalid;\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_allocate_dma_rx_ch(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to release UART DMA TX channel.
* @brief Usage: OS-HAL driver should call it to release DMA TX channel.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate release DMA channel successfull or not.\n
*	 If the return value is -#UART_ENXIO, it means argument is invalid;\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_release_dma_tx_ch(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to release UART DMA RX channel.
* @brief Usage: OS-HAL driver should call it to release DMA RX channel.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate release DMA channel successfull or not.\n
*	 If the return value is -#UART_ENXIO, it means argument is invalid;\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_release_dma_rx_ch(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to config UART DMA TX channel.
* @brief Usage: OS-HAL driver should call it to config DMA TX channel.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate config DMA channel successfull or not.\n
*	 If the return value is -#UART_ENXIO, it means argument is invalid;\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_dma_tx_config(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to config UART DMA RX channel.
* @brief Usage: OS-HAL driver should call it to config DMA RX channel.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate config DMA channel successfull or not.\n
*	 If the return value is -#UART_ENXIO, it means argument is invalid;\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_dma_rx_config(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to start UART DMA TX transaction.
* @brief Usage: OS-HAL driver should call it to start DMA TX transaction.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate start DMA channel transaction successfull or not.\n
*	 If the return value is -#UART_ENXIO, it means argument is invalid;\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_start_dma_tx(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to start UART DMA RX transaction.
* @brief Usage: OS-HAL driver should call it to start DMA RX transaction.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate start DMA channel transaction successfull or not.\n
*	 If the return value is -#UART_ENXIO, it means argument is invalid;\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_start_dma_rx(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to stop UART DMA TX transaction.
* @brief Usage: OS-HAL driver should call it to stop DMA TX transaction.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate stop DMA channel transaction successfull or not.\n
*	 If the return value is -#UART_ENXIO, it means argument is invalid;\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_stop_dma_tx(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to stop UART DMA RX transaction.
* @brief Usage: OS-HAL driver should call it to stop DMA TX transaction.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate stop DMA channel transaction successfull or not.\n
*	 If the return value is -#UART_ENXIO, it means argument is invalid;\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_stop_dma_rx(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to update DMA TX information when TX fail.
* @brief Usage: OS-HAL driver should call it when DMA TX transaction fail.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate update DMA channel information successfull or not.\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_update_dma_tx_info(struct mtk_uart_controller *ctlr);

/**
* @brief This function is used to update DMA RX information when RX fail.
* @brief Usage: OS-HAL driver should call it when DMA RX transaction fail.
* @param [in] ctlr : UART controller used with the device.
* @return To indicate update DMA channel information successfull or not.\n
*	 If the return value is -#UART_EPTR, it means ctlr is NULL;\n
*	 otherwise, it means success.\n
*/
int mtk_mhal_uart_update_dma_rx_info(struct mtk_uart_controller *ctlr);

/** @brief This defines the callback function prototype.
 * It's used for DMA mode TX transaction.\n
 * Users should register a TX callback function when using UART DMA mode.\n
 * In DMA mode, once UART TX and DMA hardware completes transsaction,
 * a DMA interrupt is triggered and a related user callback
 * is called in the DMA interrupt service routine.\n
 * This typedef is used to describe the callback function
 * that the user wants to call.
 *
 * @param [in] ctlr: mtk_uart_controller pointer, it contains register
 * base address,  data transmission information and UART hardware information.
 * @param [in] callback: The callback function given by OS-HAL
 * which will be called in UART module DMA interrupt service routine.
 * @param [in] user_data: A parameter given by OS-HAL and will
 * be passed to users when the callback function is called.
 *
 *@return
 * Return "0" if callback registers success;\n
 * Return -#UART_EPTR if ctlr or user_data is NULL;\n
 */
int mtk_mhal_uart_dma_tx_callback_register(
		struct mtk_uart_controller *ctlr,
		uart_dma_done_callback callback,
		void *user_data);

/** @brief This defines the callback function prototype.
 * It's used for DMA mode RX transaction.\n
 * Users should register a RX callback function when using UART DMA mode.\n
 * In DMA mode, once UART RX and DMA hardware completes transsaction,
 * a DMA interrupt is triggered and a related user callback
 * is called in the DMA interrupt service routine.\n
 * This typedef is used to describe the callback function
 * that the user wants to call.
 *
 * @param [in] ctlr: mtk_uart_controller pointer, it contains register
 * base address,  data transmission information and UART hardware information.
 * @param [in] callback: The callback function given by OS-HAL
 * which will be called in UART module DMA interrupt service routine.
 * @param [in] user_data: A parameter given by OS-HAL and will
 * be passed to users when the callback function is called.
 *
 *@return
 * Return "0" if callback registers success;\n
 * Return -#UART_EPTR if ctlr or user_data is NULL;\n
 */
int mtk_mhal_uart_dma_rx_callback_register(
		struct mtk_uart_controller *ctlr,
		uart_dma_done_callback callback,
		void *user_data);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

/**
 * @}
 * @}
 */

#endif /*_MHAL_UART_H_*/
