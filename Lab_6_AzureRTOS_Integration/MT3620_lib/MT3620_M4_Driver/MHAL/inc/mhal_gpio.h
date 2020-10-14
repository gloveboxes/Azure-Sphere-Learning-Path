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

#ifndef __MHAL_GPIO_H__
#define __MHAL_GPIO_H__
#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup GPIO
 * @{
 * This section introduces the General-Purpose Input-Output
 * (GPIO) APIs including terms and acronyms, supported features,
 * software architecture details on how to use this driver,
 * GPIO groups, enums and functions.
 *
 * @section MHAL_GPIO_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b GPIO                        | General-Purpose Inputs-Outputs.|
 *
 * @section MHAL_GPIO_Features_Chapter Supported Features
 * The GPIO has been designed to set GPIO input/output signal
 * and pinmux function. For more details,
 * please refer to the GPIO datasheet.
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup GPIO
 * @{
 * - Support pull-up and pull-down with different resistance functions. \n
 * - Support output-high and output-low functions. \n
 * - Support pinmux mode functions. \n
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup GPIO
 * @{
 * @section MHAL_GPIO_Driver_Usage_Chapter How to Use This Driver
 *
 * - \b SW \b Architecture: \n
 * See @ref MHAL_Overview_2_Chapter for the details of SW architecture.
 *
 *	This section describes the definition of APIs and provides
 *	an example on freeRTos to show how to use these APIs to develop
 *	an OS-related GPIO driver.\n
 *
 * - \b The \b OS-HAL \b freeRTos \b driver \b sample \b code \b is
 *    \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_gpio.c"> freeRTos GPIO sample code on github </a>
 *
 * @}
 * @}
 */

/**
* @addtogroup M-HAL
* @{
* @addtogroup GPIO
* @{
*/

/** @defgroup driver_GPIO_define Define
  * @{
  * This section introduces the Macro definition
  * which is used as GPIO M-HAL's API error return type.
  */

/** Indicate that a wrong parameter is given */
#define EINVAL	1
/** Indicate that a wrong pin number is given */
#define EPIN		2
/** Indicate that a wrong function is given. */
#define EINVALFUNC		3
/** Indicate that a pin is freed fail. */
#define EFREE		4
/** Indicate that a pin is requested fail. */
#define EQUEST		5
/** Indicate that the GPIO API function execution fails */
#define ERROR		6

/**
  * @}
  */

/** @defgroup driver_gpio_enum Enum
  * @{
  * This section introduces the enumerations
  * that GPIO should configure when calling GPIO APIs.
  */

/**@brief This enum defines the GPIO port.\n
 */

typedef enum {
	/** GPIO pin number is 0 */
	MHAL_GPIO_0 = 0,
	MHAL_GPIO_1 = 1,
	MHAL_GPIO_2 = 2,
	MHAL_GPIO_3 = 3,
	MHAL_GPIO_4 = 4,
	MHAL_GPIO_5 = 5,
	MHAL_GPIO_6 = 6,
	MHAL_GPIO_7 = 7,
	MHAL_GPIO_8 = 8,
	MHAL_GPIO_9 = 9,
	MHAL_GPIO_10 = 10,
	MHAL_GPIO_11 = 11,
	MHAL_GPIO_12 = 12,
	MHAL_GPIO_13 = 13,
	MHAL_GPIO_14 = 14,
	MHAL_GPIO_15 = 15,
	MHAL_GPIO_16 = 16,
	MHAL_GPIO_17 = 17,
	MHAL_GPIO_18 = 18,
	MHAL_GPIO_19 = 19,
	MHAL_GPIO_20 = 20,
	MHAL_GPIO_21 = 21,
	MHAL_GPIO_22 = 22,
	MHAL_GPIO_23 = 23,
	MHAL_GPIO_24 = 24,
	MHAL_GPIO_25 = 25,
	MHAL_GPIO_26 = 26,
	MHAL_GPIO_27 = 27,
	MHAL_GPIO_28 = 28,
	MHAL_GPIO_29 = 29,
	MHAL_GPIO_30 = 30,
	MHAL_GPIO_31 = 31,
	MHAL_GPIO_32 = 32,
	MHAL_GPIO_33 = 33,
	MHAL_GPIO_34 = 34,
	MHAL_GPIO_35 = 35,
	MHAL_GPIO_36 = 36,
	MHAL_GPIO_37 = 37,
	MHAL_GPIO_38 = 38,
	MHAL_GPIO_39 = 39,
	MHAL_GPIO_40 = 40,
	MHAL_GPIO_41 = 41,
	MHAL_GPIO_42 = 42,
	MHAL_GPIO_43 = 43,
	MHAL_GPIO_44 = 44,
	MHAL_GPIO_45 = 45,
	MHAL_GPIO_46 = 46,
	MHAL_GPIO_47 = 47,
	MHAL_GPIO_48 = 48,
	MHAL_GPIO_49 = 49,
	MHAL_GPIO_50 = 50,
	MHAL_GPIO_51 = 51,
	MHAL_GPIO_52 = 52,
	MHAL_GPIO_53 = 53,
	MHAL_GPIO_54 = 54,
	MHAL_GPIO_55 = 55,
	MHAL_GPIO_56 = 56,
	MHAL_GPIO_57 = 57,
	MHAL_GPIO_58 = 58,
	MHAL_GPIO_59 = 59,
	MHAL_GPIO_60 = 60,
	MHAL_GPIO_61 = 61,
	MHAL_GPIO_62 = 62,
	MHAL_GPIO_63 = 63,
	MHAL_GPIO_64 = 64,
	MHAL_GPIO_65 = 65,
	MHAL_GPIO_66 = 66,
	MHAL_GPIO_67 = 67,
	MHAL_GPIO_68 = 68,
	MHAL_GPIO_69 = 69,
	MHAL_GPIO_70 = 70,
	MHAL_GPIO_71 = 71,
	MHAL_GPIO_72 = 72,
	MHAL_GPIO_73 = 73,
	MHAL_GPIO_74 = 74,
	MHAL_GPIO_75 = 75,
	MHAL_GPIO_76 = 76,
	MHAL_GPIO_77 = 77,
	MHAL_GPIO_78 = 78,
	MHAL_GPIO_79 = 79,
	MHAL_GPIO_80 = 80,
	MHAL_GPIO_81 = 81,
	MHAL_GPIO_82 = 82,
	MHAL_GPIO_83 = 83,
	MHAL_GPIO_84 = 84,
	MHAL_GPIO_85 = 85,
	MHAL_GPIO_86 = 86,
	MHAL_GPIO_87 = 87,
	MHAL_GPIO_88 = 88,
	MHAL_GPIO_89 = 89,
	MHAL_GPIO_90 = 90,
	MHAL_GPIO_91 = 91,
	MHAL_GPIO_92 = 92,
	MHAL_GPIO_93 = 93,
	/** GPIO pin maximum number(invalid) */
	MHAL_GPIO_MAX
} mhal_gpio_pin;

typedef enum {
	/** GPIO mode number is from 0 to 7 */
	MHAL_GPIO_MODE_0 = 0,
	MHAL_GPIO_MODE_1 = 1,
	MHAL_GPIO_MODE_2 = 2,
	MHAL_GPIO_MODE_3 = 3,
	MHAL_GPIO_MODE_4 = 4,
	MHAL_GPIO_MODE_5 = 5,
	MHAL_GPIO_MODE_6 = 6,
	MHAL_GPIO_MODE_7 = 7,
	/** GPIO mode maximum number(invalid) */
	MHAL_GPIO_MODE_MAX
} mhal_gpio_mode;

/** @brief This enum defines GPIO direction.\n
 */
typedef enum {
	/** Define GPIO input direction */
	MHAL_GPIO_DIRECTION_INPUT  = 0,
	/** Define GPIO output direction */
	MHAL_GPIO_DIRECTION_OUTPUT = 1
} mhal_gpio_direction;


/** @brief This enum defines input or output data of GPIO.\n
 */
typedef enum {
	/** Define GPIO data of low */
	MHAL_GPIO_DATA_LOW  = 0,
	/** Define GPIO data of high */
	MHAL_GPIO_DATA_HIGH = 1
} mhal_gpio_data;

/** @brief This enum defines GPIO register base number.\n
 */
typedef enum {
	/** Define GPIO cm4 gpio pwm group0 base register */
	MHAL_CM4_GPIO_PWM_GRP0_BASE = 0,
	/** Define GPIO cm4 gpio pwm group1 base register */
	MHAL_CM4_GPIO_PWM_GRP1_BASE = 1,
	/** Define GPIO cm4 gpio pwm group2 base register */
	MHAL_CM4_GPIO_PWM_GRP2_BASE = 2,
	/** Define GPIO cm4 gpio pwm group3 base register */
	MHAL_CM4_GPIO_PWM_GRP3_BASE = 3,
	/** Define GPIO cm4 gpio pwm group4 base register */
	MHAL_CM4_GPIO_PWM_GRP4_BASE = 4,
	/** Define GPIO cm4 gpio pwm group5 base register */
	MHAL_CM4_GPIO_PWM_GRP5_BASE = 5,
	/** Define GPIO cm4 isu0 i2c base register */
	MHAL_CM4_ISU0_I2C_BASE = 6,
	/** Define GPIO cm4 isu1 i2c base register */
	MHAL_CM4_ISU1_I2C_BASE = 7,
	/** Define GPIO cm4 isu2 i2c base register */
	MHAL_CM4_ISU2_I2C_BASE = 8,
	/** Define GPIO cm4 isu3 i2c base register */
	MHAL_CM4_ISU3_I2C_BASE = 9,
	/** Define GPIO cm4 isu4 i2c base register */
	MHAL_CM4_ISU4_I2C_BASE = 10,
	/** Define GPIO adc base register */
	MHAL_CM4_ADC_BASE = 11,
	/** Define GPIO ca7 base register */
	MHAL_CA7_GPIO_BASE = 12,
	/** Define GPIO i2s0 base register */
	MHAL_CM4_I2S0_BASE = 13,
	/** Define GPIO i2s1 base register */
	MHAL_CM4_I2S1_BASE = 14,
	/** Define GPIO pinmux base register */
	MHAL_PINMUX_BASE = 15,
	/** GPIO register maximum number(invalid) */
	MHAL_GPIO_REG_BASE_MAX
} mhal_gpio_reg_base;


/**
 * @}
 */

/** @defgroup driver_gpio_struct Struct
  * @{
  * This section introduces the structure used by
  * GPIO OS-HAL/M-HAL/HDL.
  */

/** @ingroup IP_group_gpio_internal_struct
 * @brief This structure defines GPIO pin related property.
 */
struct mtk_pinctrl_desc_pin {
/** GPIO can be requested or freed.\n
 * True : GPIO is requested, and the others can not use it again.\n
 * False : GPIO is freed, and the others can use it again.\n
 */
	bool pinctrl_free;
	/** get gpio din value(0:input low;1:input high) */
};

/** @brief This structure defines the GPIO interface controller*/
struct mtk_pinctrl_controller {
	/** GPIO controller base address */
	void __iomem *base[MHAL_GPIO_REG_BASE_MAX];
	/** The number of gpio mode in per reg. */
	unsigned int  gpio_mode_bits;
	/** The max gpio mode in per reg. */
	unsigned int  max_gpio_mode_per_reg;
	/** The shift between two registers. */
	unsigned char  port_shf;
	/** The mask of register. */
	unsigned char  port_mask;
	/** The pinmux register offset. */
	unsigned int pinmux_offset;
};

/**
 * @}
 */

/** @defgroup driver_gpio_function Function
  * @{
  * This section provides Fixed APIs(defined as Common Interface)
  * to fully control the MediaTek GPIO HW.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function is used to get input data of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO get-input value function.
 * @param [in] pctl : GPIO Pinctrl controller used with the device.
 * @param[in] pin : Specifies the pin number to operate.(0~93)
 * @param[out] *pvalue : 0 means input low, 1 means input high.
 * @return To indicate that whether this function call is successful or not.\n
 *    If the return value is -#EINVAL,
 *    it means that a wrong parameter(except for pin number) is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#EPIN,
 *    it means that a wrong pin number is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#ERROR, it means failure.\n
 *    If the return value is 0, it means success.\n
 */
int mtk_mhal_gpio_get_input(struct mtk_pinctrl_controller *pctl,
	u32 pin, mhal_gpio_data *pvalue);

/**
 * @brief     This function is used to set output data of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO set-output value function.
 * @param[in] pctl : GPIO Pinctrl controller used with the device.
 * @param[in] pin : Specifies the pin number to operate.(0~93)
 * @param[in] out_val : Represents output data of the target GPIO.\n
 *    0, output low.\n
 *    1, output high.\n
 * @return To indicate that whether this function call is successful or not.\n
 *    If the return value is -#EINVAL,
 *    it means that a wrong parameter(except for pin number) is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#EPIN,
 *    it means that a wrong pin number is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#ERROR, it means failure.\n
 *    If the return value is 0, it means success.\n
 */
int mtk_mhal_gpio_set_output(struct mtk_pinctrl_controller *pctl,
	u32 pin, u32 out_val);

/**
 * @brief This function is used to get output data of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO get-output value function.
 * @param[in] pctl : GPIO Pinctrl controller used with the device.
 * @param[in] pin : Specifies the pin number to operate.(0~93)
 * @param[out] *pvalue : 0 means output low, 1 means output high.
 * @return To indicate that whether this function call is successful or not.\n
 *    If the return value is -#EINVAL,
 *    it means that a wrong parameter(except for pin number) is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#EPIN,
 *    it means that a wrong pin number is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#ERROR, it means failure.\n
 *    If the return value is 0, it means success.\n
 */
int mtk_mhal_gpio_get_output(struct mtk_pinctrl_controller *pctl,
	u32 pin, mhal_gpio_data *pvalue);

/**
 * @brief This function is used to set the direction of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO set-direction function.
 * @param[in] pctl : GPIO Pinctrl controller used with the device.
 * @param[in] pin : Specifies the pin number to set.(0~93)
 * @param[in] dir : Specified the direction of target GPIO,\n
 *    0, input.\n
 *    1, output.\n
 * @return To indicate that whether this function call is successful or not.\n
 *    If the return value is -#EINVAL,
 *    it means that a wrong parameter(except for pin number) is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#EPIN,
 *    it means that a wrong pin number is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#ERROR, it means failure.\n
 *    If the return value is 0, it means success.\n
 */
int mtk_mhal_gpio_set_direction(
	struct mtk_pinctrl_controller *pctl, u32 pin, u32 dir);

/**
 * @brief This function is used to get the direction of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO get-direction function.
 * @param[in] pctl : GPIO Pinctrl controller used with the device.
 * @param[in] pin : Specifies the pin number to operate.(0~93)
 * @param[out] *pvalue : 0 means input, 1 means output.
 * @return To indicate that whether this function call is successful or not.\n
 *    If the return value is -#EINVAL,
 *    it means that a wrong parameter(except for pin number) is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#EPIN,
 *    it means that a wrong pin number is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#ERROR, it means failure.\n
 *    If the return value is 0, it means success.\n
 */
int mtk_mhal_gpio_get_direction(struct mtk_pinctrl_controller *pctl,
	u32 pin, mhal_gpio_direction *pvalue);

/**
 * @brief This function is used to set the target GPIO to
 *    pull-up/pull-down state.
 * @brief Usage: OS-HAL driver should call it when setting GPIO
 *    pull-up/pull-down state.
 * @param[in] pctl : GPIO Pinctrl controller used with the device.
 * @param[in] pin : Specifies the pin number to set.(0~93)
 * @param[in] enable : Specifies the pin pullen enable or disable.
 *    True means pullen enable; false means pullen disable.
 * @param[in] isup : Specifies the pin pullsel up or down.
 *    True means pullsel up; false means pullsel down.
 * @return To indicate that whether this function call is successful or not.\n
 *    If the return value is -#EINVAL,
 *    it means that a wrong parameter(except for pin number) is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#EPIN,
 *    it means that a wrong pin number is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#ERROR, it means failure.\n
 *    If the return value is 0, it means success.\n
 */
int mtk_mhal_gpio_set_pullen_pullsel(
	struct mtk_pinctrl_controller *pctl, u32 pin, bool enable, bool isup);

/**
 * @brief This function is used to configure pinmux of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO pinmux function to
 *    set multifunction mode.
 *    Pinmux represents Pin Multiplexor which connects the pin
 *    and the onboard peripherals, so the pin will operate in a specific mode
 *    once this pin is programmed to a peripheral's function.
 * @param[in] pctl : GPIO Pinctrl controller used with the device.
 * @param[in] pin : Specifies the pin number to configure.(0~93)
 * @param[in] mode : Specifies the function for the pin.(0~7)
 * @return To indicate that whether this function call is successful or not.\n
 *    If the return value is -#EPIN,
 *    it means that a wrong parameter(except for pin number) is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#EINVALFUNC,
 *    it means that a wrong pinmux function is given,
 *    and the parameter must be verified.\n
 *    If the return value is -#ERROR, it means failure.\n
 *    If the return value is 0, it means success.\n
 */
int mtk_mhal_gpio_pmx_set_mode(
	struct mtk_pinctrl_controller *pctl, u32 pin, u32 mode);

 /**
  * @brief This function is used to get the configuration pinmux of
  *    the target GPIO.
  * @brief Usage: OS-HAL driver should call it in GPIO pinmux function to
  *    get multifunction mode. Pinmux represents Pin Multiplexor which
  *    connects the pin and the onboard peripherals,
  *    so the pin will operate in a specific mode
  *    once this pin is programmed to a peripheral's function.
  * @param[in] pctl : GPIO Pinctrl controller used with the device.
  * @param[in] pin : Specifies pin number to configure.(0~93)
  * @param[out] *pvalue : Specifies multifunction value.(0~7)
  * @return To indicate that whether this function call is successful or not.\n
  *    If the return value is -#EINVAL,
  *    it means that a wrong parameter(except for pin number) is given,
  *    and the parameter must be verified.\n
  *    If the return value is -#EPIN,
  *    it means that a wrong pin number is given,
  *    and the parameter must be verified.\n
  *    If the return value is -#ERROR, it means failure.\n
  *    If the return value is 0, it means success.\n
  */
int mtk_mhal_gpio_pmx_get_mode(struct mtk_pinctrl_controller *pctl,
	u32 pin, u32 *pvalue);

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
