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

#include "hdl_pwm.h"
#include "mhal_pwm.h"

static bool _mhal_pwm_is_channel_valid(pwm_channels pwm_num)
{
	return pwm_num < PWM_CHANNEL_MAX;
}
static int _mhal_pwm_is_dp_mode_valid(
	pwm_differential_select differential_mode)
{
	return (differential_mode >= PWM_DP_NUM) ? PWM_EPARAMETER : 0;
}
static void _mhal_pwm_calc_total_count(u32 source_clock,
		u32 frequency,
		u32 *total_count)
{
	static const u32 source_clock_table[] = {
		32700, 2*1000*1000, 26*1000*1000
	};
	u32 source_clock_rate = source_clock_table[source_clock];

	*total_count = source_clock_rate/frequency;
}

static void _mtk_mhal_pwm_group_init(void __iomem *base, pwm_clks clock_source)
{
	mtk_hdl_pwm_init(base);
	mtk_hdl_pwm_group_clock_select(base, (pwm_clk)clock_source);
}

static void _mtk_mhal_pwm_group_set(void __iomem *base, u8 index,
		u32 frequency, u16 duty_cycle, u8 enable)
{
	pwm_s0_s1_stage state = PWM_S0;
	u16 duty_step = 0;
	u16 s0_stay_cycle = 1;
	u16 s1_stay_cycle = 1;
	u16 low_level = 0;
	u8 replay_mode = 1;

	if (enable == 0) {
		/*Set duty_cycle to make sure PWM to be idle.*/
		mtk_hdl_pwm_group_config(base, index,
				state, low_level, frequency);
		pwm_debug("_mtk_mhal_pwm_group_set frequency == %d.\n",
			frequency);
		mtk_hdl_pwm_group_global_kick_enable(base, index);
		mtk_hdl_pwm_group_global_kick(base);
		mtk_hdl_pwm_group_kick(base, index);
		mtk_hdl_pwm_group_global_kick_disable(base, index);

		/*Disable PWM clock*/
		mtk_hdl_pwm_disable_clk(base, index);
	} else {
		duty_step = duty_cycle;

		mtk_hdl_pwm_group_config(base, index,
				state, duty_step, frequency);
		mtk_hdl_pwm_group_config(base, index,
				PWM_S1, duty_step, frequency);
		mtk_hdl_pwm_group_state_config(base, index,
				s0_stay_cycle, s1_stay_cycle, replay_mode);
		mtk_hdl_pwm_group_kick(base, index);
	}

}

static void _mtk_mhal_pwm_group_get(void __iomem *base, pwm_channels index,
		u32 *frequency, u16 *duty_cycle, u8 *status)
{
	pwm_s0_s1_stage	state = PWM_S0;

	mtk_hdl_pwm_group_query(base, index,
			state, duty_cycle, (u32 *)frequency, (u8 *)status);
}

int mtk_mhal_pwm_clock_select(struct mtk_pwm_controller *ctlr)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (ctlr->group_clock >= PWM_CLOCK_NUM) {
		pwm_err("invalid source clock: %d.\n", ctlr->group_clock);
		return -PWM_EPARAMETER;
	}

	mtk_hdl_pwm_group_clock_select(ctlr->base, (pwm_clk)ctlr->group_clock);

	return 0;
}

int mtk_mhal_pwm_enable_clk(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}
	mtk_hdl_pwm_enable_clk(ctlr->base, pwm_num);

	return 0;
}

int mtk_mhal_pwm_disable_clk(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}
	mtk_hdl_pwm_disable_clk(ctlr->base, pwm_num);

	return 0;
}

int mtk_mhal_pwm_init(struct mtk_pwm_controller *ctlr,
	u32 channel_bit_map)
{
	u32 channel_count = 0;
	u32 channel_num = 0;

	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (ctlr->group_number >= MAX_GROUP_NUM) {
		pwm_err("group number should be 0~%d.\n", MAX_GROUP_NUM - 1);
		return -PWM_EPARAMETER;
	}

	if (ctlr->group_clock >= PWM_CLOCK_NUM) {
		pwm_err("source clock:should be 0~%d.\n", PWM_CLOCK_NUM - 1);
		ctlr->group_clock = PWM_CLOCK_2M;
		return -PWM_ECLK;
	}

	if (!ctlr->data)
		return -PWM_EPTR;

	if (channel_bit_map & 0xFFFFFF00)
		return -PWM_EPARAMETER;

	_mtk_mhal_pwm_group_init(ctlr->base, ctlr->group_clock);

	for (channel_count = 0; channel_count < PWMMAX;
	channel_count++) {
		if (channel_bit_map & BIT(channel_count))
			channel_num++;
	}

	ctlr->data->pwm_nums = channel_num;

	return 0;
}

int mtk_mhal_pwm_deinit(struct mtk_pwm_controller *ctlr,
		u32 channel_bit_map)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (channel_bit_map & 0xFFFFFF00)
		return -PWM_EPARAMETER;

	if (channel_bit_map & 0x1)
		mtk_hdl_pwm_disable_clk(ctlr->base, PWM0);

	if (channel_bit_map & 0x2)
		mtk_hdl_pwm_disable_clk(ctlr->base, PWM1);

	if (channel_bit_map & 0x4)
		mtk_hdl_pwm_disable_clk(ctlr->base, PWM2);

	if (channel_bit_map & 0x8)
		mtk_hdl_pwm_disable_clk(ctlr->base, PWM3);

	ctlr->group_clock = PWM_CLOCK_NUM;

	return 0;
}

int mtk_mhal_pwm_set_frequency(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	u32 calculated = 0;

	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}
	if (ctlr->data->frequency == 0) {
		pwm_err("frequency cannot be %d.\n",
		ctlr->data->frequency);
		return -PWM_EPARAMETER;
	}
	_mhal_pwm_calc_total_count(ctlr->group_clock,
		ctlr->data->frequency,
		&calculated);
	_mtk_mhal_pwm_group_set(ctlr->base,
		pwm_num,
		ctlr->data->frequency,
		0,
		1);

	ctlr->data->current_frequency = ctlr->data->frequency;

	ctlr->data->total_count = calculated;
	pwm_debug("mtk mhal_pwm_set_frequency ,calculated == %d.\n",
		calculated);

	return 0;
}


int mtk_mhal_pwm_set_duty_cycle(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	u32 frequency = 0;
	u16 internal_duty = 0;
	u8 pwm_enable = 0;
	u32 total_count = 0;

	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	if (ctlr->data->duty_cycle > PWM_DUTY_CYCLE_BASE) {
		pwm_err("duty_cycle cannot be greater than %d.\n",
		ctlr->data->duty_cycle);
		return -PWM_EPARAMETER;
	}

	_mtk_mhal_pwm_group_get(ctlr->base,
		pwm_num,
		&frequency,
		&internal_duty,
		&pwm_enable);

	pwm_debug("mtk mhal_pwm_set_duty_cycle ,frequency == %d.\n",
		frequency);

	if (frequency == 0) {
		pwm_err("frequency is invalid.\n");
		return -PWM_EPARAMETER;
	}

	pwm_debug("mtk mhal_pwm_set_duty_cycle.\n");

	_mhal_pwm_calc_total_count(ctlr->group_clock,
		frequency, &total_count);

	_mtk_mhal_pwm_group_set(ctlr->base,
		pwm_num,
		frequency,
		ctlr->data->duty_cycle,
		pwm_enable);

	ctlr->data->current_duty_cycle = ctlr->data->duty_cycle;

	return 0;
}

int mtk_mhal_pwm_start(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	u32 frequency = 0;
	u16 duty_cycle = 0;
	u8 pwm_enable = 0;

	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	_mtk_mhal_pwm_group_get(ctlr->base,
		pwm_num,
		&frequency,
		&duty_cycle,
		&pwm_enable);

	pwm_enable = 1;

	pwm_debug("frequency is =%d,duty =%d.\n", frequency, duty_cycle);
	_mtk_mhal_pwm_group_set(ctlr->base,
		pwm_num,
		ctlr->data->current_frequency,
		ctlr->data->current_duty_cycle,
		pwm_enable
		);

	mtk_hdl_pwm_enable_clk(ctlr->base, pwm_num);

	return 0;
}


int mtk_mhal_pwm_stop(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	u32 frequency = 0;
	u16 duty_cycle = 0;
	u8 pwm_enable = 0;

	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	_mtk_mhal_pwm_group_get(ctlr->base,
		pwm_num,
		&frequency,
		&duty_cycle,
		&pwm_enable);

	if (frequency == 0) {
		pwm_err("frequency is invalid.\n");
		return -PWM_EPARAMETER;
	}

	pwm_enable = 0;

	_mtk_mhal_pwm_group_set(ctlr->base,
		pwm_num,
		frequency,
		duty_cycle,
		pwm_enable
		);

	return 0;
}

int mtk_mhal_pwm_get_frequency(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	u16 duty_cycle = 0;
	u32 working_frequency = 0;
	u8 pwm_enable = false;

	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}
	if (ctlr->data == NULL) {
		pwm_err("data is null.\n");
		return -PWM_EPTR;
	}
	_mtk_mhal_pwm_group_get(ctlr->base,
		pwm_num,
		&working_frequency,
		&duty_cycle,
		&pwm_enable);

	ctlr->data->current_frequency = working_frequency;

	return 0;
}

int mtk_mhal_pwm_get_duty_cycle(struct mtk_pwm_controller *ctlr,
	pwm_channels pwm_num)
{
	u32 frequency = 0;
	u16 working_duty_cycle = 0;
	u8 pwm_enable = 0;
	u32 total_count = 0;

	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (ctlr->group_clock >= PWM_CLOCK_NUM) {
		pwm_err("pwm not initilized.\n");
		return -PWM_EPARAMETER;
	}

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	_mtk_mhal_pwm_group_get(ctlr->base,
		pwm_num,
		&frequency,
		&working_duty_cycle,
		&pwm_enable);

	if (frequency == 0) {
		pwm_err("frequency is invalid.\n");
		return -PWM_EPARAMETER;
	}

	_mhal_pwm_calc_total_count(ctlr->group_clock,
		frequency,
		&total_count);

	ctlr->data->current_duty_cycle = working_duty_cycle;

	return 0;
}

int mtk_mhal_pwm_get_running_status(struct mtk_pwm_controller *ctlr,
	pwm_channels pwm_num)
{
	u32 frequency = 0;
	u16 duty_cycle = 0;
	u8 pwm_enable = false;

	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	_mtk_mhal_pwm_group_get(ctlr->base,
		pwm_num,
		&frequency,
		&duty_cycle,
		&pwm_enable);

	ctlr->data->running_status = pwm_enable ? PWM_BUSY : PWM_IDLE;

	return 0;
}

int mtk_mhal_pwm_feature_enable(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	if ((ctlr->data->global_kick_enable != 0) &&
			(ctlr->data->global_kick_enable != 1)) {
		pwm_err("Invalid global_kick_enable: %d.\n",
			ctlr->data->global_kick_enable);
		return -PWM_EPARAMETER;
	}

	if ((ctlr->data->io_ctrl_sel != 0) &&
			(ctlr->data->io_ctrl_sel != 1)) {
		pwm_err("Invalid io_ctrl_sel: %d.\n",
			ctlr->data->io_ctrl_sel);
		return -PWM_EPARAMETER;
	}

	if ((ctlr->data->polarity_set != 0) &&
			(ctlr->data->polarity_set != 1)) {
		pwm_err("Invalid polarity_set: %d.\n",
			ctlr->data->polarity_set);
		return -PWM_EPARAMETER;
	}

	mtk_hdl_pwm_group_feature_enable(ctlr->base,
		pwm_num,
		ctlr->data->global_kick_enable,
		ctlr->data->io_ctrl_sel,
		ctlr->data->polarity_set);

	return 0;
}

int mtk_mhal_pwm_config_s0_s1_freq_duty(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	if (ctlr->data->frequency == 0) {
		pwm_err("Invalid stage: %d.\n", ctlr->data->stage);
		return -PWM_EPARAMETER;
	}

	if (ctlr->data->duty_cycle > PWM_DUTY_CYCLE_BASE) {
		pwm_err("Invalid stage: %d.\n", ctlr->data->stage);
		return -PWM_EPARAMETER;
	}

	if (ctlr->data->stage >= PWM_STAGE_MAX) {
		pwm_err("Invalid stage: %d.\n", ctlr->data->stage);
		return -PWM_EPARAMETER;
	}

	pwm_debug("ctlr->data->stage %d.\n", ctlr->data->stage);
	pwm_debug("ctlr->data->frequency %d.\n", ctlr->data->frequency);
	pwm_debug("ctlr->data->duty_cycle %d.\n", ctlr->data->duty_cycle);
	mtk_hdl_pwm_group_config(ctlr->base,
		pwm_num,
		(pwm_s0_s1_stage)ctlr->data->stage,
		ctlr->data->duty_cycle,
		ctlr->data->frequency
		);

	return 0;
}
int mtk_mhal_pwm_s0_s1_stay_cycle_config(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	if ((ctlr->data->replay_mode != 0) &&
			(ctlr->data->replay_mode != 1)) {
		pwm_err("Invalid replay_mode: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}
	/*
	*Stay cycle register only has 12 bits
	*/
	if ((ctlr->data->s0_stay_cycle & 0xF000) ||
		((ctlr->data->s1_stay_cycle & 0xF000))) {
		pwm_err("Invalid s0_stay_cycle: %d.\n",
			ctlr->data->s0_stay_cycle);
		pwm_err("Invalid s1_stay_cycle: %d.\n",
			ctlr->data->s0_stay_cycle);
		return -PWM_EPARAMETER;
	}

	pwm_debug("ctlr->data->s0_stay_cycle %d.\n",
	ctlr->data->s0_stay_cycle);
	pwm_debug("ctlr->data->s1_stay_cycle %d.\n",
		ctlr->data->s1_stay_cycle);
	pwm_debug("ctlr->data->replay_mode %d.\n",
		ctlr->data->replay_mode);
	mtk_hdl_pwm_group_state_config(ctlr->base,
		pwm_num,
		ctlr->data->s0_stay_cycle,
		ctlr->data->s1_stay_cycle,
		ctlr->data->replay_mode);

	return 0;
}

int mtk_mhal_pwm_kick(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	mtk_hdl_pwm_group_kick(ctlr->base,
		pwm_num);

	return 0;
}

int mtk_mhal_pwm_global_kick(struct mtk_pwm_controller *ctlr)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	mtk_hdl_pwm_group_global_kick(ctlr->base + PWM_GLO_CTRL);

	return 0;
}

int mtk_mhal_pwm_dpsel(struct mtk_pwm_controller *ctlr,
		pwm_channels pwm_num)
{
	if (!ctlr)
		return -PWM_EPTR;

	if (!ctlr->base)
		return -PWM_EPTR;

	if (!ctlr->data)
		return -PWM_EPTR;

	if (!_mhal_pwm_is_channel_valid(pwm_num)) {
		pwm_err("Invalid channel: %d.\n", pwm_num);
		return -PWM_EPARAMETER;
	}

	if (_mhal_pwm_is_dp_mode_valid(ctlr->data->mode)) {
		pwm_err("Invalid differential mode select: %d.\n",
			ctlr->data->mode);
		return -PWM_EPARAMETER;
	}

	mtk_hdl_pwm_group_dpsel(ctlr->base,
		pwm_num,
		ctlr->data->mode);

	return 0;
}

