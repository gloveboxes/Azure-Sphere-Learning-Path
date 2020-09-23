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

static void _mtk_mhal_adc_fifo_get_length(struct mtk_adc_controller *ctlr,
	u32 *length)
{
	switch (ctlr->adc_fsm_parameter->fifo_mode) {
	case ADC_FIFO_DMA:
		/* Get the data length from VFIFO DMA FFCNT */
		*length = osai_dma_get_param(ctlr->dma_channel,
			OSAI_DMA_PARAM_VFF_FIFO_CNT);
		break;
	case ADC_FIFO_DIRECT:
		mtk_hdl_adc_fifo_get_length(ctlr->base, length);
		break;
	}
}

static int _mtk_mhal_adc_fifo_read_data(struct mtk_adc_controller *ctlr,
	u32 length, u32 *samples_data)
{
	u32 counter = 0;
	int ret = 0;

	switch (ctlr->adc_fsm_parameter->fifo_mode) {
	case ADC_FIFO_DMA:
		/* Pull out data from ADC VFIFO DMA virtual port */
		ret = osai_dma_vff_read_data(ctlr->dma_channel,
			(u8 *)&samples_data[counter], length);
		if (ret < 0)
			return ret;

		break;
	case ADC_FIFO_DIRECT:
		/* Pull out data from RX buffer register */
		for (counter = 0; counter < length; counter++) {
			samples_data[counter] =
				mtk_hdl_adc_fifo_read_data(ctlr->base);
			adc_debug("\t-length(%d),samples_data[counter]%x.\n",
				length, samples_data[counter]);
		}
		break;
	}

	return 0;
}

static int _mtk_mhal_adc_period_sample_data(struct mtk_adc_controller *ctlr)
{
	u8 channel_num = 0;
	u32 count = 0, write_point = 0;
	u32 valid_fifo_len = 0;
	u32 fifo_data[ADC_FIFO_SIZE] = {0};
	int ret = 0;

	_mtk_mhal_adc_fifo_get_length(ctlr, &valid_fifo_len);

	/*Query fifo available length */
	ret = _mtk_mhal_adc_fifo_read_data(ctlr, valid_fifo_len, &fifo_data[0]);
	if (ret)
		return ret;

	/*Get fifo data*/
	if (ctlr->adc_fsm_parameter->fifo_mode == ADC_FIFO_DMA)
		valid_fifo_len = valid_fifo_len/4;
	adc_debug("\tmtk mhal_adc_period_get_data valid fifo len(%d)-.\n",
		valid_fifo_len);

	for (count = 0; count < valid_fifo_len; count++) {
		channel_num = fifo_data[count] & ADC_CH_ID_MASK;
		/*Channel Id: 0~7*/

		if (channel_num >= ADC_CHANNEL_MAX)
			continue;

		write_point = ctlr->current_xfer[channel_num].write_point;
		/*Prev write pointer*/

		if (ctlr->adc_fsm_parameter->pmode == ADC_PMODE_PERIODIC) {
			if (ctlr->current_xfer[channel_num].count ==
				ADC_RING_BUF_SIZE) {
				adc_debug("period mode buf full, no deal!\n");
				continue;
			}
		}

		ctlr->current_xfer[channel_num].ring_buf[write_point] =
		((fifo_data[count] & ADC_DATA_MASK) >> ADC_DATA_BIT_OFFSET);

		adc_debug("ctlr->current_xfer[%d].count = %d.\n",
			channel_num, ctlr->current_xfer[channel_num].count);
		ctlr->current_xfer[channel_num].count++;
		if (ctlr->current_xfer[channel_num].count >= ADC_RING_BUF_SIZE)
		ctlr->current_xfer[channel_num].count = ADC_RING_BUF_SIZE;

		ctlr->current_xfer[channel_num].write_point++;
		/*Next write pointer , ADC sw buf full*/
		if (ctlr->current_xfer[channel_num].write_point >=
			ADC_RING_BUF_SIZE)
			ctlr->current_xfer[channel_num].write_point = 0;
	}

	ctlr->rx_done_callback(ctlr->callback_param);

	return 0;
}

static int _mtk_mhal_adc_dma_release_chan(struct mtk_adc_controller *ctlr)
{
	return osai_dma_release_chan(ctlr->dma_channel);
}

static void _mtk_mhal_adc_vdma_callback(void *data)
{
	struct mtk_adc_controller *ctlr = data;

	_mtk_mhal_adc_period_sample_data(ctlr);

	ctlr->adc_processing = false;
}

static int _mtk_mhal_adc_dma_config(struct mtk_adc_controller *ctlr)
{
	struct osai_dma_config config;

	config.interrupt_flag = OSAI_DMA_INT_VFIFO_THRESHOLD;
	config.transize = OSAI_DMA_SIZE_LONG;
	config.dir = 1;
	config.vfifo_thrsh = ctlr->channel_count * 4;
	/** Virtual FIFO size, only for Virtual FIFO DMA */
	config.vfifo_size = ctlr->adc_fsm_parameter->dma_vfifo_len;
	config.done_callback_data = ctlr;
	config.count = 0;
	config.vfifo_timeout_cnt = 0;
	config.excep_callback = NULL;
	config.excep_callback_data = NULL;

	ctlr->rx_addr =
		osai_get_phyaddr(ctlr->adc_fsm_parameter->dma_vfifo_addr);
	adc_debug("\tmtk mhal_adc_dma_config  ctlr->rx_addr~%ld.\n",
		ctlr->rx_addr);
	adc_debug("\tmtk mhal_adc_dma_config  dma_vfifo_addr %p.\n",
		ctlr->adc_fsm_parameter->dma_vfifo_addr);
	/* ADC rx */
	config.src_addr = ADC_DMA_RX_PORT;
	config.dst_addr = ctlr->rx_addr;
	config.done_callback = _mtk_mhal_adc_vdma_callback;
	adc_debug("\tmtk mhal_adc_dma_config.\n");

	return osai_dma_config(ctlr->dma_channel, &config);
}

int mtk_mhal_adc_stop_dma(struct mtk_adc_controller *ctlr)
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
	int ret = 0;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	if ((ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DMA) &&
		(ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	mtk_hal_adc_init(ctlr->base);
	if (ctlr->adc_fsm_parameter->fifo_mode == ADC_FIFO_DMA) {

		ret = osai_dma_allocate_chan(ctlr->dma_channel);
		if (ret)
			return ret;
	}

	return 0;
}

int mtk_mhal_adc_deinit(struct mtk_adc_controller *ctlr)
{
	int ret = 0;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->cg_base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	if ((ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DMA) &&
		(ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	if (ctlr->adc_fsm_parameter->fifo_mode == ADC_FIFO_DMA) {
		ret = _mtk_mhal_adc_dma_release_chan(ctlr);
		if (ret)
			return -ADC_EPTR;
	}
	mtk_hal_adc_uninit(ctlr->base);

	return 0;
}

int mtk_mhal_adc_fsm_param_set(struct mtk_adc_controller *ctlr,
		struct adc_fsm_param *adc_fsm_parameter)
{
	u8 channel_num = 0, i = 0;
	int ret = 0;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	if ((adc_fsm_parameter->avg_mode !=  ADC_AVG_1_SAMPLE) &&
		(adc_fsm_parameter->avg_mode !=  ADC_AVG_2_SAMPLE) &&
		(adc_fsm_parameter->avg_mode !=  ADC_AVG_4_SAMPLE) &&
		(adc_fsm_parameter->avg_mode !=  ADC_AVG_8_SAMPLE) &&
		(adc_fsm_parameter->avg_mode !=  ADC_AVG_16_SAMPLE) &&
		(adc_fsm_parameter->avg_mode !=  ADC_AVG_32_SAMPLE) &&
		(adc_fsm_parameter->avg_mode !=  ADC_AVG_64_SAMPLE)) {
		return -ADC_EPARAMETER;
	}

	if ((ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DMA) &&
		(ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	if ((adc_fsm_parameter->pmode != ADC_PMODE_ONE_TIME) &&
		(adc_fsm_parameter->pmode != ADC_PMODE_PERIODIC))
		return -ADC_EPARAMETER;

	/* Parameter check */
	if (adc_fsm_parameter->channel_map == 0) {
		adc_err("\tillegal channel map.\n");
		return -ADC_EPARAMETER;
	}

	for (i = 0; i < ADC_CHANNEL_MAX; i++) {
		if (adc_fsm_parameter->channel_map & BIT(i))
			channel_num++;
	}

	ctlr->channel_count = channel_num;
	ctlr->adc_processing = false;

	switch (adc_fsm_parameter->fifo_mode) {
	case ADC_FIFO_DMA:
		/* Initialize ADC Virtual FIFO DMA*/
		/*set alert*/
		adc_debug("\t-DMA Alert Enable-.\n");
		adc_debug("\t-channel_num(%d)-.\n", channel_num);
		adc_debug("\t-dma_vfifo_addr(0x%p)/dma_vfifo_len(%d)-.\n",
		adc_fsm_parameter->dma_vfifo_addr,
			adc_fsm_parameter->dma_vfifo_len);

		/* DMA mode */
		ret = _mtk_mhal_adc_dma_config(ctlr);
		if (ret)
			return ret;
		adc_debug("\tmtk mhal_adc_fsm_param_set.\n");
		mtk_hdl_adc_dma_enable(ctlr->base);

		break;
	case ADC_FIFO_DIRECT:

		adc_debug("\tFIFO mode.\n");

		/* Direct mode */
		if ((ctlr->adc_fsm_parameter->ier_mode ==
			ADC_FIFO_IER_RXFULL) ||
			(ctlr->adc_fsm_parameter->ier_mode ==
			ADC_FIFO_IER_BOTH)) {
			mtk_hdl_adc_fifo_rx_full_enable(ctlr->base);
		}

		if ((ctlr->adc_fsm_parameter->ier_mode ==
			ADC_FIFO_IER_TIMEOUT) ||
			(ctlr->adc_fsm_parameter->ier_mode ==
			ADC_FIFO_IER_BOTH)) {
			mtk_hdl_adc_fifo_rx_timeout_enable(ctlr->base);
		}

		if (adc_fsm_parameter->pmode == ADC_PMODE_ONE_TIME)
			mtk_hdl_adc_periodic_mode_set(ctlr->base,
			ADC_PMODE_ONE_TIME);
		if (adc_fsm_parameter->pmode == ADC_PMODE_PERIODIC)
			mtk_hdl_adc_periodic_mode_set(ctlr->base,
			ADC_PMODE_PERIODIC);

		adc_debug("channel_num %d.\n", channel_num);
		mtk_hdl_adc_trigger_level_set(ctlr->base, channel_num);
		/*set trigger level equal to used channel number!!*/

		break;
	}

	ctlr->adc_fsm_parameter->pmode = adc_fsm_parameter->pmode;
	ctlr->adc_fsm_parameter->avg_mode = adc_fsm_parameter->avg_mode;
	ctlr->adc_fsm_parameter->channel_map = adc_fsm_parameter->channel_map;
	ctlr->adc_fsm_parameter->period = adc_fsm_parameter->period;
	ctlr->adc_fsm_parameter->fifo_mode = adc_fsm_parameter->fifo_mode;

	mtk_hdl_adc_fsm_param_set(ctlr->base, adc_fsm_parameter->pmode,
		adc_fsm_parameter->avg_mode, adc_fsm_parameter->channel_map,
		adc_fsm_parameter->period);

	return 0;
}

int mtk_mhal_adc_fsm_param_get(struct mtk_adc_controller *ctlr,
		struct adc_fsm_param *adc_fsm_parameter)
{
	u16 pmode = 0;
	u16 avg_mode = 0;
	u16 channel_map = 0;
	u32 period = 0;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	mtk_hdl_adc_fsm_param_get(ctlr->base, &pmode,
		&avg_mode, &channel_map, &period);

	adc_fsm_parameter->pmode = (adc_pmode)pmode;
	adc_fsm_parameter->avg_mode = (adc_avg_mode)avg_mode;
	adc_fsm_parameter->channel_map = channel_map;
	adc_fsm_parameter->period = period;

	return 0;
}

int mtk_mhal_adc_start(struct mtk_adc_controller *ctlr)
{
	int ret = 0;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	adc_debug("ctlr->adc_fsm_parameter->fifo_mode == %d.\n",
		ctlr->adc_fsm_parameter->fifo_mode);
	if ((ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DMA) &&
		(ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;
	if ((ctlr->adc_fsm_parameter->pmode != ADC_PMODE_ONE_TIME) &&
		(ctlr->adc_fsm_parameter->pmode != ADC_PMODE_PERIODIC))
		return -ADC_EPARAMETER;

	if (ctlr->adc_processing == true) {
		/*ADC is processing*/
		return -ADC_EPARAMETER;
	}

	ctlr->adc_processing = true;

	switch (ctlr->adc_fsm_parameter->fifo_mode) {
	case ADC_FIFO_DMA:
		osai_dma_clr_dreq(ctlr->dma_channel);
		ret = osai_dma_start(ctlr->dma_channel);
		if (ret < 0)
			return ret;

		break;
	case ADC_FIFO_DIRECT:
		if (ctlr->adc_fsm_parameter->pmode
			== ADC_PMODE_ONE_TIME) {
			ctlr->adc_fsm_parameter->ier_mode = ADC_FIFO_IER_RXFULL;
			if ((ctlr->adc_fsm_parameter->ier_mode ==
				ADC_FIFO_IER_RXFULL) ||
				(ctlr->adc_fsm_parameter->ier_mode ==
				ADC_FIFO_IER_BOTH)) {
				mtk_hdl_adc_fifo_rx_full_enable(
					ctlr->base);
			}
			/*enable interrupt*/
		}

		mtk_hdl_adc_trigger_level_set(ctlr, ctlr->channel_count);
		/*set trigger level equal to used channel number!!*/
		break;
	}

	mtk_hdl_adc_start(ctlr->base);

	return ret;
}

int  mtk_mhal_adc_start_ch(struct mtk_adc_controller *ctlr,
		u16 ch_bit_map)
{
	int ret = 0;
	u32 channel_num = 0;
	u32 channel_count = 0;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	if ((ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DMA) &&
		(ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	if ((ctlr->adc_fsm_parameter->pmode != ADC_PMODE_ONE_TIME) &&
		(ctlr->adc_fsm_parameter->pmode != ADC_PMODE_PERIODIC))
		return -ADC_EPARAMETER;

	if (ch_bit_map > 0xFF)
		return -ADC_EPARAMETER;

	if (ctlr->adc_processing == true) {
		/*ADC is processing*/
		return -ADC_EPARAMETER;
	}

	ctlr->adc_processing = true;

	ctlr->adc_fsm_parameter->channel_map = ch_bit_map;
	for (channel_count = 0; channel_count < ADC_CHANNEL_MAX;
	channel_count++) {
		if (ctlr->adc_fsm_parameter->channel_map &
		BIT(channel_count))
			channel_num++;
	}

	ctlr->channel_count = channel_num;

	switch (ctlr->adc_fsm_parameter->fifo_mode) {
	case ADC_FIFO_DMA:
		osai_dma_clr_dreq(ctlr->dma_channel);
		osai_dma_start(ctlr->dma_channel);

		break;
	case ADC_FIFO_DIRECT:
		if (ctlr->adc_fsm_parameter->pmode
			== ADC_PMODE_ONE_TIME) {
			ctlr->adc_fsm_parameter->ier_mode = ADC_FIFO_IER_RXFULL;
			if ((ctlr->adc_fsm_parameter->ier_mode ==
				ADC_FIFO_IER_RXFULL) ||
				(ctlr->adc_fsm_parameter->ier_mode ==
				ADC_FIFO_IER_BOTH)) {
				mtk_hdl_adc_fifo_rx_full_enable(
					ctlr->base);
			}
			/*enable interrupt*/
		}

		mtk_hdl_adc_trigger_level_set(ctlr->base, channel_num);
		/*set trigger level equal to used channel number!!*/
		break;
	}

	mtk_hdl_adc_start_ch(ctlr->base, ch_bit_map);

	return ret;
}

int mtk_mhal_adc_stop(struct mtk_adc_controller *ctlr)
{
	int ret = 0;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	if ((ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DMA) &&
		(ctlr->adc_fsm_parameter->fifo_mode != ADC_FIFO_DIRECT))
		return -ADC_EPARAMETER;

	mtk_hdl_adc_stop(ctlr->base);

	mtk_hdl_adc_reset(ctlr->base);

	return ret;
}
int mtk_mhal_adc_one_shot_get_data(struct mtk_adc_controller *ctlr,
		adc_channel channel, u32 *data)
{
	u32 write_point = 0;
	u32 read_point = 0;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!data)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	if (ctlr->adc_fsm_parameter->pmode != ADC_PMODE_ONE_TIME)
		return -ADC_EPARAMETER;

	if (channel >= ADC_CHANNEL_MAX)
		return -ADC_EPARAMETER;

	write_point = ctlr->current_xfer[channel].write_point;
	/*get prev data to guarantee latest*/

	if (!write_point)
		read_point = ADC_RING_BUF_SIZE - 1;
	else
		read_point = --write_point;
	adc_debug("data point:%p, write_point %d.\n", data, read_point);
	adc_debug("data point:%p, write_point %d.\n", data, write_point);

	*data = ctlr->current_xfer[channel].ring_buf[read_point];

	adc_debug("mtk mhal_adc_one_shot_get_data : channel->%d,data %d.\n",
	channel, ctlr->current_xfer[channel].ring_buf[read_point]);

	adc_debug("data point:%d.\n", *data);

	return 0;
}

int mtk_mhal_adc_period_get_data(struct mtk_adc_controller *ctlr,
		adc_channel channel, u32 *data, u32 *length)
{
	u32 write_point = 0;
	u32 read_point = 0;
	u32 count = 0;

	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	if ((ctlr->adc_fsm_parameter->pmode != ADC_PMODE_ONE_TIME) &&
		(ctlr->adc_fsm_parameter->pmode != ADC_PMODE_PERIODIC))
		return -ADC_EPARAMETER;

	if (channel >= ADC_CHANNEL_MAX)
		return -ADC_EPARAMETER;

	ctlr->rx_size = 0;

	write_point = ctlr->current_xfer[channel].write_point;

	if (ctlr->adc_fsm_parameter->pmode == ADC_PMODE_ONE_TIME) {
		if (!write_point)
			read_point = ADC_RING_BUF_SIZE - 1;
		else
			read_point = --write_point;
		*(ctlr->rx_buf) =
			ctlr->current_xfer[channel].ring_buf[read_point];
		ctlr->rx_size = 1;
		adc_debug("one shot mode ctlr->rx_buf:%d.\n", *(ctlr->rx_buf));

		return 0;
	}

	if (ctlr->adc_fsm_parameter->pmode == ADC_PMODE_PERIODIC) {
		for (count = ctlr->current_xfer[channel].count; count > 0;
			count--) {
			/*Next write pointer ,ADC sw buf full*/
			read_point = ctlr->current_xfer[channel].read_point;
			if (read_point >= ADC_RING_BUF_SIZE) {
				ctlr->current_xfer[channel].read_point = 0;
				read_point = 0;
			}

		 data[ctlr->rx_size] =
			ctlr->current_xfer[channel].ring_buf[read_point];

		adc_debug("channel- >%d,data:%d.\n",
		channel, data[ctlr->rx_size]);
		adc_debug("channel->%d,size:%d.\n",
		channel,
		ctlr->rx_size);
		ctlr->current_xfer[channel].count--;
		ctlr->current_xfer[channel].read_point++;
		ctlr->rx_size++;
		}
		*length = ctlr->rx_size;
	}

	return 0;
}

int mtk_mhal_adc_rx_notify_callback_register(struct mtk_adc_controller *ctlr,
						adc_rx_done_callback callback,
						void *callback_param)
{
	if (!ctlr || !callback || !callback_param)
		return -ADC_EPTR;

	ctlr->callback_param = callback_param;
	ctlr->rx_done_callback = callback;

	return 0;
}

int mtk_mhal_adc_fifo_handle_rx(struct mtk_adc_controller *ctlr)
{
	if (!ctlr)
		return -ADC_EPTR;

	if (!ctlr->base)
		return -ADC_EPTR;

	if (!ctlr->adc_fsm_parameter)
		return -ADC_EPTR;

	_mtk_mhal_adc_period_sample_data(ctlr);

	if (ctlr->adc_fsm_parameter->pmode == ADC_PMODE_ONE_TIME) {
		adc_debug("mtk_hdl_adc_stop one shot.\n");
		mtk_hdl_adc_stop(ctlr->base);
	}
	ctlr->adc_processing = false;

	return 0;
}
