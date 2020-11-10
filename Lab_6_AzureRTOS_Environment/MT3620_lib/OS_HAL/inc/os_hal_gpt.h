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

#ifndef __OS_HAL_GPT_H__
#define __OS_HAL_GPT_H__

#include "mhal_gpt.h"

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup gpt
 * @{
 * This section introduces the Generic Purpose Timer (GPT), including terms
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
 * @section MHAL_GPT_Driver_Module_Chapter Supported Features
 * See @ref MHAL_GPT_Driver_Module_Chapter for the details of Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup gpt
 * @{
 * @section OS_HAL_GPT_Driver_Usage_Chapter How to use this driver
 *
 * - \b Device \b driver \b sample \b code \b is \b as \b follows: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *    @code
 *	- Initialize GPT driver:
 *       - Call mtk_os_hal_gpt_init() to initialize GPT driver.
 *
 *	- Configure GPT0 as repeat mode (GPT1 is similar):
 *      - Define a variable "struct os_gpt_int gpt0_int".
 *	  - Call mtk_os_hal_gpt_config(0, 0, &gpt0_int) to configure GPT0 clock
 *	     speed (as 1KHz) and register GPT0 user interrupt callback handle
 *	     and user data.
 *	  - Call mtk_os_hal_gpt_reset_timer(0, 1000, true) to configure GPT0
 *	     timeout value (as 1 second) and configure it as repeat mode.
 *
 *	- Configure GPT3 as one-shot mode (GPT0 and GPT1 are similar):
 *	- Define a variable "struct os_gpt_int gpt3_int".
 *	  - Call mtk_os_hal_gpt_config(3, 0, &gpt3_int) to configure GPT3 clock
 *	     speed (for GPT3 default as 1MHz) and register GPT3 user interrupt
 *	     callback handle and user data.
 *	  - Call mtk_os_hal_gpt_reset_timer(3, 1000000, false) to configure GPT3
 *	     timeout value (as 1 second) and configure it as one-shot mode.
 *
 *	- Configure GPT2 (in free-run mode, and GPT4 is similar):
 *	  - Call mtk_os_hal_gpt_config(2, 1, NULL) to configure GPT2 clock
 *	     speed (for GPT2 as 32KHz).
 *
 *	- Enable/disable GPTx (x = 0 ~ 4):
 *	  - Call mtk_os_hal_gpt_start(x) and mtk_os_hal_gpt_stop(x) to enable/
 *	     disable GPTx.
 *
 *	- Get GPTx (x = 0 ~ 4) counter value:
 *	  - Call mtk_os_hal_gpt_get_cur_count(x) to get GPTx counter value.
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
* @addtogroup gpt
* @{
*/

/** @defgroup os_hal_gpt_enum Enum
  * @{
  * This section introduces the enumerations that specify
  * the timer ID (Follow mhal_gpt.h) of the current GPT device unit.\n
  */

typedef enum {
	/** Use GPT0 as timer */
	OS_HAL_GPT0 = GPT0,
	/** Use GPT1 as timer */
	OS_HAL_GPT1 = GPT1,
	/** Use GPT2 as timer */
	OS_HAL_GPT2 = GPT2,
	/** Use GPT3 as timer */
	OS_HAL_GPT3 = GPT3,
	/** Use GPT4 as timer */
	OS_HAL_GPT4 = GPT4,
	/** The maximum GPT number (invalid) */
	OS_HAL_GPT_MAX_NUM
} GPT_ID;

/**
  * @}
  */

/** @defgroup os_hal_gpt_struct Struct
  * @{
  * This section introduces the definition of GPT user
  * interrupt handle structure.\n
  * It can be used to register GPT user interrupt handle by
  * mtk_os_hal_gpt_config().
  */

struct os_gpt_int {
	/** The user interrupt handle of GPT. */
	void (*gpt_cb_hdl)(void *);
	/** The pointer of GPT interrupt callback data, which will be passed
	  * into user interrupt handle.
	  */
	void *gpt_cb_data;
};

/**
  * @}
  */

/** @defgroup os_hal_gpt_function Function
  * @{
  * This section provides high level APIs to upper layer.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Start GPT timer count.
 *  @param [in] timer_id : GPT timer id
 *  @return
 *	0 means start successfully.\n
 *	negative means start failed.\n
 */
int mtk_os_hal_gpt_start(enum gpt_num timer_id);

/**
 * @brief  Stop GPT timer count.
 *  @param [in] timer_id : GPT timer id
 *  @return
 *	0 means stop successfully.\n
 *	negative means stop failed.\n
 */
int mtk_os_hal_gpt_stop(enum gpt_num timer_id);

/**
 * @brief  Get GPT timer counter value.
 *  @param [in] timer_id : GPT timer id
 *  @return
 *	32bit counter value.
 */
unsigned int mtk_os_hal_gpt_get_cur_count(enum gpt_num timer_id);

/**
 * @brief  Restart GPT timer count.
 *  @param [in] timer_id : GPT timer id
 *  @return
 *	0 means restart successfully.\n
 *	negative means restart failed.\n
 */
int mtk_os_hal_gpt_restart(enum gpt_num timer_id);

/**
 * @brief  Reset GPT timer mode (only works for interrupt-based timer).
 *  @param [in] timer_id : GPT timer id
 *  @param [in] count_val : GPT timeout count value
 *  @param [in] auto_repeat : open repeat mode (only works for GPT0 & GPT1)
 *  @return
 *	0 means reset timer successfully.\n
 *	negative means reset failed.\n
 */
int mtk_os_hal_gpt_reset_timer(enum gpt_num timer_id,
			       unsigned int count_val,
			       bool auto_repeat);

/**
 * @brief  Config GPT timer.
 *  @param [in] timer_id : GPT timer id
 *  @param [in] speed_32us : for GPT0, GPT1 & GPT2, it is used to choose
 *	32kHz(= 1) or 1kHz(= 0); for GPT4, it is used to choose bus_clk(= 1)
 *	or half of bus_clk(= 0); and for GPT3, it has no influence.
 *  @param [in] gpt_int : a pointer of struct os_gpt_int to set interrupt
 *	callback (only works for GPT0, GPT1 & GPT3)
 *  @return
 *	0 means config successfully.\n
 *	negative means config failed.\n
 */
int mtk_os_hal_gpt_config(enum gpt_num timer_id,
			  unsigned char speed_32us,
			  struct os_gpt_int *gpt_int);

/**
 * @brief  Register GPT irq.
 *	Called only in mtk_os_hal_gpt_init() and low power scenario.
 *  @param
 *	None
 *  @return
 *	None
 */
void mtk_os_hal_gpt_register_irq(void);

/**
 * @brief  Init GPT device.
 *	Internal assurance that it will only be executed only one time
 *  effectively.
 *  @param
 *	None
 *  @return
 *	None
 */
void mtk_os_hal_gpt_init(void);

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

#endif /* __OS_HAL_GPT_H__ */
