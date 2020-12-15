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

static void _mtk_hdl_pwm_writel(void __iomem *base,
		u32 pwm_num, u64 offset, u64 val)
{
	void __iomem *reg = 0;

	reg = base + PWM_GROUP_OFFSET + (PWM_CHANNEL_OFFSET * pwm_num) + offset;

	osai_writel(val, reg);
}

static unsigned int _mtk_hdl_pwm_readl(void __iomem *base,
		u32 pwm_num, u64 offset)
{
	u32 value = 0;
	void __iomem *reg = 0;

	reg = base + PWM_GROUP_OFFSET + (PWM_CHANNEL_OFFSET * pwm_num) + offset;
	value = osai_readl(reg);

	return value;
}

void mtk_hdl_pwm_init(void __iomem *base)
{
	u32 pwm_num = 0;

	/* Restore as default values */
	osai_writel(PWM_GLO_CTRL_DEFAULT, base);
	for (pwm_num = PWM0; pwm_num < PWMMAX; pwm_num++) {
		_mtk_hdl_pwm_writel(base, pwm_num, PWM_CTRL,
			PWM_CTRL_DEFAULT);
		_mtk_hdl_pwm_writel(base, pwm_num, PWM_PARAM_S0,
			PWM_PARAM_S0_DEFAULT);
		_mtk_hdl_pwm_writel(base, pwm_num, PWM_PARAM_S1,
			PWM_PARAM_S0_DEFAULT);
	}
}

void mtk_hdl_pwm_group_clock_select(void __iomem *base,
		pwm_clk clock_source)
{
	u32 reg_val = 0;

	reg_val = osai_readl(base);

	reg_val &= (~PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_MASK);
	reg_val |= (clock_source << PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_OFFSET);
	osai_writel(reg_val, base);
}

void mtk_hdl_pwm_enable_clk(void __iomem *base,
		u8 pwm_num)
{
	u32 reg_val = 0;

	reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_CTRL);

	/*Enable clock*/
	reg_val |= (BIT(PWM_CTRL_PWM_CLOCK_EN_OFFSET));
	_mtk_hdl_pwm_writel(base, pwm_num, PWM_CTRL, reg_val);
}

void mtk_hdl_pwm_disable_clk(void __iomem *base,
		u8 pwm_num)
{
	u32 reg_val = 0;

	/*Disable clock*/
	reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_CTRL);

	reg_val &= (~BIT(PWM_CTRL_PWM_CLOCK_EN_OFFSET));
	_mtk_hdl_pwm_writel(base, pwm_num, PWM_CTRL, reg_val);
}

void mtk_hdl_pwm_group_feature_enable(void __iomem *base,
	u8 pwm_num, u8 global_kick, u8 io_ctrl, u8 polarity)
{
	u32 reg_val = 0;

	reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_CTRL);

	/*Configure PWM*/
	if (global_kick)
		reg_val |= (BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
	else
		reg_val &= (~BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));

	if (io_ctrl)
		reg_val |= (BIT(PWM_CTRL_PWM_IO_CTRL_OFFSET));
	else
		reg_val &= (~BIT(PWM_CTRL_PWM_IO_CTRL_OFFSET));


	if (polarity)
		reg_val |= (BIT(PWM_CTRL_POLARITY_OFFSET));
	else
		reg_val &= (~BIT(PWM_CTRL_POLARITY_OFFSET));


	_mtk_hdl_pwm_writel(base, pwm_num, PWM_CTRL, reg_val);
}

void mtk_hdl_pwm_group_kick(void __iomem *base,
	u8 pwm_num)
{
	u32 reg_val = 0;

	reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_CTRL);

	reg_val |= (BIT(PWM_CTRL_KICK_OFFSET));

	_mtk_hdl_pwm_writel(base, pwm_num, PWM_CTRL, reg_val);
}

void mtk_hdl_pwm_group_global_kick_enable(void __iomem *base,
	u8 pwm_num)
{
	u32 reg_val = 0;

	switch (pwm_num) {
	case PWM0:
		reg_val = _mtk_hdl_pwm_readl(base, PWM0, PWM_CTRL);
		reg_val |= (BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM0, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM1, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM1, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM2, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM2, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM3, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM3, PWM_CTRL, reg_val);

		break;
	case PWM1:
		reg_val = _mtk_hdl_pwm_readl(base, PWM1, PWM_CTRL);
		reg_val |= (BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM1, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM0, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM0, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM2, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM2, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM3, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM3, PWM_CTRL, reg_val);

		break;
	case PWM2:
		reg_val = _mtk_hdl_pwm_readl(base, PWM2, PWM_CTRL);
		reg_val |= (BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM2, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM1, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM1, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM0, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM0, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM3, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM3, PWM_CTRL, reg_val);

		break;
	case PWM3:

		reg_val = _mtk_hdl_pwm_readl(base, PWM0, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM0, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM1, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM1, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM2, PWM_CTRL);
		reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM2, PWM_CTRL, reg_val);

		reg_val = _mtk_hdl_pwm_readl(base, PWM3, PWM_CTRL);
		reg_val |= (BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
		_mtk_hdl_pwm_writel(base, PWM3, PWM_CTRL, reg_val);

		break;
	default:
		break;
	}
}

void mtk_hdl_pwm_group_global_kick_disable(void __iomem *base,
	u8 pwm_num)
{
	u32 reg_val = 0;

	reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_CTRL);
	reg_val &= ~(BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
	_mtk_hdl_pwm_writel(base, pwm_num, PWM_CTRL, reg_val);
}
void mtk_hdl_pwm_group_global_kick(void __iomem *base)
{
	u32 reg_val = 0;

	reg_val = osai_readl(base);

	reg_val |= (BIT(PWM_GLO_CTRL_GLOBAL_KICK_OFFSET));

	osai_writel(reg_val, base);

}

void mtk_hdl_pwm_group_config(void __iomem *base, u8 pwm_num,
		pwm_s0_s1_stage stage, u16 duty_cycle, u32 pwm_freq)
{
	u32 clock_source = 0;
	u32 clk_freq = 0;
	u32 pwm_step = 0;
	u32 on_time = 0;
	u32 off_time = 0;
	u32 reg_val = 0;

	reg_val = osai_readl(base + PWM_GLO_CTRL);
	reg_val &= (PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_MASK);

	clock_source = reg_val >> PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_OFFSET;
	pwm_debug("mtk hdl_pwm_group_config clock_source ==%x.\n",
		clock_source);
	switch (clock_source) {
	case PWM_CLK_32K:
		clk_freq = (32700);
		break;
	case PWM_CLK_2M:
		clk_freq = (2 * 1000 * 1000);
		break;
	case PWM_CLK_XTAL:
		clk_freq = (26000000);
		break;

	default:
		break;
	}
	pwm_debug("mtk hdl_pwm_group_config clock_source ==%d.\n",
		clk_freq);
	/*
	* T(second)	PWM period
	* F (Hz)		PWM frequency = 1/T
	* t (second)  Tick clk period
	* f (Hz)		Tick clk frequency = 1/t
	* D (%)		Duty cycle
	* X (unit t)	Value of configurable register pwm_on_time[15:0],
	* in unit t
	* Y (unit t)	Value of configurable register pwm_off_time[15:0],
	* in unit t
	* Res (step)	PWM resolution of duty cycle on certain F, f

	* (X + Y) t = T
	* (X + Y) = T/t = f/F =Res
	* D = X/((X+Y))
	* X = D(X+Y) =  Df/F

	Y = f/F - X = f/F - Df/F = ((1-D)f)/F
	*/

	pwm_step = (clk_freq / pwm_freq);

	on_time = (duty_cycle * pwm_step) / PWM_DUTY_CYCLE_BASE;
	off_time = pwm_step - on_time;
	pwm_debug("mtk hdl_pwm_group_config pwm_freq ==%d,duty_cycle =%d.\n",
		pwm_freq, duty_cycle);

	pwm_debug("on_time == %x,off_time ==%x.\n", on_time, off_time);
	if (off_time == 1) {
		off_time = 0;
		on_time += 1;
	}

	if (stage == PWM_S0) {
		pwm_debug("on_time == %x,off_time ==%x.\n", on_time, off_time);
		reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_PARAM_S0);
		reg_val &= (~PWM_PARAM_S0_PWM_OFF_TIME_MASK);
		reg_val |= (off_time << PWM_PARAM_S0_PWM_OFF_TIME_OFFSET);
		reg_val &= (~PWM_PARAM_S0_PWM_ON_TIME_MASK);
		reg_val |= (on_time << PWM_PARAM_S0_PWM_ON_TIME_OFFSET);
		_mtk_hdl_pwm_writel(base, pwm_num, PWM_PARAM_S0, reg_val);
	} else {
		reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_PARAM_S1);
		reg_val &= (~PWM_PARAM_S1_PWM_OFF_TIME_MASK);
		reg_val |= (off_time << PWM_PARAM_S1_PWM_OFF_TIME_OFFSET);
		reg_val &= (~PWM_PARAM_S1_PWM_ON_TIME_MASK);
		reg_val |= (on_time << PWM_PARAM_S1_PWM_ON_TIME_OFFSET);
		_mtk_hdl_pwm_writel(base, pwm_num, PWM_PARAM_S1, reg_val);
	}

}

void mtk_hdl_pwm_group_state_config(void __iomem *base,
		u8 pwm_num, u16 s0_stay_cycle,
		u16 s1_stay_cycle, u8 replay_mode)
{
	u32 reg_val = 0;

	reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_CTRL);

	reg_val &= (~PWM_CTRL_S0_STAY_CYCLE_MASK);
	reg_val |= (s0_stay_cycle << PWM_CTRL_S0_STAY_CYCLE_OFFSET);

	reg_val &= (~PWM_CTRL_S1_STAY_CYCLE_MASK);
	reg_val |= (s1_stay_cycle << PWM_CTRL_S1_STAY_CYCLE_OFFSET);

	if (replay_mode)
		reg_val |= (BIT(PWM_CTRL_REPLAY_MODE_OFFSET));
	else
		reg_val &= (~BIT(PWM_CTRL_REPLAY_MODE_OFFSET));

	_mtk_hdl_pwm_writel(base, pwm_num, PWM_CTRL, reg_val);
}

void mtk_hdl_pwm_group_dpsel(void __iomem *base,
		u8 pwm_num, u8 mode)
{
	u32 reg_val = 0;

	if (pwm_num == 0) {
		reg_val = osai_readl(base);
		reg_val &= (~PWM_GLO_CTRL_PWM0_DP_SEL_MASK);
		reg_val |= (mode << PWM_GLO_CTRL_PWM0_DP_SEL_OFFSET);
		osai_writel(reg_val, base);
	} else if (pwm_num == 1) {
		reg_val = osai_readl(base);
		reg_val &= (~PWM_GLO_CTRL_PWM1_DP_SEL_MASK);
		reg_val |= (mode << PWM_GLO_CTRL_PWM1_DP_SEL_OFFSET);
		osai_writel(reg_val, base);
	} else if (pwm_num == 2) {
		reg_val = osai_readl(base);
		reg_val &= (~PWM_GLO_CTRL_PWM2_DP_SEL_MASK);
		reg_val |= (mode << PWM_GLO_CTRL_PWM2_DP_SEL_OFFSET);
		osai_writel(reg_val, base);
	} else if (pwm_num == 3) {
		reg_val = osai_readl(base);
		reg_val &= (~PWM_GLO_CTRL_PWM3_DP_SEL_MASK);
		reg_val |= (mode << PWM_GLO_CTRL_PWM3_DP_SEL_OFFSET);
		osai_writel(reg_val, base);
	}
}

void mtk_hdl_pwm_group_query(void __iomem *base, u8 pwm_num,
	pwm_s0_s1_stage stage, u16 *duty_cycle, u32 *pwm_freq, u8 *enable)
{
	u32 clock_source = 0;
	u32 clk_freq = 0;
	u32 pwm_step = 0;
	u32 on_time = 0;
	u32 off_time = 0;
	u32 reg_val = 0;

	reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_CTRL);

	if (reg_val & BIT(PWM_CTRL_PWM_CLOCK_EN_OFFSET))
		(*enable) = 1;
	else
		(*enable) = 0;

	reg_val = osai_readl(base + PWM_GLO_CTRL);
	reg_val &= (PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_MASK);

	clock_source =  reg_val >> PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_OFFSET;
	pwm_debug("mtk hdl_pwm_group_query clock_source ==%d.\n",
		clock_source);
	switch (clock_source) {
	case PWM_CLK_32K:
		clk_freq = (32700);
		break;

	case PWM_CLK_2M:
		clk_freq = (2 * 1000 * 1000);
		break;

	case PWM_CLK_XTAL:
		clk_freq = (26000000);
		break;

	default:
		break;
	}

	/*
	* T(second)	PWM period
	* F (Hz)		PWM frequency = 1/T
	* t (second)  Tick clk period
	* f (Hz)		Tick clk frequency = 1/t
	* D (%)		Duty cycle
	* X (unit t)	Value of configurable register pwm_on_time[15:0],
	* in unit t
	* Y (unit t)	Value of configurable register pwm_off_time[15:0],
	* in unit t
	* Res (step)	PWM resolution of duty cycle on certain F, f

	* (X + Y) t = T
	* (X + Y) = T/t = f/F =Res
	* D = X/((X+Y))
	* X = D(X+Y) =  Df/F

	Y = f/F - X = f/F - Df/F = ((1-D)f)/F
	*/

	if (stage == PWM_S0) {
		reg_val = _mtk_hdl_pwm_readl(base, pwm_num, PWM_PARAM_S0);
		on_time = (reg_val & PWM_PARAM_S0_PWM_ON_TIME_MASK);
		on_time = (on_time >> PWM_PARAM_S0_PWM_ON_TIME_OFFSET);
		off_time = (reg_val & PWM_PARAM_S0_PWM_OFF_TIME_MASK);
		off_time = (off_time >> PWM_PARAM_S0_PWM_OFF_TIME_OFFSET);
	}

	pwm_step = on_time + off_time;

	if (pwm_step != 0)
		(*pwm_freq) = (clk_freq / pwm_step);

	(*duty_cycle) = on_time;
	pwm_debug("mtk hdl_pwm_group_query *pwm_freq == %x.\n", *pwm_freq);
}

