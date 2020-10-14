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

#include "os_hal_gpio.h"

#define CA7_GPIO_BASE				0x30020000
#define CM4_ADC_BASE				0x38000000
#define CM4_GPIO_PWM_GRP0_BASE	0x38010000
#define CM4_GPIO_PWM_GRP1_BASE	0x38020000
#define CM4_GPIO_PWM_GRP2_BASE	0x38030000
#define CM4_GPIO_PWM_GRP3_BASE	0x38040000
#define CM4_GPIO_PWM_GRP4_BASE	0x38050000
#define CM4_GPIO_PWM_GRP5_BASE	0x38060000

#define CM4_ISU0_I2C_BASE			0x38070000
#define CM4_ISU1_I2C_BASE			0x38080000
#define CM4_ISU2_I2C_BASE			0x38090000
#define CM4_ISU3_I2C_BASE			0x380A0000
#define CM4_ISU4_I2C_BASE			0x380B0000

#define CM4_I2S0_BASE				0x380D0000
#define CM4_I2S1_BASE				0x380E0000
#define PINMUX_BASE				0x30010000

/** The number of gpio mode which cover bits in per reg. */
#define   GPIO_MODE_BITS	4
/** The max gpio mode in per reg. */
#define  MAX_GPIO_MODE_PER_REG	8
/** The shift between two registers. */
#define PORT_SHF	2
/** The mask of register. */
#define PORT_MASK	0xf
/** The pinmux register offset. */
#define PINMUX_OFFSET	0x20

static struct mtk_pinctrl_controller pctl = {
	.base[0] = (void __iomem *)CM4_GPIO_PWM_GRP0_BASE,
	.base[1] = (void __iomem *)CM4_GPIO_PWM_GRP1_BASE,
	.base[2] = (void __iomem *)CM4_GPIO_PWM_GRP2_BASE,
	.base[3] = (void __iomem *)CM4_GPIO_PWM_GRP3_BASE,
	.base[4] = (void __iomem *)CM4_GPIO_PWM_GRP4_BASE,
	.base[5] = (void __iomem *)CM4_GPIO_PWM_GRP5_BASE,
	.base[6] = (void __iomem *)CM4_ISU0_I2C_BASE,
	.base[7] = (void __iomem *)CM4_ISU1_I2C_BASE,
	.base[8] = (void __iomem *)CM4_ISU2_I2C_BASE,
	.base[9] = (void __iomem *)CM4_ISU3_I2C_BASE,
	.base[10] = (void __iomem *)CM4_ISU4_I2C_BASE,
	.base[11] = (void __iomem *)CM4_ADC_BASE,
	.base[12] = (void __iomem *)CA7_GPIO_BASE,
	.base[13] = (void __iomem *)CM4_I2S0_BASE,
	.base[14] = (void __iomem *)CM4_I2S1_BASE,
	.base[15] = (void __iomem *)PINMUX_BASE,
	.gpio_mode_bits = GPIO_MODE_BITS,
	.max_gpio_mode_per_reg = MAX_GPIO_MODE_PER_REG,
	.port_shf = PORT_SHF,
	.port_mask = PORT_MASK,
	.pinmux_offset = PINMUX_OFFSET,
};

int mtk_os_hal_gpio_get_input(os_hal_gpio_pin pin, os_hal_gpio_data *pvalue)
{
	return mtk_mhal_gpio_get_input(&pctl, pin, (mhal_gpio_data *)pvalue);
}

int mtk_os_hal_gpio_set_output(os_hal_gpio_pin pin, os_hal_gpio_data out_val)
{
	return mtk_mhal_gpio_set_output(&pctl, pin, out_val);
}

int mtk_os_hal_gpio_get_output(os_hal_gpio_pin pin, os_hal_gpio_data *pvalue)
{
	return mtk_mhal_gpio_get_output(&pctl, pin, (mhal_gpio_data *)pvalue);
}

int mtk_os_hal_gpio_set_direction(os_hal_gpio_pin pin,
	os_hal_gpio_direction dir)
{
	return mtk_mhal_gpio_set_direction(&pctl, pin, dir);
}

int mtk_os_hal_gpio_get_direction(os_hal_gpio_pin pin,
	os_hal_gpio_direction *pvalue)
{
	return mtk_mhal_gpio_get_direction(&pctl, pin,
		(mhal_gpio_direction *)pvalue);
}

int mtk_os_hal_gpio_set_pullen_pullsel(
	os_hal_gpio_pin pin, bool enable, bool isup)
{
	return mtk_mhal_gpio_set_pullen_pullsel(&pctl, pin, enable, isup);
}
