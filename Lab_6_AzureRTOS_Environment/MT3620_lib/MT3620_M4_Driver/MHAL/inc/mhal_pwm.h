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

#ifndef __MHAL_PWM_H__
#define __MHAL_PWM_H__

#include "mhal_osai.h"

 /**
  * @addtogroup M-HAL
  * @{
  * @addtogroup PWM
  * @{
  * This section introduces the Pulse-Width Modulation (PWM) APIs,
  * including terms and acronyms,
  * supported features, software architecture, details on how to
  * use this driver, PWM  defines, enums, functions and structures.
  * @section MHAL_PWM_Terms_Chapter Terms and Acronyms
  *
  * |Terms	|Details				|
  * |------------|----------------------------|
  * |\b PWM	| Pulse-Width Modulation.|
  * @section MHAL_PWM_Features_Chapter Supported Features
  *
  * - \b Normal \b Mode \n
  *   The PWM generates the fixed waveform at the specified frequency
  *   and duty. The application can query current frequency and
  *   duty in current mode. The PWM does not support interrupt mode.
  *   \n
  * - \b 2-state \b Mode \n
  *   There are two configurable states (S0 and S1) for PWM. User can
  *   set individual blink behaviors for these two	states. User can also
  *   specify S0_stay_cycle to configure the stay cycles of S0.
  *
  * - \b 2-state \b Replay \b Mode \n
  *   User can set replay mode with specified S0_stay_cycle and
  *   S1_stay_cycle. PWM would act as [S0->S1->S0->S1->S0... ].
  *
  * - \b Differential \b Mode \n
  *   One PWM channel can be selected as the base channel. Other PWM
  *   channels can be selected as auxiliary channels in the same phase or
  *   reverse phase as the base channel.
  *
  * @}
  * @}
 */
/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup PWM
 * @{
 * @section MHAL_PWM_Driver_Usage_Chapter  How to Use This Driver
 *
 * - \b SW \b Architecture: \n
 *
 *	See @ref MHAL_Overview_2_Chapter for details on the SW architecture.
 *	This section describes the definition of APIs and provides
 *	an example on FreeRTOS about how to use these APIs to develop
 *	an OS-related driver.\n
 *
 * - \b The \b OS-HAL \b freeRTos \b driver\n
 *    \b sample \b code \b is \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_pwm.c"> freeRTos PWM sample code on github </a>
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup PWM
 * @{
 */

/** @defgroup driver_pwm_define Define
* @{
* This section introduces the Macro definition
* which is used as PWM M-HAL's API error return type.
*/

#define PWM_EPTR		1
/**< Invalid argument, it means the pointer is NULL */
#define PWM_EPARAMETER	2
/**< Invalid argument, it means the parameter invalid */
#define PWM_ENOMEM		3
/**< Out of memory, it means memory malloc fail */
#define PWM_EFAULT		4
/**< Bad address */
#define PWM_EAGAIN		5
/**< Try again */
#define PWM_ETIMEOUT	6
/**< Time out */
#define PWM_ECLK	7
/**< Clock select error */

#define MAX_GROUP_NUM 3
/**< Max pwm group number */

/**
  * @}
  */

/** @defgroup driver_pwm_enum Enum
  * @{
  *	M-HAL PWM enumeration value list information, including
  *	define of the PWM channel number enumeration and
  *	define of the PWM running status enumeration.
  */

/** @brief Defines the PWM channel bit map */
typedef enum {
	PWM_BIT0  = 0,
	/**< PWM channel0 bit map*/
	PWM_BIT1  = 1,
	/**< PWM channel1 bit map*/
	PWM_BIT2  = 2,
	/**< PWM channel2 bit map*/
	PWM_BIT3  = 3,
	/**< PWM channel3 bit map*/
	PWM_BITMAX
	/**< PWM max channel <invalid>*/
}	pwm_channels_bit_map;

/** @brief Defines the PWM running status */
typedef enum {
	PWM_IDLE = 0,
	/**< PWM status is idle*/
	PWM_BUSY = 1
	/**< PWM status is busy*/
}	pwm_running_status;

/** @brief Defines the PWM channel's differential mode  */
typedef enum  {
	PWM_DP0 = 0,
	/**< PWM differential mode  select PWM channel0 */
	PWM_DP1,
	/**< PWM differential mode  select PWM channel1 */
	PWM_DP2,
	/**< PWM differential mode  select PWM channel2 */
	PWM_DP3,
	/**< PWM differential mode  select PWM channel3 */
	PWM_DP_NUM
	/**< PWM differential mode  select max <invalid>*/
} pwm_differential_select;

/** @brief Defines the PWM channel number */
typedef enum {
	PWM_CHANNEL0  = 0,
	/**< PWM channel0*/
	PWM_CHANNEL1  = 1,
	/**< PWM channel1*/
	PWM_CHANNEL2  = 2,
	/**< PWM channel2*/
	PWM_CHANNEL3  = 3,
	/**< PWM channel3*/
	PWM_CHANNEL_MAX
	/**< PWM max channel <invalid>*/
}	pwm_channels;

typedef enum {
	PWM_CLOCK_32K = 0,
	/**< 32K clock source */
	PWM_CLOCK_2M,
	/**< 2M clock source */
	PWM_CLOCK_XTAL,
	/**< xtal clock source */
	PWM_CLOCK_NUM
	/**< max clock source <invalid>*/
} pwm_clks;

typedef enum{
	PWM_STAGE_S0 = 0,
	/**< S0 stage*/
	PWM_STAGE_S1,
	/**< S1 stage */
	PWM_STAGE_MAX
	/**< max pwm 2-state stage <invalid>*/
} pwm_s0_s1_stages;

/**
  * @}
  */

/** @defgroup driver_pwm_struct Struct
  * @{
  *	M-HAL PWM structure list information, including PWM controller
  *	structure, which used to store PWM controller resource.
  */

/** @brief Used to store the PWM base address, group number, clock source,
  *	pwm channel common structure information.
 */
struct mtk_com_pwm_data {
	/** PWM controller register offset by channel */
	const void __iomem *pwm_register;
	/** PWM controller's channel number */
	u32 pwm_nums;
	/** PWM controller's channel index */
	u32 index;
	/** Defines the PWM channel's differential mode */
	pwm_differential_select  mode;
	/** The stay cycles of S0 */
	u32  s0_stay_cycle;
	/** The stay cycles of S1 */
	u32  s1_stay_cycle;
	/** Enable replay mode or one shot mode */
	u8  replay_mode;
	/** Select PWM S0 or S1 stage to configure */
	pwm_s0_s1_stages stage;
	/** PWM duty cycle,typical value(0~100) */
	u32  duty_cycle;
	/** PWM output frequency */
	u32  frequency;
	/** PWM total count calculator according to PWM frequency and clock */
	u32  total_count;
	/** PWM hardware current's output duty cycle,typical value(0~100) */
	u32  current_duty_cycle;
	/** PWM hardware current's output frequency */
	u32  current_frequency;
	/** PWM group global kick function */
	u8  global_kick_enable;
	/** PWM IO ctrl function setting */
	u8  io_ctrl_sel;
	/** PWM polarity setting */
	u8  polarity_set;
	/** PWM in busy or idle status */
	pwm_running_status running_status;
};

/** @brief Interface to PWM, used to store the hardware register base address,
 * group information and clock.
 */
struct mtk_pwm_controller {
	/** PWM controller base address */
	void __iomem *base;
	/** PWM controller group number */
	char group_number;
	/** Clock source of this PWM controller */
	pwm_clks group_clock;
	/** M-HAL pwm channel common structure information */
	struct mtk_com_pwm_data *data;
};
/**
  * @}
  */

/** @defgroup driver_pwm_function Function
  * @{
  *	M-HAL PWM API list information, including
  *	PWM controller allocating and releasing,
  *	PWM hardware initializing and de-initializing,
  *	setting PWM  frequency and duty cycle,
  *	enabling and disabling PWM hardware output,
  *	getting PWM  frequency and duty cycle,
  *	getting the current status of PWM,
  *	enabling and disabling clock.
  *
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function is used to select PWM clock source of
 *	the PWM controller
 * @brief Usage: It must be called before operating hardware;
 *	Otherwise, it will use dPWM_EFAULT clock source.
 * @param [in] ctlr : Abstract a PWM controller.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.
 */
int mtk_mhal_pwm_clock_select(struct mtk_pwm_controller *ctlr);

/**
 * @brief This function is used to enable PWM clock before operating hardware.
 * @brief Usage: It must be called to enable clock before operating hardware;
 *	otherwise, the access to PWM register will fail, so this function should
 *	be called in the sequence: alloc_controller-> enable_clk-> pwm_init.
 * @param [in] ctlr :  Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.
 */
int mtk_mhal_pwm_enable_clk(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
  * @brief This function is used to disable PWM clock.
  * @param [in] ctlr :  Abstract a PWM controller.
  * @param [in] channel_num : PWM channel number(0~3).\n
  *  For more details about the parameter,
  *  please refer to #pwm_channels.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#PWM_EPTR , it means ctlr is NULL.
 */
int mtk_mhal_pwm_disable_clk(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
 * @brief This function is used to initialize the PWM hardware.
 * @brief Usage: OS-HAL calls this API to initialize PWM hardware,
 *	and the main sequence is as bellow:\n
 *	1.This function should be called after allocating the PWM controller.\n
 *	2.Before operating PWM hardware, this API should be called first.
 * @param [in] ctlr : Abstract a PWM controller.
 * @param [in] channel_bit_map : PWM channel bit map(0x0~0xF).\n
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means channel_num is invalid.
 */

int mtk_mhal_pwm_init(struct mtk_pwm_controller *ctlr,
	u32 channel_bit_map);

/**
 * @brief This function is used to de-initialize PWM hardware.
 * @brief Usage: OS-HAL calls this API to de-initialize PWM hardware,
 *	and this function should be called before releasing the PWM controller.
 * @param [in] ctlr : Abstract a PWM controller.
 * @param [in] channel_bit_map : PWM channel bit map(0x0~0xF).\n
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means channel_num is invalid.
 */

int mtk_mhal_pwm_deinit(struct mtk_pwm_controller *ctlr,
		u32 channel_bit_map);

/**
 * @brief This function is used to set PWM frequency.
 * @brief Usage: OS-HAL calls this API to set PWM hardware
 *	at the specified frequency, and this function should be called
 *	before starting PWM hardware.
 * @param [in] ctlr : Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means parameter invalid.
*/

int mtk_mhal_pwm_set_frequency(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
 * @brief This function is used to set PWM  duty cycle.
 * @brief Usage: OS-HAL calls this API to set PWM hardware
 *	at the specified duty cycle, and this function should be called
 *	before PWM starting.
 * @param [in] ctlr :  Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means parameter invalid.
 */

int mtk_mhal_pwm_set_duty_cycle(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
 * @brief This function is used to start the PWM hardware.
 * @brief Usage: OS-HAL calls this API to start PWM hardware output
 *	waveform at the specified frequency and duty cycle,
 *	and the main sequence is as bellow:\n
 *	1.This function should be called after setting the frequency.\n
 *	2.This function should be called after setting the duty cycle.
 * @param [in] ctlr :  Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means channel_num is invalid.
 */

int mtk_mhal_pwm_start(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
 * @brief This function is used to stop the PWM hardware.
 * @brief Usage: The main sequence is as bellow:\n
 *	1.This function should be called after setting the frequency.\n
 *	2.This function should be called after setting the duty cycle.\n
 *	3.This function should be called before de-initializing.
 * @param [in] ctlr : Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctlr is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means channel_num is invalid.
 */

int mtk_mhal_pwm_stop(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
 * @brief This function is used to get current frequency of the PWM
 *	with the unit of frequency being Hz.
 * @brief Usage: The duty cycle is restored in ctlr->data.current_frequency.
 * @param [in] ctlr :  Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means parameter is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means parameter is invalid.
 */

int mtk_mhal_pwm_get_frequency(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
 * @brief This function is used to get the current duty cycle of the PWM.
 * @brief Usage: The duty cycle is restored in ctlr->data.current_duty_cycle.
 * @param [in] ctlr :  Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means parameter is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means parameter is invalid.
 */

int mtk_mhal_pwm_get_duty_cycle(struct mtk_pwm_controller *ctlr,
	pwm_channels channel_num);

/**
 * @brief This function is used to get the current status of PWM.
 * @brief Usage: The running status is restored in ctlr->data.running_status.
 * @param [in] ctlr :  Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means parameter is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means parameter is invalid.
 */

int mtk_mhal_pwm_get_running_status(struct mtk_pwm_controller *ctlr,
	pwm_channels channel_num);

/**
 * @brief This function is used to select PWM global kick, IO ctrl,
 *	polarity setting.
 * @brief Usage: It must be called to set global kick, IO ctrl,
 *	polarity in 2-state mode and this function should be called before\n
 *	kicking PWM hardware output waveform.
 * @param [in] ctlr : Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctrl is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means parameter is invalid.
 */

int mtk_mhal_pwm_feature_enable(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
 * @brief This function is used to set PWM  frequency and duty cycle
 *	in 2-state mode.
 * @brief Usage: OS-HAL calls this API to set PWM hardware  frequency
 *	and duty cycle and the main sequence is as bellow:\n
 *	1.This function should be called before kicking PWM hardware output
 *	waveform.\n
 *	2.This function can configure frequency and duty cycle of stage
 *	S0 and S1.
 * @param [in] ctlr : Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctrl is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means parameter is invalid.
 */

int mtk_mhal_pwm_config_s0_s1_freq_duty(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
 * @brief This function is used to set PWM  stay cycles of S0 and S1
 *	in 2-state mode.
 * @brief Usage: OS-HAL calls this API to set PWM stay cycles of S0 and S1.\n
 *	1.This function should be called before kicking PWM hardware
 *	output waveform.\n
 *	2.This function can configure stay cycles of stage S0 and S1.\n
 *	3.This function can configure enable replay mode or not.
 * @param [in] ctlr : Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctrl is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means parameter is invalid.
 */
int mtk_mhal_pwm_s0_s1_stay_cycle_config(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

/**
 * @brief This function is used to kick PWM  hardware output waveform
 *	in 2-state mode.
 * @brief Usage: OS-HAL calls this API to kick PWM hardware
 *	output waveform by channel.
 * @param [in] ctlr : Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctrl is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means channel_num is invalid.
 */
int mtk_mhal_pwm_kick(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);
/**
 * @brief This function is used to reset PWM hardware by group.
 * @brief Usage: OS-HAL calls this API to reset PWM hardware by group.
 * @param [in] ctlr : Abstract a PWM controller.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctrl is NULL.
 */
int mtk_mhal_pwm_global_kick(struct mtk_pwm_controller *ctlr);
/**
 * @brief This function is used to set PWM channel's differential mode.
 * @brief Usage: After setting PWM configuration, It should be called to set
 *	PWM channel's differential mode.
 * @param [in] ctlr : Abstract a PWM controller.
 * @param [in] channel_num : PWM channel number(0~3).\n
 *	For more details about the parameter,
 *	please refer to #pwm_channels.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#PWM_EPTR , it means ctrl is NULL.\n
 *	If return value is -#PWM_EPARAMETER , it means parameter is invalid.
 */
int mtk_mhal_pwm_dpsel(struct mtk_pwm_controller *ctlr,
		pwm_channels channel_num);

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

/**
  * @}
  * @}
  */
#endif /* __MHAL_PWM_H__ */


