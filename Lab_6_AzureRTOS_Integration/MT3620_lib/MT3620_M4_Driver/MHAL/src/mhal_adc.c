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

#include "hdl_adc.h"
#include "mhal_adc.h"
#include "mhal_osai.h"

#define ADC_DMA_RX_PORT					0x38000200
#define ADC_CLK_RATE 2000000
/**< ADC clock source */

static void _mtk_mhal_adc_fifo_get_length(struct mtk_adc_controller *ctlr,
	u32 *length)
{
		mtk_hdl_adc_fifo_get_length(ctlr->base, length);
}

static int _mtk_mhal_adc_fifo_read_data(struct mtk_adc_controller *ctlr,
	u32 length, u32 *samples_data)
{
	u32 counter = 0;

	/* Pull out data from RX buffer register */
	for (counter = 0; counter < length; counter++) {
		samples_data[counter] =
			mtk_hdl_adc_fifo_read_data(ctlr->base);
		adc_debug("\t-length(%d),samples_data[counter]%x.\n",
			length, samples_data[counter]);
	}

	return 0;
}

static int _mtk_mhal_adc_move_tx_point(struct mtk_adc_controller *ctlr,
			       u32 buffer_length)
{
	u8 bresut = 0;

	bresut = osai_dma_update_vfifo_swptr(ctlr->dma_channel, buffer_length);

	if (bresut) {
		adc_err("RX DMA move point fail\n");
		return -ADC_EPTR;
	}

	return 0;
}

static int _mtk_mhal_adc_fifo_sample_data(struct mtk_adc_controller *ctlr)
{
	u32 valid_fifo_len = 0;
	struct adc_fsm_param *adc_fsm_params;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	/*Query fifo available length */
	_mtk_mhal_adc_fifo_get_length(ctlr, &valid_fifo_len);

	/*Get fifo data*/
	_mtk_mhal_adc_fifo_read_data(ctlr, valid_fifo_len, adc_fsm_params->vfifo_addr);

	return 0;
}

static int _mtk_mhal_adc_dma_release_chan(struct mtk_adc_controller *ctlr)
{
	return osai_dma_release_chan(ctlr->dma_channel);
}

static void _mtk_mhal_adc_vdma_callback(void *data)
{
	struct mtk_adc_controller *ctlr = data;
	struct adc_fsm_param *adc_fsm_params;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	_mtk_mhal_adc_move_tx_point(ctlr,
		adc_fsm_params->rx_period_len);

	if (adc_fsm_params->rx_callback_func != NULL)
		adc_fsm_params->rx_callback_func(
			adc_fsm_params->rx_callback_data);

	ctlr->adc_processing = false;
}

static int _mtk_mhal_adc_dma_config(struct mtk_adc_controller *ctlr)
{
	struct osai_dma_config config;
	struct adc_fsm_param *adc_fsm_params;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	config.interrupt_flag = OSAI_DMA_INT_VFIFO_THRESHOLD;
	config.transize = OSAI_DMA_SIZE_LONG;
	config.dir = 1;

	config.vfifo_thrsh = adc_fsm_params->rx_period_len;
	/** Virtual FIFO size, only for Virtual FIFO DMA */
	config.vfifo_size = adc_fsm_params->vfifo_len;
	config.done_callback_data = ctlr;
	config.count = 0;
	config.vfifo_timeout_cnt = 0;
	config.excep_callback = NULL;
	config.excep_callback_data = NULL;
	adc_debug("\tconfig.vfifo_thrsh %d.\n", config.vfifo_thrsh);
	adc_debug("\tconfig.vfifo_size %d.\n", config.vfifo_size);

	ctlr->rx_addr =
		osai_get_phyaddr(adc_fsm_params->vfifo_addr);
	adc_debug("\tmtk mhal_adc_dma_config  vfifo_addr %p.\n",
		adc_fsm_params->vfifo_addr);
	/* ADC rx */
	config.src_addr = ADC_DMA_RX_PORT;
	config.dst_addr = ctlr->rx_addr;
	config.done_callback = _mtk_mhal_adc_vdma_callback;

	return osai_dma_config(ctlr->dma_channel, &config);
}

int _mtk_mhal_adc_stop_dma(struct mtk_adc_controller *ctlr)
{
	if (!ctlr)
		return -ADC_EPTR;

	return osai_dma_stop(ctlr->dma_channel);
}

int mtk_mhal_adc_enable_clk(struct mtk_adc_controller *ctlr)
{
	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->cg_base)
		return -ADC_EPTR;

	mtk_hdl_adc_enable_clk(ctlr->cg_base);

	return 0;
}

int mtk_mhal_adc_disable_clk(struct mtk_adc_controller *ctlr)
{
	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->cg_base)
		return -ADC_EPTR;

	mtk_hdl_adc_disable_clk(ctlr->cg_base);

	return 0;
}

int mtk_mhal_adc_init(struct mtk_adc_controller *ctlr)
{
	struct adc_fsm_param *adc_fsm_params;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	if ((adc_fsm_params->fifo_mode != ADC_FIFO_DMA) &&
		(adc_fsm_params->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	mtk_hdl_adc_reset(ctlr->base);

	mtk_hal_adc_init(ctlr->base);

	return 0;
}

int mtk_mhal_adc_deinit(struct mtk_adc_controller *ctlr)
{
	int ret = 0;
	struct adc_fsm_param *adc_fsm_params;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->cg_base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	if ((adc_fsm_params->fifo_mode != ADC_FIFO_DMA) &&
		(adc_fsm_params->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	if (adc_fsm_params->fifo_mode == ADC_FIFO_DMA) {
		ret = _mtk_mhal_adc_dma_release_chan(ctlr);
		if (ret)
			return -ADC_EPTR;
	}
	mtk_hdl_adc_reset(ctlr->base);
	mtk_hal_adc_uninit(ctlr->base);

	return 0;
}

int mtk_mhal_adc_fsm_param_set(struct mtk_adc_controller *ctlr,
		struct adc_fsm_param *adc_fsm_parameter)
{
	int ret = 0;
	adc_avg_mode avg_mode;
	int channel_num = 0;
	u32 adc_no = 0;
	u32 period;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	if ((adc_fsm_parameter->fifo_mode != ADC_FIFO_DMA) &&
		(adc_fsm_parameter->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	if ((adc_fsm_parameter->pmode != ADC_PMODE_ONE_TIME) &&
		(adc_fsm_parameter->pmode != ADC_PMODE_PERIODIC))
		return -ADC_EPARAMETER;

	/* Parameter check */
	if (adc_fsm_parameter->channel_map == 0)
		return -ADC_EPARAMETER;

	ctlr->adc_processing = false;

	switch (adc_fsm_parameter->fifo_mode) {
	case ADC_FIFO_DMA:
		/* Initialize ADC Virtual FIFO DMA*/
		/*set alert*/
		ret = osai_dma_allocate_chan(ctlr->dma_channel);
		if (ret)
			return ret;
		adc_debug("\t-vfifo_addr(0x%p)/vfifo_len(%d)-.\n",
			adc_fsm_parameter->vfifo_addr,
			adc_fsm_parameter->vfifo_len);
		adc_debug("\t-rx_period_len(%d)-.\n",
			adc_fsm_parameter->rx_period_len);

		/* DMA mode */
		ret = _mtk_mhal_adc_dma_config(ctlr);
		if (ret)
			return ret;

		mtk_hdl_adc_dma_enable(ctlr->base);

		break;
	case ADC_FIFO_DIRECT:

		adc_debug("\tFIFO mode.\n");

		/* Direct mode */
		if (adc_fsm_parameter->ier_mode ==
			ADC_FIFO_IER_RXFULL)
			mtk_hdl_adc_fifo_rx_full_enable(ctlr->base);

		if (adc_fsm_parameter->pmode == ADC_PMODE_ONE_TIME)
			mtk_hdl_adc_periodic_mode_set(ctlr->base,
			ADC_PMODE_ONE_TIME);

		mtk_hdl_adc_trigger_level_set(ctlr->base, adc_fsm_parameter->rx_period_len);
		/*set trigger level equal to used channel number!!*/

		break;
	}

	if (adc_fsm_parameter->pmode == ADC_PMODE_PERIODIC) {
		for (adc_no = 0; adc_no < ADC_CHANNEL_MAX; adc_no++) {
			if (adc_fsm_parameter->channel_map & BIT(adc_no))
				channel_num++;
		}
		avg_mode = ADC_AVG_8_SAMPLE;
		if ((adc_fsm_parameter->sample_rate  * channel_num) <= 90000)
			period =
				(ADC_CLK_RATE / adc_fsm_parameter->sample_rate) -
				(ADC_INIT_TIME + 1) - channel_num *
				(ADC_CH_STABLE_TIME + 2 +
				((1 << avg_mode) + 1)) - 1;
		else
			return -ADC_EPARAMETER;
	} else {
		period = 0;
		avg_mode = ADC_AVG_1_SAMPLE;
	}

	mtk_hdl_adc_fsm_param_set(ctlr->base, adc_fsm_parameter->pmode,
		avg_mode, adc_fsm_parameter->channel_map, period);

	return 0;
}

int mtk_mhal_adc_start(struct mtk_adc_controller *ctlr)
{
	int ret = 0;
	struct adc_fsm_param *adc_fsm_params;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	if (ctlr->adc_processing == true) {
		/*ADC is processing*/
		return -ADC_EPARAMETER;
	}

	ctlr->adc_processing = true;

	switch (adc_fsm_params->fifo_mode) {
	case ADC_FIFO_DMA:
		osai_dma_clr_dreq(ctlr->dma_channel);
		ret = osai_dma_start(ctlr->dma_channel);
		if (ret < 0)
			return ret;
		mtk_hdl_adc_start(ctlr->base);
		break;
	case ADC_FIFO_DIRECT:
		if (adc_fsm_params->pmode == ADC_PMODE_ONE_TIME) {
			adc_fsm_params->ier_mode = ADC_FIFO_IER_RXFULL;
			mtk_hdl_adc_fifo_rx_full_enable(
				ctlr->base);
			/*enable interrupt*/
		}
		mtk_hdl_adc_trigger_level_set(ctlr, adc_fsm_params->rx_period_len);
		mtk_hdl_adc_start(ctlr->base);
		/*set trigger level equal to used channel number!!*/
		break;
	}

	return ret;
}

int mtk_mhal_adc_stop(struct mtk_adc_controller *ctlr)
{
	int ret = 0;
	struct adc_fsm_param *adc_fsm_params;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	if ((adc_fsm_params->fifo_mode != ADC_FIFO_DMA) &&
		(adc_fsm_params->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	if (adc_fsm_params->fifo_mode == ADC_FIFO_DMA)
		_mtk_mhal_adc_stop_dma(ctlr);

	mtk_hdl_adc_stop(ctlr->base);

	mtk_hdl_adc_reset(ctlr->base);

	return ret;
}

int mtk_mhal_adc_fifo_handle_rx(struct mtk_adc_controller *ctlr)
{
	struct adc_fsm_param *adc_fsm_params;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	adc_fsm_params = ctlr->adc_fsm_parameter;

	_mtk_mhal_adc_fifo_sample_data(ctlr);

	if (adc_fsm_params->pmode == ADC_PMODE_ONE_TIME)
		mtk_hdl_adc_stop(ctlr->base);

	ctlr->adc_processing = false;

	return 0;
}
