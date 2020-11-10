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

#include "nvic.h"
#include "cirq_common.h"
#include "os_hal_eint.h"
#include "mhal_eint.h"

#define EINT_DEBOUNCE_BASE_ADDR				0x21000000
#define TABLE_SIZE					24

typedef struct {
	eint_number eint_number;
	u32 irq;
} eint_irq_map_t;

eint_irq_map_t eint_irq_table[TABLE_SIZE] = {
	{HAL_EINT_NUMBER_0, CM4_IRQ_GPIO_G0_0},
	{HAL_EINT_NUMBER_1, CM4_IRQ_GPIO_G0_1},
	{HAL_EINT_NUMBER_2, CM4_IRQ_GPIO_G0_2},
	{HAL_EINT_NUMBER_3, CM4_IRQ_GPIO_G0_3},
	{HAL_EINT_NUMBER_4, CM4_IRQ_GPIO_G1_0},
	{HAL_EINT_NUMBER_5, CM4_IRQ_GPIO_G1_1},
	{HAL_EINT_NUMBER_6, CM4_IRQ_GPIO_G1_2},
	{HAL_EINT_NUMBER_7, CM4_IRQ_GPIO_G1_3},
	{HAL_EINT_NUMBER_8, CM4_IRQ_GPIO_G2_0},
	{HAL_EINT_NUMBER_9, CM4_IRQ_GPIO_G2_1},
	{HAL_EINT_NUMBER_10, CM4_IRQ_GPIO_G2_2},
	{HAL_EINT_NUMBER_11, CM4_IRQ_GPIO_G2_3},
	{HAL_EINT_NUMBER_12, CM4_IRQ_GPIO_G3_0},
	{HAL_EINT_NUMBER_13, CM4_IRQ_GPIO_G3_1},
	{HAL_EINT_NUMBER_14, CM4_IRQ_GPIO_G3_2},
	{HAL_EINT_NUMBER_15, CM4_IRQ_GPIO_G3_3},
	{HAL_EINT_NUMBER_16, CM4_IRQ_GPIO_G4_0},
	{HAL_EINT_NUMBER_17, CM4_IRQ_GPIO_G4_1},
	{HAL_EINT_NUMBER_18, CM4_IRQ_GPIO_G4_2},
	{HAL_EINT_NUMBER_19, CM4_IRQ_GPIO_G4_3},
	{HAL_EINT_NUMBER_20, CM4_IRQ_GPIO_G5_0},
	{HAL_EINT_NUMBER_21, CM4_IRQ_GPIO_G5_1},
	{HAL_EINT_NUMBER_22, CM4_IRQ_GPIO_G5_2},
	{HAL_EINT_NUMBER_23, CM4_IRQ_GPIO_G5_3}
};

static int _mtk_os_hal_eint_convert_gpio_irq(eint_number eint_num,
			IRQn_Type *irq)
{
	u32 index = 0;

	for (index = 0; index < TABLE_SIZE; index++) {
		if (eint_irq_table[index].eint_number == eint_num) {
			*irq = (IRQn_Type)eint_irq_table[index].irq;
			return 0;
		}
	}
	printf("mtk_eint_convert_gpio_irq fail.\n");
	return -EINT_EINVAL;
}

int mtk_os_hal_eint_set_type(eint_number eint_num,
			eint_trigger_mode trigger_mode)
{
	IRQn_Type irq;
	int state = 0;
	void __iomem *eint_base = (void __iomem *)EINT_DEBOUNCE_BASE_ADDR;

	if (_mtk_os_hal_eint_convert_gpio_irq(eint_num, &irq) != 0)
		return -EINT_EINVAL;

	if (trigger_mode > HAL_EINT_EDGE_FALLING_AND_RISING) {
		printf("Trigger_mode value is invalid.\n");
		return -EINT_EINVAL;
	}

	NVIC_DisableIRQ(irq);

	if ((trigger_mode == HAL_EINT_LEVEL_LOW) ||
		(trigger_mode == HAL_EINT_EDGE_FALLING) ||
		(trigger_mode == HAL_EINT_EDGE_FALLING_AND_RISING))
		state = mtk_mhal_eint_set_polarity(eint_num, 1, eint_base);
	else
		state = mtk_mhal_eint_set_polarity(eint_num, 0, eint_base);

	if (state != 0)
		return state;

	if (trigger_mode == HAL_EINT_EDGE_FALLING_AND_RISING)
		state = mtk_mhal_eint_set_dual(eint_num, 1, eint_base);
	else
		state = mtk_mhal_eint_set_dual(eint_num, 0, eint_base);

	if (state != 0)
		return state;

	if ((trigger_mode == HAL_EINT_LEVEL_LOW) ||
		(trigger_mode == HAL_EINT_LEVEL_HIGH))
		cirq_irq_ctl_set_level(irq);
	else
		cirq_irq_ctl_set_edge(irq);

	NVIC_EnableIRQ(irq);

	return state;
}

int mtk_os_hal_eint_set_debounce(eint_number eint_num,
				os_hal_eint_debounce_time debounce_time)
{
	IRQn_Type irq;
	int state = 0;
	void __iomem *eint_base = (void __iomem *)EINT_DEBOUNCE_BASE_ADDR;

	if (_mtk_os_hal_eint_convert_gpio_irq(eint_num, &irq) != 0)
		return -EINT_EINVAL;

	NVIC_DisableIRQ(irq);
	state = mtk_mhal_eint_set_debounce(eint_num, debounce_time, eint_base);
	if (state != 0)
		return state;

	state = mtk_mhal_eint_enable_debounce(eint_num, eint_base);
	if (state != 0)
		return state;

	NVIC_EnableIRQ(irq);

	return irq;
}

int mtk_os_hal_eint_enable_debounce(eint_number eint_num)
{
	int state = 0;
	void __iomem *eint_base = (void __iomem *)EINT_DEBOUNCE_BASE_ADDR;

	state = mtk_mhal_eint_enable_debounce(eint_num, eint_base);

	return state;
}

int mtk_os_hal_eint_disable_debounce(eint_number eint_num)
{
	int state = 0;
	void __iomem *eint_base = (void __iomem *)EINT_DEBOUNCE_BASE_ADDR;

	state = mtk_mhal_eint_disable_debounce(eint_num, eint_base);

	return state;
}


int mtk_os_hal_eint_register(eint_number eint_num,
			eint_trigger_mode trigger_mode, void (*handle)(void))
{
	u32 sens;
	IRQn_Type irq;
	int state = 0;
	void __iomem *eint_base = (void __iomem *)EINT_DEBOUNCE_BASE_ADDR;

	if (_mtk_os_hal_eint_convert_gpio_irq(eint_num, &irq) != 0)
		return -EINT_EINVAL;

	if (trigger_mode > HAL_EINT_EDGE_FALLING_AND_RISING) {
		printf("Trigger_mode value is invalid.\n");
		return -EINT_EINVAL;
	}

	NVIC_DisableIRQ(irq);

	if ((trigger_mode == HAL_EINT_LEVEL_LOW) ||
		(trigger_mode == HAL_EINT_EDGE_FALLING) ||
		(trigger_mode == HAL_EINT_EDGE_FALLING_AND_RISING))
		state = mtk_mhal_eint_set_polarity(eint_num, 1, eint_base);
	else
		state = mtk_mhal_eint_set_polarity(eint_num, 0, eint_base);

	if (state != 0)
		return state;

	if (trigger_mode == HAL_EINT_EDGE_FALLING_AND_RISING)
		state = mtk_mhal_eint_set_dual(eint_num, 1, eint_base);
	else
		state = mtk_mhal_eint_set_dual(eint_num, 0, eint_base);

	if (state != 0)
		return state;

	if ((trigger_mode == HAL_EINT_LEVEL_LOW) ||
		(trigger_mode == HAL_EINT_LEVEL_HIGH))
		sens = 1;
	else
		sens = 0;

	NVIC_ClearPendingIRQ(irq);
	CM4_Install_NVIC(irq, DEFAULT_PRI, sens, handle, true);

	return irq;
}

int mtk_os_hal_eint_unregister(eint_number eint_num)
{
	IRQn_Type irq;
	int state = 0;
	void __iomem *eint_base = (void __iomem *)EINT_DEBOUNCE_BASE_ADDR;

	if (_mtk_os_hal_eint_convert_gpio_irq(eint_num, &irq) != 0)
		return -EINT_EINVAL;

	NVIC_UnRegister(irq);

	state = mtk_mhal_eint_set_dual(eint_num, 0, eint_base);
	if (state != 0)
		return state;

	mtk_os_hal_eint_disable_debounce(eint_num);

	return 0;
}

