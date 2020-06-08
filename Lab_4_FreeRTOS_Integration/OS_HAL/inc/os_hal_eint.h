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

#ifndef __OS_HAL_EINT_H__
#define __OS_HAL_EINT_H__

#include "mhal_eint.h"

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup eint
 * @{
 * This section introduces the External Interrupt Controller(EINT) APIs
 * including terms and acronyms, supported features,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section OS_HAL_EINT_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                                           |
 * |------------------------|------------|
 * |\b EINT                 | External Interrupt Controller.|
 *
 * @section OS_HAL_EINT_Features_Chapter Supported Features
 * See @ref HAL_EINT_Features_Chapter for the details of  Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup eint
 * @{
 * @section OS_HAL_EINT_Driver_Usage_Chapter How to use this driver
 *
 * - \b Device \b driver \b sample \b code \b is \b as \b follows: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *    @code
 *	- Register EINT
 *	 -Call mtk_os_hal_eint_register(eint_number eint_num,
 *			eint_trigger_mode trigger_mode, void (*handle)(void))
 *		to register eint.
 *
 *	- Unregister EINT
 *   -Call mtk_os_hal_eint_unregister(eint_number eint_num)
 *		to unregister eint.
 *
 *	- Set EINT debounce
 *	  -Call mtk_os_hal_eint_set_debounce(eint_number eint_num,
 *			os_hal_eint_debounce_time debounce_time)
 *		to set debounce
 *
 *	- Enable/disable debounce
 *	 - Call mtk_os_hal_eint_enable_debounce(eint_number eint_num)
 *		or mtk_os_hal_eint_disable_debounce(eint_number eint_num)
 *		to enable/disable eint debounce function.
 *
 *    @endcode
 *
 * @}
 * @}
 */

/**
* @addtogroup OS-HAL
* @{
* @addtogroup eint
* @{
*/

/** @defgroup os_hal_eint_enum Enum
  * @{
  * This section introduces the enumerations
  * that EINT uses while set debounce time.
  */

/** @brief This enum defines the range of EINT debounce time(unit: ms).
 */

typedef enum {
	OS_HAL_EINT_DB_TIME_1 = 1,
	OS_HAL_EINT_DB_TIME_2 = 2,
	OS_HAL_EINT_DB_TIME_4 = 4,
	OS_HAL_EINT_DB_TIME_8 = 8,
	OS_HAL_EINT_DB_TIME_16 = 16,
	OS_HAL_EINT_DB_TIME_32 = 32,
	OS_HAL_EINT_DB_TIME_64 = 64,
	OS_HAL_EINT_DB_TIME_128 = 128,
	OS_HAL_EINT_DB_TIME_MAX = OS_HAL_EINT_DB_TIME_128
} os_hal_eint_debounce_time;

/**
 * @}
 */

/** @defgroup os_hal_eint_function Function
  * @{
   * This section provides high level APIs to upper layer.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@brief This function is used to register EINT.
 *@brief Usage: If User want use EINT, should call it to register EINT.
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@param [in] trigger_mode : EINT trigger mode.
 *
 *@param [in] handle : The callback function when  EINT is triggered.
 *
 *@return
 * Return value is positive number mean it is irq number.
 * Return -#EINVAL if eint_num or trigger_mode is invalid argument.
 */
int mtk_os_hal_eint_register(eint_number eint_num,
			eint_trigger_mode trigger_mode, void (*handle)(void));

/**
 *@brief This function is used to unregister EINT.
 *@brief Usage: After register EINT, if user want close EINT function.
 *				User should call it to unregister eint.
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@return
 * Return "0" if unregister success.
 * Return -#EINVAL if eint_num is invalid argument.
 */
int mtk_os_hal_eint_unregister(eint_number eint_num);

/**
 *@brief This function is used to set EINT debounce time and enable it.
 *@brief Usage: After register EINT, if user want use debounce function,
 *              user should call it to set EINT debounce time and enable it.
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@param [in] debounce_time : debounce_time is the EINT debounce time,
 *   the value is OS_HAL_EINT_DB_TIME_1 ~ OS_HAL_EINT_DB_TIME_MAX.
 *@return
 * Return "0" if set debounce success.
 * Return -#EINVAL if eint_num or debounce_time is invalid argument.
 */
int mtk_os_hal_eint_set_debounce(eint_number eint_num,
			os_hal_eint_debounce_time debounce_time);

/**
 *@brief This function is used to set EINT trigger type.
 *@brief Usage: After register EINT, if user want to change trigger mode,
 *			User can call it to change EINT trigger mode.
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@param [in] trigger_mode : EINT trigger mode.
 *@return
 * Return "0" if set type success.
 * Return -#EINVAL if eint_num or trigger_mode is invalid argument.
 */
int mtk_os_hal_eint_set_type(eint_number eint_num,
			eint_trigger_mode trigger_mode);

/**
 *@brief This function is used to enable debounce function.
 *@brief Usage: After disable debounce, user can call it to enable debounce.
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@return
 * Return "0" if enable debounce success.
 * Return -#EINVAL if eint_num is invalid argument.
 */
int mtk_os_hal_eint_enable_debounce(eint_number eint_num);

/**
 *@brief This function is used to disable debounce function.
 *@brief Usage:User can call it to disable debounce function.
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@return
 * Return "0" if set type success.
 * Return -#EINVAL if eint_num is invalid argument.
 */
int mtk_os_hal_eint_disable_debounce(eint_number eint_num);

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

#endif /* __HAL_EINT_H__ */

