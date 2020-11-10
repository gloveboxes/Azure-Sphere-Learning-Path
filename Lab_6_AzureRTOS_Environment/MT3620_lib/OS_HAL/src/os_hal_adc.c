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

#ifdef OSAI_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif
#include "nvic.h"

#include "os_hal_adc.h"
#include "os_hal_dma.h"

#define CM4_ADC_BASE					0x38000000
#define CM4_ADC_TOPCFGAON_CLK_RG		0x30030208

struct mtk_adc_controller_rtos {
	struct mtk_adc_controller *ctlr;
	/* the type based on OS */
#ifdef OSAI_FREERTOS
	QueueHandle_t rx_completion;
#else
	volatile u8 rx_completion;
#endif
};
static struct adc_fsm_param adc_fsm_parameter;

static struct mtk_adc_controller adc_controller;

static struct mtk_adc_controller_rtos g_adc_ctlr_rtos;

static struct mtk_adc_controller_rtos *_mtk_os_hal_adc_get_ctlr(void)
{
	return &g_adc_ctlr_rtos;
}

static int _mtk_os_hal_adc_irq_handler(
	struct mtk_adc_controller_rtos *ctlr_rtos)
{
#ifdef OSAI_FREERTOS
	BaseType_t x_higher_priority_task_woken = pdFALSE;
#endif

	mtk_mhal_adc_fifo_handle_rx(ctlr_rtos->ctlr);

#ifdef OSAI_FREERTOS
	xSemaphoreGiveFromISR(ctlr_rtos->rx_completion,
				  &x_higher_priority_task_woken);
	portYIELD_FROM_ISR(x_higher_priority_task_woken);
#else
	ctlr_rtos->rx_completion++;
#endif

	return 0;
}

static void _mtk_os_hal_adc_irq_event(void)
{
	struct mtk_adc_controller_rtos *ctlr_rtos =
		_mtk_os_hal_adc_get_ctlr();

	_mtk_os_hal_adc_irq_handler(ctlr_rtos);
}

static int _mtk_os_hal_adc_request_irq(struct mtk_adc_controller *ctlr)
{
	CM4_Install_NVIC(CM4_IRQ_ADC, CM4_ADC_PRI, IRQ_LEVEL_TRIGGER,
		_mtk_os_hal_adc_irq_event, TRUE);

	return 0;
}

static int _mtk_os_hal_adc_wait_for_completion_timeout(
				struct mtk_adc_controller_rtos *ctlr_rtos,
				int time_ms)
{
#ifdef OSAI_FREERTOS
	if (pdTRUE != xSemaphoreTake(ctlr_rtos->rx_completion,
				     time_ms / portTICK_RATE_MS))
		return -1;
#else
	extern volatile u32 sys_tick_in_ms;
	uint32_t start_tick = sys_tick_in_ms;

	while (ctlr_rtos->rx_completion == 0) {
		if (sys_tick_in_ms - start_tick > time_ms)
			return -1;
	}
	ctlr_rtos->rx_completion--;
#endif

	return 0;
}

int mtk_os_hal_adc_ctlr_init(void)
{
	struct mtk_adc_controller_rtos *ctlr_rtos;
	struct mtk_adc_controller *ctlr;
	int ret = 0;

	ctlr_rtos =	_mtk_os_hal_adc_get_ctlr();
	if (!ctlr_rtos)
		return -ADC_EPTR;

	ctlr_rtos->ctlr = &adc_controller;

	ctlr = ctlr_rtos->ctlr;

	ctlr->adc_fsm_parameter = &adc_fsm_parameter;

	ctlr->base = (void __iomem *)CM4_ADC_BASE;
	ctlr->cg_base = (void __iomem *)CM4_ADC_TOPCFGAON_CLK_RG;

	ret = mtk_mhal_adc_enable_clk(ctlr);
	if (ret)
		return ret;

#ifdef OSAI_FREERTOS
	if (!ctlr_rtos->rx_completion)
		ctlr_rtos->rx_completion = xSemaphoreCreateBinary();
#else
	ctlr_rtos->rx_completion = 0;
#endif

	ret = mtk_mhal_adc_init(ctlr);
	if (ret)
		return ret;

	ret = _mtk_os_hal_adc_request_irq(ctlr);
	if (ret)
		return ret;

	return ret;
}

int mtk_os_hal_adc_ctlr_deinit(void)
{
	int ret = 0;
	struct mtk_adc_controller_rtos *ctlr_rtos =
		_mtk_os_hal_adc_get_ctlr();
	struct mtk_adc_controller *ctlr;

	if (!ctlr_rtos)
		return -ADC_EPTR;

	if (!ctlr_rtos->ctlr)
		return -ADC_EPTR;

	ctlr = ctlr_rtos->ctlr;

	/* Disable ADC IRQ */
	NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ADC);
#ifdef OSAI_FREERTOS
	vSemaphoreDelete(ctlr_rtos->rx_completion);
	ctlr_rtos->rx_completion = NULL;
#else
	ctlr_rtos->rx_completion = 0;
#endif

	ret = mtk_mhal_adc_stop(ctlr);
	if (ret)
		return ret;

	ret = mtk_mhal_adc_deinit(ctlr);
	if (ret)
		return ret;

	ret = mtk_mhal_adc_disable_clk(ctlr);
	if (ret)
		return ret;

	return 0;
}

int mtk_os_hal_adc_fsm_param_set(struct adc_fsm_param *adc_fsm_parameter)
{
	struct mtk_adc_controller_rtos *ctlr_rtos;
	struct mtk_adc_controller *ctlr;
	struct adc_fsm_param *adc_fsm_params;
	int channel_num = 0;
	u32 adc_no = 0;
	int ret = 0;

	ctlr_rtos =	_mtk_os_hal_adc_get_ctlr();
	if (!ctlr_rtos)
		return -ADC_EPTR;

	ctlr = ctlr_rtos->ctlr;

	if (ctlr->adc_fsm_parameter == NULL)
		return -ADC_EPTR;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	for (adc_no = 0; adc_no < ADC_CHANNEL_MAX; adc_no++) {
		if (adc_fsm_parameter->channel_map & BIT(adc_no))
			channel_num++;
	}

	adc_fsm_params->pmode = adc_fsm_parameter->pmode;
	adc_fsm_params->channel_map = adc_fsm_parameter->channel_map;
	adc_fsm_params->fifo_mode = adc_fsm_parameter->fifo_mode;
	adc_fsm_params->ier_mode = adc_fsm_parameter->ier_mode;
	adc_fsm_params->vfifo_addr =
		adc_fsm_parameter->vfifo_addr;

	if (adc_fsm_params->pmode == ADC_PMODE_ONE_TIME) {
		adc_fsm_params->rx_period_len = channel_num;
		adc_fsm_params->fifo_mode = ADC_FIFO_DIRECT;
		adc_fsm_params->vfifo_len =
			adc_fsm_parameter->vfifo_len;
		if (adc_fsm_params->vfifo_len != adc_fsm_params->rx_period_len)
			return -ADC_EPARAMETER;

	} else {
		adc_fsm_params->sample_rate = adc_fsm_parameter->sample_rate;
		adc_fsm_params->rx_period_len =
			adc_fsm_parameter->rx_period_len * 4;
		adc_fsm_params->vfifo_len =
			adc_fsm_parameter->vfifo_len * 4;
		adc_fsm_params->fifo_mode = ADC_FIFO_DMA;
		if (adc_fsm_params->vfifo_len < adc_fsm_params->rx_period_len ||
			(adc_fsm_params->rx_period_len % channel_num) != 0)
			return -ADC_EPARAMETER;
	}

	if (adc_fsm_params->fifo_mode == ADC_FIFO_DMA) {
		adc_fsm_params->rx_callback_func =
			adc_fsm_parameter->rx_callback_func;
		adc_fsm_params->rx_callback_data =
			adc_fsm_parameter->rx_callback_data;
		ctlr->dma_channel = VDMA_ADC_RX_CH29;
	}
	ret = mtk_mhal_adc_fsm_param_set(ctlr, adc_fsm_params);
	if (ret)
		return ret;

	return 0;
}

int mtk_os_hal_adc_trigger_one_shot_once(void)
{
	int ret = 0;
	struct mtk_adc_controller_rtos *ctlr_rtos;
	struct mtk_adc_controller *ctlr;
	struct adc_fsm_param *adc_fsm_params;

	ctlr_rtos =	_mtk_os_hal_adc_get_ctlr();
	if (!ctlr_rtos)
		return -ADC_EPTR;

	ctlr = ctlr_rtos->ctlr;

	if (ctlr->adc_fsm_parameter == NULL)
		return -ADC_EPTR;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	if (adc_fsm_params->pmode != ADC_PMODE_ONE_TIME)
		return -ADC_EPARAMETER;

	ret = mtk_mhal_adc_start(ctlr);
	if (ret)
		return ret;

	ret = _mtk_os_hal_adc_wait_for_completion_timeout(ctlr_rtos,
		1000);
	if (ret) {
		printf("Take adc master Semaphore timeout!\n");
		return ret;
	}

	return ret;
}

int mtk_os_hal_adc_period_start(void)
{
	int ret = 0;
	struct mtk_adc_controller_rtos *ctlr_rtos;
	struct mtk_adc_controller *ctlr;
	struct adc_fsm_param *adc_fsm_params;

	ctlr_rtos =	_mtk_os_hal_adc_get_ctlr();
	if (!ctlr_rtos)
		return -ADC_EPTR;

	ctlr = ctlr_rtos->ctlr;

	if (ctlr->adc_fsm_parameter == NULL)
		return -ADC_EPTR;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	if ((adc_fsm_params->pmode != ADC_PMODE_PERIODIC) ||
		(adc_fsm_params->fifo_mode != ADC_FIFO_DMA))
		return -ADC_EPARAMETER;

	ret = mtk_mhal_adc_start(ctlr);
	if (ret)
		return ret;

	return ret;
}

int mtk_os_hal_adc_period_stop(void)
{
	int ret = 0;
	struct mtk_adc_controller_rtos *ctlr_rtos;
	struct mtk_adc_controller *ctlr;
	struct adc_fsm_param *adc_fsm_params;

	ctlr_rtos =	_mtk_os_hal_adc_get_ctlr();
	if (!ctlr_rtos)
		return -ADC_EPTR;

	ctlr = ctlr_rtos->ctlr;

	if (ctlr->adc_fsm_parameter == NULL)
		return -ADC_EPTR;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	if ((adc_fsm_params->pmode != ADC_PMODE_PERIODIC) ||
		(adc_fsm_params->fifo_mode != ADC_FIFO_DMA))
		return -ADC_EPARAMETER;

	ret = mtk_mhal_adc_stop(ctlr);
	if (ret)
		return ret;

	return ret;
}

