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

#ifndef __OS_HAL_WDT_H__
#define __OS_HAL_WDT_H__

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup wdt
 * @{
 *
 *  These sections introduce the Watchdog Timer (WDT) APIs including terms and
 * acronyms, supported features, details on how to use this driver, enums,
 * structures and functions.
 *
 * @section OS_HAL_WDT_Terms_Chapter Terms and Acronyms
 *
 * | Terms |                       Details                                  |
 * |-------|----------------------------------------------------------------|
 * |WDT    |Watchdog Timer                                                  |
 * |HW-RST |Hardware Reset. It can be provided by WDT counter timeout(hwrst)|
 * |SW-RST |Software Reset. It can be provided by WDT function(swrst)       |
 *
 * @section OS_HAL_WDT_Features_Chapter Supported Features
 * See @ref MHAL_WDT_Features_Chapter for the details of  Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup wdt
 * @{
 *
 * @section OS_HAL_WDT_Driver_Usage_Chapter How to Use This Driver
 *
 * - \b Device \b driver \b sample \b code \b is \b as \b follows: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *    @code
 *      - Initialize WDT driver:
 *        - Call mtk_os_hal_wdt_init() to initialize WDT driver.
 *
 *      - Configure WDT timeout value:
 *        - Call mtk_os_hal_wdt_set_timeout(T) to set WDT timeout value as T
 *          second.
 *
 *      - Configure WDT as interrupt mode:
 *        - Define a variable "struct os_wdt_int wdt_int" if necessary.
 *        - Call mtk_os_hal_wdt_register_irq(&wdt_int) to register WDT user
 *          interrupt callback handle and user data. (If input NULL to
 *          mtk_os_hal_wdt_register_irq(), the default callback handle will be
 *          registered which will reset system immediately.)
 *        - Call mtk_os_hal_wdt_config(OS_WDT_TRIGGER_IRQ) to sec WDT as
 *          interrupt mode.
 *
 *      - Configure WDT as reset mode:
 *        - Call mtk_os_hal_wdt_config(OS_WDT_TRIGGER_RESET) to sec WDT as
 *          reset mode.
 *
 *      - Enable/disable WDT:
 *        - Call mtk_os_hal_wdt_enable() and mtk_os_hal_wdt_disable() to enable/
 *          disable WDT.
 *
 *      - Restart WDT counter:
 *        - Call mtk_os_hal_wdt_restart() to restart WDT counter.
 *
 *      - Trigger system reset immediately:
 *        - Call mtk_os_hal_wdt_sw_reset() to trigger SW-RST immediately.
 *        - Call mtk_os_hal_wdt_hw_reset() to trigger HW-RST immediately.
 *    @endcode
 *
 * @}
 * @}
 */

/**
* @addtogroup OS-HAL
* @{
* @addtogroup wdt
* @{
*/


/**
 * @defgroup os_hal_wdt_struct Struct
 *  This section introduces the structure used by WDT OS-HAL used.
 * @{
 */

/**
 * @brief  The definition of WDT user interrupt handle structure.\n
 *	It can be used to register WDT user interrupt handle by
 *	mtk_os_hal_wdt_register_irq().
 */
struct os_wdt_int {
	/** The user interrupt handle of WDT. */
	void (*wdt_cb_hdl)(void *);
	/** The pointer of WDT interrupt callback data, which will be passed
	  * into user interrupt handle.
	  */
	void *wdt_cb_data;
};

/**
  * @}
  */

/**
 * @defgroup os_hal_wdt_enum Enum
 *  This section introduces the enumerations used by WDT OS-HAL used.
 * @{
 */

/**
 * @brief  The enum definition of WDT event mode.\n
 *	It is used to configure WDT event modes by mtk_os_hal_wdt_config().
 */
enum os_wdt_mode {
	/** Configure WDT to reset mode. */
	OS_WDT_TRIGGER_RESET = 0,
	/** Configure WDT to IRQ mode. */
	OS_WDT_TRIGGER_IRQ = 1,
};

/**
 * @brief  The enum definition of system reset status.\n
 *	It is used to mark the different kinds of system reset status.
 */
enum os_wdt_rst_sta {
	/** No abnormal reset status. */
	OS_WDT_NONE_RST = 0,
	/** Reset by WDT swrst function. */
	OS_WDT_SW_RST = 1,
	/** Reset by WDT counter timeout. */
	OS_WDT_HW_RST = 2,
};

/**
  * @}
  */

/** @defgroup os_hal_wdt_function Function
  * @{
   * This section provides high level APIs to upper layer.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  This function is used to enable WDT.
 *  @param
 *	None
 *  @return
 *	None
 */
void mtk_os_hal_wdt_enable(void);

/**
 * @brief  This function is used to disable WDT.
 *  @param
 *	None
 *  @return
 *	None
 */
void mtk_os_hal_wdt_disable(void);

/**
 * @brief  This function is used to set WDT timeout value.
 *  @param [in] sec : WDT timeout value(unit: second). It should be less then
 *		64 second.
 *  @return
 *	None
 */
int mtk_os_hal_wdt_set_timeout(unsigned int sec);

/**
 * @brief  This function is used to restart WDT counter.
 *  @param
 *	None
 *  @return
 *	None
 */
void mtk_os_hal_wdt_restart(void);

/**
 * @brief  This function is used to reset the M4 core immediately by WDT
 *	software reset. Once this API been invoked, the M4 core will reboot
 *	immediately. mtk_os_hal_wdt_init() and mtk_os_hal_wdt_get_reset_status()
 *	could be invoked later to get the reboot reason. This API is usually
 *	used when M4 APP trigger reboot.
 *  @param
 *	None
 *  @return
 *	None
 */
void mtk_os_hal_wdt_sw_reset(void);

/**
 * @brief  This function is used to reset the M4 core immediately by WDT
 *	counter timeout. Once this API been invoked, the M4 core will reboot
 *	immediately. mtk_os_hal_wdt_init() and mtk_os_hal_wdt_get_reset_status()
 *	could be invoked later to get the reboot reason. This API is usually
 *	been invoked in WDT irq handler.
 *  @param
 *	None
 *  @return
 *	None
 */
void mtk_os_hal_wdt_hw_reset(void);

/**
 * @brief  This function is used to get core last reset status value.
 *	The status is only valid if mtk_os_hal_wdt_init() has been called.
 *  @param
 *	None
 *  @return
 *	#OS_WDT_NONE_RST: system boots normally in this time.\n
 *	#OS_WDT_SW_RST: system reboots by WDT swrst.\n
 *	#OS_WDT_HW_RST: system reboots by WDT counter timeout.
 */
enum os_wdt_rst_sta mtk_os_hal_wdt_get_reset_status(void);

/**
 * @brief  This function is used to config WDT irq mode.
 *	#OS_WDT_TRIGGER_IRQ means IRQ will be triggered when WDT timeout, while
 *	#OS_WDT_TRIGGER_RESET means the M4 core will be reset immediately when
 *	WDT timeout.
 *  @param [in] irq : config WDT to trigger irq(input OS_WDT_TRIGGER_IRQ), or
 *		trigger reset(input OS_WDT_TRIGGER_RESET)
 *  @return
 *	None
 */
void mtk_os_hal_wdt_config(enum os_wdt_mode mode);

/**
 * @brief  This function is used to register user interrupt handle for WDT.
 *  @param [in] wdt_int : a pointer of struct os_wdt_int to register user
 *		interrupt handle(wdt_cb_hdl) and callback data(wdt_cb_data).
 *		When WDT interrupt is triggered, wdt_cb_hdl(wdt_cb_data) will be
 *		called without any other process.
 *		If wdt_int is NULL, a default interrupt handle will be
 *		registered which will cause system reset after WDT interruption.
 *  @return
 *	None
 */
void mtk_os_hal_wdt_register_irq(struct os_wdt_int *wdt_int);

/**
 * @brief  This function is used to initialize WDT device(with disable WDT) and
 *	to make WDT driver effective.
 *	   Internal assurance that it will only be executed only one time
 *	effectively.
 *  @param
 *	None
 *  @return
 *	None
 */
void mtk_os_hal_wdt_init(void);

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

#endif /* __OS_HAL_WDT_H__ */
