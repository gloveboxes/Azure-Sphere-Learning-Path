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

#include "os_hal_eint.h"
#include "os_hal_gpt.h"
#include "os_hal_lp.h"

#define CM4_RGU_MEM_CTRL_BASE (CM4_RGU_LP_BASE + 0x100)

#define CM4_WIC_PEND_EINT_MASK1 BITS(20, 31)
#define CM4_WIC_PEND_EINT_MASK2 BITS(0, 11)
#define CM4_WIC_PEND_GPT3_MASK BIT(2)
#define CM4_WIC_PEND_EINT_SHIFT1 20
#define CM4_WIC_PEND_EINT_SHIFT2 12

#define CM4_CFG_IRQ_MASK(x, y) BITS(x, y)
#define CM4_CFG_WIC_MASK(x, y) BITS(x, y)
#define CM4_CFG_IRQ_EINT_MASK(x) BIT(x)
#define CM4_CFG_WIC_EINT_MASK(x) BIT(x)
#define CM4_CFG_IRQ_GPT3_MASK BIT(2)
#define CM4_CFG_WIC_GPT3_MASK BIT(2)

#define CM4_HW_MEM_PDN_MASK BITS(0, 25)
#define CM4_HW_MEM_SLP_MASK BITS(0, 25)
#define CM4_HW_MEM_PDN_SYSRAM_MASK BITS(0, 3)

#define CM4_SLP_EN BIT(0)
#define CM4_DEEP_SLP_EN BIT(1)

#define CM4_WAKEUP_FROM_DSLEEP BIT(0)

#define WAKEUP_SRC_MASK BITS(0, 24)
#define WAKEUP_SRC_EINT_MASK BITS(0, 23)
#define WAKEUP_SRC_GPT3_MASK BIT(24)

#define GPT_CLK_1M_FREQ 1000000

static void _mtk_os_hal_lp_disable_cm4_systick(void)
{
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
}

static void _mtk_os_hal_lp_enable_cm4_systick(void)
{
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

static void _mtk_os_hal_lp_disable_irq_wic_src(void)
{
	/*disable all IRQ source*/
	HAL_REG_32(CM4_CFG_IRQ_DIS0) = CM4_CFG_IRQ_MASK(0, 31);
	HAL_REG_32(CM4_CFG_IRQ_DIS1) = CM4_CFG_IRQ_MASK(0, 31);
	HAL_REG_32(CM4_CFG_IRQ_DIS2) = CM4_CFG_IRQ_MASK(0, 31);
	HAL_REG_32(CM4_CFG_IRQ_DIS3) |= CM4_CFG_IRQ_MASK(0, 3);

	/*disable all WIC source*/
	HAL_REG_32(CM4_CFG_WIC_DIS0) = CM4_CFG_WIC_MASK(0, 31);
	HAL_REG_32(CM4_CFG_WIC_DIS1) = CM4_CFG_WIC_MASK(0, 31);
	HAL_REG_32(CM4_CFG_WIC_DIS2) = CM4_CFG_WIC_MASK(0, 31);
	HAL_REG_32(CM4_CFG_WIC_DIS3) |= CM4_CFG_WIC_MASK(0, 3);
}

static void _mtk_os_hal_lp_enable_irq_wic_src(void)
{
	/*enable all IRQ source*/
	HAL_REG_32(CM4_CFG_IRQ_DIS0) = (~(CM4_CFG_IRQ_MASK(0, 31)));
	HAL_REG_32(CM4_CFG_IRQ_DIS1) = (~(CM4_CFG_IRQ_MASK(0, 31)));
	HAL_REG_32(CM4_CFG_IRQ_DIS2) = (~(CM4_CFG_IRQ_MASK(0, 31)));
	HAL_REG_32(CM4_CFG_IRQ_DIS3) &= (~(CM4_CFG_IRQ_MASK(0, 3)));

	/*enable all WIC source*/
	HAL_REG_32(CM4_CFG_WIC_DIS0) = (~(CM4_CFG_WIC_MASK(0, 31)));
	HAL_REG_32(CM4_CFG_WIC_DIS1) = (~(CM4_CFG_WIC_MASK(0, 31)));
	HAL_REG_32(CM4_CFG_WIC_DIS2) = (~(CM4_CFG_WIC_MASK(0, 31)));
	HAL_REG_32(CM4_CFG_WIC_DIS3) &= (~(CM4_CFG_WIC_MASK(0, 3)));
}

static void _mtk_os_hal_lp_clear_wic_sta(void)
{
	HAL_REG_32(CM4_WIC_SW_CLR_EN_ADDR) |= CM4_WIC_SW_CLR_EN_MASK;
	HAL_REG_32(CM4_WIC_SW_CLR_ADDR) |= CM4_WIC_SW_CLR_MASK;
	HAL_REG_32(CM4_WIC_SW_CLR_ADDR) &= (~(CM4_WIC_SW_CLR_MASK));
}

static uint32_t _mtk_os_hal_lp_check_wic_pend_sta(void)
{
	uint32_t wic_pend_sta;
	uint32_t wic_pend_sta0 = HAL_REG_32(CM4_WIC_PEND_STA0);
	uint32_t wic_pend_sta1 = HAL_REG_32(CM4_WIC_PEND_STA1);

	wic_pend_sta0 &= CM4_WIC_PEND_EINT_MASK1;
	wic_pend_sta0 >>= CM4_WIC_PEND_EINT_SHIFT1;
	wic_pend_sta1 &= CM4_WIC_PEND_EINT_MASK2;
	wic_pend_sta1 <<= CM4_WIC_PEND_EINT_SHIFT2;
	wic_pend_sta = (wic_pend_sta0 | wic_pend_sta1);

	return wic_pend_sta;
}

static void _mtk_os_hal_lp_gpt3_cb_hdl(void *unused)
{
	mtk_os_hal_gpt_stop(GPT3);

	/* clear WIC IRQ status */
	_mtk_os_hal_lp_clear_wic_sta();
}

static struct os_gpt_int gpt3_int = {
	.gpt_cb_hdl = _mtk_os_hal_lp_gpt3_cb_hdl,
	.gpt_cb_data = NULL,
};

static void _mtk_os_hal_lp_eint_isr(void)
{
	uint32_t wic_pend_sta;
	int i;

	wic_pend_sta = _mtk_os_hal_lp_check_wic_pend_sta();
	for (i = HAL_EINT_NUMBER_0; i < HAL_EINT_NUMBER_MAX; i++)
		if (wic_pend_sta & BIT(i))
			mtk_os_hal_eint_unregister((eint_number)i);

	/* clear WIC IRQ status */
	_mtk_os_hal_lp_clear_wic_sta();
}

static void _mtk_os_hal_lp_config_wakeup_src(uint32_t wakeup_src)
{
	int i = 0;
	uint32_t tmp = wakeup_src;

	/* disable cm4 irq and wic sources */
	_mtk_os_hal_lp_disable_irq_wic_src();

	/* clear WIC IRQ status */
	_mtk_os_hal_lp_clear_wic_sta();

	/*config GPT3 as wakeup source*/
	tmp = wakeup_src & WAKEUP_SRC_GPT3_MASK;
	if (tmp) {
		/*enable GPT3 related IRQ source*/
		HAL_REG_32(CM4_CFG_IRQ_DIS0) &= (~(CM4_CFG_IRQ_GPT3_MASK));
		HAL_REG_32(CM4_CFG_WIC_DIS0) &= (~(CM4_CFG_WIC_GPT3_MASK));
	}

	/*config EINT as wakeup source*/
	tmp = wakeup_src & WAKEUP_SRC_EINT_MASK;
	if (tmp)
		for (i = HAL_EINT_NUMBER_0; i < HAL_EINT_NUMBER_MAX; i++) {
			if (tmp & BIT(i)) {
				mtk_os_hal_eint_register((eint_number)i,
					HAL_EINT_LEVEL_LOW,
					_mtk_os_hal_lp_eint_isr);
				mtk_os_hal_eint_set_debounce((eint_number)i,
					OS_HAL_EINT_DB_TIME_32);

				/*enable EINT related IRQ source*/
				if (i < HAL_EINT_NUMBER_12) {
					HAL_REG_32(CM4_CFG_IRQ_DIS0) &=
						(~(CM4_CFG_IRQ_EINT_MASK(
						i + 20)));
					HAL_REG_32(CM4_CFG_WIC_DIS0) &=
						(~(CM4_CFG_WIC_EINT_MASK(
						i + 20)));
				} else {
					HAL_REG_32(CM4_CFG_IRQ_DIS1) &=
						(~(CM4_CFG_IRQ_EINT_MASK(
						i - 12)));
					HAL_REG_32(CM4_CFG_WIC_DIS1) &=
						(~(CM4_CFG_WIC_EINT_MASK(
						i - 12)));
				}
			}
		}
}

static void _mtk_os_hal_lp_pre_config(lp_scenario lp)
{
	P_IOT_CM4_RGU_SLP_CTRL_TypeDef pLPRGUBase =
		(P_IOT_CM4_RGU_SLP_CTRL_TypeDef)(CM4_RGU_LP_BASE);
	P_IOT_CM4_RGU_MEM_CTRL_TypeDef pLPRGUMEMBase =
		(P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_RGU_MEM_CTRL_BASE);

	switch (lp) {
	case CPU_CLOCK_GATING:
		/*set memory power*/
		(pLPRGUMEMBase->CM4_HW_MEM_PDN) &= (~(CM4_HW_MEM_PDN_MASK));
		(pLPRGUMEMBase->CM4_HW_MEM_SLP) &= (~(CM4_HW_MEM_SLP_MASK));

		/*set deep/legacy sleep control bit*/
		(pLPRGUBase->CM4_SLP_CTRL) &= (~(CM4_DEEP_SLP_EN));
		(pLPRGUBase->CM4_SLP_CTRL) &= (~(CM4_SLP_EN));
		break;
	case LEGACY_SLEEP:
		/*set memory power*/
		(pLPRGUMEMBase->CM4_HW_MEM_PDN) &= (~(CM4_HW_MEM_PDN_MASK));
		(pLPRGUMEMBase->CM4_HW_MEM_PDN) |= CM4_HW_MEM_PDN_SYSRAM_MASK;
		(pLPRGUMEMBase->CM4_HW_MEM_SLP) |= CM4_HW_MEM_SLP_MASK;

		/*set deep/legacy sleep control bit*/
		(pLPRGUBase->CM4_SLP_CTRL) &= (~(CM4_DEEP_SLP_EN));
		(pLPRGUBase->CM4_SLP_CTRL) |= CM4_SLP_EN;
		break;
	case DEEP_SLEEP:
		/*set memory power*/
		(pLPRGUMEMBase->CM4_HW_MEM_PDN) |= CM4_HW_MEM_PDN_MASK;
		(pLPRGUMEMBase->CM4_HW_MEM_SLP) &= (~(CM4_HW_MEM_SLP_MASK));

		/*set deep/legacy sleep control bit*/
		(pLPRGUBase->CM4_SLP_CTRL) |= CM4_DEEP_SLP_EN;
		(pLPRGUBase->CM4_SLP_CTRL) |= CM4_SLP_EN;
		break;
	default:
		printf("[%s]Not support low power scenario!!!\n", __func__);
		break;
	}

	/*enable CM4 System Control Register SleepDeep bit 0xE000ED10[2] 1'b1*/
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

void mtk_os_hal_lp_config_gpt3_timeout(uint32_t time_val)
{
	mtk_os_hal_gpt_config(GPT3, 0, &gpt3_int);
	mtk_os_hal_gpt_reset_timer(GPT3, GPT_CLK_1M_FREQ * time_val, false);
	mtk_os_hal_gpt_restart(GPT3);
	mtk_os_hal_gpt_start(GPT3);
}

int mtk_os_hal_lp_enter(uint32_t wakeup_src, lp_scenario lp)
{
	uint32_t primask_backup;
	uint32_t wic_pend_sta;
	int i = 0;
	int ret = 0;

	if (wakeup_src & (~(WAKEUP_SRC_MASK))) {
		ret = -1;
		printf("[%s]Select wakeup source error.\n", __func__);
		return ret;
	}

	if ((lp != CPU_CLOCK_GATING) && (lp != LEGACY_SLEEP) &&
		(lp != DEEP_SLEEP)) {
		ret = -1;
		printf("[%s]Select NOT support low power scenario.\n",
			__func__);
		return ret;
	}

	 /**
	  *reinstall wic pending irq to NVIC as resuming from deep sleep not
	  *trigger irq handler
	  */
	/*reinstall GPT3 irq handler*/
	if (HAL_REG_32(CM4_WIC_PEND_STA0) & CM4_WIC_PEND_GPT3_MASK) {
		mtk_os_hal_gpt_config(GPT3, 0, &gpt3_int);
		mtk_os_hal_gpt_start(GPT3);
		mtk_os_hal_gpt_stop(GPT3);
	}

	/*reinstall EINT irq handler*/
	wic_pend_sta = _mtk_os_hal_lp_check_wic_pend_sta();
	if (wic_pend_sta)
		for (i = HAL_EINT_NUMBER_0; i < HAL_EINT_NUMBER_MAX; i++)
			if (wic_pend_sta & BIT(i))
				mtk_os_hal_eint_register((eint_number)i,
					HAL_EINT_LEVEL_LOW,
					_mtk_os_hal_lp_eint_isr);

	_mtk_os_hal_lp_disable_cm4_systick();
	local_irq_save(primask_backup);
	_mtk_os_hal_lp_config_wakeup_src(wakeup_src);
	_mtk_os_hal_lp_pre_config(lp);

	__asm volatile ("dsb");
	__asm volatile ("wfi");
	__asm volatile ("isb");

	_mtk_os_hal_lp_enable_irq_wic_src();
	local_irq_restore(primask_backup);
	_mtk_os_hal_lp_enable_cm4_systick();

	return 0;
}

int mtk_os_hal_lp_dslp_check(dslp_check stage)
{
	int ret = 0;
	P_IOT_CM4_RGU_SLP_CTRL_TypeDef pLPRGUBase =
		(P_IOT_CM4_RGU_SLP_CTRL_TypeDef)(CM4_RGU_LP_BASE);

	if ((stage != PRE_DEEP_SLEEP) && (stage != POST_DEEP_SLEEP)) {
		ret = -1;
		printf("[%s]Select NOT support low power stage to check.\n",
			__func__);
		return ret;
	}

	switch (stage) {
	case PRE_DEEP_SLEEP:
		/* Clear deep sleep status */
		pLPRGUBase->CM4_SLP_AUX |= CM4_WAKEUP_FROM_DSLEEP;
		printf("Clear deep sleep status done.\n");
		break;
	case POST_DEEP_SLEEP:
		/* Get deep sleep status */
		ret = (pLPRGUBase->CM4_SLP_AUX) & CM4_WAKEUP_FROM_DSLEEP;
		if (ret)
			printf("Have entered deep sleep and recovered successfully!\n");
		else
			printf("Enter deep sleep failed!!!\n");
		break;
	}

	return 0;
}
