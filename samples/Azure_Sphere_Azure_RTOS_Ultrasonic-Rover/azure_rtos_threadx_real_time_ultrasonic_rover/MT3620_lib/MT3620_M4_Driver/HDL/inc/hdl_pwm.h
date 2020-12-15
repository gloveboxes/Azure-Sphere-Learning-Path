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

#ifndef __HDL_PWM_H__
#define __HDL_PWM_H__

#include "mhal_osai.h"

#define DISABLE_PWM_DEBUG

#define pwm_err(fmt, arg...)\
	do {\
		osai_print("[%s]: ", __func__);\
		osai_print(fmt, ##arg);\
	} while (0)

#ifndef DISABLE_PWM_DEBUG
#define pwm_debug(fmt, arg...)\
	do {\
		osai_print("[%s]: ", __func__);\
		osai_print(fmt, ##arg);\
	} while (0)
#else
#define pwm_debug(fmt, arg...)
#endif

/** @brief Defines the PWM channel number */
typedef enum {
	PWM0  = 0,
	/**< PWM channel0*/
	PWM1  = 1,
	/**< PWM channel1*/
	PWM2  = 2,
	/**< PWM channel2*/
	PWM3  = 3,
	/**< PWM channel3*/
	PWMMAX
	/**< PWM max channel <invalid>*/
}	pwm_channel;

typedef enum {
	PWM_CLK_32K = 0,
	/**< 32K clock source */
	PWM_CLK_2M,
	/**< 2M clock source */
	PWM_CLK_XTAL,
	/**< xtal clock source */
	PWM_CLK_NUM
	/**< max clock source <invalid>*/
} pwm_clk;

typedef enum{
	PWM_S0 = 0,
	/**< S0 stage*/
	PWM_S1,
	/**< S1 stage */
	PWM_STATE_NUM
	/**< max pwm 2-state stage <invalid>*/
} pwm_s0_s1_stage;

#define PWM_GLO_CTRL			0x00
#define PWM_CTRL				0x00
#define PWM_PARAM_S0			0x04
#define PWM_PARAM_S1			0x08

#define PWM_GLO_CTRL_DEFAULT	0xE400
#define PWM_CTRL_DEFAULT		0x04
#define PWM_PARAM_S0_DEFAULT	0x00
#define PWM_PARAM_S1_DEFAULT	0x00

#define PWM_DUTY_CYCLE_BASE		(1000)
#define PWM_STAY_CYCLE_MAX		(0xFFF)

#define PWM_GROUP_OFFSET		(0x100)
#define PWM_CHANNEL_OFFSET		(0x10)
#define PWM_MAX_NUM				(4)

#define PWM_GLO_CTRL_PWM3_DP_SEL_OFFSET                 (14)
#define PWM_GLO_CTRL_PWM3_DP_SEL_MASK                   (BITS(14, 15))
#define PWM_GLO_CTRL_PWM2_DP_SEL_OFFSET                 (12)
#define PWM_GLO_CTRL_PWM2_DP_SEL_MASK                   (BITS(12, 13))
#define PWM_GLO_CTRL_PWM1_DP_SEL_OFFSET                 (10)
#define PWM_GLO_CTRL_PWM1_DP_SEL_MASK                   (BITS(10, 11))
#define PWM_GLO_CTRL_PWM0_DP_SEL_OFFSET                 (8)
#define PWM_GLO_CTRL_PWM0_DP_SEL_MASK                   (BITS(8, 9))

/* PWM_GLO_CTRL */
#define PWM_GLO_CTRL_PWM_GLOBAL_RESET_OFFSET        (3)
#define PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_OFFSET      (1)
#define PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_MASK        (BITS(1, 2))
#define PWM_GLO_CTRL_GLOBAL_KICK_OFFSET             (0)

/* PWM_CTRL */
#define PWM_CTRL_S1_STAY_CYCLE_OFFSET               (20)
#define PWM_CTRL_S1_STAY_CYCLE_MASK                 (BITS(20, 31))
#define PWM_CTRL_S0_STAY_CYCLE_OFFSET               (8)
#define PWM_CTRL_S0_STAY_CYCLE_MASK                 (BITS(8, 19))
#define PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET      (5)
#define PWM_CTRL_PWM_CLOCK_EN_OFFSET                (4)
#define PWM_CTRL_PWM_IO_CTRL_OFFSET                 (3)
#define PWM_CTRL_POLARITY_OFFSET                    (2)
#define PWM_CTRL_REPLAY_MODE_OFFSET                 (1)
#define PWM_CTRL_KICK_OFFSET                        (0)

/* PWM_PARAM_S0 */
#define PWM_PARAM_S0_PWM_OFF_TIME_OFFSET            (16)
#define PWM_PARAM_S0_PWM_OFF_TIME_MASK              (BITS(16, 31))
#define PWM_PARAM_S0_PWM_ON_TIME_OFFSET             (0)
#define PWM_PARAM_S0_PWM_ON_TIME_MASK               (BITS(0, 15))

/* PWM_PARAM_S1 */
#define PWM_PARAM_S1_PWM_OFF_TIME_OFFSET            (16)
#define PWM_PARAM_S1_PWM_OFF_TIME_MASK              (BITS(16, 31))
#define PWM_PARAM_S1_PWM_ON_TIME_OFFSET             (0)
#define PWM_PARAM_S1_PWM_ON_TIME_MASK               (BITS(0, 15))

#ifdef __cplusplus
extern "C" {
#endif

void mtk_hdl_pwm_init(void __iomem *base);

void mtk_hdl_pwm_group_clock_select(void __iomem *base, pwm_clk clock_source);

void mtk_hdl_pwm_enable_clk(void __iomem *base, u8 pwm_num);

void mtk_hdl_pwm_disable_clk(void __iomem *base, u8 pwm_num);

void mtk_hdl_pwm_group_feature_enable(void __iomem *base,
		u8 pwm_num,
		u8 global_kick,
		u8 io_ctrl,
		u8 polarity);

void mtk_hdl_pwm_group_kick(void __iomem *base, u8 pwm_num);

void mtk_hdl_pwm_group_global_kick_enable(void __iomem *base, u8 pwm_num);

void mtk_hdl_pwm_group_global_kick_disable(void __iomem *base, u8 pwm_num);

void mtk_hdl_pwm_group_global_kick(void __iomem *base);

void mtk_hdl_pwm_group_config(void __iomem *base,
		u8 pwm_num,
		pwm_s0_s1_stage state,
		u16 duty_cycle,
		u32 pwm_freq);

void mtk_hdl_pwm_group_state_config(void __iomem *base,
		u8 pwm_num,
		u16 s0_stay_cycle,
		u16 s1_stay_cycle,
		u8 ucReplayMode);

void mtk_hdl_pwm_group_dpsel(void __iomem *base,
		u8 pwms,
		u8 mode);

void mtk_hdl_pwm_group_query(void __iomem *base, u8 pwm_num,
		pwm_s0_s1_stage state,
		u16 *duty_Cycle,
		u32 *pwm_freq,
		u8 *enable);

#ifdef __cplusplus
}
#endif

#endif /* __HDL_PWM_H__ */

