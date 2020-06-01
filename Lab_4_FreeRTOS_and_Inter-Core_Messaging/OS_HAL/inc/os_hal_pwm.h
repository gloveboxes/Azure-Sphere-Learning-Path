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

#ifndef __OS_HAL_PWM_H__
#define __OS_HAL_PWM_H__

#include "mhal_pwm.h"
/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup pwm
 * @{
 * This section introduces the Pulse-Width Modulation (PWM) APIs
 * including terms and acronyms, supported features,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section OS_HAL_PWM_Terms_Chapter Terms and Acronyms
 *
 * |Terms  |Details					|
 * |------------|----------------------------|
 * |\b PWM | Pulse-Width Modulation.	|
 *
 * @section OS_HAL_PWM_Features_Chapter Supported Features
 * See @ref MHAL_PWM_Features_Chapter for the details of  Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup pwm
 * @{
 * @section OS_HAL_PWM_Driver_Usage_Chapter How to use this driver
 *
 * - \b Device \b driver \b sample \b code \b is \b as \b follows: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *	  @code
 *
 *
 *      -PWM normal mode:
 *
 *        -Initialize the PWM module.
 *            - Call mtk_os_hal_pwm_ctlr_init(pwm_groups group_num,
 *            u32 channel_bit_map)
 *
 *        -Select PWM global kick, IO ctrl, polarity setting.
 *            - Call mtk_os_hal_pwm_feature_enable(pwm_groups group_num,
 *            pwm_channels pwm_num,
 *            bool global_kick_enable,
 *            bool io_ctrl_sel,
 *            bool polarity_set)
 *
 *        -Config normal mode freq & duty.
 *            - Call mtk_os_hal_pwm_config_freq_duty_normal(
 *            pwm_groups group_num, pwm_channels pwm_num,
 *            u32	frequency, u32	duty_cycle)
 *
 *        -Enable pwm waveform output.
 *            - Call mtk_os_hal_pwm_start_normal(pwm_groups group_num,
 *            pwm_channels pwm_num)
 *
 *        -Disabe pwm waveform output.
 *            - Call mtk_os_hal_pwm_stop_normal(pwm_groups group_num,
 *            pwm_channels pwm_num)
 *
 *        -Release the PWM module back to its original state.
 *            - Call mtk_os_hal_pwm_ctlr_deinit(pwm_groups group_num,
 *            u32 channel_bit_map)
 *
 *
 *      - PWM 2-state mode:
 *
 *        -Initialize the PWM module.
 *            - Call mtk_os_hal_pwm_ctlr_init(pwm_groups group_num,
 *            u32 channel_bit_map)
 *
 *        -Select PWM global kick, IO ctrl, polarity setting.
 *            - Call mtk_os_hal_pwm_feature_enable(pwm_groups group_num,
 *            pwm_channels pwm_num,
 *            bool global_kick_enable,
 *            bool io_ctrl_sel,
 *            bool polarity_set)
 *
 *        -Set PWM  frequency and duty cycle of S0 stage in 2-state mode.
 *            - Call mtk_os_hal_pwm_config_freq_duty_2_state(
 *            pwm_groups group_num,
 *            pwm_channels pwm_num,
 *            struct mtk_com_pwm_data state_config)
 *
 *        -Set PWM  frequency and duty cycle of S1 stage in 2-state mode.
 *            - Call mtk_os_hal_pwm_config_freq_duty_2_state(
 *            pwm_groups group_num,
 *            pwm_channels pwm_num,
 *            struct mtk_com_pwm_data state_config)
 *
 *        -set PWM stay cycles of S0 & S1 in 2-state mode.
 *            - Call mtk_os_hal_pwm_config_stay_cycle_2_state(
 *            pwm_groups group_num,
 *            pwm_channels pwm_num,
 *            struct mtk_com_pwm_data state_config)
 *
 *        -Disabe pwm waveform output.
 *            - Call mtk_os_hal_pwm_stop_normal(pwm_groups group_num,
 *            pwm_channels pwm_num)
 *
 *        -Release the PWM module back to its original state.
 *            - Call mtk_os_hal_pwm_ctlr_deinit(pwm_groups group_num,
 *            u32 channel_bit_map)
 *
 *
 *      - PWM differential mode:
 *
 *        -Initialize the PWM module.
 *            - Call mtk_os_hal_pwm_ctlr_init(pwm_groups group_num,
 *            u32 channel_bit_map)
 *
 *        -Select PWM global kick, IO ctrl, polarity setting.
 *            - Call mtk_os_hal_pwm_feature_enable(pwm_groups group_num,
 *            pwm_channels pwm_num,
 *            bool global_kick_enable,
 *            bool io_ctrl_sel,
 *            bool polarity_set)
 *
 *        -Config normal mode freq & duty.
 *            - Call mtk_os_hal_pwm_config_freq_duty_normal(
 *            pwm_groups group_num, pwm_channels pwm_num,
 *            u32	frequency, u32	duty_cycle)
 *        -Set PWM channel to differential mode.
 *            - Call mtk_os_hal_pwm_config_dpsel(pwm_groups group_num,
 *            pwm_channels pwm_num,
 *            pwm_differential_select mode)
 *
 *        -Enable pwm waveform output.
 *            - Call mtk_os_hal_pwm_start_normal(pwm_groups group_num,
 *            pwm_channels pwm_num)
 *
 *        -Disabe pwm waveform output.
 *            - Call mtk_os_hal_pwm_stop_normal(pwm_groups group_num,
 *            pwm_channels pwm_num)
 *
 *        -Release the PWM module back to its original state.
 *            - Call mtk_os_hal_pwm_ctlr_deinit(pwm_groups group_num,
 *            u32 channel_bit_map)
 *
 *	@endcode
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup pwm
 * @{
 */

/** @defgroup os_hal_pwm_define Define
* @{
* This section introduces the Macro definition
* which is used as PWM channel number.
*/

#define MAX_CHANNEL_NUM 4
/**< Max channel number */

/**
  * @}
  */

/** @defgroup os_hal_pwm_enum Enum
  * @{
  *	OS-HAL PWM enumeration value list information, including
  *	define of  the PWM group number enumeration, the PWM
  *	channel number enumeration and	define of the PWM
  *	running status enumeration.
  */

/** @brief Defines the PWM group number */
typedef enum {
	OS_HAL_PWM_GROUP0  = 0,
	/**< PWM group0*/
	OS_HAL_PWM_GROUP1  = 1,
	/**< PWM group1*/
	OS_HAL_PWM_GROUP2  = 2,
	/**< PWM group2*/
	OS_HAL_PWM_GROUP_MAX
	/**< PWM max group <invalid>*/
}	pwm_groups;

/** @brief Defines the PWM channel bit map */
typedef enum {
	OS_HAL_PWM_0 = 1,
	/**< PWM channel0 bit map*/
	OS_HAL_PWM_1 = 2,
	/**< PWM channel1 bit map*/
	OS_HAL_PWM_2 = 4,
	/**< PWM channel2 bit map*/
	OS_HAL_PWM_3 = 8,
	/**< PWM channel3 bit map*/
	OS_HAL_PWM_MAP_MAX
	/**< PWM max channel  bit map<invalid>*/
}	pwms_bit_map;
/**
  * @}
  */

/** @defgroup os_hal_pwm_function Function
  * @{
  *	OS-HAL PWM API list information, including
  *	PWM hardware initializing and de-initializing,
  *	setting PWM  frequency and duty cycle,
  *	enabling and disabling PWM hardware output,
  *	feature enabling.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Init PWM controller.
 *
 *  @param group_num : PWM group number, 0 is group0 , 1 is group1, 2 is group2
 *  @param channel_bit_map : OS_HAL_PWM_0 = 0x1,  OS_HAL_PWM_1 = 0x2,
 *	OS_HAL_PWM_2 = 0x4,
 *	OS_HAL_PWM_3 = 0x8
 *	For example: use PWM0 and PWM1
 *	channel_bit_map = OS_HAL_PWM_0 | OS_HAL_PWM_1
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_ECLK , it means clock select invalid.
 */
int mtk_os_hal_pwm_ctlr_init(pwm_groups group_num, u32 channel_bit_map);
/**
 * @brief  Deinit PWM controller.
 *
 *  @param group_num : PWM group number, 0 is group0 , 1 is group1, 2 is group2
 *  @param channel_bit_map : OS_HAL_PWM_0 = 0x1,  OS_HAL_PWM_1 = 0x2,
 *	OS_HAL_PWM_2 = 0x4,
 *	OS_HAL_PWM_3 = 0x8
 *	For example: use PWM0 and PWM1
 *	channel_bit_map = OS_HAL_PWM_0 | OS_HAL_PWM_1
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means pwm_num is invalid.
 */
int mtk_os_hal_pwm_ctlr_deinit(pwm_groups group_num, u32 channel_bit_map);
/**
 * @brief  Config freq & duty.
 *
 *  @param group_num : PWM group number, 0 is group0 , 1 is group1, 2 is group2
 *  @param pwm_num : PWM number, 0 is pwm0 , 1 is pwm1, 2 is pwm2, 3 is pwm3
 *  @param frequency : PWM frequency select, unit is Hz. if set 1000,
 *	waveform frequency is 1000Hz.  The maxinum supported frequency is
 *	the half of source clock frequency. When using the 26MHz Xtal as clock
 *	source, the maxinum supported frequency is 13MHz.
 *  @param duty_cycle : PWM duty_cycle ,resolution is 0.1, ex: duty is 50%,
 *	duty_cycle = (50/0.1) = 500
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means pwm_num is invalid.
 */
int mtk_os_hal_pwm_config_freq_duty_normal(pwm_groups group_num,
	pwm_channels pwm_num, u32  frequency, u32  duty_cycle);

/**
 * @brief  PWM feature enable.
 *
 *  @param group_num : PWM group number, 0 is group0 , 1 is group1, 2 is group2
 *  @param pwm_num : PWM number, 0 is pwm0 , 1 is pwm1, 2 is pwm2, 3 is pwm3
 *  @param global_kick_enable : PWM global kick feature enable, typical set
 *	as 0: pwm channel is independent with other pwm channel, 1: all pwm
 *	channel with global_kick_enable set will output waveform at the same
 *	time.
 *  @param io_ctrl_sel : PWM io ctrl select, typical set as 0: PIO as output,
 *	when pwm channel is disable, waveform will keep low, 1:open drain.
 *  @param polarity_set : PWM plus level polarity select, typical set as 0: pwm
 *	waveform is high level active, 1:pwm waveform is low level active.
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means pwm_num is invalid.
 */
int mtk_os_hal_pwm_feature_enable(pwm_groups group_num,
	pwm_channels pwm_num,
	bool global_kick_enable,
	bool io_ctrl_sel,
	bool polarity_set);

/**
 * @brief Config 2-state freq & duty.
 *
 *  @param group_num : PWM group number, 0 is group0 , 1 is group1, 2 is group2
 *  @param pwm_num : PWM number, 0 is pwm0 , 1 is pwm1, 2 is pwm2, 3 is pwm3
 *  @param state_config : store the PWM base address, group number,
 *	clock source, pwm channel common structure information
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means pwm_num is invalid.
 */
int mtk_os_hal_pwm_config_freq_duty_2_state(pwm_groups group_num,
		pwm_channels pwm_num,
		struct mtk_com_pwm_data state_config);
/**
 * @brief Config 2-state stay cycle.
 *
 *  @param group_num : PWM group number, 0 is group0 , 1 is group1, 2 is group2
 *  @param pwm_num : PWM number, 0 is pwm0 , 1 is pwm1, 2 is pwm2, 3 is pwm3
 *  @param state_config : store the PWM base address, group number,
 *	clock source, pwm channel common structure information
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means pwm_num is invalid.
*/
int mtk_os_hal_pwm_config_stay_cycle_2_state(pwm_groups group_num,
		pwm_channels pwm_num,
		struct mtk_com_pwm_data state_config);
/**
 * @brief  Config dpsel.
 *
 *  @param group_num : PWM group number, 0 is group0 , 1 is group1, 2 is group2
 *  @param pwm_num : PWM number, 0 is pwm0 , 1 is pwm1, 2 is pwm2, 3 is pwm3
 *  @param mode : Defines the PWM channel's differential mode
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means pwm_num is invalid.
 */
int mtk_os_hal_pwm_config_dpsel(pwm_groups group_num,
		pwm_channels pwm_num,
		pwm_differential_select mode);
/**
 * @brief  Start PWM controller.
 *
 *  @param group_num : PWM group number, 0 is group0 , 1 is group1, 2 is group2
 *  @param pwm_num : PWM number, 0 is pwm0 , 1 is pwm1, 2 is pwm2, 3 is pwm3
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means pwm_num is invalid.
 */
int mtk_os_hal_pwm_start_normal(pwm_groups group_num,
		pwm_channels pwm_num);
/**
 * @brief  Stop PWM controller.
 *
 *  @param group_num : PWM group number, 0 is group0 , 1 is group1, 2 is group2
 *  @param pwm_num : PWM number, 0 is pwm0 , 1 is pwm1, 2 is pwm2, 3 is pwm3
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means pwm_num is invalid.
 */
int mtk_os_hal_pwm_stop_normal(pwm_groups group_num,
		pwm_channels pwm_num);

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

