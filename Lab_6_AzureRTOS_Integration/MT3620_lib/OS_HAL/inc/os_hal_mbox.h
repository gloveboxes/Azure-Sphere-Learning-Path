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

#ifndef __OS_HAL_MBOX_H__
#define __OS_HAL_MBOX_H__

#include "mhal_mbox.h"

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup mbox
 * @{
 * This section introduces the Mailbox (MBOX) APIs
 * including terms and acronyms, supported features,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section OS_HAL_MBOX_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b FIFO                       | First In, First Out.|
 * |\b MBOX                       | Mailbox.|
 *
 * @section OS_HAL_MBOX_Features_Chapter Supported Features
 * See @ref MHAL_MBOX_Features_Chapter for the details of Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup mbox
 * @{
 * @section OS_HAL_MBOX_Driver_Usage_Chapter How to use this driver
 *
 * - \b Device \b driver \b sample \b code \b is \b as \b follows: \n
 *  - sample code (this is the user application sample code on FreeRTOS):
 *    @code
 *	- Open MBOX channel
 *	 -Call mtk_os_hal_mbox_open_channel(mbox_channel_t channel)
 *	   to open MBOX channel.
 *
 *	- Register sw interrupt callback
 *	  -Call mtk_os_hal_mbox_sw_int_register_cb(mbox_channel_t channel,
 *			mtk_os_hal_mbox_cb cb, u32 irq_status)
 *
 *	- Register FIFO interrupt callback
 *	  -Call mtk_os_hal_mbox_fifo_register_cb(mbox_channel_t channel,
 *			mtk_os_hal_mbox_cb cb, struct mbox_fifo_event *mask)
 *
 *	- Getting/setting MBOX hardware settings
 *	  -Call mtk_os_hal_mbox_ioctl(mbox_channel_t channel, mbox_ioctl_t ctrl,
 *			void *arg)
 *
 *	- Write FIFO
 *	 -Call  mtk_os_hal_mbox_fifo_write(mbox_channel_t channel,
 *		const struct mbox_fifo_item *buf, mbox_tr_type_t type)
 *
 *	- Read FIFO
 *	 -Call  mtk_os_hal_mbox_fifo_read(mbox_channel_t channel,
 *		const struct mbox_fifo_item *buf, mbox_tr_type_t type)
 *
 *	- Unregister sw interrupt callback
 *	  -Call mtk_os_hal_mbox_sw_int_unregister_cb(mbox_channel_t channel)
 *
 *	- Unregister FIFO interrupt callback
 *	  -Call mtk_os_hal_mbox_fifo_unregister_cb(mbox_channel_t channel)
 *
 *	- Close MBOX channel
 *	 -Call mtk_os_hal_mbox_close_channel(mbox_channel_t channel)
 *	   to close MBOX channel.
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
 * @addtogroup mbox
 * @{
 */

/** @defgroup os_hal_mbox_enum Enum
  * @{
  * This section introduces the enumerations that are used by MBOX OS-HAL
  * and user application.
  */

/** @brief MBOX channel */
typedef enum {
	/** For A7 <-> M4 */
	OS_HAL_MBOX_CH0 = 0,
	/** For M4 <-> M4 */
	OS_HAL_MBOX_CH1 = 1,
	/** Invalid channel */
	OS_HAL_MBOX_CH_MAX
} mbox_channel_t;

/**
  * @}
  */

/** @defgroup os_hal_mbox_define Define
  * @{
  * This section introduces the Macro definition
  * which indicates the software interrupt amount of MBOX.
  */

/** Software interrupt amount */
#define MBOX_NUM_SW_INT				(8)

/**
  * @}
  */

/** @defgroup os_hal_mbox_struct Struct
  * @{
  * This section introduces the structures that are used by MBOX OS-HAL
  * and user application.
  */

/** @brief The argument of user callback */
struct mtk_os_hal_mbox_cb_data {
	/** FIFO interrupt information */
	struct mbox_fifo_event event;
	/** Software interrupt information */
	struct mbox_swint_info swint;
};

/**
  * @}
  */

/** @defgroup os_hal_mbox_typedef Typedef
  * @{
  * This section introduces the typedef that MBOX OS-HAL used.
  */

/** @brief This defines the callback function prototype.
 *  @brief Usage: It's called when FIFO or software interrupt come.\n
 *   User can register the callback function when using
 *   MBOX mtk_os_hal_mbox_sw_int_register_cb and/or
 *   mtk_os_hal_mbox_fifo_register_cb API.\n
 *   Please do NOT operate MBOX in callback function.
 *
 * @param [in] data : the argument of user callback.
 */
typedef void (*mtk_os_hal_mbox_cb)(struct mtk_os_hal_mbox_cb_data *data);

/**
  * @}
  */

/** @defgroup os_hal_mbox_function Function
  * @{
  * This section provides high level APIs to upper layer.
  */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Open MBOX channel.
 *  @param [in] channel : MBOX channel,
 *   it can be OS_HAL_MBOX_CH0~OS_HAL_MBOX_CH1.
 *
 *  @return #MBOX_OK means success.
 *  @return -#MBOX_EDEFAULT means fail.
 */
int mtk_os_hal_mbox_open_channel(mbox_channel_t channel);

/** @brief Close MBOX channel.
 *  @param [in] channel : MBOX channel,
 *   it can be OS_HAL_MBOX_CH0~OS_HAL_MBOX_CH1.
 *
 *  @return #MBOX_OK means success.
 *  @return -#MBOX_EDEFAULT means fail.
 */
int mtk_os_hal_mbox_close_channel(mbox_channel_t channel);

/**
 *@brief This function is used to read MBOX channel FIFO data.
 *@brief Usage: Reading data and/or cmd from FIFO when receiving
 * write interrupt or non-empty interrupt.
 *@param [in] channel : MBOX channel.
 *@param [out] buf : Buffer to load data and/or cmd.
 *@param [in] type : Transfer type; see @ref mbox_tr_type_t for details.
 *
 *@return
 * Return #MBOX_OK if reading succeeds.\n
 * Return others if reading fails.
 */
int mtk_os_hal_mbox_fifo_read(mbox_channel_t channel,
			struct mbox_fifo_item *buf, mbox_tr_type_t type);

/**
 *@brief This function is used to write data to MBOX channel FIFO.
 *@brief Usage: Writing data and/or cmd to FIFO.
 *@param [in] channel : MBOX channel.
 *@param [in] buf : The data and/or cmd to be transferred.
 *@param [in] type : Transfer type; see @ref mbox_tr_type_t for details.
 *
 *@return
 * Return #MBOX_OK if writing succeeds.\n
 * Return others if writing fails.
 */
int mtk_os_hal_mbox_fifo_write(mbox_channel_t channel,
		const struct mbox_fifo_item *buf, mbox_tr_type_t type);

/**
 *@brief Control various hardware settings of MBOX
 *@brief Usage: Getting/setting MBOX hardware settings.
 *@param [in] channel : MBOX channel.
 *@param [in] ctrl :\n
 *	Operation on Non-Full/Non-Empty threshold values: \n
 *	MBOX_IOSET_NE_THRS,\n
 *	MBOX_IOGET_NE_THRS,\n
 *	MBOX_IOSET_NF_THRS,\n
 *	MBOX_IOGET_NF_THRS. \n
 *	\n
 *	Hardware semaphore operations: \n
 *	MBOX_IOSET_SEM_POST,\n
 *	MBOX_IOSET_SEM_PEND. \n
 *	\n
 *	Software interrupt operations: \n
 *	MBOX_IOSET_SWINT_TRIG,\n
 *	MBOX_IOSET_SWINT_EN,\n
 *	MBOX_IOGET_SWINT_EN,\n
 *	MBOX_IOSET_SWINT_CTRL,\n
 *	MBOX_IOGET_SWINT_STS,\n
 *	MBOX_IOSET_CLEAR_SWINT. \n
 *	\n
 *	FIFO interrupt operations: \n
 *	MBOX_IOSET_INT_EN,\n
 *	MBOX_IOGET_INT_EN,\n
 *	MBOX_IOGET_INT_STS,\n
 *	MBOX_IOSET_CLEAR_INT. \n
 *	\n
 *	Get FIFO count values: \n
 *	MBOX_IOGET_POST_FIFO_CNT,\n
 *	MBOX_IOGET_ACPT_FIFO_CNT.
 *
 *@param [in] arg :\n
 *	MBOX_IOSET_NE_THRS: u32*, threshold value; \n
 *	MBOX_IOSET_NF_THRS: u32*, threshold value; \n
 *	\n
 *	MBOX_IOSET_SEM_POST: not used; \n
 *	\n
 *	MBOX_IOSET_SWINT_TRIG: u32*, interrupt number (0 ~ 7); \n
 *	MBOX_IOSET_SWINT_EN: u32*, interrupt mask (0 ~ 0xFF); \n
 *	MBOX_IOSET_SWINT_CTRL: struct mbox_swint_arg*; \n
 *	MBOX_IOSET_CLEAR_SWINT: u32*, software interrupt mask to be cleared; \n
 *	\n
 *	MBOX_IOSET_INT_EN: struct mbox_int_arg*; \n
 *	MBOX_IOSET_CLEAR_INT: struct mbox_int_arg*; \n
 *	\n
 *
 *@param [out] arg :\n
 *	MBOX_IOGET_NE_THRS: u32*, buffer to load threshold value; \n
 *	MBOX_IOGET_NF_THRS: u32*, buffer to load threshold value; \n
 *	\n
 *	MBOX_IOSET_SEM_PEND: u32*, buffer to load sema status; \n
 *	\n
 *	MBOX_IOGET_SWINT_EN: u32*, buffer to load sw int mask; \n
 *	MBOX_IOGET_SWINT_STS: u32*, buffer to load the status value; \n
 *	\n
 *	MBOX_IOGET_INT_EN: struct mbox_fifo_event*, buffer to load int mask; \n
 *	MBOX_IOGET_INT_STS: struct mbox_fifo_event*, to load int status; \n
 *	\n
 *	MBOX_IOGET_POST_FIFO_CNT: u32*, buffer to load the FIFO count
 *	which indicates how many elements have already been written;\n
 *	MBOX_IOGET_ACPT_FIFO_CNT: u32*, buffer to load the FIFO count
 *	which indicates how many elements can be read. \n
 *

 *@return
 * Return #MBOX_OK if operation succeeds.\n
 * Return others if operation fails .
 */
int mtk_os_hal_mbox_ioctl(mbox_channel_t channel, mbox_ioctl_t ctrl,
			void *arg);

/**
 *@brief This function is used to register user software interrupt callback.
 *@param [in] channel : MBOX channel.
 *@param [in] cb : User callback funtion pointer.
 *@param [in] irq_status : Software interrupt enable mask.
 *	Set irq_status bit[i] = 1 to enable software interrupt #i, i = 0~7.
 *
 *@return
 * Return #MBOX_OK if registration succeeds.\n
 * Return others if registration fails.
 */
int mtk_os_hal_mbox_sw_int_register_cb(mbox_channel_t channel,
			mtk_os_hal_mbox_cb cb, u32 irq_status);

/**
 *@brief This function is used to unregister user software interrupt callback.
 *@param [in] channel : MBOX channel.
 *
 *@return
 * Return #MBOX_OK if unregistration succeeds.\n
 * Return -#MBOX_EDEFAULT if unregistration fails.
 */
int mtk_os_hal_mbox_sw_int_unregister_cb(mbox_channel_t channel);

/**
 *@brief This function is used to register user FIFO interrupt callback.
 *@param [in] channel : MBOX channel.
 *@param [in] cb : User callback funtion pointer.
 *@param [in] mask : Fifo interrupt enable mask.
 *
 *@return
 * Return #MBOX_OK if registration succeeds.\n
 * Return others if registration fails.
 */
int mtk_os_hal_mbox_fifo_register_cb(mbox_channel_t channel,
			mtk_os_hal_mbox_cb cb, struct mbox_fifo_event *mask);

/**
 *@brief This function is used to unregister user FIFO interrupt callback.
 *@param [in] channel : MBOX channel.
 *
 *@return
 * Return #MBOX_OK if unregistration succeeds.\n
 * Return -#MBOX_EDEFAULT if unregistration fails.
 */
int mtk_os_hal_mbox_fifo_unregister_cb(mbox_channel_t channel);

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

#endif /* __OS_HAL_MBOX_H__ */
