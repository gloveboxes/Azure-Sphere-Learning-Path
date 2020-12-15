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

#ifndef __OS_HAL_LP_H__
#define __OS_HAL_LP_H__

#include <stdint.h>

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup lp
 * @{
 * This section introduces the Low Power (LP) APIs
 * including terms and acronyms, supported features, software architecture,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section OSHAL_LP_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b LP                        | Low Power.|
 * |\b LSLP                       | Legacy Sleep Low Power.|
 * |\b DSLP                        | Deep Sleep Low Power.|
 *
 * @section OSHAL_LP_Features_Chapter Supported Features
 *
 * The ARM Cortex-M4F I/O subsystems support three kinds of low power
 * scenarios, including CPU Clock Gating Low Power Scenario, Legacy Sleep Low
 * Power Scenario, and Deep Sleep Low Power Scenario.\n
 * For more details, please refer to LP datasheet.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup lp
 * @{
 * @section OSHAL_LP_Driver_Usage_Chapter How to use this driver
 *
 * - \b SW \b Architecture: \n
 *  - LP driver only has OS HAL and the driver flow for entering low power
 *  scenario is as follows:
 *    - Check whether WIC has a pending wake-up event.
 *    - Disable ARM Cortex-M4F I/O subsystems systick.
 *    - Prevent exception service.
 *    - Configure wake-up source so that system can resume.
 *    - Configure memory power and set low power control bit.
 *    - Trigger WFI to enter low power scenario.\n
 *  - The driver flow for resuming is as follows. (Note that this flow doesn't
 *  apply for deep sleep low power scenario, which uses reboot flow.)
 *    - Restore exception service.
 *    - Enable ARM Cortex-M4F I/O subsystems systick.
 *
 * - \b Device \b driver \b sample \b code \b is \b as \b follows: \n
 *  This section describes the definition of APIs and provides
 *  an example of how to use these APIs to enter
 *  different low power scenarios.\n
 *  - sample code (this is the user application sample code):
 *    @code
 *	// first call this function to clear deep sleep low power scenario bit
 *	mtk_os_hal_lp_dslp_check(PRE_DEEP_SLEEP);
 *
 *	// call this function to enter selected low power scenario
 *	// in the example below, select GPT3 as wake-up source and enter DSLP
 *	mtk_os_hal_lp_enter(GPT3_WAKEUP_SRC_24, DEEP_SLEEP);
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
* @addtogroup lp
* @{
*/

/** @defgroup driver_lp_enum Enum
  * @{
  * This section introduces the enumerations
  * that LP should configure before entering LP.
  */

/** @brief DSLP enter/exit stage definition
* Before entering DSLP, user should use
* the \b #PRE_DEEP_SLEEP parameter to call #mtk_os_hal_lp_dslp_check() to
* clear deep sleep bit.\n
* When the system has resumed, users can use the \b #POST_DEEP_SLEEP parameter
* to check whether the system resumes from DSLP scenario.
*/
typedef enum {
	/** It is used before entering DSLP stage */
	PRE_DEEP_SLEEP = 0,
	/** It is used after exiting DSLP stage */
	POST_DEEP_SLEEP
} dslp_check;

/** @brief LP three scenarios definition
* Users should select the following parameters when entering related
* low power scenario.
*/
typedef enum {
	/** CPU Clock Gating is 0 */
	CPU_CLOCK_GATING = 0,
	/** Legacy Sleep is 1 */
	LEGACY_SLEEP,
	/** Deep Sleep is 2 */
	DEEP_SLEEP
} lp_scenario;

/** @brief LP wake-up source definition
* Users should select the following parameter to configure wake-up source in
* #mtk_os_hal_lp_enter().
*/
typedef enum {
	/** Wake-up source EINT0 is 0 */
	EINT_WAKEUP_SRC_0 = (1 << 0),
	/** Wake-up source EINT1 is 1 */
	EINT_WAKEUP_SRC_1 = (1 << 1),
	/** Wake-up source EINT2 is 2 */
	EINT_WAKEUP_SRC_2 = (1 << 2),
	/** Wake-up source EINT3 is 3 */
	EINT_WAKEUP_SRC_3 = (1 << 3),
	/** Wake-up source EINT4 is 4 */
	EINT_WAKEUP_SRC_4 = (1 << 4),
	/** Wake-up source EINT5 is 5 */
	EINT_WAKEUP_SRC_5 = (1 << 5),
	/** Wake-up source EINT6 is 6 */
	EINT_WAKEUP_SRC_6 = (1 << 6),
	/** Wake-up source EINT7 is 7 */
	EINT_WAKEUP_SRC_7 = (1 << 7),
	/** Wake-up source EINT8 is 8 */
	EINT_WAKEUP_SRC_8 = (1 << 8),
	/** Wake-up source EINT9 is 9 */
	EINT_WAKEUP_SRC_9 = (1 << 9),
	/** Wake-up source EINT10 is 10 */
	EINT_WAKEUP_SRC_10 = (1 << 10),
	/** Wake-up source EINT11 is 11 */
	EINT_WAKEUP_SRC_11 = (1 << 11),
	/** Wake-up source EINT12 is 12 */
	EINT_WAKEUP_SRC_12 = (1 << 12),
	/** Wake-up source EINT13 is 13 */
	EINT_WAKEUP_SRC_13 = (1 << 13),
	/** Wake-up source EINT14 is 14 */
	EINT_WAKEUP_SRC_14 = (1 << 14),
	/** Wake-up source EINT15 is 15 */
	EINT_WAKEUP_SRC_15 = (1 << 15),
	/** Wake-up source EINT16 is 16 */
	EINT_WAKEUP_SRC_16 = (1 << 16),
	/** Wake-up source EINT17 is 17 */
	EINT_WAKEUP_SRC_17 = (1 << 17),
	/** Wake-up source EINT18 is 18 */
	EINT_WAKEUP_SRC_18 = (1 << 18),
	/** Wake-up source EINT19 is 19 */
	EINT_WAKEUP_SRC_19 = (1 << 19),
	/** Wake-up source EINT20 is 20 */
	EINT_WAKEUP_SRC_20 = (1 << 20),
	/** Wake-up source EINT21 is 21 */
	EINT_WAKEUP_SRC_21 = (1 << 21),
	/** Wake-up source EINT22 is 22 */
	EINT_WAKEUP_SRC_22 = (1 << 22),
	/** Wake-up source EINT23 is 23 */
	EINT_WAKEUP_SRC_23 = (1 << 23),
	/** Wake-up source GPT3 is 24 */
	GPT3_WAKEUP_SRC_24 = (1 << 24),
	/** Total wake-up source number is 25 */
	WAKEUP_SRC_MAX = 25
} lp_wakeup_src;

/**
  * @}
  */

/** @defgroup driver_lp_function Function
  * @{
  * This section provides Fixed APIs (defined as Common Interface)
  * to fully support three low power scenarios of ARM Cortex-M4F
  * I/O subsystems.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function is used to configure timeout value of GPT3.
 * @brief Usage: It's called before selecting GPT3 as wakeup source.
 * @param [in] time_val : GPT3 timeout value and unit is second.
 *
 * @return
 * None.
 */
void mtk_os_hal_lp_config_gpt3_timeout(uint32_t time_val);

/**
 * @brief This function is used to check whether ARM Cortex-M4F I/O
 * subsystems have entered DSLP.
 * @brief Usage: It's called before entering DSLP and after exiting DSLP.
 * @param [in] stage : Different DSLP stages.
 *
 * @return
 * Return "0" if DSLP status bit is operated successfully.\n
 * Return "-1" if the parameter \b stage is invalid.
 */
int mtk_os_hal_lp_dslp_check(dslp_check stage);

/**
 * @brief This function is used to enter different low power scenarios.
 * @brief Usage: It's called when ARM Cortex-M4F I/O subsystems
 * need to enter low power scenario.
 * @param [in] wakeup_src : Wakeup source used to wake up system. The
 * supported wakeup sources are defined in \b #lp_wakeup_src enumeration and
 * more than one wakeup sources can be selected by bitwise OR operation at the
 * same time.
 * @param [in] lp : Low power scenario that the system will enter.
 *
 * @return
 * Return "0" if the system enters low power scenario successfully.\n
 * Return "-1" if the parameter \b wakeup_src or \b lp is invalid.
 */
int mtk_os_hal_lp_enter(uint32_t wakeup_src, lp_scenario lp);

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
