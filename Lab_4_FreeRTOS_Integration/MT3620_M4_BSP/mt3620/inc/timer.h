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

#ifndef __TIMER_H__
#define __TIMER_H__

/* timer information (only can be used by system but not other modules) */

#define TICK_TIMER_ID			0	/* GPT0 */
#define DELAY_TIMER_ID			2	/* GPT2 */

#define TICK_TIMER_REPEAT_MS		500

/**
 * @brief  get DELAY_TIMER counter value.
 *  @param
 *	None
 *  @return
 *	DELAY_TIMER counter value.
 */
unsigned int get_current_count(void);

/**
 * @brief  get 64-bit system timestamp.
 *  @param
 *	None
 *  @return
 *	system_timestamp (32.768kHz ticks or usec).
 *  @note
 *	Current system timestamp function relies on the update regularly by
 * TICK_TIMER interrupt handle.
 */
unsigned long long get_current_system_timestamp(void);
unsigned long long get_current_system_us(void);

/**
 * @brief  busy delay by tick count.
 *  @param [in] count: excepted delay count ticks.
 *	None
 *  @return
 *	Return when count ticks of DELAY_TIMER counter has been passed.
 */
void delay_time(unsigned int count);

/**
 * @brief  busy delay by millisecond count.
 *  @param [in] ms: excepted delay ms millisecond.
 *	None
 *  @return
 *	Return when ms millisecond of DELAY_TIMER counter has been passed.
 */
void delay_ms(unsigned int ms);

/**
 * @brief  busy delay by microsecond count.
 *  @param [in] us: excepted delay us microsecond.
 *	None
 *  @return
 *	Return when us microsecond has been passed.
 */
void delay_us(unsigned int us);

//#include <os_hal_wdt.h>

#ifdef CFG_WDT_SUPPORT

/**
 * @brief  Open the function of kick WDT periodically by TICK_TIMER.
 *  @param [in] sec: kick WDT every sec seconds
 *  @return
 *	None
 */
void mtk_timer_start_kick_wdt(unsigned int sec);

/**
 * @brief  Close the function of kick WDT periodically by TICK_TIMER.
 *  @param
 *	None
 *  @return
 *	None
 */
void mtk_timer_stop_kick_wdt(void);

#endif /* CFG_WDT_SUPPORT */

/**
 * @brief  Initializate GPT and system timers(TICK_TIMER & DELAY_TIMER).
 *  @param
 *	None
 *  @return
 *	None
 *  @note
 *	Internal assurance that it will only be executed only one time
 * effectively.
 */
void mtk_timer_init(void);


#endif /* __TIMER_H__ */

