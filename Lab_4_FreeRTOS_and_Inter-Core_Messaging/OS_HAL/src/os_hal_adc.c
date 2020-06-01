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

#if defined(OSAI_ENABLE_DMA) && !defined(OSAI_FREERTOS)
static __attribute__((section(".sysram"))) uint8_t adc_dma_buf[ADC_DMA_BUF_WORD_SIZE];
#endif

#define CM4_ADC_BASE				0x38000000
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

struct mtk_adc_controller_rtos *_mtk_os_hal_adc_get_ctlr(void)
{
	return &g_adc_ctlr_rtos;
}

static int _mtk_os_hal_adc_irq_handler(struct mtk_adc_controller *ctlr)
{
	if (!ctlr)
		return -ADC_EPTR;

	if (ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DIRECT)
		return -ADC_EPARAMETER;

	mtk_mhal_adc_fifo_handle_rx(ctlr);

	return 0;
}

static void _mtk_os_hal_adc_irq_event(void)
{
	struct mtk_adc_controller_rtos *ctlr_rtos =
		_mtk_os_hal_adc_get_ctlr();
	struct mtk_adc_controller *ctlr;

	ctlr = ctlr_rtos->ctlr;
	_mtk_os_hal_adc_irq_handler(ctlr);

}


static int _mtk_os_hal_adc_request_irq(struct mtk_adc_controller *ctlr)
{
	if (!ctlr)
		return -ADC_EPTR;

	CM4_Install_NVIC(CM4_IRQ_ADC, CM4_ADC_PRI, IRQ_LEVEL_TRIGGER,
		_mtk_os_hal_adc_irq_event, TRUE);


	return 0;
}

static int _mtk_os_hal_adc_wait_for_completion_timeout(
				struct mtk_adc_controller_rtos
				*ctlr_rtos, int time_ms)
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

static int _mtk_os_hal_adc_rx_done_callback(void *data)
{
#ifdef OSAI_FREERTOS
	BaseType_t x_higher_priority_task_woken = pdFALSE;
	struct mtk_adc_controller_rtos *ctlr_rtos = data;

	/* while using DMA mode, release semaphore in this callback */
	xSemaphoreGiveFromISR(ctlr_rtos->rx_completion,
			      &x_higher_priority_task_woken);
	portYIELD_FROM_ISR(x_higher_priority_task_woken);
#else
	struct mtk_adc_controller_rtos *ctlr_rtos = data;

	ctlr_rtos->rx_completion++;
#endif
	return 0;
}

int mtk_os_hal_adc_ctlr_init(adc_pmode pmode, adc_fifo_mode fifo_mode,
					u16 bit_map)
{
	struct mtk_adc_controller_rtos *ctlr_rtos;
	struct mtk_adc_controller *ctlr;
	u32 channel_index = 0;
	int ret = 0;

	ctlr_rtos =	_mtk_os_hal_adc_get_ctlr();
	if (!ctlr_rtos)
		return -ADC_EPTR;

	if ((pmode != ADC_PMODE_ONE_TIME) && (pmode != ADC_PMODE_PERIODIC))
		return -ADC_EPARAMETER;

#ifndef OSAI_ENABLE_DMA
	if (fifo_mode == ADC_FIFO_DMA)
		return -ADC_EPARAMETER;
#endif

	if ((fifo_mode != ADC_FIFO_DIRECT) && (fifo_mode != ADC_FIFO_DMA))
		return -ADC_EPARAMETER;

	ctlr_rtos->ctlr = &adc_controller;

	if (!ctlr_rtos->ctlr)
		return -ADC_EPTR;

	ctlr = ctlr_rtos->ctlr;

	ctlr->adc_fsm_parameter = &adc_fsm_parameter;

	ctlr->base = (void __iomem *)CM4_ADC_BASE;
	ctlr->cg_base = (void __iomem *)CM4_ADC_TOPCFGAON_CLK_RG;

	ret = mtk_mhal_adc_enable_clk(ctlr);
	if (ret)
		return ret;

	ctlr->adc_fsm_parameter->pmode = pmode;
	ctlr->adc_fsm_parameter->avg_mode = ADC_AVG_32_SAMPLE;
	ctlr->adc_fsm_parameter->channel_map = bit_map;
	ctlr->adc_fsm_parameter->period = PMODE_PERIOD;
	ctlr->adc_fsm_parameter->fifo_mode = fifo_mode;
	ctlr->adc_fsm_parameter->ier_mode = ADC_FIFO_IER_RXFULL;
	if (ctlr->adc_fsm_parameter->fifo_mode == ADC_FIFO_DMA) {
#ifdef OSAI_ENABLE_DMA

#ifdef OSAI_FREERTOS
		ctlr->adc_fsm_parameter->dma_vfifo_addr = pvPortMalloc(ADC_DMA_BUF_WORD_SIZE);
		ctlr->adc_fsm_parameter->dma_vfifo_len = ADC_DMA_BUF_WORD_SIZE;
#else
		ctlr->adc_fsm_parameter->dma_vfifo_addr = (u32 *)adc_dma_buf;
		ctlr->adc_fsm_parameter->dma_vfifo_len = ADC_DMA_BUF_WORD_SIZE;
#endif

#else	/* OSAI_ENABLE_DMA */
		ctlr->adc_fsm_parameter->dma_vfifo_addr = NULL;
		ctlr->adc_fsm_parameter->dma_vfifo_len = 0;
#endif

		ctlr->dma_channel = VDMA_ADC_RX_CH29;
		ctlr->use_dma = 1;
	}

	for (channel_index = 0; channel_index < ADC_CHANNEL_MAX;
			channel_index++) {
		ctlr->current_xfer[channel_index].count = 0;
		ctlr->current_xfer[channel_index].write_point = 0;
		ctlr->current_xfer[channel_index].read_point = 0;
	}

	ret = mtk_mhal_adc_init(ctlr);
	if (ret)
		return ret;

	if (fifo_mode == ADC_FIFO_DIRECT) {
		ctlr->use_dma = 0;
		ret = _mtk_os_hal_adc_request_irq(ctlr);
		if (ret)
			return ret;
	}
#ifdef OSAI_FREERTOS
	if (!ctlr_rtos->rx_completion)
		ctlr_rtos->rx_completion = xSemaphoreCreateBinary();
#else
	ctlr_rtos->rx_completion = 0;
#endif

	ret = mtk_mhal_adc_rx_notify_callback_register(ctlr,
					 _mtk_os_hal_adc_rx_done_callback,
					 (void *)ctlr_rtos);
	if (ret)
		return ret;

	ret = mtk_mhal_adc_fsm_param_set(ctlr, ctlr->adc_fsm_parameter);
	if (ret)
		return ret;

	return ret;
}

int mtk_os_hal_adc_start(void)
{
	struct mtk_adc_controller_rtos *ctlr_rtos =
		_mtk_os_hal_adc_get_ctlr();

	if (!ctlr_rtos)
		return -ADC_EPTR;

	return mtk_mhal_adc_start(ctlr_rtos->ctlr);
}

int mtk_os_hal_adc_start_ch(u16 ch_bit_map)
{
	struct mtk_adc_controller_rtos *ctlr_rtos =
		_mtk_os_hal_adc_get_ctlr();

	if (!ctlr_rtos)
		return -ADC_EPTR;

	 return mtk_mhal_adc_start_ch(ctlr_rtos->ctlr, ch_bit_map);
}

int mtk_os_hal_adc_ctlr_deinit(void)
{
	int ret = 0;
	struct mtk_adc_controller_rtos *ctlr_rtos =
		_mtk_os_hal_adc_get_ctlr();

	if (!ctlr_rtos)
		return -ADC_EPTR;

	if (!ctlr_rtos->ctlr)
		return -ADC_EPTR;

#ifdef OSAI_FREERTOS
	vSemaphoreDelete(ctlr_rtos->rx_completion);
	ctlr_rtos->rx_completion = NULL;
#else
	ctlr_rtos->rx_completion = 0;
#endif

	if (ctlr_rtos->ctlr->adc_fsm_parameter->fifo_mode == ADC_FIFO_DIRECT) {
		/* Disable ADC IRQ */
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ADC);
	}

	ret = mtk_mhal_adc_stop(ctlr_rtos->ctlr);
	if (ret)
		return ret;

	ret = mtk_mhal_adc_deinit(ctlr_rtos->ctlr);
	if (ret)
		return ret;

	ret = mtk_mhal_adc_disable_clk(ctlr_rtos->ctlr);
	if (ret)
		return ret;

	if (ctlr_rtos->ctlr->adc_fsm_parameter->fifo_mode == ADC_FIFO_DMA) {
#if defined(OSAI_ENABLE_DMA) && defined(OSAI_FREERTOS)
		vPortFree(ctlr_rtos->ctlr->adc_fsm_parameter->dma_vfifo_addr);
#endif
	}

	return 0;
}

int mtk_os_hal_adc_fsm_param_set(struct adc_fsm_param *adc_fsm_parameter)
{
	struct mtk_adc_controller_rtos *ctlr_rtos;
	struct mtk_adc_controller *ctlr;
	u32 channel_index = 0;
	int ret = 0;

	ctlr_rtos =	_mtk_os_hal_adc_get_ctlr();
	if (!ctlr_rtos)
		return -ADC_EPTR;

	if (!adc_fsm_parameter)
		return -ADC_EPTR;

#ifndef OSAI_ENABLE_DMA
	if (adc_fsm_parameter->fifo_mode == ADC_FIFO_DMA)
		return -ADC_EPARAMETER;
#endif

	ctlr = ctlr_rtos->ctlr;

	ctlr->adc_fsm_parameter->pmode = adc_fsm_parameter->pmode;
	ctlr->adc_fsm_parameter->avg_mode = adc_fsm_parameter->avg_mode;
	ctlr->adc_fsm_parameter->channel_map = adc_fsm_parameter->channel_map;
	ctlr->adc_fsm_parameter->period = adc_fsm_parameter->period;
	ctlr->adc_fsm_parameter->fifo_mode = adc_fsm_parameter->fifo_mode;
	ctlr->adc_fsm_parameter->ier_mode = adc_fsm_parameter->ier_mode;

	if (ctlr->adc_fsm_parameter->fifo_mode == ADC_FIFO_DMA)
		ctlr->adc_fsm_parameter->dma_vfifo_len = ADC_DMA_BUF_WORD_SIZE;

	for (channel_index = 0; channel_index < ADC_CHANNEL_MAX;
		channel_index++) {
		ctlr->current_xfer[channel_index].count = 0;
		ctlr->current_xfer[channel_index].write_point = 0;
		ctlr->current_xfer[channel_index].read_point = 0;
	}

	ret = mtk_mhal_adc_fsm_param_set(ctlr, adc_fsm_parameter);
	if (ret)
		return ret;

	return 0;
}
int mtk_os_hal_adc_one_shot_get_data(adc_channel sample_channel, u32 *data)
{
	struct mtk_adc_controller_rtos *ctlr_rtos;
	struct mtk_adc_controller *ctlr;
	int ret = 0;

	ctlr_rtos =	_mtk_os_hal_adc_get_ctlr();
	if (!ctlr_rtos)
		return -ADC_EPTR;

	ctlr = ctlr_rtos->ctlr;

	if ((ctlr == NULL) || (ctlr->adc_fsm_parameter == NULL))
		return -ADC_EPTR;

	if (sample_channel > ADC_CHANNEL_7)
		return -ADC_EPARAMETER;

	if ((ctlr->adc_fsm_parameter->pmode != ADC_PMODE_ONE_TIME) ||
		(ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	ret = _mtk_os_hal_adc_wait_for_completion_timeout(ctlr_rtos, 1000);
	if (ret)
		printf("Take adc master Semaphore timeout!\n");

	ret = mtk_mhal_adc_one_shot_get_data(ctlr_rtos->ctlr,
		sample_channel, data);
	if (ret)
		return ret;

	return 0;
}

int mtk_os_hal_adc_period_get_data(u32 (*rx_buf)[32], u32 *length)
{
	struct mtk_adc_controller_rtos *ctlr_rtos;
	struct mtk_adc_controller *ctlr;
	int ret = 0;
	int channel_count = 0;
	u32 size = 0;

	ctlr_rtos =	_mtk_os_hal_adc_get_ctlr();
	if (!ctlr_rtos)
		return -ADC_EPTR;

	ctlr = ctlr_rtos->ctlr;
	if ((ctlr == NULL) || (ctlr->adc_fsm_parameter == NULL))
		return -ADC_EPTR;

	ret = mtk_mhal_adc_start(ctlr_rtos->ctlr);
	if (ret)
		return ret;

	ret = _mtk_os_hal_adc_wait_for_completion_timeout(ctlr_rtos, 1000);
	if (ret)
		printf("Take adc master Semaphore timeout!\n");
	for (channel_count = 0; channel_count < ADC_CHANNEL_MAX;
			channel_count++) {
		if (ctlr->adc_fsm_parameter->channel_map & BIT(channel_count)) {
			ret = mtk_mhal_adc_period_get_data(ctlr,
				(adc_channel)channel_count,
				rx_buf[channel_count], &size);
		if (ret)
			return ret;
	}
		length[channel_count] = size;

	}
	ret = mtk_mhal_adc_stop(ctlr);
	if (ret)
		return ret;

	if (ctlr->adc_fsm_parameter->fifo_mode == ADC_FIFO_DMA) {
		ret = mtk_mhal_adc_stop_dma(ctlr);
		if (ret)
			return ret;
	}

	return 0;
}
