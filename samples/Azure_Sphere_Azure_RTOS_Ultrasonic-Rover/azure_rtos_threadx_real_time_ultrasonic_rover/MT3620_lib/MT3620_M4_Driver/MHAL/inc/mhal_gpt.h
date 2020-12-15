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

#ifndef __MHAL_GPT_H__
#define __MHAL_GPT_H__

#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup GPT
 * @{
 * These sections introduce the Generic Purpose Timer (GPT), including terms
 * and acronyms, supported features. And any simple usage with these APIs and
 * current system case of GPT are introduced in the section on how to use this
 * driver.
 *
 * @section MHAL_GPT_Driver_Terms_Chapter Terms and Acronyms
 *
 * | Terms |                       Details                                  |
 * |-------|----------------------------------------------------------------|
 * |GPT    |Generic Purpose Timer                                           |
 * |ISR    |Interrupt Service Routines                                      |
 *
 *
 * @section MHAL_GPT_Driver_Module_Chapter Supported Features
 *
 *  Each IO-CM4 has one GPT device (which is placed on APB bus) respectively to
 * support time calculation application.\n
 *  Those GPT device units are the same. And each GPT device unit contains five
 * timers which are designed as follows:\n
 *
 * | ID |       Mode        |  Clock Source  |  Clock speed  |Interrupt signal|
 * |----|-------------------|----------------|---------------|----------------|
 * |GPT0|one-shot and repeat|32kHz           |1kHz or 32kHz  |GPT             |
 * |GPT1|one-shot and repeat|32kHz           |1kHz or 32kHz  |GPT             |
 * |GPT2|free-run           |32kHz           |1kHz or 32kHz  |n/a             |
 * |GPT3|one-shot           |XTAL Clock      |1MHz           |GPT3            |
 * |GPT4|free-run           |Bus Clock |1/2 or 1 * Bus Clock |n/a             |
 *
 *  For more details, please refer to the GPT datasheet.\n
 *
 * @section MHAL_GPT_Driver_Usage_Chapter How to Use This Driver
 *
 * - \b Software \b Architecture: \n
 *
 *  See @ref MHAL_Overview_2_Chapter for the details of SW architecture.\n
 *
 *  This section describes the definition of APIs and provides an example on
 * FreeRTOS to show how to use these APIs to develop an OS-related GPT driver.\n
 *  Also, this section provides an example about the sequence of APIs that
 * should be used by the GPT driver.\n
 *
 * - \b Scope \b of \b M-HAL \b GPT \b APIs \n
 *
 *  M-HAL GPT APIs are used to control timers hardware behaviors(specify timer
 * with the pointer of struct hal_gpt_dev and a timer ID as enum #gpt_num).\n
 *  - For all GPT timers, mtk_mhal_gpt_start(), mtk_mhal_gpt_stop() and
 * mtk_mhal_gpt_restart_count() can be used to enable, disable and restart timer
 * counter, and mtk_mhal_gpt_get_count() can be used to get counter's value.\n
 *  - For timers with multiple clock speeds, mtk_mhal_gpt_config_clk() can be
 * used to configure timer clock speeds. And for GPT3, mtk_mhal_gpt_config_clk()
 * is still necessary to calibrate its counter frequency.\n
 *  - For interrupt-based timers, mtk_mhal_gpt_set_compare() can be used to set
 * time of interrupt trigger after timer is enabled. And
 * mtk_mhal_gpt_config_mode() can be used to configure timer interrupt mode.\n
 *    mtk_mhal_gpt_enable_irq() and mtk_mhal_gpt_disable_irq() can be used to
 * enable and disable interrupt source of GPT timer, and
 * mtk_mhal_gpt_get_irq_status() and mtk_mhal_gpt_clear_irq_status() can be used
 * to get and clear interrupt status of GPT timer interrupt source.\n
 *   Note: \n
 *    Here, GPT3 is designed to have a fixed enable interrupt mode in the
 * hardware. mtk_mhal_gpt_clear_irq_status() is necessary for it, but
 * mtk_mhal_gpt_enable_irq(), mtk_mhal_gpt_disable_irq() and
 * mtk_mhal_gpt_get_irq_status() are invalid for it, and always returns
 * -#GPT_EACCES. Enable or disable interrupt for GPT3 needs to unmask or mask
 * the signal of upper interrupt controller.\n
 *
 * - \b The \b OS-HAL \b Driver \b Sample \b Code\n
 * \b for \b FreeRTOS \b is \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/FreeRTOS/OS_HAL/src/os_hal_gpt.c"> freeRTos GPT sample code on github </a>
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup GPT
 * @{
 */

/** @defgroup driver_gpt_define Define
  *  This section introduces the macro definitions  which are used by GPT
  * M-HAL's APIs error return type.
  * @{
  */

/** Invalid argument. It means the input pointer is NULL */
#define GPT_EPTR		1
/** Invalid argument. It means the HW unit does not exist. */
#define GPT_ENODEV		2
/**
  * Permission denied.
  * It means the configuration is not supported by HW design.
  */
#define GPT_EACCES		3

/**
  * @}
  */

/**
 * @defgroup driver_gpt_enum Enum
 * @{
 *  This section introduces the enumerations that GPT M-HAL used.
 */

/**
 * @brief  GPT timer ID enum definition.\n
 *  It can be used to specify the timer ID of the current GPT device unit.
 */
enum gpt_num {
	GPT0 = 0,
	GPT1 = 1,
	GPT2 = 2,
	GPT3 = 3,
	GPT4 = 4,
	GPT_MAX_NUM
};

/**
 * @brief  GPT timer interrupt mode enum definition.\n
 *  It can be used to set GPT timer's interrupt mode by
 * mtk_mhal_gpt_config_mode().
 */
enum gpt_mode {
	/** Configure GPT timer as one-shot mode. */
	GPT_ONE_SHOT = 0,
	/** Configure GPT timer as repeat mode. */
	GPT_REPEAT
};

/**
 * @brief  GPT timer's clock speeds enum definition.\n
 *  It can be used to set GPT timer's clock speed by mtk_mhal_gpt_config_clk().
 * - For GPT0, GPT1 and GPT2, GPT_SPEED_1K is 1kHz and GPT_SPEED_32K is 32kHz.\n
 * - For GPT3, it means nothing.\n
 * - For GPT4, GPT_SPEED_1K is half-bus clock and GPT_SPEED_32K is bus clock.\n
 */
enum gpt_clk {
	/** Configure GPT timer's clock speed as 1K mode. */
	GPT_SPEED_1K = 0,
	/** Configure GPT timer's clock speed as 32K mode. */
	GPT_SPEED_32K
};

/**
 * @}
 */

/**
 * @defgroup driver_gpt_struct Struct
 *  This section introduces the structure used by GPT M-HAL used.
 * @{
 */

/**
 * @brief  The interface to GPT device.
 *  It can be used to specify the GPT device unit.
 */
struct hal_gpt_dev {
	/** Register base address of the GPT device unit. */
	void __iomem *cm4_gpt_base;
};

/**
 * @}
 */

/**
 * @defgroup driver_gpt_function Function
 *  This section provides Fixed APIs(defined as Common Interface) to fully
 * control the MediaTek GPT HW.
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  This function is used to set GPT timeout value.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @param [in] cmp_val : GPT timeout count value. (unit: one tick of timer
 *		clock speed)
 *  @return
 *	0: Success.\n
 *	-#GPT_EPTR: Failure if gpt_dev or gpt_dev->cm4_gpt_base is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 *	-#GPT_EACCES: Failure if this timer no need to set timeout value.\n
 */
int mtk_mhal_gpt_set_compare(struct hal_gpt_dev *gpt_dev,
			     enum gpt_num timer_id,
			     u32 cmp_val);

/**
 * @brief  This function is used to configure GPT interrupt mode.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @param [in] mode : Mode as enum #gpt_mode.
 *  @return
 *	0: Success.\n
 *	-#GPT_EPTR: Failure if gpt_dev or gpt_dev->cm4_gpt_base is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 *	-#GPT_EACCES: Failure if this timer cannot choose interrupt mode.\n
 */
int mtk_mhal_gpt_config_mode(struct hal_gpt_dev *gpt_dev,
			     enum gpt_num timer_id,
			     enum gpt_mode mode);

/**
 * @brief  This function is used to enable GPT IRQ.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @return
 *	0: Success.\n
 *	-#GPT_EPTR: Failure if gpt_dev or gpt_dev->cm4_gpt_base is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 *	-#GPT_EACCES: Failure if this timer has no interrupt mode or cannot
 *		enable IRQ through GPT register.\n
 */
int mtk_mhal_gpt_enable_irq(struct hal_gpt_dev *gpt_dev,
			    enum gpt_num timer_id);

/**
 * @brief  This function is used to disable GPT IRQ.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @return
 *	0: Success.\n
 *	-#GPT_EPTR: Failure if gpt_dev or gpt_dev->cm4_gpt_base is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 *	-#GPT_EACCES: Failure if this timer has no interrupt mode or cannot
 *		disable	IRQ through GPT register.\n
 */
int mtk_mhal_gpt_disable_irq(struct hal_gpt_dev *gpt_dev,
			     enum gpt_num timer_id);

/**
 * @brief  This function is used to get GPT IRQ status.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @param [out] int_sta : A pointer to a variable which is used to return timer
 *		interrupt status.\n
 *		1: Interrupt status exists.\n
 *		0: No interrupt status.
 *  @return
 *	0: Get timer's interrupt status successfully.\n
 *	-#GPT_EPTR: Failure if gpt_dev, gpt_dev->cm4_gpt_base or int_sta is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 *	-#GPT_EACCES: Failure if this timer has no interrupt mode or cannot get
 *		IRQ status from GPT register.\n
 */
int mtk_mhal_gpt_get_irq_status(struct hal_gpt_dev *gpt_dev,
				enum gpt_num timer_id,
				unsigned int *int_sta);

/**
 * @brief  This function is used to clear GPT IRQ status.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @return
 *	0: Success.\n
 *	-#GPT_EPTR: Failure if gpt_dev or gpt_dev->cm4_gpt_base is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 *	-#GPT_EACCES: Failure if this timer has no interrupt mode and cannot
 *		clear IRQ status in GPT register.\n
 */
int mtk_mhal_gpt_clear_irq_status(struct hal_gpt_dev *gpt_dev,
				   enum gpt_num timer_id);

/**
 * @brief  This function is used to start GPT.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @return
 *	0: Success.\n
 *	-#GPT_EPTR: Failure if gpt_dev or gpt_dev->cm4_gpt_base is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 */
int mtk_mhal_gpt_start(struct hal_gpt_dev *gpt_dev,
		       enum gpt_num timer_id);

/**
 * @brief  This function is used to stop GPT.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @return
 *	0: Success.\n
 *	-#GPT_EPTR: Failure if gpt_dev or gpt_dev->cm4_gpt_base is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 */
int mtk_mhal_gpt_stop(struct hal_gpt_dev *gpt_dev,
		      enum gpt_num timer_id);

/**
 * @brief  This function is used to restart GPT counter.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @return
 *	0: Success.\n
 *	-#GPT_EPTR: Failure if gpt_dev or gpt_dev->cm4_gpt_base is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 */
int mtk_mhal_gpt_restart_count(struct hal_gpt_dev *gpt_dev,
			       enum gpt_num timer_id);

/**
 * @brief  This function is used to get GPT counter's value.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @param [out] cnt_val : A pointer to a variable which is used to return timer
 *			counter's value. (unit: one tick of timer clock speed)
 *  @return
 *	0: Success. And the timer counter's value is saved in *cnt_val.\n
 *	-#GPT_EPTR: Failure if gpt_dev, gpt_dev->cm4_gpt_base or cnt_val is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 */
int mtk_mhal_gpt_get_count(struct hal_gpt_dev *gpt_dev,
			   enum gpt_num timer_id,
			   u32 *cnt_val);

/**
 * @brief  This function is used to configure GPT clock speed.
 *  @param [in] gpt_dev : A pointer of struct hal_gpt_dev.
 *  @param [in] timer_id : Timer ID of current GPT device.
 *  @param [in] clk : Clock speed as enum #gpt_clk.
 *  @return
 *	0: Success.\n
 *	-#GPT_EPTR: Failure if gpt_dev or gpt_dev->cm4_gpt_base is NULL
 *		pointer.\n
 *	-#GPT_ENODEV: Failure if timer ID beyond the limitation.\n
 */
int mtk_mhal_gpt_config_clk(struct hal_gpt_dev *gpt_dev,
			    enum gpt_num timer_id,
			    enum gpt_clk clk);

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

#endif /* __MHAL_GPT_H__ */
