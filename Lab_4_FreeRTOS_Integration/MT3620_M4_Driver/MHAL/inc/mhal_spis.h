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

#ifndef __MHAL_SPI_SLAVE__H__
#define __MHAL_SPI_SLAVE__H__

#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup SPIS
 * @{
 * This section introduces the Serial Peripheral Interface Slave (SPIS) APIs
 * including terms and acronyms, supported features, software architecture,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section MHAL_SPIS_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b DMA                        | Direct Memory Access.|
 * |\b FIFO                       | First In, First Out.|
 * |\b SPI                        | Serial Peripheral Interface.|
 *
 * @section MHAL_SPIS_Features_Chapter Supported Features
 * The supported features of SPIS are as follows:
 * 1. Support FIFO mode.
 * 2. Support DMA mode.
 * 3. Support CPOL/CPHA mode 0~3.
 * 4. Only support half-duplex transaction.
 * 5. Only support MSB(Most Significant Bit) transaction.
 * 6. The unit of transaction length is double word(4Bytes).
 * 7. For more details, please refer to SPIS datasheet.
 *
 * @}
 * @}
 */

/**
* @addtogroup M-HAL
* @{
* @addtogroup SPIS
* @{
* - \b Note \n
* The SPIS HW provides SPIS_H2DMB/SPIS_D2HMB to handshake information
* with external MCU.\n
*  - SPIS_H2DMB:\n
*     1. Bit[6:0] is valid.
*     2. Bit[6]: Always is 1 to represent this is a valid H2DMB.
*     3. Bit[5]: The definition of Read/Write flag,
*         0 means spim wants to read data from spis;
*         1 means spim wants to write data to spis.
*     4. Bit[4]: The definition of whether use spis's DMA or not.
*         0 is using FIFO mode;
*         1 is using DMA mode.
*     5. Bit[3:0]: The definition of transfer length.
*         spim can choose 1~8 double word(4 bytes).
*
*  - SPIS_D2HMB:\n
*     1. Bit[6:0] is valid.
*     2. Bit[6:3]: Reserved.
*     3. Bit[2:0]: The definition of spis transmission status,
*         see #spis_d2hmb_state.
* @}
* @}
*/

/**
* @addtogroup M-HAL
* @{
* @addtogroup SPIS
* @{
* @section MHAL_SPIS_Driver_Usage_Chapter How To Use This Driver
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
* \b sample \b code \b is \b as \b follows:\n
*  - Sample code (freeRTos doesn't have SPI framework,
* so this sample code provides APIs for User Application):
*    @code
*    TBD
*    @endcode
*
* - \b How \b to \b develop \b user \b application
*    \b by \b using \b OS-HAL \b API:\n
*  - Sample code (this is the user application sample code on freeRTos):
*    @code
*    TBD
*
*    @endcode
*
*
* @}
* @}
*/

/**
* @addtogroup M-HAL
* @{
* @addtogroup SPIS
* @{
*/

/** @defgroup driver_spis_define Define
  * @{
  * This section introduces the Macro definitions
  * that are used to combine H2DMB and error return type.
  */

/** @brief Users use this Macro to combine H2DMB.
 * This combined H2DMB will be sent to spi salve,
 * with the following information:\n
 * x: Transmission direction (R/W), defined in #spis_direction_cmd.\n
 * y: Use spis DMA or not (0: FIFO; 1: DMA).\n
 * z: Data length (unit: double word).\n
*/
#define SET_TRANSFER_H2DMB(x, y, z) \
	((0x1 << 6) | (x << 5) | (y << 4) | ((z) & 0xf))

/** Invalid argument, it means the pointer is NULL */
#define EPTR		1
/** Invalid argument, it means the transfer length is not supported */
#define ELENGTH		2
/** Invalid argument, it means the buffer is not 4Bytes align */
#define EMEM		3
/** DMA resource is busy */
#define EBUSY		4
/** Invalid argument, it means the H2DMB is invalid */
#define EH2DMB		5


/**
  * @}
  */

/** @defgroup driver_spis_enum Enum
  * @{
  * This section introduces the enumerations
  * that should be configured before performing transfer.
  */

/** @brief SPI slave's clock polarity definition.
* Before performing transfer, users should make sure that
* the \b cpol parameter is defined in #mtk_spis_config structure and is
* configured through #mtk_mhal_spis_setup_hw().
*/
enum spis_cpol {
	/** Clock polarity is 0 */
	SPIS_CPOL_0 = 0,
	/** Clock polarity is 1 */
	SPIS_CPOL_1 = 1
};

/** @brief SPI slave's clock format definition.
* Before performing transfer, users should make sure that
* the \b cpha parameter is defined in #mtk_spis_config structure and is
* configured through #mtk_mhal_spis_setup_hw().
*/
enum spis_cpha {
	/** Clock format is 0 */
	SPIS_CPHA_0 = 0,
	/** Clock format is 1 */
	SPIS_CPHA_1 = 1
};

/** @brief SPI slave's supported opcode.
* Before performing transfer, spi master should send different OPCODE
* to let spi slave controller know the current transaction type.
*/
enum spis_opcode {
	/** spim writes data to spis */
	OPCODE_SPIM_WRITE_SPIS_RX = 0x1,
	/** spim reads data from spis */
	OPCODE_SPIM_READ_SPIS_TX = 0x2,
	/** spim reads spis status */
	OPCODE_SPIM_GET_SPIS_STATUS = 0x3,
	/** spim sets spis irq */
	OPCODE_SPIM_SET_SPIS_IRQ = 0x8,
	/** spim clears spis D2HMB */
	OPCODE_SPIM_CLEAR_SPIS_D2HMB = 0x9,
	/** spim sends H2DMB to spis */
	OPCODE_SPIM_SEND_SPIS_H2DMB = 0xa,
};

/** @brief SPI slave's supported transaction status.
* This status will be updated to register SPIS_STATUS(0x60)
* by writing register SPIS_D2HMB(0x68).
* spi master checks spi slave transaction status via this status.
*/
enum spis_d2hmb_state {
	/** spis  idle */
	SPIS_IDLE = 0x0,
	/** spis preparation transfer ready */
	SPIS_READY = 0x1,
	/** spis transfer done */
	SPIS_DONE = 0x2,
	/** spis transfer error */
	SPIS_ERR = 0x3,
};

/** @brief SPI  transmission direction cmd definition.
* spi master tells spi slave through H2DMB
* whether this transmission wants to read or write.
* The SPI transmission direction cmd will also be used in the
* callback function(users need to prepare spi slave
* TX/RX Buffer and Data according to the direction cmd).
*/
enum spis_direction_cmd {
	/** spim wants to  read data */
	SPIM_READ_DATA = 0,
	/** spim wants to write data*/
	SPIM_WRITE_DATA = 1,
};

/**
  * @}
  */

/** @defgroup driver_spis_typedef Typedef
  * @{
  * This section introduces the typedef that SPIS M-HAL used.
  */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief	This defines the callback function prototype.
 * It's used for DMA mode transaction.\n
 * User should register a callback function when using SPI DMA mode.\n
 * In DMA mode, mtk_mhal_spis_dma_transfer_one()  returns a value
 * once the SPI hardware register is assigned and the DMA configured.\n
 * During this process, the transaction is usually incomplete, once it
 * completes, a DMA interrupt is triggered and a related user callback
 * is called in the DMA interrupt service routine.\n
 * This typedef is used to describe the callback function
 * what the user wants to call.
 *
 * @param [in] user_data: spis controller struct mtk_spis_controller.
 * @sa  #mtk_mhal_spis_dma_done_callback_register()
 *
 *@return  Return "0" if callback register successfully, or return "-1" if fail.
 */
typedef int (*spis_dma_done_callback) (void *user_data);


/** @brief	This defines the callback function prototype.
 * It's used for handle spis tx/rx buffer before transmission.\n
 * User should register a callback function to specify
 * and fill spis tx/rx buffer according to the cmd parameter.\n
 * Before transmission, spim will send h2dmb to spis,
 * h2dmb will contain the transfer direction, length of data,
 * transfer mode (dma/fifo) information.\n
 * spis driver will parse this information,
 * and fill the parsed results into the struct mtk_spis_transfer.\n
 * This callback function will  be called to specify the tx or rx buffer
 * that spis will operate on this transmission.\n
 * Therefore, the user needs to specify and fill the buffer
 * according to the cmd parameter
 * (including the transfer direction information).\n
 * This typedef is used to describe the callback function
 * what the user wants to call.
 *
 * @param [in] user_data: spis controller struct mtk_spis_controller..
 * @param [in] cmd: including the transfer direction information,
 * there are 2 options. defined in #spis_direction_cmd
 * @sa  #mtk_mhal_spis_handle_transfer_callback_register()
 *
 *@return  Return "0" if callback register successfully, or return "-1" if fail.
 */

typedef int (*spis_handle_transfer_callback) (void *use_data,
			enum spis_direction_cmd cmd);

/**
  * @}
  */

/** @defgroup driver_spis_struct Struct
  * @{
  * This section introduces the structure that SPIS OS-HAL/M-HAL/HDL used.
  */

/** @brief The common configuration can be set for the SPIS HW. */
struct mtk_spis_config {
	/** clock polarity */
	enum spis_cpol cpol;
	/** clock phase */
	enum spis_cpha cpha;
};

/** @brief I/O INTERFACE between SPI OS-HAL and M-HAL.
 * struct mtk_spis_transfer needs to be configured before each transfer,\n
 * User should always provide rx_buf or tx_buf, others are copied by
 * parsing H2DMB\n
 */
struct mtk_spis_transfer {
	/** Data to be written (DMA-safe memory), or NULL */
	void *tx_buf;
	/** Data to be read (DMA-safe memory), or NULL */
	void *rx_buf;
	/** Size of rx and tx buffers (in bytes) */
	unsigned int len;
	/** Spim read or write data, 0: Read(spis TX), 1: Write(spis RX) */
	int direction;
	/** Spis support FIFO and DMA mode, 0:FIFO, 1: DMA */
	int use_dma;
};

/** @brief Used to record spis status and required resources
  * during transmisson.
  */
struct mtk_spis_private {
	/**  D2HMB state, defined in #spis_d2hmb_state*/
	enum spis_d2hmb_state d2hmb_state;
	/** TX DMA physical addr,
	 * it's the PA of mtk_spis_transfer->tx_buf
	 */
	dma_addr_t tx_dma;
	/** RX DMA physical addr,
	 * it's the PA of mtk_spis_transfer->rx_buf
	 */
	dma_addr_t rx_dma;
	/** user_data is used to store spis hw setting.
	*/
	void *user_data;
	/** This function is used to register user's DMA done callback
	* to OS-HAL layer
	*/
	spis_dma_done_callback dma_done_callback;
	/** This function is used to register user's handle transfer callback
	* to OS-HAL layer
	*/
	spis_handle_transfer_callback handle_transfer_callback;
};

/** @brief Interface to SPI slave,
 * it's used to pass arguments among OS-HAL/M-HAL/HDL.
 */
struct mtk_spis_controller {
	/** spis controller base address */
	void __iomem *base;
	/** CG base address of this spis controller */
	void __iomem *cg_base;
	/** TX DMA channel */
	int dma_tx_chan;
	/** RX DMA channel */
	int dma_rx_chan;
	/** the  mtk_spis_transfer */
	struct mtk_spis_transfer *xfer;
	/** M-HAL private structure, used by M-HAL only */
	struct mtk_spis_private *mdata;
};

/**
  * @}
  */

/** @defgroup driver_spis_function Function
  * @{
  * This section provides Fixed APIs(defined as Common Interface)
  * to fully control the MediaTek SPIS HW.
  */

/**
 *@brief This function is used to dump spis register for debugging.
 *@brief Usage:Used for OS-HAL to dump SPIS register.
 *@param [in] ctlr : SPI controller used with the device.
 *
 *@return
 * Return "0" if dumping register successfully.\n
 * Return -#EPTR if ctlr is NULL.
 */
int mtk_mhal_spis_dump_reg(struct mtk_spis_controller *ctlr);

/**
 *@brief This function is used to get SPIS HW status.
 *@brief Usage: Used to get SPIS HW irq status.
 *@param [in] ctlr : SPI controller used with the device.
 *
 *@return
 * Return irq status if getting irq status successfully.\n
 * Return -#EPTR if ctlr is NULL.
 */
int mtk_mhal_spis_get_irq_status(struct mtk_spis_controller *ctlr);


/**
 *@brief This function is used to clear SPIS HW status.
 *@brief Usage: Used to clear SPIS HW irq status.
 *@param [in] ctlr : SPI controller used with the device.
 *
 *@return
 * Return "0" if clear irq status successfully.\n
 * Return -#EPTR if ctlr is NULL.
 */
int mtk_mhal_spis_clear_irq_status(struct mtk_spis_controller *ctlr);

/**
 *@brief This function is used to handle H2DMB which is sent from spi master.
 *@brief Usage: Used to to parse the H2DMB cmd
 * to fill struct mtk_spis_transfer, and then clear H2DMB status.
 *@param [in] ctlr : SPI controller used with the device.
 * @par       Example
 * @code
 * spis_irq_handler()
 * {
 *	 irq_status = mtk_mhal_spis_get_irq_status(ctlr);
 *	 mtk_mhal_spis_clear_irq_status(ctlr);
 *
 *	 switch (irq_status) {
 *	 case SPIM_WRITE_IRQ_MB_TO_SPIS:
 *		 mtk_mhal_spis_handle_h2dmb(ctlr);
 *		 mtk_mhal_spis_fifo_transfer_one(ctlr, xfer);
 *	 break;
 *     }
 *	mtk_mhal_spis_send_d2hmb(ctrl);
 *	mtk_mhal_spis_clear_irq_status(ctrl);
 * }
 *
 * @endcode
 *
 *@return
 * Return "0" if  handling H2DMB successfully.\n
 * Return -#EPTR if ctlr or xfer is NULL.
 * Return -#EH2DMB if H2DMB is illegal.
 * Return -#ELENGTH if xfer len is not supported,
 */
int mtk_mhal_spis_handle_h2dmb(struct mtk_spis_controller *ctlr);

/**
 *@brief This function is used to update spis transmisson status
 *@brief Usage: Used to update spis transmisson
 * status(defined in #spis_d2hmb_state) from
 * d2hmb_state(defined in #mtk_spis_private)to regiset SPIS_STATUS(0x60).
 * spim can get this status by read this register.
 *@param [in] ctlr : SPI controller used with the device.
 * @par       Example
 * @code
 * spis_irq_handler()
 * {
 *	 irq_status = mtk_mhal_spis_get_irq_status(ctlr);
 *	 mtk_mhal_spis_clear_irq_status(ctlr);
 *
 *	 switch (irq_status) {
 *	 case SPIM_WRITE_DATA_TO_SPIS:
 *		 mtk_mhal_spis_handle_rx(ctlr);
 *	 break;
 *     }
 *	mtk_mhal_spis_send_d2hmb(ctrl);
 *	mtk_mhal_spis_clear_irq_status(ctrl);
 * }
 *
 * @endcode
 *
 *@return
 * Return "0" if update D2HMB status successfully.\n
 * Return -#EPTR if ctlr is NULL.
 */
int mtk_mhal_spis_send_d2hmb(struct mtk_spis_controller *ctlr);

/**
 *@brief This function is used to update spis RX transmisson status to
 * d2hmb_state(defined in #mtk_spis_private)
 * and read FIFO data from SPIS HW.
 *@brief Usage: Used to update spis transmisson status,
 * and while using FIFO mode, copy data from  rx FIFO register
 * to xfer->rx_buffer.
 *@param [in] ctlr : SPI controller used with the device.
 * @par       Example
 * @code
 * spis_irq_handler()
 * {
 *	 irq_status = mtk_mhal_spis_get_irq_status(ctlr);
 *	 mtk_mhal_spis_clear_irq_status(ctlr);
 *
 *	 switch (irq_status) {
 *	 case SPIM_WRITE_DATA_TO_SPIS:
 *		 mtk_mhal_spis_handle_rx(ctlr);
 *	 break;
 *    }
 *	mtk_mhal_spis_send_d2hmb(ctrl);
 *	mtk_mhal_spis_clear_irq_status(ctrl);
 * }
 *
 * @endcode
 *
 *@return
 * Return "0" if handle rx successfully.\n
 * Return -#EPTR if ctlr or xfer is NULL.
 * Return -#ELENGTH if ctlr or xfer is NULL.
 */
int mtk_mhal_spis_handle_rx(struct mtk_spis_controller *ctlr);

/**
 *@brief This function is used to update spis TX transmisson status to
 * d2hmb_state(defined in #mtk_spis_private)
 *@brief Usage: Used to update spis transmisson status.
 *@param [in] ctlr : SPI controller used with the device.
 * @par       Example
 * @code
 * spis_irq_handler()
 * {
 *	 irq_status = mtk_mhal_spis_get_irq_status(ctlr);
 *	 mtk_mhal_spis_clear_irq_status(ctlr);
 *
 *	 switch (irq_status) {
 *	 case SPIM_READ_DATA_FROM_SPIS:
 *		 mtk_mhal_spis_handle_tx(ctlr);
 *	 break;
 *    }
 *	mtk_mhal_spis_send_d2hmb(ctrl);
 *	mtk_mhal_spis_clear_irq_status(ctrl);
 * }
 *
 * @endcode
 *
 *@return
 * Return "0" if handle tx successfully.\n
 * Return -#EPTR if ctlr or xfer is NULL.
 */
int mtk_mhal_spis_handle_tx(struct mtk_spis_controller *ctlr);


/**
 *@brief This function is used to initialize SPIS HW based on *config settings.
 *@brief Usage: Used to *config setting to init HW.
 * It must be called before transfer.
 *@param [in] ctlr : SPI controller used with the device.
 *@param [in] config : HW setting
 *
 * @par       Example
 * @code
 *	 struct mtk_spis_config spi_default_config = {
 *		 .cpol = SPI_CPOL_0,
 *		 .cpha = SPI_CPHA_0,
 *	 };
 *
 *	mtk_mhal_spis_setup_hw(ctlr, &spi_default_config);
 *
 * @endcode
 *
 *@return
 * Return "0" if setting SPIS HW successfully.\n
 * Return -#EPTR if ctlr or config is NULL.
 */
int mtk_mhal_spis_setup_hw(struct mtk_spis_controller *ctlr,
			      struct mtk_spis_config *config);

/**
 *@brief This function is used to enable SPIS HW interrupt.
 *@brief Usage: It must be called before transfer to enable irq firstly.
 *@param [in] ctlr : SPI controller used with the device.
 *
 *@return
 * Return "0" if enable irq successfully.\n
 * Return -#EPTR if ctlr is NULL.
 */
int mtk_mhal_spis_enable_irq(struct mtk_spis_controller *ctlr);

/** @brief	 This function is used to register user's DMA callback to M-HAL.
 * It's used for handling spis tx/rx buffer before transmission.\n
 * User should register a callback function to specify
 * and fill spis TX/RX buffer according to the cmd parameter.\n
 * Before transmission, spim will send H2DMB to spis,
 * H2DMB will contain the transfer direction, length of data,
 * transfer mode (DMA/FIFO) information.\n
 * spis driver will parse this information,  and fill the parsed results
 * into the struct mtk_spis_transfer.\n
 * This callback function will  be called to specify the TX or RX buffer
 * that spis will operate on this transmission.\n
 * Therefore, the users need to specify and fill the buffer
 * according to the cmd parameter
 * (including the transfer direction information).\n
 *
 * @param [in] ctlr : SPI controller used with the device.
 * @param [in] callback : The callback function given by OS-HAL
 * which will be called at SPI slave's DMA interrupt service routine.
 *
 *@return
 * Return "0" if callback registers successfully.\n
 * Return -#EPTR if ctlr or callback is NULL.
 */
int mtk_mhal_spis_handle_transfer_callback_register(
					struct mtk_spis_controller *ctlr,
					spis_handle_transfer_callback callback);

/**
 *@brief This function is used to perform a single mtk_spi_transfer by FIFO mode
 *@brief Usage: this function triggers SPIS HW to
 * perform a single mtk_spi_transfer.\n
 * It must be called after mtk_mhal_spis_send_d2hmb(),
 * while the function returns, representing that
 * SPIS HW is ready to receive or send data.
 *@param [in] ctlr : SPI controller used with the device.
 *@param [in] xfer : The read/write buffer pair user should transfer.
 *
 *@return
 * Return "0" if the transfer is in progress.\n
 * Return -#EPTR if ctlr or xfer is NULL.\n
 * Return -#ELENGTH if xfer len is not supported.
 */
int mtk_mhal_spis_fifo_transfer_one(struct mtk_spis_controller *ctlr,
				    struct mtk_spis_transfer *xfer);


/** @brief	 This function is used to register user's DMA callback to M-HAL.
 * It's used for DMA mode transaction.
 * In DMA mode, mtk_mhal_spis_dma_transfer_one()  returns a value
 * once the SPI hardware register is assigned and the DMA is configured.\n
 * During this process the transaction is usually not completed. Once it is
 * completed, a DMA interrupt is triggered and a related user callback
 * is called in the DMA interrupt service routine.

 * @param [in] ctlr : SPI controller used with the device.
 * @param [in] callback : The callback function given by OS-HAL
 * which will be called at SPI master's DMA interrupt service routine.
 *
 *@return
 * Return "0" if callback registers successfully.\n
 * Return -#EPTR if ctlr or callbackis NULL.
 */
int mtk_mhal_spis_dma_done_callback_register(
					struct mtk_spis_controller *ctlr,
					spis_dma_done_callback callback);

/**
 *@brief This function is used to perform a single mtk_spis_transfer
 * by DMA mode.
 *@brief Usage: This function triggers SPIS HW to
 * perform a single mtk_spis_transfer.
 * It must be called after mtk_mhal_spis_send_d2hmb(),
 * while the function returns, representing that
 * SPIS HW is ready to receive or send data.
 *@param [in] ctlr : SPI controller used with the device.
 *@param [in] xfer : The read/write buffer pair user should transfer.
 *
 *@return
 * Return "0" if the transfer is in progress.\n
 * Return -#EPTR if ctlr or xfer is NULL.\n
 * Return -#ELENGTH if xfer len is not supported.
 */
int mtk_mhal_spis_dma_transfer_one(struct mtk_spis_controller *ctlr,
				   struct mtk_spis_transfer *xfer);


/**
 *@brief This function is used to allocate SPIS DMA channel.
 *@brief Usage: User should call it to allocate DMA channel after
 * call  #mtk_mhal_spis_alloc_controller and set dma_chan in initial flow.
 *@param [in] ctlr : SPI controller used with the device.
 *@return
 * Return 0 if users allocate DMA channel successfully.\n
 * Return -#EPTR if ctlr or xfer is NULL.\n
 * Return -#EBUSY if DMA chan is busy.
 */
int mtk_mhal_spis_allocate_dma_chan(struct mtk_spis_controller *ctlr);

/**
 *@brief This function is used to release SPIS DMA channel.
 *@brief Usage: user should call it to release DMA channel
 * before call  #mtk_mhal_spis_release_controller in de-initial flow.
 *@param [in] ctlr : SPI controller used with the device.
 *@return
 * Return 0 if users allocate DMA channel successfully.\n
 * Return -#EPTR if ctlr or xfer is NULL.\n
 * Return -#EBUSY if DMA chan is already released.
 */
int mtk_mhal_spis_release_dma_chan(struct mtk_spis_controller *ctlr);

/**
 *@brief This function is used to enable SPIS clock before transfer.
 *@brief Usage: It must be called to enable clock before transferring,
 *otherwise the access to SPIS register will fail .
 *@param [in] ctlr : SPI controller used with the device.
 *
 *@return
 * Return "0" if enable SPIS clock successfully.\n
 * Return -#EPTR if ctlr is NULL.
 */
int mtk_mhal_spis_enable_clk(struct mtk_spis_controller *ctlr);

/**
 *@brief This function is used to disable SPIS clock before transfer.
 *@brief Usage: It needs to be called after transfer to disable clock.
 *@param [in] ctlr : SPIS controller used with the device.
 *
 *@return
 * Return "0" if disable SPIS clock successfully.\n
 * Return -#EPTR if ctlr is NULL.
 */
int mtk_mhal_spis_disable_clk(struct mtk_spis_controller *ctlr);

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
