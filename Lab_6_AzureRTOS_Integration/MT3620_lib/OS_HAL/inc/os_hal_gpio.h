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

 #ifndef __OS_HAL_GPIO_H__
#define __OS_HAL_GPIO_H__

#include "mhal_gpio.h"

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup gpio
 * @{
 * This section introduces the Inter-Integrated Circuit (GPIO) APIs
 * including terms and acronyms, supported features,
 * details on how to use this driver, enums and functions.
 *
 * @section OS_HAL_GPIO_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b GPIO                        | General-Purpose Inputs-Outputs.|
 *
 * @section OS_HAL_GPIO_Features_Chapter Supported Features
 * See @ref MHAL_GPIO_Features_Chapter for the details of  Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup gpio
 * @{
 * @section OS_HAL_GPIO_Driver_Usage_Chapter How to use this driver
 *
 * - \b How \b to \b develop \b user \b application \b by \b using
 *    \b OS-HAL \b API: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *    @code
 *
 *    - Set GPIO output high/low:
 *      -Call mtk_os_hal_gpio_request(pin) to get gpio request resource.
 *      -Call mtk_os_hal_gpio_set_direction(pin, OS_HAL_GPIO_DIR_OUTPUT)
 *        to set gpio direction as output mode.
 *      -Call mtk_os_hal_gpio_set_output(pin, OS_HAL_GPIO_DATA_HIGH)
 *        to set gpio output high.
 *      -Call mtk_os_hal_gpio_set_output(pin, OS_HAL_GPIO_DATA_LOW)
 *        to set gpio output low.
 *      -Call mtk_os_hal_gpio_free(pin) to free gpio request resource.
 *
 *    - Set GPIO input mode:
 *      -Call mtk_os_hal_gpio_request(pin) to get gpio request resource.
 *      -Call mtk_os_hal_gpio_set_direction(pin, OS_HAL_GPIO_DIR_INPUT)
 *        to set gpio direction as input mode.
 *      -Call mtk_os_hal_gpio_set_pullen_pullsel(pin, false, false)
 *        to disable gpio pull-up and pull-down state.
 *      -Call mtk_os_hal_gpio_set_pullen_pullsel(pin, ture, false)
 *        to set gpio as pull-down state.
 *      -Call mtk_os_hal_gpio_set_pullen_pullsel(pin, ture, ture)
 *        to set gpio as pull-up state.
 *      -Call mtk_os_hal_gpio_get_input(pin, pvalue) to get gpio input value.
 *      -Call mtk_os_hal_gpio_free(pin) to free gpio request resource.
 *
 *    @endcode
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup gpio
 * @{
 */

/** @defgroup os_hal_gpio_enum Enum
  * @{
  * This section introduces the enumerations
  * that GPIO should configure when calling GPIO APIs.
  */

/**@brief This enum defines the GPIO port.\n
 */

typedef enum {
	/** GPIO pin number is 0 */
	OS_HAL_GPIO_0 = 0,
	OS_HAL_GPIO_1 = 1,
	OS_HAL_GPIO_2 = 2,
	OS_HAL_GPIO_3 = 3,
	OS_HAL_GPIO_4 = 4,
	OS_HAL_GPIO_5 = 5,
	OS_HAL_GPIO_6 = 6,
	OS_HAL_GPIO_7 = 7,
	OS_HAL_GPIO_8 = 8,
	OS_HAL_GPIO_9 = 9,
	OS_HAL_GPIO_10 = 10,
	OS_HAL_GPIO_11 = 11,
	OS_HAL_GPIO_12 = 12,
	OS_HAL_GPIO_13 = 13,
	OS_HAL_GPIO_14 = 14,
	OS_HAL_GPIO_15 = 15,
	OS_HAL_GPIO_16 = 16,
	OS_HAL_GPIO_17 = 17,
	OS_HAL_GPIO_18 = 18,
	OS_HAL_GPIO_19 = 19,
	OS_HAL_GPIO_20 = 20,
	OS_HAL_GPIO_21 = 21,
	OS_HAL_GPIO_22 = 22,
	OS_HAL_GPIO_23 = 23,
	OS_HAL_GPIO_24 = 24,
	OS_HAL_GPIO_25 = 25,
	OS_HAL_GPIO_26 = 26,
	OS_HAL_GPIO_27 = 27,
	OS_HAL_GPIO_28 = 28,
	OS_HAL_GPIO_29 = 29,
	OS_HAL_GPIO_30 = 30,
	OS_HAL_GPIO_31 = 31,
	OS_HAL_GPIO_32 = 32,
	OS_HAL_GPIO_33 = 33,
	OS_HAL_GPIO_34 = 34,
	OS_HAL_GPIO_35 = 35,
	OS_HAL_GPIO_36 = 36,
	OS_HAL_GPIO_37 = 37,
	OS_HAL_GPIO_38 = 38,
	OS_HAL_GPIO_39 = 39,
	OS_HAL_GPIO_40 = 40,
	OS_HAL_GPIO_41 = 41,
	OS_HAL_GPIO_42 = 42,
	OS_HAL_GPIO_43 = 43,
	OS_HAL_GPIO_44 = 44,
	OS_HAL_GPIO_45 = 45,
	OS_HAL_GPIO_46 = 46,
	OS_HAL_GPIO_47 = 47,
	OS_HAL_GPIO_48 = 48,
	OS_HAL_GPIO_49 = 49,
	OS_HAL_GPIO_50 = 50,
	OS_HAL_GPIO_51 = 51,
	OS_HAL_GPIO_52 = 52,
	OS_HAL_GPIO_53 = 53,
	OS_HAL_GPIO_54 = 54,
	OS_HAL_GPIO_55 = 55,
	OS_HAL_GPIO_56 = 56,
	OS_HAL_GPIO_57 = 57,
	OS_HAL_GPIO_58 = 58,
	OS_HAL_GPIO_59 = 59,
	OS_HAL_GPIO_60 = 60,
	OS_HAL_GPIO_61 = 61,
	OS_HAL_GPIO_62 = 62,
	OS_HAL_GPIO_63 = 63,
	OS_HAL_GPIO_64 = 64,
	OS_HAL_GPIO_65 = 65,
	OS_HAL_GPIO_66 = 66,
	OS_HAL_GPIO_67 = 67,
	OS_HAL_GPIO_68 = 68,
	OS_HAL_GPIO_69 = 69,
	OS_HAL_GPIO_70 = 70,
	OS_HAL_GPIO_71 = 71,
	OS_HAL_GPIO_72 = 72,
	OS_HAL_GPIO_73 = 73,
	OS_HAL_GPIO_74 = 74,
	OS_HAL_GPIO_75 = 75,
	OS_HAL_GPIO_76 = 76,
	OS_HAL_GPIO_77 = 77,
	OS_HAL_GPIO_78 = 78,
	OS_HAL_GPIO_79 = 79,
	OS_HAL_GPIO_80 = 80,
	OS_HAL_GPIO_81 = 81,
	OS_HAL_GPIO_82 = 82,
	OS_HAL_GPIO_83 = 83,
	OS_HAL_GPIO_84 = 84,
	OS_HAL_GPIO_85 = 85,
	OS_HAL_GPIO_86 = 86,
	OS_HAL_GPIO_87 = 87,
	OS_HAL_GPIO_88 = 88,
	OS_HAL_GPIO_89 = 89,
	OS_HAL_GPIO_90 = 90,
	OS_HAL_GPIO_91 = 91,
	OS_HAL_GPIO_92 = 92,
	OS_HAL_GPIO_93 = 93,
	/** GPIO pin maximum number(invalid) */
	OS_HAL_GPIO_MAX
} os_hal_gpio_pin;

/** @brief This enum defines GPIO direction.\n
 */
typedef enum {
	/** Define GPIO input direction */
	OS_HAL_GPIO_DIR_INPUT  = MHAL_GPIO_DIRECTION_INPUT,
	/** Define GPIO output direction */
	OS_HAL_GPIO_DIR_OUTPUT = MHAL_GPIO_DIRECTION_OUTPUT
} os_hal_gpio_direction;


/** @brief This enum defines input or output data of GPIO.\n
 */
typedef enum {
	/** Define GPIO data of low */
	OS_HAL_GPIO_DATA_LOW  = MHAL_GPIO_DATA_LOW,
	/** Define GPIO data of high */
	OS_HAL_GPIO_DATA_HIGH = MHAL_GPIO_DATA_HIGH
} os_hal_gpio_data;

/**
  * @}
  */

/** @defgroup os_hal_gpio_function Function
  * @{
  * This section provides high level APIs to upper layer.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     This function is used to request the target GPIO.
 * @brief Usage: OS-HAL driver should call it before other GPIO function
 *    to request the GPIO. It is used to get the gpio resource.
 *    If we call it on the second time, the API function
 *    will return -#EQUEST to indicate request fail.
 * @param[in] pin : Specifies the pin number to operate.(0~93)
 * @return    To indicate that whether the pin is requested
 *    successfully or not.\n
 *    If the return value is -#EQUEST, it means GPIO fails to be requested.\n
 *    If the return value is 0, it means GPIO is requested successfully.\n
 */

int mtk_os_hal_gpio_request(os_hal_gpio_pin pin);

/**
 * @brief This function is used to free the target GPIO.
 * @brief Usage: OS-HAL driver should call it after other GPIO functions
 *    to free the GPIO. It is used to free the gpio resource.
 *    If we call it on the second time, the API function
 *    will return -#EFREE to indicate free fail.
 * @param[in] pin : Specifies the pin number to operate.(0~93)
 * @return  To indicate that whether the pin is freed successfully or not.\n
 *    If the return value is -#EFREE, it means that GPIO is not freed.\n
 *    If the return value is 0, it means that GPIO is freed successfully.\n
 */
int mtk_os_hal_gpio_free(os_hal_gpio_pin pin);

/**
 * @brief This function is used to get input data of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO get-input value function.
 *    This API function get the input value of pin and save it to
 *    mtk_pinctrl_controller->mtk_pins[pin].din
 * @param[in] pin : Specifies the pin number to operate.(0~93)
 * @param [out] pvalue : Get DIN value and save it to address pvalue.
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
int mtk_os_hal_gpio_get_input(os_hal_gpio_pin pin, os_hal_gpio_data *pvalue);

/**
 * @brief     This function is used to set output data of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO set-output value function.
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
int mtk_os_hal_gpio_set_output(os_hal_gpio_pin pin, os_hal_gpio_data out_val);

/**
 * @brief This function is used to get output data of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO get-output value function.
 *    This API function get the output value of pin and save it to
 *    mtk_pinctrl_controller->mtk_pins[pin].dout
 * @param[in] pin : Specifies the pin number to operate.(0~93)
 * @param [out] pvalue : Get DOUT value and save it to address pvalue
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
int mtk_os_hal_gpio_get_output(os_hal_gpio_pin pin, os_hal_gpio_data *pvalue);

/**
 * @brief This function is used to set the direction of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO set-direction function.
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
int mtk_os_hal_gpio_set_direction(os_hal_gpio_pin pin,
	os_hal_gpio_direction dir);

/**
 * @brief This function is used to get the direction of the target GPIO.
 * @brief Usage: OS-HAL driver should call it in GPIO get-direction function.
 *    This API function get the direction value of pin and save it to
 *    mtk_pinctrl_controller->mtk_pins[pin].dir
 * @param[in] pin : Specifies the pin number to operate.(0~93)
 * @param [out] pvalue : Get DIR value and save it to address pvalue
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
int mtk_os_hal_gpio_get_direction(os_hal_gpio_pin pin,
	os_hal_gpio_direction *pvalue);

/**
 * @brief This function is used to set the target GPIO to
 *    pull-up/pull-down state.
 * @brief Usage: OS-HAL driver should call it when setting GPIO
 *    pull-up/pull-down state.
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
int mtk_os_hal_gpio_set_pullen_pullsel(
	os_hal_gpio_pin pin, bool enable, bool isup);

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
