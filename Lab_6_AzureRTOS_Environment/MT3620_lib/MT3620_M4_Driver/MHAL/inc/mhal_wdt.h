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
 * @section MHAL_WDT_Terms_Chapter Terms and Acronyms
 *
 * | Terms |                       Details                                  |
 * |-------|----------------------------------------------------------------|
 * |WDT    |Watchdog Timer                                                  |
 * |HW-RST |Hardware Reset. It can be provided by WDT counter timeout(hwrst)|
 * |SW-RST |Software Reset. It can be provided by WDT function(swrst)       |
 *
 * @section MHAL_WDT_Features_Chapter Supported Features
 *
 *  Each IO-CM4 has one WDT device respectively to support system operation
 * monitoring.\n
 *  WDT device provides the counter with the 32kHz clock source and it can
 * trigger two types of events: interrupt and reset. And there are two ways to
 * trigger these events: counter timeout(HW-RST) and setting swrst register
 * (SW-RST).\n
 *  For more details, please refer the WDT datasheet.
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup WDT
 * @{
 *
 * @section MHAL_WDT_Driver_Usage_Chapter How to Use This Driver
 *
 * - \b SW \b Architecture: \n
 *
 *  See @ref MHAL_Overview_2_Chapter for details of SW architecture.\n
 *
 *  This section describes the definition of APIs and provides an example on
 * freeRTos to show to use these APIs to develop an OS-related WDT driver.\n
 *  Also, this section provides an example about the sequence of APIs that
 * should be used by the WDT driver.\n
 *
 * - \b The \b OS-HAL \b freeRTos \b driver\n
 * \b sample \b code \b is \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_wdt.c"> freeRTos WDT sample code on github </a>
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

#ifdef __cplusplus
extern "C" {
#endif

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

#endif /* __MHAL_WDT_H__ */
