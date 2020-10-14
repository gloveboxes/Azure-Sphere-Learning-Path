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

#ifndef __OS_HAL_GPIOIF_H__
#define __OS_HAL_GPIOIF_H__

#include "mhal_osai.h"
#include "mhal_gpioif.h"

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup gpioif
 * @{
 * This section introduces the Inter-Integrated Circuit (GPIOIF) APIs
 * including terms and acronyms, supported features,
 * details on how to use this driver, enums and functions.
 *
 * @section OS_HAL_GPIOIF_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b GPIOIF  | General-Purpose Inputs-Outputs Interface.|
 * |\b FIFO                       | First In, First Out.|
 *
 * @section OS_HAL_GPIOIF_Features_Chapter Supported Features
 * See @ref MHAL_GPIOIF_Features_Chapter for the details of  Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup gpioif
 * @{
 * @section OS_HAL_GPIOIF_Driver_Usage_Chapter How to use this driver
 *
 * - \b How \b to \b develop \b user \b application \b by \b using
 *    \b OS-HAL \b API: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *	   @code
 *
 *  //1. Call this function to initialize GPIOIF HW
 *  mtk_os_hal_gpioif_ctlr_init(group);
 *    //2. Define user struct and interrupt callback function
 *  struct mtk_gpioif_test {
 *   u32 group;
 *  };
 *
 *  int mtk_os_hal_gpioif_int_callback(void *data)
 *  {
 *   struct mtk_gpioif_test *gpioif_test = data;
 *
 *   return 0;
 *  }
 *    - Set GPIOIF Event Counter Direction mode:
 *   -Call mtk_os_hal_gpioif_int_callback_register(3,
 *           mtk_os_hal_gpioif_int_callback, (void *)gpioif_test)
 *   to register callback function.
 *   -Call mtk_os_hal_gpioif_hardware_reset(group, mode, active_reset)
 *   to enable hardware reset of GPIOIF event counter.
 *   -Call mtk_os_hal_gpioif_de_glitch(group, gpio, enable, min_p, init_v)
 *   to add GPIOIF GPIO_0 & GPIO_1 & GPIO_2 of one group de-glitch setting.
 *    -Call mtk_os_hal_gpioif_limit_comparator(group, sa_limit_v,
 *      interrupt_limit_v)
 *    to set different interrupt mode or saturation mode.
 *    -Call mtk_os_hal_gpioif_set_direction_mode(group, control_setting,
 *    low_limit, high_limit, reset_value, clock_source)
 *    to set GPIOIF Direction mode setting.
 *    -Call mtk_os_hal_gpioif_interrupt_control(group, enable, clear, bit)
 *    to clear all interrupt then enable all interrupt.
 *   -Input high/low singal to GPIOIF GPIO_0 and GPIO_1,
 *    the interrupt callback function will be called
 *    swhen interrupt is asserted.
 *
 *    - Set GPIOIF Event Counter Up Down mode:
 *   -Call mtk_os_hal_gpioif_int_callback_register(3,
 *     mtk_os_hal_gpioif_int_callback, (void *)gpioif_test)
 *     to register callback function.
 *   -Call mtk_os_hal_gpioif_hardware_reset(group, mode, active_reset)
 *     to enable hardware reset of GPIOIF event counter.
 *   -Call mtk_os_hal_gpioif_de_glitch(group, gpio, enable, min_p, init_v)
 *     to add GPIOIF GPIO_0 & GPIO_1 & GPIO_2 of one group de-glitch setting.
 *    -Call mtk_os_hal_gpioif_limit_comparator(group, sa_limit_v,
 *     interrupt_limit_v)
 *     to set different interrupt mode or saturation mode.
 *    -Call mtk_os_hal_gpioif_set_updown_mode(group, control_setting,
 *     low_limit, high_limit, reset_value, clock_source)
 *     to set GPIOIF Up Down mode setting.
 *    -Call mtk_os_hal_gpioif_interrupt_control(group, enable, clear, bit)
 *     to clear all interrupt then enable all interrupt.
 *   -Input high/low singal to GPIOIF GPIO_0 and GPIO_1,
 *     the interrupt callback function will be called
 *     when interrupt is asserted.
 *
 *    - Set GPIOIF Event Counter Quadrature mode:
 *   -Call mtk_os_hal_gpioif_int_callback_register(3,
 *     mtk_os_hal_gpioif_int_callback, (void *)gpioif_test)
 *     to register callback function.
 *   -Call mtk_os_hal_gpioif_hardware_reset(group, mode, active_reset)
 *     to enable hardware reset of GPIOIF event counter.
 *   -Call mtk_os_hal_gpioif_de_glitch(group, gpio, enable, min_p, init_v)
 *     to add GPIOIF GPIO_0 & GPIO_1 & GPIO_2 of one group de-glitch setting.
 *    -Call mtk_os_hal_gpioif_limit_comparator(group, sa_limit_v,
 *     interrupt_limit_v)
 *     to set different interrupt mode or saturation mode.
 *    -Call mtk_os_hal_gpioif_set_quadrature_mode(group, control_setting,
 *      low_limit, high_limit, reset_value, clock_source)
 *      to set GPIOIF Quadrature mode setting.
 *    -Call mtk_os_hal_gpioif_interrupt_control(group, enable, clear, bit)
 *      to clear all interrupt then enable all interrupt.
 *   -Input high/low singal to GPIOIF GPIO_0 and GPIO_1,
 *      the interrupt callback function will be called
 *      when interrupt is asserted.
 *
 *    - Set GPIOIF Capture Counter Mode:
 *   -Call mtk_os_hal_gpioif_int_callback_register(3,
 *     mtk_os_hal_gpioif_int_callback, (void *)gpioif_test)
 *     to register callback function.
 *   -Call mtk_os_hal_gpioif_hardware_reset(group, mode, active_reset)
 *     to enable hardware reset of GPIOIF event counter.
 *   -Call mtk_os_hal_gpioif_de_glitch(group, gpio, enable, min_p, init_v)
 *     to add GPIOIF GPIO_0 & GPIO_1 & GPIO_2 of one group de-glitch setting.
 *    -Call mtk_os_hal_gpioif_set_capture_mode(group, edge_type_gpio_0,
 *      edge_type_gpio_1, clock_source)
 *      to set GPIOIF Capture Counter Mode setting.
 *    -Call mtk_os_hal_gpioif_set_quadrature_mode(group, control_setting,
 *      low_limit, high_limit, reset_value, clock_source)
 *      to set GPIOIF Quadrature mode setting.
 *    -Call mtk_os_hal_gpioif_interrupt_control(group, enable, clear, bit)
 *      to clear all interrupt then enable all interrupt.
 *   -Input high/low singal to GPIOIF GPIO_0 and GPIO_1,
 *     the interrupt callback function will be called
 *     when interrupt is asserted.
 *
 *    @endcode
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup gpioif
 * @{
 */

/** @defgroup os_hal_gpioif_define Define
  * @{
  * This section introduces the Macro definition.
  */

/** This defines GPIOIF maximum available group number */
#define MTK_GPIOIF_MAX_GRP_NUM	6
/**
  * @}
  */


/** @defgroup os_hal_gpioif_enum Enum
  * @{
  * This section introduces the enumerations
  * that GPIOIF should configure when calling GPIOIF APIs.
  */

/**
 * @brief This enum defines GPIOIF group selection.\n
 */
typedef enum {
	/** GPIOIF group0(gpio0~3) */
	GPIOIF_GROUP_0 = 0,
	/** GPIOIF group1(gpio4~7) */
	GPIOIF_GROUP_1 = 1,
	/** GPIOIF group2(gpio8~11) */
	GPIOIF_GROUP_2 = 2,
	/** GPIOIF group3(gpio12~15) */
	GPIOIF_GROUP_3 = 3,
	/** GPIOIF group4(gpio16~19) */
	GPIOIF_GROUP_4 = 4,
	/** GPIOIF group5(gpio20~23) */
	GPIOIF_GROUP_5 = 5
} gpioif_group;

/**
 * @brief This enum defines GPIOIF counter mode which can support.\n
 */
typedef enum {
	/** GPIOIF Event Counter Mode */
	MTK_OS_GPIOIF_EVENT_COUNTER_MODE = 0,
	/** GPIOIF Capture Counter Mode */
	MTK_OS_GPIOIF_CAP_COUNTER_MODE = 1,
	/** GPIOIF Both Event Counter Mode and Capture Counter Mode */
	MTK_OS_GPIOIF_EVENT_CAP_COUNTER_MODE = 2,
	/**  GPIOIF maximum supported Mode  <invalid> */
	MTK_OS_GPIOIF_MODE_MAX
} mtk_os_gpioif_mode;

/**
 * @brief This enum defines GPIOIF clock source selection.\n
 */
typedef enum {
	/** GPIOIF Embedded XTAL KHz(26MHz) clock source */
	GPIOIF_CLOCK_26MHZ = 0,
	/** GPIOIF Embedded 32KHz clock source */
	GPIOIF_CLOCK_32KHZ = 1,
	/** GPIOIF AXI1 MHz(125MHz) clock source */
	GPIOIF_CLOCK_125MHZ = 2,
	/** GPIOIF 200MHz clock source */
	GPIOIF_CLOCK_200MHZ = 3,
	/**  GPIOIF maximum clock source <invalid> */
	GPIOIF_CLOCK_MAX
} gpioif_source_clock;

/**
 * @brief This enum defines GPIOIF saturation mode selection of
 *    GPIO event counter when reaching limit value .\n
 */
typedef enum {
	/** Not saturare on limit value */
	GPIOIF_NOT_SA_LIMIT_V = 0,
	/** Saturare on low limit value */
	GPIOIF_SA_LOW_LIMIT_V = 1,
	/** Saturare on high limit value */
	GPIOIF_SA_HIGH_LIMIT_V = 2,
	/** Back to reset value when the counter reaches low limit value */
	GPIOIF_SA_RESET_LOW_LIMIT_V = 3,
	/** Back to reset value when the counter reaches high limit value */
	GPIOIF_SA_RESET_HIGH_LIMIT_V = 4,
	/**  GPIOIF maximum saturare mode limit selection <invalid> */
	GPIOIF_SA_MAX
} gpioif_sa_mode_limit_select;

/**
 * @brief This enum defines GPIOIF interrupt mode selection of
 *    GPIO event counter.\n
 */
typedef enum {
	/** Not interrupt */
	GPIOIF_NOT_INTERRUPT = 0,
	/** Interrupt issued when the counter reaches low limit value */
	GPIOIF_INTERRUPT_LOW_LIMIT_V = 1,
	/** Interrupt issued when the counter reaches high limit value */
	GPIOIF_INTERRUPT_HIGH_LIMIT_V = 2,
	/** Interrupt issued when the counter reaches
	* both low and high limit value
	*/
	GPIOIF_INTERRUPT_BOTH_V = 3,
	/**  GPIOIF maximum interrupt mode limit selection <invalid> */
	GPIOIF_INTERRUPT_MAX
} gpioif_interrupt_limit_select;

/**
  * @}
  */

/** @defgroup os_hal_gpioif_typedef Typedef
  * @{
  * This section introduces the typedef that gpioif OS-HAL used.
  */

/** @brief	This defines the callback function prototype.
 * It's used for GPIOIF interrupt function handle.\n
 * User can register a callback function when using GPIOIF interrupt.\n
 * When one interrupt is triggered and a related user callback
 * is called in OS-HAL interrupt handle function which we register.\n
 * This typedef is used to describe the callback function
 * what the user wants to call.
 *
 * @param [in] user_data : An OS-HAL defined parameter provided
 * by #mtk_os_hal_gpioif_int_callback_register().
 *
 * @return  Return "0" if callback register successfully,
 *    or return "-1" if fail.
 */
typedef int (*gpioif_int_callback) (void *user_data);

/**
  * @}
  */

/** @defgroup os_hal_gpioif_function Function
  * @{
  * This section provides high level APIs to upper layer.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Init GPIOIF controller.
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return "0" if callback register successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_ctlr_init(gpioif_group group);

/**
 * @brief  Deinit GPIOIF controller.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return "0" if callback register successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_ctlr_deinit(gpioif_group group);

/**
 * @brief This function is used to set GPIOIF direction mode.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5).
 * @param [in] control_setting : Define Control edge type of GPIOIF Event
 *    Counter(1~6).
 * @param [in] low_limit : Define Low limit value for GPIOIF Event Counter.
 *    (0~0xffffffff)
 * @param [in] high_limit : Defines High limit value for GPIOIF Event Counter.
 *    (0~0xffffffff)
 * @param [in] reset_value : Defines Reset value for GPIOIF Event Counter.
 *    (0~0xffffffff)
 * @param [in] clock_source : Defines GPIOIF clock source selection(0~3).
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_set_direction_mode(
	gpioif_group group, u8 control_setting, u32 low_limit,
	u32 high_limit, u32 reset_value, gpioif_source_clock clock_source);

/**
 * @brief This function is used to set GPIOIF updown mode.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5).
 * @param [in] control_setting : Define Control edge type of GPIOIF Event
 *    Counter(1~6).
 * @param [in] low_limit : Define Low limit value for GPIOIF Event Counter.
 *    (0~0xffffffff)
 * @param [in] high_limit : Defines High limit value for GPIOIF Event Counter.
 *    (0~0xffffffff)
 * @param [in] reset_value : Defines Reset value for GPIOIF Event Counter.
 *    (0~0xffffffff)
 * @param [in] clock_source : Defines GPIOIF clock source selection(0~3).
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_set_updown_mode(
	gpioif_group group, u8 control_setting, u32 low_limit,
	u32 high_limit, u32 reset_value, gpioif_source_clock clock_source);

/**
 * @brief This function is used to set GPIOIF quadrature mode.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5).
 * @param [in] control_setting : Define Control edge type of GPIOIF Event
 *    Counter(1~6).
 * @param [in] low_limit : Define Low limit value for GPIOIF Event Counter.
 *    (0~0xffffffff)
 * @param [in] high_limit : Defines High limit value for GPIOIF Event Counter.
 *    (0~0xffffffff)
 * @param [in] reset_value : Defines Reset value for GPIOIF Event Counter.
 *    (0~0xffffffff)
 * @param [in] clock_source : Defines GPIOIF clock source selection(0~3).
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_set_quadrature_mode(
	gpioif_group group, u8 control_setting, u32 low_limit,
	u32 high_limit, u32 reset_value, gpioif_source_clock clock_source);

/**
 * @brief This function is used to set GPIOIF capture mode.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5).
 * @param [in] edge_type_gpio_0 : Define edge type of GPIO_0 capture(0~3).
 * @param [in] edge_type_gpio_1 : Define edge type of GPIO_1 capture(0~3)..
 * @param [in] clock_source : Defines GPIOIF clock source selection(0~3).
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_set_capture_mode(
	gpioif_group group, u8 edge_type_gpio_0, u8 edge_type_gpio_1,
	gpioif_source_clock clock_source);

/**
 * @brief This function is used to control interrupt.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] enable : 0: disable;  1: enable.
 * @param [in] enable : 0: not clear;  1: clear.
 * @param [in] bit : Specifies the GPIOIF interrupt value to set.(0~0xffffffff)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_interrupt_control(
	gpioif_group group, u8 enable, u8 clear, u32 bit);

/**
 * @brief This function is used to set interrupt mode or
 *    saturation mode of the GPIO event counter.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] sa_limit_v : Defines GPIOIF saturation mode selection of
 *    GPIO event counter when reaching limit value.(0~4)
 * @param [in] interrupt_limit_v : defines GPIOIF interrupt mode
 *    selection of GPIO event counter.(0~3)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_limit_comparator(
	gpioif_group group, gpioif_sa_mode_limit_select sa_limit_v,
	gpioif_interrupt_limit_select interrupt_limit_v);

/**
 * @brief  This function is used to set GPIOIF de-glitch setting.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] gpio : GPIO number(0~2)
 * @param [in] enable : 0: disable;  1: enable.
 * @param [in] min_p :
 *    Minimum pulse width of the GPIOIF de-glitch circuit(0~16383)
 * @param [in] init_v :
 *    Initial output value of the GPIOIF de-glitch circuit(0 or 1)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_de_glitch(
	gpioif_group group, u8 gpio, u8 enable, u32 min_p, u32 init_v);

/**
 * @brief This function is used to set event counter mode setting
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */

int mtk_os_hal_gpioif_event_counter_setting(
	gpioif_group group, u8 control_setting, u32 low_limit, u32 high_limit,
	u32 reset_value, u8 event_cnt_mode);

/**
 * @brief This function is used to reset all GPIOIF group globally and
 *    make the register of the GPIOIF group to default value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_global_cr_reset(gpioif_group group);

/**
 * @brief  This function uses GPIO_2 pin to
 *    reset GPIOIF event counter.
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] mode : Specifies the GPIOIF mode numer to set.(0~2)
 * @param [in] active_reset : 0: low; 1: high.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_hardware_reset(gpioif_group group,
	mtk_os_gpioif_mode mode, u8 active_reset);

/**
 * @brief This function is used to initialize all software counter
 *    in interrupt function.
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_interrupt_count_init(gpioif_group group);

/**
 * @brief This function is used to initialize software capture fifo counter.
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_capture_fifo_init(gpioif_group group);

/**
 * @brief This function is used to dump all software counter
 *    in interrupt function.
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_dump_int(gpioif_group group);

/**
 * @brief This function is used to read gpio event counter value.
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [out] pvalue : Read register value and save it to
 *    address pvalue(0~0xffffffff)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_read_gpio_event_count(gpioif_group group, u32 *pvalue);

/**
 * @brief This function is used to read reset value
 *    of GPIOIF Event Counter.
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [out] pvalue : Read register value and save it to
 *    address pvalue(0~0xffffffff)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_read_reset_val(gpioif_group group, u32 *pvalue);

/**
 * @brief This function is used to read low limit value
 *    of GPIOIF Event Counter.
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [out] pvalue : Read register value and save it to
 *    address pvalue(0~0xffffffff)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_read_low_limit_val(gpioif_group group, u32 *pvalue);

/**
 * @brief This function is used to read high limit value
 *    of GPIOIF Event Counter.
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [out] pvalue : Read register value and save it to
 *    address pvalue(0~0xffffffff)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_read_high_limit_val(gpioif_group group, u32 *pvalue);

/**
 * @brief  This function is used to select the GPIOIF clock source.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] clock_source : Specifies the GPIOIF clock source selection.(0~3)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_select_clock_source(gpioif_group group,
	gpioif_source_clock clock_source);

/**
 * @brief This function is used to enable/disable GPIOIF source clock.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] enable : clock is enable or not(0:disable; 1:enable).
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_counter_clock_setting(gpioif_group group, u8 enable);

/**
 * @brief  This function is used to perform software reset of
 *    the GPIO event counter.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] mode : Specifies the GPIOIF mode numer to set.(0~2)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_software_reset(gpioif_group group,
	mtk_os_gpioif_mode mode);

/**
 * @brief This function is used to enable event counter mode.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_enable_event_counter(gpioif_group group);

/**
 * @brief This function is used to disable event counter mode.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_disable_event_counter(gpioif_group group);

/**
 * @brief This function is used to disable capture counter mode.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_os_hal_gpioif_disable_capture_counter(gpioif_group group);

/**
 * @brief  This function is used to read capture fifo0 value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [out] pvalue : Specifies the GPIOIF capture fifo0 value.(0~0xffffffff)
 *
 * @return
 * Return"0" if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_read_gpio_cap_fifo0_value(
	gpioif_group group, u32 *pvalue);

/**
 * @brief  This function is used to read capture fifo1 value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [out] pvalue : Specifies the GPIOIF capture fifo1 value.(0~0xffffffff)
 *
 * @return
 * Return"0" if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_read_gpio_cap_fifo1_value(
	gpioif_group group, u32 *pvalue);

/**
 * @brief  This function is used to get GPIOIF interrupt event low count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return interrupt event low count value if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_event_low_count(gpioif_group group);

/**
 * @brief  This function is used to get GPIOIF interrupt event high count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return interrupt event high count value if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_event_high_count(gpioif_group group);

/**
 * @brief  This function is used to get GPIOIF gpio2 interrupt
 *    reset done count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return gpio2 interrupt reset done count value
 * if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_gpio2_rst_done_count(gpioif_group group);

/**
 * @brief  This function is used to get GPIOIF
 *    interrupt event overflow count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return interrupt event overflow count value
 * if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_event_over_count(gpioif_group group);

/**
 * @brief  This function is used to get GPIOIF
 *    interrupt event underflow count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return interrupt event underflow count value
 * if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_event_uf_count(gpioif_group group);

/**
 * @brief  This function is used to get GPIOIF
 *    interrupt capture fifo0 count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return interrupt capture fifo0 count value
 * if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_cap_f0_full_count(gpioif_group group);

/**
 * @brief  This function is used to get GPIOIF
 *    interrupt capture fifo1 count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return interrupt capture fifo1 count value
 * if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_cap_f1_full_count(gpioif_group group);

/**
 * @brief  This function is used to get GPIOIF interrupt
 *    reset capture fifo0 count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return interrupt reset capture fifo0 count value
 * if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_reset_cap_f0_full_count(gpioif_group group);

/**
 * @brief  This function is used to get GPIOIF interrupt
 *    reset capture fifo1 count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return interrupt reset capture fifo1 count value
 * if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_reset_cap_f1_full_count(gpioif_group group);

/**
 * @brief  This function is used to get capture fifo0 not empty count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return fifo0 not empty count value if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_cap_f0_np_count(gpioif_group group);

/**
 * @brief  This function is used to get capture fifo1 not empty count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return fifo1 not empty count value if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_cap_f1_np_count(gpioif_group group);

/**
 * @brief  This function is used to get capture fifo0 empty count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return fifo0 empty count value if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_cap_f0_p_count(gpioif_group group);

/**
 * @brief  This function is used to get capture fifo1 empty count value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return fifo1 empty count value if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_int_cap_f1_p_count(gpioif_group group);

/**
 * @brief  This function is used to enable/disable interrupt.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] bit : Specifies the GPIOIF interrupt value to set.(0~0xffffffff)
 * @param [in] enable : 0 means disable interrupt, 1 means enable interrupt.
 *
 * @return
 * Return "0" if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_interrupt_bit_wise(gpioif_group group,
	u32 bit, u8 enable);

/**
 * @brief  This function is used to get capture fifo0 count.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return "0" if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_cap_fifo0_count(gpioif_group group);

/**
 * @brief  This function is used to get capture fifo1 count.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 *
 * @return
 * Return "0" if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_cap_fifo1_count(gpioif_group group);

/**
 * @brief  This function is used to get capture fifo0 value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] idex : Specifies the GPIOIF fifo0 number.(0~15)
 *
 * @return
 * Return "0" if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_cap_fifo0_val(gpioif_group group, u32 idex);

/**
 * @brief  This function is used to get capture fifo1 value.
 *
 * @param [in] group : Specifies the GPIOIF group number to set.(0~5)
 * @param [in] idex : Specifies the GPIOIF fifo1 number.(0~15)
 *
 * @return
 * Return "0" if function executes successfully.\n
 * Return -#GPIOIF_EINVAL if fail.\n
 */
int mtk_os_hal_gpioif_get_cap_fifo1_val(gpioif_group group, u32 idex);

/** @brief This function is used to register user's interrupt callback.
 * It's used for GPIOIF interrupt function handle.\n
 * User can register a callback function when using GPIOIF interrupt.\n
 * When one interrupt is triggered and a related user callback
 * is called in OS-HAL interrupt handle function which we register.\n
 * This typedef is used to describe the callback function
 * what the user wants to call.
 *
 * @param [in] ctlr : GPIOIF controller used with the device.
 * @param [in] callback : The callback function given by test layer.
 * which will be called before we set gpioif related setting.
 * @param [in] user_data : A parameter given by OS-HAL and will
 * be passed to user when the callback function is called.
 *
 *@return
 * Return "0" if callback register successfully.\n
 * Return -#GPIOIF_EINVAL if ctlr or callback or user_data is NULL.
 */
int mtk_os_hal_gpioif_int_callback_register(gpioif_group group,
	gpioif_int_callback callback, void *user_data);

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
