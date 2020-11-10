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

#include "irq.h"
#include "nvic.h"
#include "os_hal_dma.h"

#define DMA_BASE 0x21080000
#define DMA_SYSRAM_ORIGIN 0x22000000
#define DMA_SYSRAM_SIZE 0x10000
#define DMA_SYSRAM_END (DMA_SYSRAM_ORIGIN + DMA_SYSRAM_SIZE)

#define DMA_CHANNEL_MAX (VDMA_ADC_RX_CH29 + 1)

/** @brief options for DMA status */
enum dma_sta {
	IDLE,    /**< DMA status as idle state*/
	RUNNING, /**< DMA status as running state*/
	STOP,    /**< DMA status as stop state*/
	PAUSE,   /**< DMA status as pause state */
};

struct dma_controller_rtos {
	enum dma_sta status;
	struct dma_interrupt interrupt_1;
	struct dma_interrupt interrupt_2;
	struct dma_controller *ctlr;
};

static struct dma_controller_rtos g_dma_ctlr_rtos[DMA_CHANNEL_MAX];
static struct dma_controller g_dma_ctlr[DMA_CHANNEL_MAX];
static struct dma_ctrl g_dma_ctrl_mode[DMA_CHANNEL_MAX];
static struct dma_config g_dma_config[DMA_CHANNEL_MAX];

static inline enum dma_type _mtk_os_hal_dma_get_chn_type(enum dma_channel chn)
{
	if (chn <= DMA_ISU4_RX_CH9)
		return DMA_TYPE_HALFSIZE;
	else if (chn == DMA_M2M_CH12)
		return DMA_TYPE_FULLSIZE;
	else if (chn >= VDMA_ISU0_TX_CH13 && chn <= VDMA_ISU4_RX_CH22)
		return DMA_TYPE_VFF;
	else if (chn >= VDMA_I2S0_TX_CH25 && chn <= VDMA_ADC_RX_CH29)
		return DMA_TYPE_VFF;
	else
		return (enum dma_type)-1;
}

static struct dma_controller_rtos *_mtk_os_hal_dma_get_ctlr(
	enum dma_channel chn)
{
	if (_mtk_os_hal_dma_get_chn_type(chn) == (enum dma_type)-1)
		return NULL;
	return &g_dma_ctlr_rtos[chn];
}

static void _mtk_os_hal_dma_irq_handler(void)
{
	struct dma_controller_rtos *ctrl_rtos;
	enum dma_channel chn;
	int status;

	NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_M4DMA);

	for (chn = DMA_ISU0_TX_CH0; chn <= VDMA_ADC_RX_CH29; chn++) {
		ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
		if (ctrl_rtos == NULL || ctrl_rtos->ctlr == NULL)
			continue;
		status = mtk_mhal_dma_get_status(ctrl_rtos->ctlr);
		if (status & DMA_STATUS_INTERRUPT)
			mtk_mhal_dma_clear_irq_status(ctrl_rtos->ctlr);
	}

	NVIC_EnableIRQ((IRQn_Type)CM4_IRQ_M4DMA);
}

static int _mtk_os_hal_dma_done_callback_1(void *data)
{
	struct dma_controller_rtos *ctlr_rtos = data;

	if (ctlr_rtos->interrupt_1.isr_cb != NULL)
		ctlr_rtos->interrupt_1.isr_cb(ctlr_rtos->interrupt_1.cb_data);

	return 0;
}

static int _mtk_os_hal_dma_done_callback_2(void *data)
{
	struct dma_controller_rtos *ctlr_rtos = data;

	if (ctlr_rtos->interrupt_2.isr_cb != NULL)
		ctlr_rtos->interrupt_2.isr_cb(ctlr_rtos->interrupt_2.cb_data);

	return 0;
}

static void _mtk_os_hal_dma_reset_controller_rtos(
	struct dma_controller_rtos *ctrl_rtos)
{
	if (ctrl_rtos->ctlr != NULL) {
		if (ctrl_rtos->ctlr->cfg != NULL) {
			memset(ctrl_rtos->ctlr->cfg, 0x0,
				sizeof(*(ctrl_rtos->ctlr->cfg)));
			ctrl_rtos->ctlr->cfg = NULL;
		}

		if (ctrl_rtos->ctlr->ctrls != NULL) {
			memset(ctrl_rtos->ctlr->ctrls, 0x0,
				sizeof(*(ctrl_rtos->ctlr->ctrls)));
			ctrl_rtos->ctlr->ctrls = NULL;
		}
		memset(ctrl_rtos->ctlr, 0x0, sizeof(*(ctrl_rtos->ctlr)));
		ctrl_rtos->ctlr = NULL;
	}

	ctrl_rtos->interrupt_1.isr_cb = NULL;
	ctrl_rtos->interrupt_1.cb_data = NULL;
	ctrl_rtos->interrupt_2.isr_cb = NULL;
	ctrl_rtos->interrupt_2.cb_data = NULL;
	ctrl_rtos->status = IDLE;
}

int mtk_os_hal_dma_alloc_chan(enum dma_channel chn)
{
	struct dma_controller_rtos *ctrl_rtos;
	int ret = 0;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;
	if (ctrl_rtos->status == IDLE) {
		/* clear dma_controller_rtos setting */
		if (ctrl_rtos->ctlr != NULL)
			_mtk_os_hal_dma_reset_controller_rtos(ctrl_rtos);

		ctrl_rtos->ctlr = &g_dma_ctlr[chn];
		ctrl_rtos->ctlr->ctrls = &g_dma_ctrl_mode[chn];
		ctrl_rtos->ctlr->cfg = &g_dma_config[chn];

		ctrl_rtos->ctlr->base = (void __iomem *)DMA_BASE;
		ctrl_rtos->ctlr->chn = chn;
		ctrl_rtos->ctlr->chn_type =
			_mtk_os_hal_dma_get_chn_type(chn);
		if (ctrl_rtos->ctlr->chn_type == -1) {
			printf("invalid dma channel number: %d\n", chn);
			return -DMA_EPARAM;
		}

		mtk_mhal_dma_clock_enable(ctrl_rtos->ctlr);

		CM4_Install_NVIC(CM4_IRQ_M4DMA, CM4_DMA_PRI, IRQ_LEVEL_TRIGGER,
				 _mtk_os_hal_dma_irq_handler, TRUE);

		mtk_mhal_dma_reset(ctrl_rtos->ctlr);

		ctrl_rtos->status = RUNNING;
	} else {
		printf("chn %d is being used\n", chn);
		if (ctrl_rtos->ctlr != NULL) {
			printf("chn(%d) status(%d)\n", ctrl_rtos->ctlr->chn,
				   ctrl_rtos->status);
		}
		ret = -DMA_EBUSY;
	}
	return ret;
}

static void _mtk_os_hal_dma_set_control_mode(struct dma_controller *ctrl,
				      struct dma_control_mode *ctrl_mode)
{
	ctrl->ctrls->burst_type = (enum dma_burst_type)ctrl_mode->burst_type;
	ctrl->ctrls->bw_transfer_en = ctrl_mode->bw_transfer_en;
	ctrl->ctrls->transize = (enum dma_transize)ctrl_mode->transize;

	ctrl->ctrls->wrap_en = ctrl_mode->wrap_settings.wrap_en;
	ctrl->ctrls->wrap_side = ctrl_mode->wrap_settings.wrap_side;
	ctrl->cfg->bw_limiter = ctrl_mode->bw_limiter;
	ctrl->cfg->wrap_point = ctrl_mode->wrap_settings.wrap_point;
	ctrl->cfg->wrap_to_addr = ctrl_mode->wrap_settings.wrap_to_addr;
}

static int _mtk_os_hal_dma_addr_check(u32 addr)
{
	if ((addr >= DMA_SYSRAM_ORIGIN) && (addr < DMA_SYSRAM_END))
		return 0;

	return -DMA_EPTR;
}

int mtk_os_hal_dma_config(enum dma_channel chn, struct dma_setting *setting)
{
	struct dma_controller_rtos *ctrl_rtos;
	struct dma_controller *ctrl;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	ctrl = ctrl_rtos->ctlr;
	if (ctrl == NULL) {
		printf("dma channel has not been allocated!\n");
		return -DMA_EPTR;
	}

	_mtk_os_hal_dma_set_control_mode(ctrl, &(setting->ctrl_mode));

	if (ctrl->chn_type == DMA_TYPE_FULLSIZE) {
		if (_mtk_os_hal_dma_addr_check(setting->src_addr)) {
			printf(
				"DMA support SYSRAM memory only, src(0x%x) out of range, valid address range [0x%x-0x%x]\n",
				setting->src_addr, DMA_SYSRAM_ORIGIN,
				DMA_SYSRAM_END);
			return -DMA_EPTR;
		}
		ctrl->cfg->addr_1 = setting->src_addr;
		if (_mtk_os_hal_dma_addr_check(setting->dst_addr)) {
			printf(
				"DMA support SYSRAM memory only, dst(0x%x) out of range,valid address range [0x%x-0x%x]\n",
				setting->dst_addr, DMA_SYSRAM_ORIGIN,
				DMA_SYSRAM_END);
			return -DMA_EPTR;
		}
		ctrl->cfg->addr_2 = setting->dst_addr;
		ctrl->cfg->count = setting->count;
		ctrl->ctrls->src_inc_en = 1;
		ctrl->ctrls->dst_inc_en = 1;
		/* set interrupt flag for FULL-SIZE DMA channel */
		ctrl->ctrls->int_en = !!(setting->interrupt_flag
			& DMA_INT_COMPLETION);
	} else {
		ctrl->ctrls->dir = (enum dma_dir)setting->dir;
		if (ctrl->ctrls->dir == MEM_2_PERI) {
			if (_mtk_os_hal_dma_addr_check(setting->src_addr)) {
				printf(
					"DMA support SYSRAM memory only,src(0x%x)out of range, valid address range[0x%x-0x%x]\n",
					setting->src_addr, DMA_SYSRAM_ORIGIN,
					DMA_SYSRAM_END);
				return -DMA_EPTR;
			}
			ctrl->cfg->addr_1 = setting->src_addr;
			ctrl->cfg->addr_2 = setting->dst_addr;
			ctrl->ctrls->src_inc_en = 1;
			ctrl->ctrls->dst_inc_en = 0;
		} else {
			if (_mtk_os_hal_dma_addr_check(setting->dst_addr)) {
				printf(
					"DMA support SYSRAM memory only,dst(0x%x)out of range, valid address range[0x%x-0x%x]\n",
					setting->dst_addr, DMA_SYSRAM_ORIGIN,
					DMA_SYSRAM_END);
				return -DMA_EPTR;
			}
			ctrl->cfg->addr_1 = setting->dst_addr;
			ctrl->cfg->addr_2 = setting->src_addr;
			ctrl->ctrls->src_inc_en = 0;
			ctrl->ctrls->dst_inc_en = 1;
		}
		ctrl->cfg->count = setting->count;
		ctrl->cfg->reload_en = setting->reload_en;
		if (ctrl->chn_type == DMA_TYPE_VFF) {
			ctrl->cfg->count = setting->vfifo.fifo_thrsh;
			ctrl->cfg->timeout_cnt = setting->vfifo.timeout_cnt;
			ctrl->cfg->alert_cmp_type =
			    setting->vfifo.alert_cmp_type;
			ctrl->cfg->alert_len = setting->vfifo.alert_len;
			ctrl->cfg->fifo_size = setting->vfifo.fifo_size;
			/* set interrupt flag for VFF DMA channel */
			ctrl->ctrls->int_en = !!(setting->interrupt_flag
				& DMA_INT_VFIFO_THRESHOLD);
			ctrl->ctrls->timeout_int_en = !!(setting->interrupt_flag
				& DMA_INT_VFIFO_TIMEOUT);
		} else {
			/* set interrupt flag for HALF-SIZE DMA channel */
			ctrl->ctrls->int_en = !!(setting->interrupt_flag
				& DMA_INT_COMPLETION);
			ctrl->ctrls->half_int_en = !!(setting->interrupt_flag
				& DMA_INT_HALF_COMPLETION);
		}
	}

	ctrl->cfg->isr_callback_1 =
	    (dma_isr_callback)_mtk_os_hal_dma_done_callback_1;
	ctrl->cfg->isr_cb_data_1 = ctrl_rtos;
	ctrl->cfg->isr_callback_2 =
	    (dma_isr_callback)_mtk_os_hal_dma_done_callback_2;
	ctrl->cfg->isr_cb_data_2 = ctrl_rtos;

	return mtk_mhal_dma_config(ctrl);
}

int mtk_os_hal_dma_start(enum dma_channel chn)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_start(ctrl_rtos->ctlr);
}

int mtk_os_hal_dma_stop(enum dma_channel chn)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;
	ctrl_rtos->status = IDLE;

	return mtk_mhal_dma_stop(ctrl_rtos->ctlr);
}

int mtk_os_hal_dma_pause(enum dma_channel chn)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_pause(ctrl_rtos->ctlr);
}

int mtk_os_hal_dma_resume(enum dma_channel chn)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_resume(ctrl_rtos->ctlr);
}

int mtk_os_hal_dma_get_status(enum dma_channel chn)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_get_status(ctrl_rtos->ctlr);
}

int mtk_os_hal_dma_register_isr(enum dma_channel chn,
	dma_interrupt_callback isr_cb, void *cb_data,
	enum dma_interrupt_type isr_type)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	switch (isr_type) {
	case DMA_INT_COMPLETION:
		ctrl_rtos->interrupt_1.isr_cb = isr_cb;
		ctrl_rtos->interrupt_1.cb_data = cb_data;
		break;
	case DMA_INT_HALF_COMPLETION:
		ctrl_rtos->interrupt_2.isr_cb = isr_cb;
		ctrl_rtos->interrupt_2.cb_data = cb_data;
		break;
	case DMA_INT_VFIFO_THRESHOLD:
		ctrl_rtos->interrupt_1.isr_cb = isr_cb;
		ctrl_rtos->interrupt_1.cb_data = cb_data;
		break;
	case DMA_INT_VFIFO_TIMEOUT:
		ctrl_rtos->interrupt_2.isr_cb = isr_cb;
		ctrl_rtos->interrupt_2.cb_data = cb_data;
		break;
	default:
		printf("unknown dma isr_type %d\n", isr_type);
		return -DMA_EPARAM;
	}

	return 0;
}

int mtk_os_hal_dma_dump_register(enum dma_channel chn)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_dump_reg(ctrl_rtos->ctlr);
}

int mtk_os_hal_dma_set_param(enum dma_channel chn,
	enum dma_param_type param_type, u32 value)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_set_param(ctrl_rtos->ctlr,
				      (enum dma_param)param_type, value);
}

int mtk_os_hal_dma_get_param(enum dma_channel chn,
	enum dma_param_type param_type)
{
	struct dma_controller_rtos *ctrl_rtos = NULL;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_get_param(ctrl_rtos->ctlr,
				      (enum dma_param)param_type);
}

int mtk_os_hal_dma_update_swptr(enum dma_channel chn, u32 length_byte)
{
	struct dma_controller_rtos *ctrl_rtos = NULL;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_update_swptr(ctrl_rtos->ctlr, length_byte);
}

int mtk_os_hal_dma_vff_read_data(enum dma_channel chn, u8 *buffer, u32 length)
{
	struct dma_controller_rtos *ctrl_rtos = NULL;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_vff_read_data(ctrl_rtos->ctlr, buffer, length);
}

int mtk_os_hal_dma_release_chan(enum dma_channel chn)
{
	int ret;
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	ret = mtk_mhal_dma_stop(ctrl_rtos->ctlr);
	if (ret) {
		printf("stop dma fail!\n");
		return ret;
	}

	mtk_mhal_dma_clock_disable(ctrl_rtos->ctlr);

	_mtk_os_hal_dma_reset_controller_rtos(ctrl_rtos);

	return ret;
}

int mtk_os_hal_dma_reset(enum dma_channel chn)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;
	ctrl_rtos->status = IDLE;

	return mtk_mhal_dma_reset(ctrl_rtos->ctlr);
}

int mtk_os_hal_dma_clr_dreq(enum dma_channel chn)
{
	struct dma_controller_rtos *ctrl_rtos;

	ctrl_rtos = _mtk_os_hal_dma_get_ctlr(chn);
	if (ctrl_rtos == NULL)
		return -DMA_EPTR;

	return mtk_mhal_dma_clr_dreq(ctrl_rtos->ctlr);
}
