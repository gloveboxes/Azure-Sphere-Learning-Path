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

#ifndef __OS_HAL_SPIM_H__
#define __OS_HAL_SPIM_H__

#include "mhal_spim.h"

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup spim
 * @{
 * This section introduces the Serial Peripheral Interface Master (SPIM) APIs
 * including terms and acronyms, supported features,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section OS_HAL_SPIM_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b DMA                        | Direct Memory Access.|
 * |\b FIFO                       | First In, First Out.|
 * |\b SPI                        | Serial Peripheral Interface.|
 *
 * @section OS_HAL_SPIM_Features_Chapter Supported Features
 * See @ref MHAL_SPIM_Features_Chapter for the details of  Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup spim
 * @{
 * @section OS_HAL_SPIM_Driver_Usage_Chapter How to use this driver
 *
 * - \b Device \b driver \b sample \b code \b is \b as \b follows: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *    @code
 *	- init SPIM
 *	 -Call mtk_os_hal_spim_ctlr_init(spim_num bus_num)
 *	   to init spim bus.
 *
 *	- Use FIFO or DMA mode to do one blocking/synchronous SPI data transfer
 *	  -Call mtk_os_hal_spim_transfer(spim_num bus_num,
			     struct mtk_spi_config *config,
			     struct mtk_spi_transfer *xfer)
 *
 *	- Use FIFO or DMA mode to do one asynchronous SPI transfer
 *	  -Call mtk_os_hal_spim_async_transfer(spim_num bus_num,
				   struct mtk_spi_config *config,
				   struct mtk_spi_transfer *xfer,
				   spi_usr_complete_callback complete,
				   void *context)
 *
 *	- uninit SPIM
 *	 - Call  mtk_os_hal_spim_ctlr_deinit(spim_num bus_num) to uninit
 *	    spim and release resource.
 *
 *    @endcode
 *
 *
 * @}
 * @}
 */

/**
* @addtogroup OS-HAL
* @{
* @addtogroup spim
* @{
*/

/** @defgroup os_hal_spim_enum Enum
  * @{
  * This section introduces the enumerations
  * that SPIM uses while performing transfer.
  */

typedef enum {
	/** Use ISU0 as SPIM port */
	OS_HAL_SPIM_ISU0 = 0,
	/** Use ISU1 as SPIM port */
	OS_HAL_SPIM_ISU1 = 1,
	/** Use ISU2 as SPIM port */
	OS_HAL_SPIM_ISU2 = 2,
	/** Use ISU3as SPIM port */
	OS_HAL_SPIM_ISU3 = 3,
	/** Use ISU4 as SPIM port */
	OS_HAL_SPIM_ISU4 = 4,
	/** The maximum ISU number (invalid) */
	OS_HAL_SPIM_ISU_MAX
} spim_num;

/**
  * @}
  */

/** @defgroup os_hal_spim_typedef Typedef
  * @{
  * This section introduces the typedef that SPIM OS-HAL used.
  */

/** @brief This defines the callback function prototype.
 * It's called to report transaction completions.\n
 * User can register the callback function when using
 * SPIM mtk_os_hal_spim_async_transfer API.\n
 * When one transaction is done, this user callback will be called in
 * fifo interrupt function (fifo mode) or in dma callback function (dma mode).
 *
 * @param [in] context : the argument to spi_usr_complete_callback()
 * when it's called.
 */
typedef int (*spi_usr_complete_callback) (void *context);

/**
  * @}
  */

/** @defgroup os_hal_spim_function Function
  * @{
   * This section provides high level APIs to upper layer.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Dump SPIM register value.
 *
 *  @param [in] bus_num : SPIM ISU Port number,
 *  it can be OS_HAL_SPIM_ISU0~OS_HAL_SPIM_ISU4
 *
 *  @return negative value means fail.
 *  @return 0 means success.
 */
int mtk_os_hal_spim_dump_reg(spim_num bus_num);

/**
 * @brief  Init SPIM controller.
 *
 *  @param [in] bus_num : SPIM ISU Port number,
 *  it can be OS_HAL_SPIM_ISU0~OS_HAL_SPIM_ISU4
 *
 *  @return negative value means fail.
 *  @return 0 means success.
 */
int mtk_os_hal_spim_ctlr_init(spim_num bus_num);

/**
 * @brief  Deinit SPIM controller.
 *
 *  @param [in] bus_num : SPIM ISU Port number,
 *  it can be OS_HAL_SPIM_ISU0~OS_HAL_SPIM_ISU4
 *
 *  @return negative value means fail.
 *  @return 0 means success.
 */
int mtk_os_hal_spim_ctlr_deinit(spim_num bus_num);

/**
 * @brief  use FIFO or DMA mode to do
 * one blocking/synchronous SPI data transfer.
 *
 *  @param [in] bus_num : SPIM ISU Port number,
 *  it can be OS_HAL_SPIM_ISU0~OS_HAL_SPIM_ISU4
 *  @param [in] config : the HW setting
 *  @param [in] xfer : the data should be read/writen.
 *
 * Note:
 *
 * The transfer data format should be as belows:
 *	- Half-duplex transaction:
 *	  There is only one valid transaction at a time on a single direction:
 *	  either Send or Receive.
 *	  - Send: Device driver should provide mtk_spi_transfer->opcode,
 *		 mtk_spi_transfer->opcode_len, mtk_spi_transfer->tx_buf,
 *	 mtk_spi_transfer->len and set mtk_spi_transfer->rx_buf = NULL.
 *		 mtk_spi_transfer->opcode_len should be 1~4bytes,
 *		 mtk_spi_transfer->len should be 0~32bytes.
 *	  - Receive: Device driver should provide mtk_spi_transfer->opcode,
 *		 mtk_spi_transfer->opcode_len, mtk_spi_transfer->rx_buf,
 *	 mtk_spi_transfer->len and set mtk_spi_transfer->tx_buf = NULL.
 *		 mtk_spi_transfer->opcode_len should be 0~4bytes,
 *		 mtk_spi_transfer->len should be 1~32bytes.
 *
 *	- Full-duplex transaction:
 *	  There are two valid mutually inclusive transactions: Send and Receive.
 *	  - Device driver should provide mtk_spi_transfer->len,
 *		mtk_spi_transfer->opcode, mtk_spi_transfer->opcode_len,
 *		mtk_spi_transfer->tx_buf and set mtk_spi_transfer->rx_buf
 *		at the same time.
 *	  - mtk_spi_transfer->opcode_len should be 1~4bytes,
 *		mtk_spi_transfer->len should be 1~16bytes.
 *
 *  @return negative value means fail.
 *  @return 0 means success.
 */
int mtk_os_hal_spim_transfer(spim_num bus_num,
			     struct mtk_spi_config *config,
			     struct mtk_spi_transfer *xfer);

/**
 * @brief  use FIFO or DMA mode to do one asynchronous SPI transfer.
 *
 *  @param [in] bus_num : SPIM ISU Port number,
 *  it can be OS_HAL_SPIM_ISU0~OS_HAL_SPIM_ISU4
 *  @param [in] config : the HW setting
 *  @param [in] xfer : the data should be read/writen.
 * @param [in] complete : called to report transaction completions
 * @param [in] context : the argument to complete() when it's called
 *
 *  @return negative value means fail.
 *  @return 0 means success.
 */
int mtk_os_hal_spim_async_transfer(spim_num bus_num,
				   struct mtk_spi_config *config,
				   struct mtk_spi_transfer *xfer,
				   spi_usr_complete_callback complete,
				   void *context);

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

#endif
