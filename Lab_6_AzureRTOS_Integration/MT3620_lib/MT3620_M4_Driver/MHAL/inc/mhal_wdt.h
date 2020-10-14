/*
 * (C) 2005-2019 MediaTek Inc. All rights reserved.
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

#ifndef __MHAL_WDT_H__
#define __MHAL_WDT_H__

#include <mhal_osai.h>

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup WDT
 * @{
 *
 *  These sections introduce the Watchdog Timer (WDT), including terms and
 * acronyms, supported features. And the usage of these APIs and current
 * system case of WDT are introduced in the section on how to use this driver.
 *
 * @section MHAL_WDT_Driver_Terms_Chapter Terms and Acronyms
 *
 * | Terms |                       Details                                  |
 * |-------|----------------------------------------------------------------|
 * |WDT    |Watchdog Timer                                                  |
 * |HW-RST |Hardware Reset. It can be provided by WDT counter timeout(hwrst)|
 * |SW-RST |Software Reset. It can be provided by WDT function(swrst)       |
 *
 * @section MHAL_WDT_Driver_Module_Chapter Supported Features
 *
 *  Each IO-CM4 has one WDT device respectively to support system operation
 * monitoring.\n
 *  WDT device provides the counter with the 32kHz clock source and it can
 * trigger two types of events: interrupt and reset. And there are two ways to
 * trigger these events: counter timeout(HW-RST) and setting swrst register
 * (SW-RST).\n
 *  For more details, please refer the WDT datasheet.
 *
 * @section MHAL_WDT_Driver_Usage_Chapter How to Use This Driver
 *
 * - \b Software \b Architecture \b of \b WDT \n
 *
 *  See @ref MHAL_Overview_2_Chapter for details of SW architecture.\n
 *
 *  This section describes the definition of APIs and provides an example on
 * FreeRTOS to show to use these APIs to develop an OS-related WDT driver.\n
 *  Also, this section provides an example about the sequence of APIs that
 * should be used by the WDT driver.\n
 *
 * - \b Scope \b of \b M-HAL \b WDT \b APIs \n
 *
 *  M-HAL WDT APIs are used to control WDT hardware behaviors.\n
 *  - For all WDT, mtk_mhal_wdt_enable() and mtk_mhal_wdt_restart() can be used
 * to enable, disable and restart WDT counter, mtk_mhal_wdt_set_timeout() can be
 * used to set timeout value(unit: tick by 32kHz) for WDT.\n
 *  - mtk_mhal_wdt_config() can be used to select the type of event that WDT
 * will trigger.\n
 *  - mtk_mhal_wdt_hwrst() and mtk_mhal_wdt_swrst() will trigger the event
 * immediately in WDT HW-RST way or WDT SW-RST way. It will affect reset status
 * of WDT.\n
 *
 * - \b The \b OS-HAL \b Driver \b Sample \b Code \b for \b FreeRTOS: \n
 *
 *  FreeRTOS does not have WDT framework, so this sample needs to provide driver
 * APIs for User Application:\n
 *
 * @code
 *	#include <mhal_wdt.h>
 *	#include <os_hal_wdt.h>
 *	#include <timer.h>
 *	#include <nvic.h>
 *	#include <irq.h>
 *
 *	#define OS_WDT_LOG(string, args...)	printf("[os_wdt]"string, ##args)
 *
 *	// defined in os_hal_wdt.h
 *	struct os_wdt_int {
 *		void (*wdt_cb_hdl)(void *);
 *		void *wdt_cb_data;
 *	};
 *
 *	// defined in os_hal_wdt.h
 *	enum os_wdt_mode {
 *		OS_WDT_TRIGGER_RESET = 0,
 *		OS_WDT_TRIGGER_IRQ = 1,
 *	};
 *
 *	// defined in os_hal_wdt.h
 *	enum os_wdt_rst_sta {
 *		OS_WDT_NONE_RST = 0,
 *		OS_WDT_SW_RST = 1,
 *		OS_WDT_HW_RST = 2,
 *	};
 *
 *	struct os_wdt_dev {
 *		struct hal_wdt_dev	hal_dev;
 *		struct os_wdt_int	int_info;
 *		unsigned int		irq_id;
 *		enum os_wdt_rst_sta	reset_status;
 *		bool			inited;
 *	};
 *
 *	static struct os_wdt_dev wdt_dev;
 *
 *	static bool _mtk_os_hal_wdt_is_inited(struct os_wdt_dev *dev)
 *	{
 *		if (!dev->inited) {
 *			OS_WDT_LOG("wdt device initialization failed\n");
 *			return false;
 *		}
 *		return true;
 *	}
 *
 *	// internal functions for interrupt
 *
 *	static void _mtk_os_hal_wdt_default_irq_hdl(void *unused)
 *	{
 *		unsigned int rst_sta = 0;
 *
 *		mtk_mhal_wdt_get_status(&(wdt_dev.hal_dev), &rst_sta);
 *		OS_WDT_LOG("default_irq_hdl: sta %x\n", rst_sta);
 *
 *		mtk_mhal_wdt_enable(&(wdt_dev.hal_dev), 0);
 *		// some information can be dumped in here...
 *		mtk_mhal_wdt_hwrst(&(wdt_dev.hal_dev));
 *	}
 *
 *	static void _mtk_os_hal_wdt_isr(void)
 *	{
 *		if (wdt_dev.int_info.wdt_cb_hdl)
 *			wdt_dev.int_info.wdt_cb_hdl(
 *					wdt_dev.int_info.wdt_cb_data);
 *	}
 *
 *	// external functions
 *
 *	void mtk_os_hal_wdt_enable(void)
 *	{
 *		if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
 *			return;
 *
 *		mtk_mhal_wdt_restart(&(wdt_dev.hal_dev));
 *		mtk_mhal_wdt_enable(&(wdt_dev.hal_dev), 1);
 *	}
 *
 *	void mtk_os_hal_wdt_disable(void)
 *	{
 *		if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
 *			return;
 *
 *		mtk_mhal_wdt_enable(&(wdt_dev.hal_dev), 0);
 *	}
 *
 *	int mtk_os_hal_wdt_set_timeout(unsigned int sec)
 *	{
 *		if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
 *			return -WDT_EPTR;
 *
 *		return mtk_mhal_wdt_set_timeout(&(wdt_dev.hal_dev), sec);
 *	}
 *
 *	void mtk_os_hal_wdt_restart(void)
 *	{
 *		if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
 *			return;
 *
 *		mtk_mhal_wdt_restart(&(wdt_dev.hal_dev));
 *	}
 *
 *	void mtk_os_hal_wdt_sw_reset(void)
 *	{
 *		if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
 *			return;
 *
 *		mtk_mhal_wdt_config(&(wdt_dev.hal_dev), 0);
 *		mtk_mhal_wdt_swrst(&(wdt_dev.hal_dev));
 *	}
 *
 *	void mtk_os_hal_wdt_hw_reset(void)
 *	{
 *		if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
 *			return;
 *
 *		mtk_mhal_wdt_config(&(wdt_dev.hal_dev), 0);
 *		mtk_mhal_wdt_hwrst(&(wdt_dev.hal_dev));
 *	}
 *
 *	enum os_wdt_rst_sta mtk_os_hal_wdt_get_reset_status(void)
 *	{
 *		return wdt_dev.reset_status;
 *	}
 *
 *	void mtk_os_hal_wdt_config(enum os_wdt_mode mode)
 *	{
 *		if (!_mtk_os_hal_wdt_is_inited(&wdt_dev))
 *			return;
 *
 *		mtk_mhal_wdt_config(&(wdt_dev.hal_dev), mode);
 *	}
 *
 *	void mtk_os_hal_wdt_register_irq(struct os_wdt_int *wdt_int)
 *	{
 *		if (!wdt_int || !(wdt_int->wdt_cb_hdl)) {
 *			wdt_dev.int_info.wdt_cb_hdl =
 *				_mtk_os_hal_wdt_default_irq_hdl;
 *			wdt_dev.int_info.wdt_cb_data = NULL;
 *		} else {
 *			wdt_dev.int_info.wdt_cb_hdl = wdt_int->wdt_cb_hdl;
 *			wdt_dev.int_info.wdt_cb_data = wdt_int->wdt_cb_data;
 *		}
 *	}
 *
 *	// device initialization
 *
 *	void mtk_os_hal_wdt_init(void)
 *	{
 *		unsigned int rst_sta;
 *
 *		if (wdt_dev.inited)
 *			return;
 *
 *		// initialize WDT devices
 *		wdt_dev.hal_dev.cm4_wdt_base = (void __iomem *)(0x21020000);
 *		wdt_dev.irq_id = CM4_IRQ_WDT_M4_IO;
 *
 *		// get WDT reset status
 *		mtk_mhal_wdt_get_status(&(wdt_dev.hal_dev), &rst_sta);
 *		wdt_dev.reset_status = (enum os_wdt_rst_sta)rst_sta;
 *
 *		// disable WDT
 *		mtk_mhal_wdt_enable(&(wdt_dev.hal_dev), 0);
 *
 *		// register default interrupt handle
 *		mtk_os_hal_wdt_register_irq(NULL);
 *		// request irq from system
 *		NVIC_Register(wdt_dev.irq_id, _mtk_os_hal_wdt_isr);
 *		NVIC_SetPriority(wdt_dev.irq_id, CM4_WDT_PRI);
 *		NVIC_EnableIRQ(wdt_dev.irq_id);
 *
 *		wdt_dev.inited = true;
 *	}
 * @endcode
 *
 * - \b How \b to \b Develop \b User \b Application \b by \b Using \b OS-HAL
 *   \b API:\n
 *
 *  - Sample Code (The user application sample code on FreeRTOS.)\n
 *
 *    @code
 *    - Initialize WDT driver:
 *      - Call mtk_os_hal_wdt_init() to initialize WDT driver.
 *
 *    - Configure WDT timeout value:
 *      - Call mtk_os_hal_wdt_set_timeout(T) to set WDT timeout value as T
 *        second.
 *
 *    - Configure WDT as interrupt mode:
 *      - Define a variable "struct os_wdt_int wdt_int" if necessary.
 *      - Call mtk_os_hal_wdt_register_irq(&wdt_int) to register WDT user
 *        interrupt callback handle and user data. (If input NULL to
 *        mtk_os_hal_wdt_register_irq(), the default callback handle will be
 *        registered which will reset system immediately.)
 *      - Call mtk_os_hal_wdt_config(OS_WDT_TRIGGER_IRQ) to sec WDT as
 *        interrupt mode.
 *
 *    - Configure WDT as reset mode:
 *      - Call mtk_os_hal_wdt_config(OS_WDT_TRIGGER_RESET) to sec WDT as
 *        reset mode.
 *
 *    - Enable/disable WDT:
 *      - Call mtk_os_hal_wdt_enable() and mtk_os_hal_wdt_disable() to enable/
 *        disable WDT.
 *
 *    - Restart WDT counter:
 *      - Call mtk_os_hal_wdt_restart() to restart WDT counter.
 *
 *    - Trigger system reset immediately:
 *      - Call mtk_os_hal_wdt_sw_reset() to trigger SW-RST immediately.
 *      - Call mtk_os_hal_wdt_hw_reset() to trigger HW-RST immediately.
 * @endcode
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup WDT
 * @{
 */

/** @defgroup driver_wdt_define Define
  *  This section introduces the macro definitions  which are used by WDT
  * M-HAL's APIs error return type.
  * @{
  */

/** Invalid argument. It means the input pointer is NULL */
#define WDT_EPTR		1
/** Invalid argument. It means the input value is illegal */
#define WDT_EINVAL		2
/** Fault return. It means some fault occurred. */
#define WDT_EFAULT		3

/**
  * @}
  */

/**
 * @defgroup driver_wdt_struct Struct
 *  This section introduces the structure used by WDT M-HAL used.
 * @{
 */

/**
 * @brief  The interface to WDT device.
 * It can be used to specify the WDT hardware module.
 */
struct hal_wdt_dev {
	/** Register base address of WDT */
	void __iomem *cm4_wdt_base;
};

/**
 * @}
 */

/**
 * @defgroup driver_wdt_function Function
 *  This section provides Fixed APIs(defined as Common Interface) to fully
 * control the MediaTek WDT HW.
 * @{
 */

/**
 * @brief  This function is used to enable or disable WDT.
 *  @param [in] wdt_dev : A pointer of struct hal_wdt_dev.
 *  @param [in] en : Disable (= 0) or enable (= 1) WDT.
 *  @return
 *	0: Success.\n
 *	-#WDT_EPTR: Failure if wdt_dev or wdt_dev->cm4_wdt_base is NULL
 *		pointer.\n
 */
int mtk_mhal_wdt_enable(struct hal_wdt_dev *wdt_dev,
			u32 en);

/**
 * @brief  This function is used to configure WDT mode
 *  @param [in] wdt_dev : A pointer of struct hal_wdt_dev.
 *  @param [in] irq : WDT trigger reset (= 0) or trigger irq (= 1) event.
 *  @return
 *	0: Success.\n
 *	-#WDT_EPTR: Failure if wdt_dev or wdt_dev->cm4_wdt_base is NULL
 *		pointer.\n
 */
int mtk_mhal_wdt_config(struct hal_wdt_dev *wdt_dev,
			u8 irq);

/**
 * @brief  This function is used to set WDT timeout value.
 *  @param [in] wdt_dev : A pointer of struct hal_wdt_dev.
 *  @param [in] sec : WDT timeout value (unit: second).
 *  @return
 *	0: Success.\n
 *	-#WDT_EPTR: Failure if wdt_dev or wdt_dev->cm4_wdt_base is NULL
 *		pointer.\n
 *	-#WDT_EINVAL: Failure if timeout value exceeds HW setting limitation.\n
 */
int mtk_mhal_wdt_set_timeout(struct hal_wdt_dev *wdt_dev,
			     u32 sec);

/**
 * @brief  This function is used to restart WDT counter.
 *  @param [in] wdt_dev : A pointer of struct hal_wdt_dev.
 *  @return
 *	0: Success.\n
 *	-#WDT_EPTR: Failure if wdt_dev or wdt_dev->cm4_wdt_base is NULL
 *		pointer.\n
 */
int mtk_mhal_wdt_restart(struct hal_wdt_dev *wdt_dev);

/**
 * @brief  This function is used to reset immediately by WDT HW-RST.\n
 *  @param [in] wdt_dev : A pointer of struct hal_wdt_dev.
 *  @return
 *	never return by reset: Success.\n
 *	-#WDT_EPTR: Failure if wdt_dev or wdt_dev->cm4_wdt_base is NULL
 *		pointer.\n
 *	-#WDT_EFAULT: Failure if reset failure.
 */
int mtk_mhal_wdt_hwrst(struct hal_wdt_dev *wdt_dev);

/**
 * @brief  This function is used to reset immediately by WDT SW-RST.\n
 *  @param [in] wdt_dev : A pointer of struct hal_wdt_dev.
 *  @return
 *	never return by reset: Success.\n
 *	-#WDT_EPTR: Failure if wdt_dev or wdt_dev->cm4_wdt_base is NULL
 *		pointer.\n
 *	-#WDT_EFAULT: Failure if reset failure.
 */
int mtk_mhal_wdt_swrst(struct hal_wdt_dev *wdt_dev);

/**
 * @brief  This function is used to get WDT reset status.
 *  @param [in] wdt_dev : A pointer of struct hal_wdt_dev
 *  @param [out] rst_sta : A pointer to a variable which is used to return reset
 *		status.\n
 *		0: No reset state.\n
 *		1: Software reset (by WDT SW-RST) state.\n
 *		2: Hardware reset (by WDT timeout) state.\n
 *  @return
 *	0: Get reset status successfully.\n
 *	-#WDT_EPTR: Failure if wdt_dev, wdt_dev->cm4_wdt_base or rst_sta is NULL
 *		pointer.\n
 */
int mtk_mhal_wdt_get_status(struct hal_wdt_dev *wdt_dev,
			    unsigned int *rst_sta);

/**
 * @}
 */

/**
 * @}
 * @}
 */

#endif /* __MHAL_WDT_H__ */
