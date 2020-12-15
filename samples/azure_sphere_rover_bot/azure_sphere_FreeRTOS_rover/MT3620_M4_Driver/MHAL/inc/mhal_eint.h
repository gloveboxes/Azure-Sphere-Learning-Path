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

#ifndef __MHAL_EINT_H__
#define __MHAL_EINT_H__

#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup EINT
 * @{
 * This section introduces the External Interrupt Controller(EINT) APIs
 * including terms and acronyms, supported features,  software architecture
 * details on how to use this driver, EINT function groups, enums and functions.
 *
 * @section HAL_EINT_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                           |
 * |------------------------|------------|
 * |\b EINT                 | External Interrupt Controller.|
 * @section HAL_EINT_Features_Chapter Supported features
 * The EINT controller has been designed to process interrupts
 * from an external source or peripheral.
 * For more details, please refer to EINT datasheet.
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup EINT
 * @{

 * - \b Support \b debounce  \n
 * Since the external event may be unstable in a certain period,
 * a debounce mechanism is introduced to ensure the functionality.
 * The circuitry is mainly used to verify that the input remains
 * stable for a programmable number of periods of the clock.
 * Call #mtk_mhal_eint_set_debounce() function to set debounce time.
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup EINT
 * @{
 * @section MHAL_EINT_Driver_Usage_Chapter How to use this driver
 *
 * - \b SW \b Architecture \b is \b as \b follows: \n
 * See @ref MHAL_Overview_2_Chapter for the details of SW architecture.
 *
 *	This section describes the definition of APIs and provides
 *	an example on FreeRTOS about how to use these APIs to develop
 *	an OS-related EINT driver.\n
 *
 * - \b The \b OS-HAL \b FreeRTOS \b driver\n
 * \b sample \b code \b is \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/FreeRTOS/OS_HAL/src/os_hal_eint.c"> freeRTos EINT sample code on github </a>
 *
 * @}
 * @}
 */

/**
* @addtogroup M-HAL
* @{
* @addtogroup EINT
* @{
*/

/** @defgroup driver_eint_define Define
  * @{
  * This section introduces the Macro definition
  * which is used as EINT M-HAL's API error return type.
  */

/** Bad address */
#define EINT_EFAULT		1
/** Invalid argument */
#define EINT_EINVAL		2

/**
  * @}
  */

/** @defgroup driver_eint_enum Enum
  * @{
  *  This section introduces the enumerations that EINT M-HAL used.
  */

/** @brief This enum defines the range of EINT number .
 */

typedef enum {
	HAL_EINT_NUMBER_0 = 0,
	HAL_EINT_NUMBER_1 = 1,
	HAL_EINT_NUMBER_2 = 2,
	HAL_EINT_NUMBER_3 = 3,
	HAL_EINT_NUMBER_4 = 4,
	HAL_EINT_NUMBER_5 = 5,
	HAL_EINT_NUMBER_6 = 6,
	HAL_EINT_NUMBER_7 = 7,
	HAL_EINT_NUMBER_8 = 8,
	HAL_EINT_NUMBER_9 = 9,
	HAL_EINT_NUMBER_10 = 10,
	HAL_EINT_NUMBER_11 = 11,
	HAL_EINT_NUMBER_12 = 12,
	HAL_EINT_NUMBER_13 = 13,
	HAL_EINT_NUMBER_14 = 14,
	HAL_EINT_NUMBER_15 = 15,
	HAL_EINT_NUMBER_16 = 16,
	HAL_EINT_NUMBER_17 = 17,
	HAL_EINT_NUMBER_18 = 18,
	HAL_EINT_NUMBER_19 = 19,
	HAL_EINT_NUMBER_20 = 20,
	HAL_EINT_NUMBER_21 = 21,
	HAL_EINT_NUMBER_22 = 22,
	HAL_EINT_NUMBER_23 = 23,
	HAL_EINT_NUMBER_MAX
} eint_number;

/** @brief This enum defines the EINT trigger mode.
 * when registering EINT interrupt or setting trigger type,
 * users should use this enum.
 */

typedef enum {
	/** EINT trigger mode is low level */
	HAL_EINT_LEVEL_LOW     = 0,
	/** EINT trigger mode is high level */
	HAL_EINT_LEVEL_HIGH    = 1,
	/** EINT trigger mode is falling edge */
	HAL_EINT_EDGE_FALLING  = 2,
	/** EINT trigger mode is rising edge */
	HAL_EINT_EDGE_RISING   = 3,
	/** EINT trigger mode is dual-edge */
	HAL_EINT_EDGE_FALLING_AND_RISING = 4,
} eint_trigger_mode;
/**
 * @}
 */

/** @defgroup driver_eint_function Function
  * @{
  * This section provides Fixed APIs(defined as Common Interface)
  * to fully control the MediaTek EINT HW.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@brief This function is used to enable EINT debounce.
 *@brief Usage: OS-HAL driver should call it in EINT register
 *                     function to enable debounce.
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@param [in] eint_base : EINT base address used to operate hardware register.
 *
 *@return
 * Return "0" if enable debounce success.\n
 * Return -#EFAULT if eint_base is NULL.\n
 * Return -#EINVAL if eint_num is invalid argument.\n
 */
extern int mtk_mhal_eint_enable_debounce(
			eint_number eint_num, void __iomem *eint_base);
/**
 *@brief This function is used to disable EINT debounce.
 *@brief Usage: OS-HAL driver should call it in EINT unregister
 *                     function to disable debounce.
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@param [in] eint_base : EINT base address used to operate hardware register.
 *
 *@return
 * Return "0" if disable debounce success.\n
 * Return -#EFAULT if eint_base is NULL.\n
 * Return -#EINVAL if eint_num is invalid argument.\n
 */
extern int mtk_mhal_eint_disable_debounce(
			eint_number eint_num, void __iomem *eint_base);

/**
 *@brief This function is used to set EINT polarity.
 *@brief Usage: OS-HAL driver should call it in EINT register
 *                     or set trigger type function
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@param [in] eint_base : EINT base address used to operate hardware register.
 *@param [in] pol : Polarity type
 *
 *@return
 * Return "0" if set polarity success.\n
 * Return -#EFAULT if eint_base is NULL.\n
 * Return -#EINVAL if eint_num or pol is invalid argument.\n
 */
extern int mtk_mhal_eint_set_polarity(eint_number eint_num,
			u32 pol, void __iomem *eint_base);

/**
 *@brief This function is used to set EINT dual-edge.
 *@brief Usage: OS-HAL driver should call it in EINT register
 *                     function to set EINT dual-edge.
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@param [in] eint_base : EINT base address used to operate hardware register.
 *@param [in] dual : Dual =1 :dual-edge detect function enable.
 *                           Dual =0 :dual-edge detect function disable.
 *
 *@return
 * Return "0" if set dual success.\n
 * Return -#EFAULT if eint_base is NULL.\n
 * Return -#EINVAL if eint_num or dual is invalid argument.\n
 */
extern int mtk_mhal_eint_set_dual(eint_number eint_num,
			u32 dual, void __iomem *eint_base);

/**
 *@brief This function is used to set EINT debounce time.
 *@brief Usage: OS-HAL driver should call it in EINT register
 *                    function to set EINT debounce time
 *@param [in] eint_num : eint_num is the EINT number, the value is
 *                                  HAL_EINT_NUMBER_0 ~ HAL_EINT_NUMBER_MAX-1.
 *@param [in] eint_base : EINT base address used to operate hardware register.
 *@param [in] debounce_time : EINT trigger's debounce time(unit: ms).
 *                                  Value range is 1, 2, 4, 8, 16, 32, 64, 128.
 *
 *@return
 * Return "0" if set debounce success.\n
 * Return -#EFAULT if eint_base is NULL.\n
 * Return -#EINVAL if eint_num is invalid argument.\n
 */
extern int mtk_mhal_eint_set_debounce(eint_number eint_num,
			u32 debounce_time, void __iomem *eint_base);

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
