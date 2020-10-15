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

#include "os_hal_pwm.h"

#define GROUP0_PWM_BASE			0x38011000
#define GROUP1_PWM_BASE			0x38021000
#define GROUP2_PWM_BASE			0x38031000

static const unsigned long pwm_base_addr[MAX_GROUP_NUM] = {
	GROUP0_PWM_BASE,
	GROUP1_PWM_BASE,
	GROUP2_PWM_BASE,
};

static const pwm_clks pwm_clock_source[MAX_GROUP_NUM] = {
	PWM_CLOCK_XTAL,
	PWM_CLOCK_XTAL,
	PWM_CLOCK_XTAL
};

static const unsigned long group0_register[MAX_CHANNEL_NUM] = {
	0x100, 0x110, 0x120, 0x130
};

static const unsigned long group1_register[MAX_CHANNEL_NUM] = {
	0x100, 0x110, 0x120, 0x130
};

static const unsigned long group2_register[MAX_CHANNEL_NUM] = {
	0x100, 0x110, 0x120, 0x130
};

static struct mtk_com_pwm_data group0_pwm_data = {
	.pwm_register = &group0_register[0],
	.pwm_nums = 4,
	.index = 0,
};
static struct mtk_com_pwm_data group1_pwm_data = {
	.pwm_register = &group1_register[0],
	.pwm_nums = 4,
	.index = 4,
};
static struct mtk_com_pwm_data group2_pwm_data = {
	.pwm_register = &group2_register[0],
	.pwm_nums = 4,
	.index = 8,
};

struct mtk_com_pwm_data *pwm_common_data[MAX_GROUP_NUM] = {
	&group0_pwm_data, &group1_pwm_data, &group2_pwm_data
};

static struct mtk_pwm_controller g_pwm_controller[MAX_GROUP_NUM];

struct mtk_pwm_controller_rtos {
	struct mtk_pwm_controller *ctlr;
};

static struct mtk_pwm_controller_rtos g_pwm_ctlr_rtos[MAX_GROUP_NUM];

static struct mtk_pwm_controller_rtos *_mtk_os_hal_pwm_get_ctlr(
	pwm_groups group_num)
{
	return &g_pwm_ctlr_rtos[group_num];
}

int mtk_os_hal_pwm_ctlr_init(pwm_groups group_num, u32 channel_bit_map)
{
	struct mtk_pwm_controller_rtos *ctlr_rtos;
	struct mtk_pwm_controller *ctlr;
	int ret = 0;

	ctlr_rtos = _mtk_os_hal_pwm_get_ctlr(group_num);
	if (!ctlr_rtos)
		return -PWM_EPTR;

	ctlr_rtos->ctlr = &g_pwm_controller[group_num];

	if (!ctlr_rtos->ctlr)
		return -PWM_EPTR;

	ctlr = ctlr_rtos->ctlr;

	ctlr->base = (void __iomem *)pwm_base_addr[group_num];
	ctlr->group_number = group_num;
	ctlr->group_clock = pwm_clock_source[group_num];

	ctlr->data = pwm_common_data[group_num];

	ret = mtk_mhal_pwm_init(ctlr, channel_bit_map);
	if (ret)
		return ret;

	return 0;
}

int mtk_os_hal_pwm_ctlr_deinit(pwm_groups group_num, u32 channel_bit_map)
{
	struct mtk_pwm_controller_rtos *ctlr_rtos;
	struct mtk_pwm_controller *ctlr;
	int ret = 0;
	u32	bit_map = 0;

	ctlr_rtos = _mtk_os_hal_pwm_get_ctlr(group_num);
	if (!ctlr_rtos)
		return -PWM_EPTR;

	ctlr = ctlr_rtos->ctlr;
	bit_map = channel_bit_map;

	ret = mtk_mhal_pwm_deinit(ctlr, bit_map);
	if (ret)
		return ret;

	return 0;
}

int mtk_os_hal_pwm_config_freq_duty_normal(pwm_groups group_num,
		pwm_channels pwm_num, u32  frequency, u32  duty_cycle)
{
	struct mtk_pwm_controller_rtos *ctlr_rtos;
	struct mtk_pwm_controller *ctlr;
	int ret = 0;

	ctlr_rtos = _mtk_os_hal_pwm_get_ctlr(group_num);
	if (!ctlr_rtos)
		return -PWM_EPTR;

	ctlr = ctlr_rtos->ctlr;

	ctlr->data->frequency = frequency;
	ctlr->data->duty_cycle = duty_cycle;

	ret = mtk_mhal_pwm_set_frequency(ctlr_rtos->ctlr, pwm_num);
	if (ret)
		return ret;

	ret = mtk_mhal_pwm_set_duty_cycle(ctlr_rtos->ctlr, pwm_num);
	if (ret)
		return ret;

	return 0;
}

int mtk_os_hal_pwm_feature_enable(pwm_groups group_num,
		pwm_channels pwm_num,
		bool global_kick_enable,
		bool io_ctrl_sel,
		bool polarity_set)
{
	struct mtk_pwm_controller_rtos *ctlr_rtos;
	struct mtk_pwm_controller *ctlr;
	int ret = 0;

	ctlr_rtos = _mtk_os_hal_pwm_get_ctlr(group_num);
	if (!ctlr_rtos)
		return -PWM_EPTR;

	ctlr = ctlr_rtos->ctlr;

	ctlr->data->global_kick_enable = global_kick_enable;
	ctlr->data->io_ctrl_sel = io_ctrl_sel;
	ctlr->data->polarity_set = polarity_set;

	ret = mtk_mhal_pwm_feature_enable(ctlr_rtos->ctlr, pwm_num);
	if (ret)
		return ret;

	return 0;
}

int mtk_os_hal_pwm_config_freq_duty_2_state(pwm_groups group_num,
		pwm_channels pwm_num,
		struct mtk_com_pwm_data state_config)
{
	struct mtk_pwm_controller_rtos *ctlr_rtos;
	struct mtk_pwm_controller *ctlr;
	int ret = 0;

	ctlr_rtos = _mtk_os_hal_pwm_get_ctlr(group_num);
	if (!ctlr_rtos)
		return -PWM_EPTR;

	ctlr = ctlr_rtos->ctlr;

	ctlr->data->frequency = state_config.frequency;
	ctlr->data->duty_cycle = state_config.duty_cycle;
	ctlr->data->stage = state_config.stage;

	ret = mtk_mhal_pwm_config_s0_s1_freq_duty(ctlr_rtos->ctlr, pwm_num);
	if (ret)
		return ret;

	return 0;
}
int mtk_os_hal_pwm_config_stay_cycle_2_state(pwm_groups group_num,
		pwm_channels pwm_num,
		struct mtk_com_pwm_data state_config)
{
	struct mtk_pwm_controller_rtos *ctlr_rtos;
	struct mtk_pwm_controller *ctlr;
	int ret = 0;

	ctlr_rtos = _mtk_os_hal_pwm_get_ctlr(group_num);
	if (!ctlr_rtos)
		return -PWM_EPTR;

	ctlr = ctlr_rtos->ctlr;

	ctlr->data->s0_stay_cycle = state_config.s0_stay_cycle;
	ctlr->data->s1_stay_cycle = state_config.s1_stay_cycle;
	ctlr->data->replay_mode = state_config.replay_mode;

	ret = mtk_mhal_pwm_s0_s1_stay_cycle_config(ctlr_rtos->ctlr,
			pwm_num);
	if (ret)
		return ret;

	ret = mtk_mhal_pwm_enable_clk(ctlr_rtos->ctlr, pwm_num);
	if (ret)
		return ret;

	ret = mtk_mhal_pwm_kick(ctlr_rtos->ctlr, pwm_num);
	if (ret)
		return ret;

	return 0;
}

int mtk_os_hal_pwm_config_dpsel(pwm_groups group_num,
	pwm_channels pwm_num,
	pwm_differential_select mode)
{
	struct mtk_pwm_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_pwm_get_ctlr(group_num);

	if (!ctlr_rtos)
		return -PWM_EPTR;

	ctlr_rtos->ctlr->data->mode = mode;

	return mtk_mhal_pwm_dpsel(ctlr_rtos->ctlr, pwm_num);
}

int mtk_os_hal_pwm_start_normal(pwm_groups group_num,
		pwm_channels pwm_num)
{
	struct mtk_pwm_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_pwm_get_ctlr(group_num);

	if (!ctlr_rtos)
		return -PWM_EPTR;

	return mtk_mhal_pwm_start(ctlr_rtos->ctlr, pwm_num);
}

int mtk_os_hal_pwm_stop_normal(pwm_groups group_num,
		pwm_channels pwm_num)
{
	struct mtk_pwm_controller_rtos *ctlr_rtos =
	    _mtk_os_hal_pwm_get_ctlr(group_num);

	if (!ctlr_rtos)
		return -PWM_EPTR;

	return mtk_mhal_pwm_stop(ctlr_rtos->ctlr, pwm_num);
}

