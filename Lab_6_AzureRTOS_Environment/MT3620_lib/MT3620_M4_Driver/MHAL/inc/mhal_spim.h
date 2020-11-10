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

#ifndef __MHAL_SPI_MASTER__H__
#define __MHAL_SPI_MASTER__H__

#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup SPIM
 * @{
 * This section introduces the Serial Peripheral Interface Master (SPIM) APIs
 * including terms and acronyms, supported features, software architecture,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section MHAL_SPIM_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b DMA                        | Direct Memory Access.|
 * |\b FIFO                       | First In, First Out.|
 * |\b SPI                        | Serial Peripheral Interface.|
 *
 * @section MHAL_SPIM_Features_Chapter Supported Features
 *
 * The SPI controller supports a wide range of SPI interface devices,
 * including full-duplex transaction ability to communicate with both
 * half-duplex and full-duplex devices.\n
 * For more details, please refer to SPIM datasheet.
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup SPIM
 * @{
 * - \b Support \b transaction \b format. \n
 *    SPIM driver's user can use struct mtk_spi_transfer to
 *    judge either the transfer is half-duplex or full-duplex.
 *
 *  - Half-duplex transaction:
 *    There is only one valid transaction at a time on a single direction:
 *    either Send or Receive.
 *    - Send: Device driver should provide mtk_spi_transfer->opcode,
 *       mtk_spi_transfer->opcode_len, mtk_spi_transfer->tx_buf,
 *	mtk_spi_transfer->len and set mtk_spi_transfer->rx_buf = NULL.\n
 *       mtk_spi_transfer->opcode_len should be 1~4bytes,
 *       mtk_spi_transfer->len should be 0~32bytes.
 *    - Receive: Device driver should provide mtk_spi_transfer->opcode,
 *       mtk_spi_transfer->opcode_len, mtk_spi_transfer->rx_buf,
 *	mtk_spi_transfer->len and set mtk_spi_transfer->tx_buf = NULL.\n
 *       mtk_spi_transfer->opcode_len should be 0~4bytes,
 *       mtk_spi_transfer->len should be 1~32bytes.
 *
 *  - Full-duplex transaction:
 *    There are two valid mutually inclusive transactions: Send and Receive.
 *    - Device driver should provide mtk_spi_transfer->len,
 *      mtk_spi_transfer->opcode, mtk_spi_transfer->opcode_len,
 *      mtk_spi_transfer->tx_buf and set mtk_spi_transfer->rx_buf
 *      at the same time.
 *    - mtk_spi_transfer->opcode_len should be 1~4bytes,
 *      mtk_spi_transfer->len should be 1~16bytes.
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup SPIM
 * @{
 * @section MHAL_SPIM_Driver_Usage_Chapter How to use this driver
 *
 * - \b SW \b Architecture: \n
 * See @ref MHAL_Overview_2_Chapter for the details of SW architecture.
 *
 *  This section describes the definition of APIs and provides
 *  an example on freeRTos about how to use these APIs to develop
 *  an OS-related SPI driver.\n
 *  Also this section provides an example about the sequence of APIs
 *  that should be used by the SPI slave.
 *
 * - \b The \b OS-HAL \b freeRTos \b driver\n
 * \b sample \b code \b is \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_spim.c"> freeRTos SPIM sample code on github </a>
 *
 * @}
 * @}
 */

/**
* @addtogroup M-HAL
* @{
* @addtogroup SPIM
* @{
*/

/** @defgroup driver_spim_define Define
  * @{
  * This section introduces the Macro definition
  * which is used as SPIM M-HAL's API error return type.
  */

/** Invalid argument, it means the pointer is NULL */
#define SPIM_EPTR		1
/** Invalid argument, it means the transfer length isn't supported */
#define SPIM_ELENGTH		2
/** Out of memory, it means memory malloc fail */
#define SPIM_ENOMEM		3
/** DMA resource busy */
#define SPIM_EBUSY		4

/** SPIM DMA tmp buffer size */
#define MTK_SPIM_DMA_BUFFER_BYTES	(48 + 4)

/**
  * @}
  */

/** @defgroup driver_spim_enum Enum
  * @{
  * This section introduces the enumerations
  * that SPIM should configure before performing transfer.
  */

/** @brief SPI master's clock polarity definition.
* Before performing transfer, user should make sure
* the \b cpol parameter is defined in #mtk_spi_config structure and
* configured through #mtk_mhal_spim_prepare_hw().
*/
enum spi_cpol {
	/** Clock polarity is 0 */
	SPI_CPOL_0 = 0,
	/** Clock polarity is 1 */
	SPI_CPOL_1 = 1
};

/** @brief SPI master's clock format definition.
* Before performing transfer, user should make sure
* the \b cpha parameter is defined in #mtk_spi_config structure and
* configured through #mtk_mhal_spim_prepare_hw().
*/
enum spi_cpha {
	/** Clock format is 0 */
	SPI_CPHA_0 = 0,
	/** Clock format is 1 */
	SPI_CPHA_1 = 1
};

/** @brief SPI master's transaction bit order definition.
* Before performing transfer, user should make sure
* \b tx_mlsb and \b rx_mlsb parameters are defined
* in #mtk_spi_config structure and
* configured through #mtk_mhal_spim_prepare_hw().
*/
enum spi_mlsb {
	/** Send/Receive data transfer LSB first */
	SPI_LSB = 0,
	/** Send/Receive data transfer MSB first */
	SPI_MSB = 1
};

/** @brief SPI master's slave device select definition.
* Before performing transfer, user should
* configured through #mtk_mhal_spim_prepare_hw().
*/
enum spi_slave_sel {
	/** select device 0 */
	SPI_SELECT_DEVICE_0 = 0,
	/** select device 1 */
	SPI_SELECT_DEVICE_1 = 1,
};

/**
  * @}
  */

/** @defgroup driver_spim_typedef Typedef
  * @{
  * This section introduces the typedef that SPIM M-HAL used.
  */

/** @brief	This defines the callback function prototype.
 * It's used for DMA mode transaction.\n
 * User should register a callback function when using SPI DMA mode.\n
 * In DMA mode, mtk_mhal_spim_dma_transfer_one()  returns a value
 * once the SPI hardware register is assigned and the DMA configured.\n
 * During this process, the transaction is usually incomplete, once it
 * completes, a DMA interrupt is triggered and a related user callback
 * is called in the DMA interrupt service routine.\n
 * This typedef is used to describe the callback function
 * what the user wants to call.
 *
 * @param [in] user_data: An OS-HAL defined parameter provided
 * by #mtk_mhal_spim_dma_done_callback_register().
 * @sa  #mtk_mhal_spim_dma_done_callback_register()
 *
 *@return  Return "0" if callback register successfully, or return "-1" if fail.
 */
typedef int (*spi_dma_done_callback) (void *user_data);

/**
  * @}
  */

/** @defgroup driver_spim_struct Struct
  * @{
  * This section introduces the structure that SPIM OS-HAL/M-HAL/HDL used.
  */

/** @brief The common configuration can be set for the SPIM HW. */
struct mtk_spi_config {
	/** clock polarity */
	enum spi_cpol cpol;
	/** clock phase */
	enum spi_cpha cpha;
	/** tx per-word bits-on-wire */
	enum spi_mlsb tx_mlsb;
	/** rx per-word bits-on-wire */
	enum spi_mlsb rx_mlsb;
	/** define select which spi slave device */
	enum spi_slave_sel slave_sel;
};

/** @brief I/O INTERFACE between SPI OS-HAL and M-HAL.
 * struct mtk_spi_transfer - a read/write buffer pair.\n
 * SPI transfers always write the same number of bytes as they read.\n
 * OS-HAL should always provide rx_buf and/or tx_buf.
 */
struct mtk_spi_transfer {
	/** data to be written (DMA-safe memory), or NULL */
	const void *tx_buf;
	/** data to be read (DMA-safe memory), or NULL */
	void *rx_buf;
	/** size of rx and tx buffers (in bytes) */
	u32 len;

	/** opcode data to be written (on MOSI pin) */
	u32 opcode;

	/** size of opcode length (in bytes)\n
	 * Note on MT3620:\n
	 * half duplex:\n
	 * TX only: it should be 1~4.
	 * RX only: it should be 0~4.\n
	 * full duplex: it should be 1~4.
	 */
	u32 opcode_len;

	/** spi support FIFO & DMA mode, 0:FIFO, 1: DMA */
	u32 use_dma;

	/** config SPIM HW SCK speed that talk with device for this transfer.
	 * The maximum CLK freq is 40Mhz and the suggestion value of speed_khz
	 * is 250 (250Khz),500(500Khz),1000(1Mhz),2000(2Mhz),4000(4Mhz),
	 * 6000(6Mhz),8000(8Mhz),10000(10Mhz),40000(40Mhz).\n
	 * If speed_khz=0, SPIM HW use default 1Mhz.
	 */
	u32 speed_khz;
};

/** @brief M-HAL privite structure.
 * It's only used by DMA mode to programming M-HAL.
 * OS-HAL is not need to care this structure.
 */
struct mtk_spi_private {
	/** used for tx temp buf */
	u8 *tx_buf;

	/** temp tx DMA physical addr,
	 * it's the PA of mtk_spi_transfer->tx_buf
	 */
	dma_addr_t tx_dma;
	/** temp rx DMA physical addr,
	 * it's the PA of mtk_spi_transfer->rx_buf
	 */
	dma_addr_t rx_dma;

	/** temp transfer len to DMA */
	u32 xfer_len;

	/** user_data is a OS-HAL defined parameter provided
	* by #mtk_mhal_spim_dma_done_callback_register().
	*/
	void *user_data;
	/** This function is used to register user's DMA done callback
	* to OS-HAL layer
	*/
	spi_dma_done_callback dma_done_callback;
};

/** @brief Interface to SPI master,
 * it's used to pass arguments between OS-HAL/M-HAL/HDL.
 */
struct mtk_spi_controller {
	/** spi controller base address */
	void __iomem *base;
	/** CG base address of this spi controller */
	void __iomem *cg_base;

	/** used for DMA tx temp buf, the size of tx buffer
	* should be MTK_SPIM_DMA_BUFFER_BYTES
	*/
	u8 *dma_tmp_tx_buf;

	/** TX DMA channel */
	int dma_tx_chan;
	/** RX DMA channel */
	int dma_rx_chan;

	/** the temp variable of current mtk_spi_transfer */
	struct mtk_spi_transfer *current_xfer;

	/** M-HAL privite structure, used by M-HAL only */
	struct mtk_spi_private *mdata;
};

/**
  * @}
  */

/** @defgroup driver_spim_function Function
  * @{
  * This section provides Fixed APIs(defined as Common Interface)
  * to fully control the MediaTek SPIM HW.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@brief This function is used to dump spi register for debugging.
 *@brief Usage:Used for OS-HAL to dump SPIM register.
 *@param [in] ctlr : SPI controller used with the device.
 *
 *@return
 * Return "0" if dump register successfully.\n
 * Return -#SPIM_EPTR if ctlr is NULL.
 */
int mtk_mhal_spim_dump_reg(struct mtk_spi_controller *ctlr);

/**
 *@brief This function is used to clear SPIM HW status.
 *@brief Usage: It's called in irq function to clear SPIM HW irq status.
 *@param [in] ctlr : SPI controller used with the device.
 *
 *@return
 * Return "0" if clear irq status successfully.\n
 * Return -#SPIM_EPTR if ctlr is NULL.
 */
int mtk_mhal_spim_clear_irq_status(struct mtk_spi_controller *ctlr);

/**
 *@brief This function is used to read FIFO data from SPIM HW.
 *@brief Usage: it must be called in irq function with FIFO mode.
 * while using FIFO mode, read rx FIFO register
 * and copy data to xfer->rx_buffer.
 *@param [in] ctlr : SPI controller used with the device.
 *@param [in] xfer : the read/write buffer pair user should transfer.
 * @par       Example
 * @code
 *  spim_irq_handler()
 *  {
 *	mtk_mhal_spim_clear_irq_status(ctlr);
 *	if (use FIFO mode)
 *		mtk_mhal_spim_fifo_handle_rx(ctlr, xfer);
 *  }
 *
 * @endcode
 *
 *@return
 * Return "0" if read FIFO data successfully.\n
 * Return -#SPIM_EPTR if ctlr or xfer is NULL.
 */
int mtk_mhal_spim_fifo_handle_rx(struct mtk_spi_controller *ctlr,
				  struct mtk_spi_transfer *xfer);

/**
 *@brief This function is used to initialize SPIM HW based on *config settings.
 *@brief Usage: Use *config setting to init HW.
 * It must be called before transferring.
 *@param [in] ctlr : SPI controller used with the device.
 *@param [in] config : HW setting
 *
 * @par       Example
 * @code
 *	 struct mtk_spi_config spi_default_config = {
 *		 .cpol = SPI_CPOL_0,
 *		 .cpha = SPI_CPHA_0,
 *		 .rx_mlsb = SPI_MSB,
 *		 .tx_mlsb = SPI_MSB,
 *		 .slave_sel = SPI_SELECT_DEVICE_0,
 *	 };
 *
 *	mtk_mhal_spim_prepare_hw(ctlr, &spi_default_config);
 *
 * @endcode
 *
 *@return
 * Return "0" if configure SPIM HW successfully.\n
 * Return -#SPIM_EPTR if ctlr or config is NULL.
 */
int mtk_mhal_spim_prepare_hw(struct mtk_spi_controller *ctlr,
			      struct mtk_spi_config *config);

/**
 *@brief This function is used to initialize SPIM
 * based on mtk_spi_transfer setting.
 *@brief Usage: Initialize HW according to *xfer arg(such as speed).
 * It must be called before transferring.
 *@param [in] ctlr : SPI controller used with the device.
 *@param [in] xfer : The read/write buffer pair user should transfer.
 * @par       Example
 * @code
 *	 struct mtk_spi_transfer xfer;
 *
 *	 memset(&xfer, 0, sizeof(struct mtk_spi_transfer));
 *
 *	 xfer.use_dma = 0;
 *	 xfer.tx_buf = tx_buf;
 *	 xfer.rx_buf = rx_buf;
 *	 xfer.len = len;
 *
 *	 mtk_mhal_spim_prepare_transfer(ctlr, &xfer);
 *
 * @endcode
 *
 *@return
 * Return "0" if configure SPIM HW successfully.\n
 * Return -#SPIM_EPTR if ctlr or xfer is NULL.
 */
int mtk_mhal_spim_prepare_transfer(struct mtk_spi_controller *ctlr,
				    struct mtk_spi_transfer *xfer);

/**
 *@brief This function is used to perform a single mtk_spi_transfer by FIFO mode
 *@brief Usage: this function triggers SPIM HW to
 * perform a single mtk_spi_transfer.\n
 *while the function returns, the transfer may not be completed.\n
 *So OS-HAL should wait for SPIM HW irq to be finished (maybe use semaphore).
 *@param [in] ctlr : SPI controller used with the device.
 *@param [in] xfer : The read/write buffer pair user should transfer.
 *
 *@return
 * Return "0" if the transfer is in progress.\n
 * Return -#SPIM_EPTR if ctlr or xfer is NULL.\n
 * Return -#SPIM_ELENGTH if xfer opcode_len/len is not supported.
 * see Note in @ref MHAL_SPIM_Features_Chapter for details.
 */
int mtk_mhal_spim_fifo_transfer_one(struct mtk_spi_controller *ctlr,
				    struct mtk_spi_transfer *xfer);


/** @brief	 This function is used to register user's DMA callback to M-HAL.
 * It's used for DMA mode transaction.
 * In DMA mode, mtk_mhal_spim_dma_transfer_one()  returns a value
 * once the SPI hardware register is assigned and the DMA is configured.\n
 * During this process the transaction is usually not completed, once it is
 * completed, a DMA interrupt is triggered and a related user callback
 * is called in the DMA interrupt service routine.
 *
 * @param [in] ctlr : SPI controller used with the device.
 * @param [in] callback : The callback function given by OS-HAL
 * which will be called at SPI master's DMA interrupt service routine.
 * @param [in] user_data : A parameter given by OS-HAL and will
 * be passed to user when the callback function is called.
 *
 *@return
 * Return "0" if callback register successfully.\n
 * Return -#SPIM_EPTR if ctlr or callback or user_data is NULL.
 */
int mtk_mhal_spim_dma_done_callback_register(struct mtk_spi_controller *ctlr,
					     spi_dma_done_callback callback,
					     void *user_data);

/**
 *@brief This function is used to perform a single mtk_spi_transfer by DMA mode.
 *@brief Usage: This function triggers SPIM HW to
 * perform a single mtk_spi_transfer.
 *When the function returns, the transfer may not be completed.
 *So OS-HAL should wait for SPIM HW irq to be finished (maybe use semaphore).
 *@param [in] ctlr : SPI controller used with the device.
 *@param [in] xfer : The read/write buffer pair user should transfer.
 *
 *@return
 * Return "0" if the transfer is in progress.\n
 * Return -#SPIM_EPTR if ctlr or xfer is NULL.\n
 * Return -#SPIM_ELENGTH if xfer opcode_len/len is not supported.
 * Return -#SPIM_ENOMEM if allocate temp buffer fail.
 */
int mtk_mhal_spim_dma_transfer_one(struct mtk_spi_controller *ctlr,
				   struct mtk_spi_transfer *xfer);

/**
 *@brief This function is used to allocate SPIM DMA channel.
 *@brief Usage: User should call it to allocate DMA channel after
 * call  #mtk_mhal_spim_alloc_controller and set dma_chan in initial flow.
 *@param [in] ctlr : SPI controller used with the device.
 *@return
 * Return 0 if users allocate DMA channel successfully.\n
 * Return -#SPIM_EPTR if ctlr or xfer is NULL.\n
 * Return -#SPIM_EBUSY if dma chan is busy.
 */
int mtk_mhal_spim_allocate_dma_chan(struct mtk_spi_controller *ctlr);

/**
 *@brief This function is used to release SPIM DMA channel.
 *@brief Usage: user should call it to release DMA channel
 * before call  #mtk_mhal_spim_release_controller in de-initial flow.
 *@param [in] ctlr : SPI controller used with the device.
 *@return
 * Return 0 if users allocate DMA channel successfully.\n
 * Return -#SPIM_EPTR if ctlr or xfer is NULL.\n
 * Return -#SPIM_EBUSY if dma chan is already released.
 */
int mtk_mhal_spim_release_dma_chan(struct mtk_spi_controller *ctlr);

/**
 *@brief This function is used to enable SPIM clock before transfer.
 *@brief Usage: It must be called to enable clock before transferring,
 *otherwise the access to SPIM register will fail .
 *@param [in] ctlr : SPI controller used with the device.
 *
 *@return
 * Return "0" if enable SPIM clock successfully.\n
 * Return -#SPIM_EPTR if ctlr is NULL.
 */
int mtk_mhal_spim_enable_clk(struct mtk_spi_controller *ctlr);

/**
 *@brief This function is used to disable SPIM clock before transfer.
 *@brief Usage: It needs to be called after transfer to disable clock.
 *@param [in] ctlr : SPIM controller used with the device.
 *
 *@return
 * Return "0" if disable SPIM clock successfully.\n
 * Return -#SPIM_EPTR if ctlr is NULL.
 */
int mtk_mhal_spim_disable_clk(struct mtk_spi_controller *ctlr);

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
