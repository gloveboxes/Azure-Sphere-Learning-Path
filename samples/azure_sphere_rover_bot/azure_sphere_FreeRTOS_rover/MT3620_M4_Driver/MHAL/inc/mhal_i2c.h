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

#ifndef __MHAL_I2C__H__
#define __MHAL_I2C__H__

#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup I2C
 * @{
 * This section introduces the Inter-Integrated Circuit (I2C) APIs
 * including terms and acronyms, supported features, software architecture,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section MHAL_I2C_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                          |
 * |------------------------------|------------------------------------------|
 * |\b DMA                        | Direct Memory Access.|
 * |\b FIFO                       | First In, First Out. |
 * |\b I2C                        | Inter-Integrated Circuit. |
 *
 * @section MHAL_I2C_Features_Chapter Supported features
 *
 * The supported features of I2C controller are as follows.
 * 1. I2C compliant master mode operation
 * 2. I2C compliant slave mode operation and support slave address programable
 * 3. Clock speed adjustable to 50 kHz, 100 kHz, 200 kHz, 400 kHz and 1000 kHz
 * 4. Multi-write per transfer
 * 5. Multi-read per transfer
 * 6. I2C_FIFO transfer mode
 * 7. DMA transfer mode
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup I2C
 * @{
 * @section MHAL_I2C_Driver_Usage_Chapter How to Use This Driver
 *
 * - \b SW \b Architecture: \n
 *  See @ref MHAL_Overview_2_Chapter for the details of SW architecture.
 *
 *  This section describes the definition of APIs and provides
 *  an example on FreeRTOS to show how to use these APIs to develop
 *  an OS-related I2C driver.\n
 *  Also this section provides an example of the sequence of APIs
 *  that should be used by the I2C slave.
 *
 * - \b The \b OS-HAL \b FreeRTOS \b driver\n
 * \b sample \b code \b is \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/FreeRTOS/OS_HAL/src/os_hal_i2c.c"> freeRTos I2C sample code on github </a>
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup I2C
 * @{
 */

/** @defgroup driver_i2c_define Define
  * @{
  * This section introduces the Macro definition
  * that I2C OS-HAL/M-HAL/HDL used.
  */

/**I2C execute OK*/
#define I2C_OK				0
/**It means the pointer is NULL*/
#define I2C_EPTR			1
/**FIFO status abnormal*/
#define I2C_EFIFO			2
/**No such device or address*/
#define I2C_ENXIO			6
/**Device busy*/
#define I2C_EBUSY			16
/**Invalid argument*/
#define I2C_EINVAL			22
/**Connection timed out*/
#define I2C_ETIMEDOUT			110

/** I2C complete data transfer*/
#define I2C_TRANS_DONE		0
/**I2C module triggers an complete interrupt, but need
  *  to wait for the DMA module to complete the transfer.
  */
#define I2C_WAIT_DMA		1

/**
  * @}
  */

/** @defgroup driver_i2c_enum Enum
  * @{
  * This section introduces the enumerations
  * that I2C should configure before performing transfer.
  */

/** @brief I2C master's clock frequency definition.
 * Before performing transfer, users should make sure that
 * the speed_kHz parameter is defined in #mtk_i2c_controller structure and
 * configured through #mtk_mhal_i2c_init_speed().
 */
enum i2c_speed_kHz {
	/**Transmit data with 50 kbps.*/
	I2C_SCL_50kHz = 1,
	/**Transmit data with 100 kbps.*/
	I2C_SCL_100kHz = 2,
	/**Transmit data with 200 kbps.*/
	I2C_SCL_200kHz = 3,
	/**Transmit data with 400 kbps.*/
	I2C_SCL_400kHz = 4,
	/**Transmit data with 1000 kbps.*/
	I2C_SCL_1000kHz = 5
};

/** @brief I2C controller operation mode definition.
 * Before performing transfer, users should make sure that the
 * i2c_trans_op parameter is defined in mtk_i2c_controller structure
 * and configured through mtk_mhal_i2c_trigger_transfer().
 */
enum i2c_trans_op {
	/** I2C master writes data to slave*/
	I2C_MASTER_WR = 1,
	/** I2C master reads data from slave*/
	I2C_MASTER_RD = 2,
	/**I2C slave receives data from master*/
	I2C_SLAVE_RX = 3,
	/** I2C slave sends data to master*/
	I2C_SLAVE_TX = 4
};

/*@brief I2C controller role definition
 * Before performing transfer, users should indicate the
 * controller as the master role or slave role.
*/
enum i2c_trans_mode {
	/**Set I2C controller to master role*/
	I2C_MASTER_MODE = 0,
	/**Set I2C controller to slave role*/
	I2C_SLAVE_MODE = 1
};

/**
  * @}
  */

/** @defgroup driver_i2c_typedef Typedef
  * @{
  * This section introduces the typedef that I2C M-HAL used.
  */

/** @brief This defines the callback function prototype.
 * It's used for DMA mode transaction.\n
 * Users should register a callback function when using DMA mode.\n
 * In DMA mode, once I2C and DMA hardware have completed the transaction,
 * a DMA interrupt is triggered and a related user callback
 * is called in the DMA interrupt service routine.\n
 * This typedef is used to describe the callback function
 * that the user wants to call.
 *
 * @param [in] user_data : An OS-HAL defined parameter is private
 * by #mtk_mhal_i2c_dma_done_callback_register().
 * @sa #mtk_mhal_i2c_dma_done_callback_register()
 *
 * @return  Return "0" if callback registers successfully;
 * otherwise, return "-1".
 */
typedef int (*i2c_dma_done_callback) (void *user_data);

/**
  * @}
  */


/** @defgroup driver_i2c_struct Struct
  * @{
  * This section introduces the structure that I2C OS-HAL/M-HAL/HDL used.
  */

/** @brief Interface to messages.
 * The struct i2c_msg needs to be configured before each transfer,
 * and each i2c_msg represents a packet. One transaction of I2C master
 * module can support up to three consecutive packets.
 */
struct i2c_msg {
	/**The address of slave device which the master access.
	 * Don't care when I2C controller as a slave device role.
	 */
	u8 addr;
	/**I2C transfer operation mode.
	 * When I2C controller is set as a master role, the #i2c_trans_op
	 * can choose I2C_WRRD, I2C_WR or I2C_RD.
	 * When I2C controller is set as a slave role, the #i2c_trans_op
	 * can choose I2C_SLAVE_RX or I2C_SLAVE_TX.
	 */
	enum i2c_trans_op flags;
	/** Read or write data length, unit: byte */
	u16 len;
	/** Pointer to read or write data buffer */
	u8 *buf;
};

/** @brief M-HAL private structure.
 * It's only used by DMA mode to program M-HAL.
 * OS-HAL does not need to care this structure.
 */
struct mtk_i2c_private {
	/** used for tx temp buf */
	u8 *tx_buff;
	/** used for rx temp buf */
	u8 *rx_buff;
	/** temp tx DMA physical addr*/
	dma_addr_t tx_dma_addr;
	/** temp rx DMA physical addr*/
	dma_addr_t rx_dma_addr;
	/** Temp transfer length to DMA */
	u32 tx_len;
	/** Temp transfer length to DMA */
	u32 rx_len;
	/** User_data is an OS-HAL defined parameter provided
	* by #mtk_mhal_i2c_dma_done_callback_register().
	*/
	void *user_data;

	/** This function is used to register user's DMA done callback
	* to OS-HAL layer
	*/
	i2c_dma_done_callback dma_done_callback;
};

/** @brief The mtk_i2c_controller contains hardware information( such
  *  as base address) of i2c controller, data transmission information
  *  and transmission methode,etc.
  */
struct mtk_i2c_controller {
	/** I2C controller base address*/
	void __iomem *base;

	/** Clock gate base address of I2C controller*/
	void __iomem *cg_base;

	/** TX DMA channel */
	int dma_tx_chan;
	/** RX DMA channel */
	int dma_rx_chan;

	/** Configure I2C transfer message*/
	struct i2c_msg *msg;

	/** The number of message*/
	u8 msg_num;

	/** I2C supports FIFO mode and DMA mode, 0: FIFO mode, 1: DMA mode*/
	bool dma_en;

	/** Interrupt flag, this parameter will be set
	 * in #mtk_mhal_i2c_irq_handle(). The OS-HAL driver
	 * can judge the transmission status based on this flag.
	 */
	u32 irq_stat;

	/**Set the transmission timeout period*/
	u32 timeout;

	/** I2C master transfer speed*/
	enum i2c_speed_kHz i2c_speed;

	/** 0: Set I2C controller as a master or slave role */
	enum i2c_trans_mode i2c_mode;

	/**I2C master transfer mode, it is used to HDL driver*/
	u8 op;

	/**Set i2c slave address, default by 0x20*/
	u8 slave_addr;

	/** M-HAL private structure is used by M-HAL only */
	struct mtk_i2c_private *mdata;
};

/**
 * @}
 */

/** @defgroup driver_i2c_function Function
 * @{
 * This section provides Fixed APIs(defined as Common Interface)
 * to fully control the MediaTek I2C HW.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@brief This function is used to initialize I2C and DMA hardware.
 *@brief Usage : Initialize hardware according to i2c arg.
 * It must be called if i2c transfer fail.
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *
 *@return
 * Return "0" if configure I2C hardware successfully.\n
 * Return -#I2C_EPTR if *i2c is NULL.\n
 * Return -#I2C_EINVAL if *i2c contains illegal parameter.
 */
int mtk_mhal_i2c_init_hw(struct mtk_i2c_controller *i2c);

/**
 *@brief This function is used to trigger I2C controller transfer.
 *@brief Usage : While the function returns, the transfer may not be
 * completed. So OS-HAL should wait for I2C hardware irq to be finished
 * (maybe use semaphore). If I2C controller is set as master role,
 * It will start transfer when the function returns. If I2C controller
 * is set as slave role, It will wait I2C master to generate start signal.
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *
 *@return
 * Return "0" if the transfer is in progress.\n
 * Return -#I2C_EPTR if *i2c is NULL.
 * Return -#I2C_EINVAL if *i2c contains illegal parameter.
 * Return -#I2C_EBUSY if I2C hardware busy.
 */
int mtk_mhal_i2c_trigger_transfer(struct mtk_i2c_controller *i2c);

/**
 *@brief Get I2C irq status and clear irq status reg.
 *@brief Usage : This function will get interrupt status, assign to
 * i2c->irq_stat and clear irq status. It's called by I2C irq function.
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *
 *@return
 * Return "0" if I2C get and clear interrupt status successfully.\n
 * Return "-1" if I2C interrupt abnormal.\n
 * Return -#I2C_EPTR if *i2c is NULL.
 * Return -#I2C_EINVAL if *i2c contains illegal parameter.
 */
int mtk_mhal_i2c_irq_handle(struct mtk_i2c_controller *i2c);

/**
 *@brief This fuction is used to analyze the type of i2c interrupt.
 * And read Rx FIFO register then copy data to i2c->msg->buf
 * if I2C transfers data successfully when using FIFO mode.
 *@brief Usage : OS-HAL driver must call this function after
 * I2C hardware completes data transfer.
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *
 *@return
 * Return "0" if the transfer is in progress.\n
 * Return -#I2C_EPTR if *i2c is NULL.
 * Return -#I2C_EINVAL if *i2c contains illegal parameter.
 */
int mtk_mhal_i2c_result_handle(struct mtk_i2c_controller *i2c);

/**
 *@brief Enable I2C clock before transfer
 *@brief Usage : This function is used to enable I2C clock before
 * OS-HAL calls any function that sets the I2C module register operation,
 * otherwise the access to I2C register will fail.
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *
 *@return
 * Return "0" if enable I2C clock successfully.\n
 * Return -#I2C_EPTR if *i2c is NULL.
 * Return -#I2C_EINVAL if *i2c contains illegal parameter.
 */
int mtk_mhal_i2c_enable_clk(struct mtk_i2c_controller *i2c);

/**
 *@brief This function is used to disable I2C clock after transfer.
 *@brief Usage : It needs to be called after data transfer to disable clock.
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *
 *@return
 * Return "0" if disable I2C clock successfully.\n
 * Return -#I2C_EPTR if *i2c is NULL.
 * Return -#I2C_EINVAL if *i2c contains illegal parameter.
 */
int mtk_mhal_i2c_disable_clk(struct mtk_i2c_controller *i2c);

/**
 *@brief This function is used to dump I2C register value for debugging.
 *@brief Usage : Used for dump I2C register.
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *
 *@return
 * Return "0" if dump register value successfully.\n
 * Return -#I2C_EPTR if *i2c is NULL.
 * Return -#I2C_EINVAL if *i2c contains illegal parameter.
 */
int mtk_mhal_i2c_dump_register(struct mtk_i2c_controller *i2c);

/**
 *@brief This function is used to initialize transfer speed
 *@brief Usage : OS-HAL driver calls this function to set I2C transfer
 * speed and it uses default transfer speed 100 khz by default when I2C
 * controller is set as master role. OS-HAL driver does not call this function
 * if I2C controller is set as slave role.
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *@param [in] i2c_speed_kHz : It can be set to one of the enum #i2c_speed_kHz.
 *
 *@return
 * Return "0" if I2C sets speed successfully.\n
 * Return -#I2C_EPTR if i2c is NULL.\n
 * Return -#I2C_EINVAL if a parameter is invalid.
 */
int mtk_mhal_i2c_init_speed(struct mtk_i2c_controller *i2c,
			    enum i2c_speed_kHz);

/**
 *@brief This function is used to set I2C salve address
 *@brief Usage : OS-HAL driver calls this function to set I2C slave address
 * before transfer when I2C hardware controller is set as a slave role,
 * it which means OS-HAL driver does not call this function when hardware
 * is set as an I2C master role.
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *@param [in] slv_addr : Set controller slave address.
 *
 *@return
 * Return "0" if I2C sets slave address successfully.\n
 * Return -#I2C_EPTR if i2c is NULL.\n
 * Rerurn -#I2C_EINVAL If a parameter is invalid then I2C slave address
 * uses 0x20 by default.
 */
int mtk_mhal_i2c_init_slv_addr(struct mtk_i2c_controller *i2c, u8 slv_addr);

/**@brief This defines the callback function prototype.
 * It's used for DMA mode transaction.\n
 * Users should register a callback function when using I2C DMA mode.\n
 * In DMA mode, once I2C and DMA hardware completes transsaction,
 * a DMA interrupt is triggered and a related user callback
 * is called in the DMA interrupt service routine.\n
 * This typedef is used to describe the callback function
 * that the user wants to call.
 *
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *@param [in] callback : The callback function given by OS-HAL
 * which will be called in I2C module DMA interrupt service routine.
 *@param [in] user_data : A parameter given by OS-HAL and will
 * be passed to users when the callback function is called.
 *
 *@return
 * Return "0" if callback registers successfully.\n
 * Return -#I2C_EPTR if i2c or user_data is NULL.
 */
int mtk_mhal_i2c_dma_done_callback_register(
		struct mtk_i2c_controller *i2c,
		i2c_dma_done_callback callback,
		void *user_data);

/**
 *@brief This function is used to allocate I2C DMA channel.
 *@brief Usage: User should call it to allocate DMA channel
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *@return
 * Return 0 if users allocate DMA channel successfully.\n
 * Return -#I2C_EPTR if i2c is NULL.\n
 * Return -#I2C_EBUSY if dma chan is busy.
 */
int mtk_mhal_i2c_request_dma(struct mtk_i2c_controller *i2c);

/**
 *@brief This function is used to release I2C DMA channel.
 *@brief Usage: user should call it to release DMA channel
 *@param [in] i2c : mtk_i2c_controller pointer, it contains register
 * base address, data transmission information and i2c hardware information.
 *@return
 * Return 0 if users release DMA channel successfully.\n
 * Return -#I2C_EPTR if i2c is NULL.\n
 */
int mtk_mhal_i2c_release_dma(struct mtk_i2c_controller *i2c);

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
