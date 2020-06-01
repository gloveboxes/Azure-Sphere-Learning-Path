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

#include "hdl_gpio.h"
#include "mhal_osai.h"
#include "mhal_gpio.h"

/* GPIO  PWM */
#define PAD_GPIO_GPIO0_MASK			(BIT(0))
#define PAD_GPIO_GPIO1_MASK			(BIT(1))
#define PAD_GPIO_GPIO2_MASK			(BIT(2))
#define PAD_GPIO_GPIO3_MASK			(BIT(3))
/* GPIO ISU0_I2C */
#define PAD_GPIO_SPI0_SCK_MASK		(BIT(0))
#define PAD_GPIO_SPI0_MOSI_MASK		(BIT(1))
#define PAD_GPIO_SPI0_MISO_MASK		(BIT(2))
#define PAD_GPIO_SPI0_CSA_MASK		(BIT(3))
#define PAD_GPIO_SPI0_CSB_MASK		(BIT(4))
/* GPIO I2S0 */
#define PAD_GPIO_L2S0_TX_MASK			(BIT(0))
#define PAD_GPIO_L2S0_MCLK_MASK		(BIT(1))
#define PAD_GPIO_L2S0_FS_MASK			(BIT(2))
#define PAD_GPIO_L2S0_RX_MASK		(BIT(3))
#define PAD_GPIO_L2S0_BCLK_MASK		(BIT(4))
/* GPIO  ADC */
#define PAD_GPIO_ADC0_MASK			(BIT(0))
#define PAD_GPIO_ADC1_MASK			(BIT(1))
#define PAD_GPIO_ADC2_MASK			(BIT(2))
#define PAD_GPIO_ADC3_MASK			(BIT(3))
#define PAD_GPIO_ADC4_MASK			(BIT(4))
#define PAD_GPIO_ADC5_MASK			(BIT(5))
#define PAD_GPIO_ADC6_MASK			(BIT(6))
#define PAD_GPIO_ADC7_MASK			(BIT(7))
/*GPIO NOMAP*/
#define PAD_GPIO_ANTSEL0_MASK			(BIT(0))
#define PAD_GPIO_ANTSEL1_MASK			(BIT(1))
#define PAD_GPIO_UART_RX_CA7_MASK	(BIT(2))
#define PAD_GPIO_UART_TX_CA7_MASK	(BIT(3))
#define PAD_GPIO_UART_RTS_CA7_MASK	(BIT(4))
#define PAD_GPIO_UART_CTS_CA7_MASK	(BIT(5))
#define PAD_GPIO_SWD_DIO_MASK		(BIT(6))
#define PAD_GPIO_SWD_CLK_MASK		(BIT(7))
#define PAD_GPIO_SWO_MASK			(BIT(8))
#define PAD_GPIO_UART_RX_HSP_MASK	(BIT(9))
#define PAD_GPIO_UART_TX_HSP_MASK	(BIT(10))
#define PAD_GPIO_UART_RTS_HSP_MASK	(BIT(11))
#define PAD_GPIO_UART_CTS_HSP_MASK	(BIT(12))
#define PAD_GPIO_UART_RX_IO0_MASK	(BIT(13))
#define PAD_GPIO_UART_TX_IO0_MASK	(BIT(14))
#define PAD_GPIO_UART_RTS_IO0_MASK	(BIT(15))
#define PAD_GPIO_UART_CTS_IO0_MASK	(BIT(16))
#define PAD_GPIO_UART_RX_IO1_MASK	(BIT(17))
#define PAD_GPIO_UART_TX_IO1_MASK	(BIT(18))
#define PAD_GPIO_UART_RTS_IO1_MASK	(BIT(19))
#define PAD_GPIO_UART_CTS_IO1_MASK	(BIT(20))
#define PAD_GPIO_UART_TX_N9_MASK		(BIT(21))

static u32 _mtk_mhal_gpio_grp0_to_grp5_map(
	struct mtk_pinctrl_controller *pctl,
	u32 base_addr, u32 pin, u32 write)
{
	u32 dout = 0;
	u32 baseaddr, mask;
	u32 shift = 0;
	u32 gpio_masks[4] = {PAD_GPIO_GPIO0_MASK, PAD_GPIO_GPIO1_MASK,
				PAD_GPIO_GPIO2_MASK, PAD_GPIO_GPIO3_MASK};

	shift = pin % 4;
	mask = gpio_masks[shift];
	switch (pin) {
	case 0: case 1: case 2: case 3:
		baseaddr = (u32)pctl->base[MHAL_CM4_GPIO_PWM_GRP0_BASE]
					+ base_addr;
		break;

	case 4: case 5: case 6: case 7:
		baseaddr = (u32)pctl->base[MHAL_CM4_GPIO_PWM_GRP1_BASE]
					+ base_addr;
		break;

	case 8: case 9: case 10: case 11:
		baseaddr = (u32)pctl->base[MHAL_CM4_GPIO_PWM_GRP2_BASE]
					+ base_addr;
		break;

	case 12: case 13: case 14: case 15:
		baseaddr = (u32)pctl->base[MHAL_CM4_GPIO_PWM_GRP3_BASE]
					+ base_addr;
		break;

	case 16: case 17: case 18: case 19:
		baseaddr = (u32)pctl->base[MHAL_CM4_GPIO_PWM_GRP4_BASE]
					+ base_addr;
		break;

	case 20: case 21: case 22: case 23:
		baseaddr = (u32)pctl->base[MHAL_CM4_GPIO_PWM_GRP5_BASE]
					+ base_addr;
		break;
	default:
		return -ERROR;
	}

	mtk_hdl_gpio_modify_reg((void *)baseaddr,
		mask, shift, write, &dout);

	return dout;
}


static u32 _mtk_mhal_gpio_isu0_to_isu4_map(
	struct mtk_pinctrl_controller *pctl,
	u32 base_addr, u32 pin, u32 write)
{
	u32 dout = 0;
	u32 baseaddr, mask;
	u32 shift = 0;
	u32 gpio_masks[5] = {PAD_GPIO_SPI0_SCK_MASK, PAD_GPIO_SPI0_MOSI_MASK,
				PAD_GPIO_SPI0_MISO_MASK, PAD_GPIO_SPI0_CSA_MASK,
				PAD_GPIO_SPI0_CSB_MASK};

	if ((pin >= 26) && (pin <= 40))
		shift = (pin - 26) % 5;
	else if ((pin >= 66) && (pin <= 80))
		shift = (pin - 66) % 5;

	mask = gpio_masks[shift];
	switch (pin) {
	case 26: case 27: case 28: case 29: case 30:
		baseaddr = (u32)pctl->base[MHAL_CM4_ISU0_I2C_BASE] + base_addr;
		break;
	case 31: case 32: case 33: case 34: case 35:
		baseaddr = (u32)pctl->base[MHAL_CM4_ISU1_I2C_BASE] + base_addr;
		break;
	case 36: case 37: case 38: case 39: case 40:
		baseaddr = (u32)pctl->base[MHAL_CM4_ISU2_I2C_BASE] + base_addr;
		break;
	case 66: case 67: case 68: case 69: case 70:
		baseaddr = (u32)pctl->base[MHAL_CM4_ISU3_I2C_BASE] + base_addr;
		break;
	case 71: case 72: case 73: case 74: case 75:
		baseaddr = (u32)pctl->base[MHAL_CM4_ISU4_I2C_BASE] + base_addr;
		break;
	default:
		return -ERROR;
	}

	mtk_hdl_gpio_modify_reg((void *)baseaddr,
		mask, shift, write, &dout);

	return dout;
}

static u32 _mtk_mhal_gpio_i2s0_to_i2s1_map(
	struct mtk_pinctrl_controller *pctl,
	u32 base_addr, u32 pin, u32 write)
{
	u32 dout = 0;
	u32 baseaddr, mask;
	u32 shift = 0;
	u32 gpio_masks[5] = {PAD_GPIO_L2S0_TX_MASK, PAD_GPIO_L2S0_MCLK_MASK,
				PAD_GPIO_L2S0_FS_MASK, PAD_GPIO_L2S0_RX_MASK,
				PAD_GPIO_L2S0_BCLK_MASK};

	shift = (pin - 56) % 5;

	mask = gpio_masks[shift];
	switch (pin) {
	case 56: case 57: case 58: case 59: case 60:
		baseaddr = (u32)pctl->base[MHAL_CM4_I2S0_BASE] + base_addr;
		break;
	case 61: case 62: case 63: case 64: case 65:
		baseaddr = (u32)pctl->base[MHAL_CM4_I2S1_BASE] + base_addr;
		break;
	default:
		return -ERROR;
	}

	mtk_hdl_gpio_modify_reg((void *)baseaddr,
		mask, shift, write, &dout);

	return dout;
}


static u32 _mtk_mhal_gpio_adc_map(
	struct mtk_pinctrl_controller *pctl,
	u32 base_addr, u32 pin, u32 write)
{
	u32 dout = 0;
	u32 baseaddr, mask;
	u32 shift = 0;
	u32 gpio_masks[8] = {PAD_GPIO_ADC0_MASK, PAD_GPIO_ADC1_MASK,
				PAD_GPIO_ADC2_MASK, PAD_GPIO_ADC3_MASK,
				PAD_GPIO_ADC4_MASK, PAD_GPIO_ADC5_MASK,
				PAD_GPIO_ADC6_MASK, PAD_GPIO_ADC7_MASK};
	shift = (pin - 41) % 8;

	mask = gpio_masks[shift];
	baseaddr = (u32)pctl->base[MHAL_CM4_ADC_BASE] + base_addr;

	mtk_hdl_gpio_modify_reg((void *)baseaddr,
		mask, shift, write, &dout);

	return dout;
}

static u32 _mtk_mhal_gpio_ca7_map(
	struct mtk_pinctrl_controller *pctl,
	u32 base_addr, u32 pin, u32 write)
{
	u32 dout = 0;
	u32 baseaddr, mask;
	u32 shift = 0;
	u32 gpio_masks[22] = {PAD_GPIO_ANTSEL0_MASK, PAD_GPIO_ANTSEL1_MASK,
		PAD_GPIO_UART_RX_CA7_MASK, PAD_GPIO_UART_TX_CA7_MASK,
		PAD_GPIO_UART_RTS_CA7_MASK, PAD_GPIO_UART_CTS_CA7_MASK,
		PAD_GPIO_SWD_DIO_MASK, PAD_GPIO_SWD_CLK_MASK,
		PAD_GPIO_SWO_MASK, PAD_GPIO_UART_RX_HSP_MASK,
		PAD_GPIO_UART_TX_HSP_MASK, PAD_GPIO_UART_RTS_HSP_MASK,
		PAD_GPIO_UART_CTS_HSP_MASK, PAD_GPIO_UART_RX_IO0_MASK,
		PAD_GPIO_UART_TX_IO0_MASK, PAD_GPIO_UART_RTS_IO0_MASK,
		PAD_GPIO_UART_CTS_IO0_MASK, PAD_GPIO_UART_RX_IO1_MASK,
		PAD_GPIO_UART_TX_IO1_MASK, PAD_GPIO_UART_RTS_IO1_MASK,
		PAD_GPIO_UART_CTS_IO1_MASK, PAD_GPIO_UART_TX_N9_MASK};

	if ((pin >= 24) && (pin <= 25))
		shift = (pin - 24) % 22;
	else if ((pin >= 49) && (pin <= 55))
		shift = (pin - 47) % 22;
	else if ((pin >= 81) && (pin <= 93))
		shift = (pin - 72) % 22;

	mask = gpio_masks[shift];
	baseaddr = (u32)pctl->base[MHAL_CA7_GPIO_BASE] + base_addr;

	mtk_hdl_gpio_modify_reg((void *)baseaddr,
		mask, shift, write, &dout);

	return dout;
}

static int _mtk_mhal_gpio_reg_map(
	struct mtk_pinctrl_controller *pctl,
	u32 baseaddr, u32 pin, u32 write)
{
	u32 count = 0;
	u32 dout = 0;
	u32 base_addr = 0;

	base_addr = baseaddr;

	if ((pin == 24) || (pin == 25)
		|| ((pin >= 49) && (pin <= 55))
		|| ((pin >= 81) && (pin <= 93))) {

		count = base_addr / 0x10;

		if (base_addr == 0x04)
			base_addr = 0x0;

		if ((count >= 2) && (count <= 7))
			base_addr += 0x10 * (count - 1);
	}

	if (((pin >= 26) && (pin <= 40))
		|| ((pin >= 66) && (pin <= 80))) {
		if (base_addr == 0x04)
			base_addr = 0xc;
	}

	if ((pin >= 56) && (pin <= 65)) {
		if (base_addr == 0x04)
			base_addr = 0x0;

		base_addr = base_addr + 0x100;
	}

	switch (pin) {
	case 0: case 1: case 2: case 3:
	case 4: case 5: case 6: case 7:
	case 8: case 9: case 10: case 11:
	case 12: case 13: case 14: case 15:
	case 16: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
		dout = _mtk_mhal_gpio_grp0_to_grp5_map(pctl,
				base_addr, pin, write);
		break;

	/*PIN 24 25 Need to mpa register*/
	case 24: case 25: case 49: case 50: case 51:
	case 52: case 53: case 54: case 55: case 81:
	case 82: case 83: case 84: case 85: case 86:
	case 87: case 88: case 89: case 90: case 91:
	case 92: case 93:
		dout = _mtk_mhal_gpio_ca7_map(pctl,
				base_addr, pin, write);
		break;

	case 26: case 27: case 28: case 29: case 30:
	case 31: case 32: case 33: case 34: case 35:
	case 36: case 37: case 38: case 39: case 40:
	case 66: case 67: case 68: case 69: case 70:
	case 71: case 72: case 73: case 74: case 75:
		dout = _mtk_mhal_gpio_isu0_to_isu4_map(pctl,
				base_addr, pin, write);
		break;

	case 41: case 42: case 43: case 44:
	case 45: case 46: case 47: case 48:
		dout = _mtk_mhal_gpio_adc_map(pctl,
				base_addr, pin, write);
		break;

	case 56: case 57: case 58: case 59: case 60:
	case 61: case 62: case 63: case 64: case 65:
		dout = _mtk_mhal_gpio_i2s0_to_i2s1_map(pctl,
				base_addr, pin, write);
		break;

	default:
		return 0;

	}

	return dout;
}

int mtk_mhal_gpio_request(struct mtk_pinctrl_controller *pctl, u32 pin)
{
	u32 value = 0;

	if (!pctl)
		return -EINVAL;

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if (pctl->mtk_pins[pin].pinctrl_free == true) {
		mtk_mhal_gpio_pmx_get_mode(pctl, pin, &value);
		gpio_err("pin is request as mode %d fail\n", value);
		return -EQUEST;
	} else if (pctl->mtk_pins[pin].pinctrl_free == false) {
		pctl->mtk_pins[pin].pinctrl_free = true;
	}

	return 0;
}

int mtk_mhal_gpio_free(struct mtk_pinctrl_controller *pctl, u32 pin)
{
	u32 value = 0;

	if (!pctl)
		return -EINVAL;

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if (pctl->mtk_pins[pin].pinctrl_free == true) {
		pctl->mtk_pins[pin].pinctrl_free = false;
	} else if (pctl->mtk_pins[pin].pinctrl_free == false) {
		mtk_mhal_gpio_pmx_get_mode(pctl, pin, &value);
		gpio_err("pin[%d] is free as mode %d fail\n", pin, value);
		return -EFREE;
	}

	return 0;
}

int mtk_mhal_gpio_get_input(struct mtk_pinctrl_controller *pctl,
	u32 pin, mhal_gpio_data *pvalue)
{
	u32 i;

	if ((!pctl) || (pvalue == NULL))
		return -EINVAL;

	for (i = 0; i < MHAL_GPIO_REG_BASE_MAX; i++) {
		if (!pctl->base[i])
			return -EINVAL;
	}

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if ((pin >= MHAL_GPIO_76) && (pin <= MHAL_GPIO_80))
		return -EPIN;

	/* DIN + 0x04*/
	*pvalue = (mhal_gpio_data)_mtk_mhal_gpio_reg_map(pctl, 0x04, pin, 0x0);

	gpio_debug("GPIO%d input value = 0x%x\n", pin, *pvalue);

	return 0;
}

int mtk_mhal_gpio_set_output(struct mtk_pinctrl_controller *pctl,
	u32 pin, u32 out_val)
{
	u32 i;

	if (!pctl)
		return -EINVAL;

	for (i = 0; i < MHAL_GPIO_REG_BASE_MAX; i++) {
		if (!pctl->base[i])
			return -EINVAL;
	}

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if (out_val > 1)
		return -EINVAL;

	if ((pin >= MHAL_GPIO_76) && (pin <= MHAL_GPIO_80))
		return -EPIN;

	if (out_val == 1)
		_mtk_mhal_gpio_reg_map(pctl, 0x10, pin, 0x1);
	else
		_mtk_mhal_gpio_reg_map(pctl, 0x10, pin, 0x2);
	return 0;
}

int mtk_mhal_gpio_get_output(struct mtk_pinctrl_controller *pctl,
	u32 pin, mhal_gpio_data *pvalue)
{
	u32 i;

	if ((!pctl) || (pvalue == NULL))
		return -EINVAL;

	for (i = 0; i < MHAL_GPIO_REG_BASE_MAX; i++) {
		if (!pctl->base[i])
			return -EINVAL;
	}

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if ((pin >= MHAL_GPIO_76) && (pin <= MHAL_GPIO_80))
		return -EPIN;

	/* DOUT + 0x10 */
	*pvalue = (mhal_gpio_data)_mtk_mhal_gpio_reg_map(pctl, 0x10, pin, 0x0);
	gpio_debug("GPIO%d get ouput value = 0x%x\n", pin, *pvalue);

	return 0;
}

int mtk_mhal_gpio_set_direction(
	struct mtk_pinctrl_controller *pctl, u32 pin, u32 dir)
{
	u32 i;

	if (!pctl)
		return -EINVAL;

	for (i = 0; i < MHAL_GPIO_REG_BASE_MAX; i++) {
		if (!pctl->base[i])
			return -EINVAL;
	}

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if (dir > 1)
		return -EINVAL;

	if ((pin >= MHAL_GPIO_76) && (pin <= MHAL_GPIO_80))
		return -EPIN;

	if (dir == 1) {
		/* IES_RESET +0x68*/
		_mtk_mhal_gpio_reg_map(pctl, 0x68, pin, 0x1);
		/* OE_SET +0x24*/
		_mtk_mhal_gpio_reg_map(pctl, 0x24, pin, 0x1);

	} else {
		/* OE_RESET +0x28*/
		_mtk_mhal_gpio_reg_map(pctl, 0x28, pin, 0x1);
		/* IES +0x60*/
		_mtk_mhal_gpio_reg_map(pctl, 0x60, pin, 0x1);
	}
	return 0;
}

int mtk_mhal_gpio_get_direction(struct mtk_pinctrl_controller *pctl,
	u32 pin, mhal_gpio_direction *pvalue)
{
	u32 i;

	if ((!pctl) || (pvalue == NULL))
		return -EINVAL;

	for (i = 0; i < MHAL_GPIO_REG_BASE_MAX; i++) {
		if (!pctl->base[i])
			return -EINVAL;
	}

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if ((pin >= MHAL_GPIO_76) && (pin <= MHAL_GPIO_80))
		return -EPIN;

	/* OE + 0x20 */
	*pvalue = (mhal_gpio_direction)_mtk_mhal_gpio_reg_map(pctl,
			0x20, pin, 0x0);

	gpio_debug("GPIO%d dir = 0x%x\n", pin, *pvalue);
	return 0;
}


static void _mtk_mhal_gpio_set_pu_pd_ctrl_by_sw_cr(
	struct mtk_pinctrl_controller *pctl)
{
	int i;

	for (i = MHAL_CM4_GPIO_PWM_GRP0_BASE;
		i <= MHAL_CM4_GPIO_PWM_GRP5_BASE; i++) {
		mtk_hdl_gpio_set_pu_pd_ctrl_by_sw_cr(pctl->base[i]);
	}
}

int mtk_mhal_gpio_set_pullen_pullsel(
	struct mtk_pinctrl_controller *pctl, u32 pin, bool enable, bool isup)
{
	int i;

	if (!pctl)
		return -EINVAL;

	for (i = 0; i < MHAL_GPIO_REG_BASE_MAX; i++) {
		if (!pctl->base[i])
			return -EINVAL;
	}

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if ((pin >= MHAL_GPIO_76) && (pin <= MHAL_GPIO_80))
		return -EPIN;

	if (pin <= 23) {
		/* set gpio0~23 PU/PD control from software CR */
		_mtk_mhal_gpio_set_pu_pd_ctrl_by_sw_cr(pctl);
	}

	if (enable) {
		if (isup) {
			/* reset pull down -> reset pull up -> set pull up */
			/*PD_RESET 0x48 */
			_mtk_mhal_gpio_reg_map(pctl, 0x48, pin, 0x1);
			/*PU_RESET 0x38 */
			_mtk_mhal_gpio_reg_map(pctl, 0x38, pin, 0x1);
			/*PU_SET 0x34 */
			_mtk_mhal_gpio_reg_map(pctl, 0x34, pin, 0x1);
		} else {
			/* reset pull up -> reset pull down -> set pull down */
			/*PU_RESET 0x38 */
			_mtk_mhal_gpio_reg_map(pctl, 0x38, pin, 0x1);
			/*PD_RESET 0x48 */
			_mtk_mhal_gpio_reg_map(pctl, 0x48, pin, 0x1);
			/*PD_SET 0x44 */
			_mtk_mhal_gpio_reg_map(pctl, 0x44, pin, 0x1);
		}
	} else {
		/*PU_RESET 0x38 */
		_mtk_mhal_gpio_reg_map(pctl, 0x38, pin, 0x1);
		/*PD_RESET 0x48 */
		_mtk_mhal_gpio_reg_map(pctl, 0x48, pin, 0x1);
	}
	return 0;
}

int mtk_mhal_gpio_pmx_set_mode(
	struct mtk_pinctrl_controller *pctl, u32 pin, u32 mode)
{
	u32 i;
	u32 reg_addr;
	unsigned char bit;
	u32 val;
	u32 mask;

	if (!pctl)
		return -EINVAL;

	for (i = 0; i < MHAL_GPIO_REG_BASE_MAX; i++) {
		if (!pctl->base[i])
			return -EINVAL;
	}

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if (mode >= MHAL_GPIO_MODE_MAX)
		return -EINVAL;

	if ((pin >= MHAL_GPIO_76) && (pin <= MHAL_GPIO_80))
		return -EPIN;

	mask = (1L << pctl->gpio_mode_bits) - 1;
	reg_addr = ((pin / pctl->max_gpio_mode_per_reg)
			<< pctl->port_shf)
			+ pctl->pinmux_offset;

	mode &= mask;
	bit = pin % pctl->max_gpio_mode_per_reg;
	mask <<= (pctl->gpio_mode_bits * bit);
	val = (mode << (pctl->gpio_mode_bits * bit));

	mtk_hdl_gpio_set_mode_reg(
		pctl->base[MHAL_PINMUX_BASE], mask, val, reg_addr);

	return 0;
}

int mtk_mhal_gpio_pmx_get_mode(struct mtk_pinctrl_controller *pctl,
	u32 pin, u32 *pvalue)
{
	u32 i;
	u32 reg_addr;
	unsigned char bit;
	u32 reg_val;
	u32 mask;

	if ((!pctl) || (pvalue == NULL))
		return -EINVAL;

	for (i = 0; i < MHAL_GPIO_REG_BASE_MAX; i++) {
		if (!pctl->base[i])
			return -EINVAL;
	}

	if (pin >= MHAL_GPIO_MAX)
		return -EPIN;

	if ((pin >= MHAL_GPIO_76) && (pin <= MHAL_GPIO_80))
		return -EPIN;

	mask = (1L << pctl->gpio_mode_bits) - 1;
	reg_addr = ((pin / pctl->max_gpio_mode_per_reg)
			<< pctl->port_shf)
			+ pctl->pinmux_offset;

	bit = pin % pctl->max_gpio_mode_per_reg;
	mask <<= (pctl->gpio_mode_bits * bit);

	mtk_hdl_gpio_get_mode_reg_val(
		pctl->base[MHAL_PINMUX_BASE], reg_addr, &reg_val);
	reg_val &= mask;
	*pvalue = reg_val >> (pctl->gpio_mode_bits * bit);

	gpio_debug("GPIO%d mode = 0x%x\n", pin, *pvalue);
	return 0;
}
