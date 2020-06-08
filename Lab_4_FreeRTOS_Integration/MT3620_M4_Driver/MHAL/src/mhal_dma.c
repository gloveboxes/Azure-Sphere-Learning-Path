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

#include "mhal_osai.h"
#include "mhal_dma.h"
#include "hdl_dma.h"

static int _mtk_mhal_dma_config_fullsize_dma(struct dma_controller *controller)
{
	u32 control_settings = 0;
	void __iomem *chn_base = DMA_GET_CHN_BASE(controller->base,
						  controller->chn);
	struct dma_ctrl *ctrls = controller->ctrls;
	struct dma_config *cfg = controller->cfg;

	if (ctrls == NULL || cfg == NULL) {
		dma_err(
		    "dma control(%p) or config parameters(%p) is NULL!\n",
		    ctrls, cfg);
		return -DMA_EPARAM;
	}

	/* dma channel work mode control settings */
	if (ctrls->wrap_en == true) {
		control_settings |= DMA_CON_WPEN;
		if (ctrls->wrap_side == true) {
			/* wrap_side = 1, address-wrapping
			 * on destination side
			 */
			control_settings |= DMA_CON_WPSD;
		} else {
			/* wrap_side = 0, address-wrapping
			 * on source side
			 */
			control_settings &= ~DMA_CON_WPSD;
		}
	}

	if (ctrls->int_en == true)
		control_settings |= DMA_CON_ITEN;

	control_settings &= ~DMA_CON_DRQ;

	if (ctrls->dst_inc_en == true)
		control_settings |= DMA_CON_DINC;

	if (ctrls->src_inc_en == true)
		control_settings |= DMA_CON_SINC;

	switch (ctrls->transize) {
	case DMA_SIZE_BYTE:
		control_settings |= DMA_CON_SIZE_BYTE;
		break;
	case DMA_SIZE_SHORT:
		control_settings |= DMA_CON_SIZE_SHORT;
		if (ctrls->burst_type == DMA_BURST_TYPE_16BEAT) {
			ctrls->burst_type = DMA_BURST_TYPE_8BEAT;
			dma_err("BUSRT_16BEAT cannot be with SIZE_SHORT, ");
			dma_err("change to 8BEAT\n");
		}
		break;
	case DMA_SIZE_LONG:
		control_settings |= DMA_CON_SIZE_LONG;
		if (ctrls->burst_type == DMA_BURST_TYPE_8BEAT ||
		    ctrls->burst_type == DMA_BURST_TYPE_16BEAT) {
			ctrls->burst_type = DMA_BURST_TYPE_4BEAT;
			dma_err("BUSRT_8BEAT and BUSRT_16BEAT cannot be ");
			dma_err("used with SIZE_LONG, change to 4BEAT!\n");
		}
		break;
	default:
		dma_err("default size is 0(DMA_CON_SIZE_BYTE)\n");
		return -DMA_EPARAM;
	}

	switch (ctrls->burst_type) {
	case DMA_BURST_TYPE_SINGLE:
		control_settings |= DMA_CON_BURST_SINGLE;
		break;
	case DMA_BURST_TYPE_4BEAT:
		control_settings |= DMA_CON_BURST_4BEAT;
		break;
	case DMA_BURST_TYPE_8BEAT:
		control_settings |= DMA_CON_BURST_8BEAT;
		break;
	case DMA_BURST_TYPE_16BEAT:
		control_settings |= DMA_CON_BURST_16BEAT;
		break;
	default:
		dma_err("unused burst-type: %d\n", ctrls->burst_type);
		return -DMA_EPARAM;
	}

	/* Before starting dma configuration, should stop dma firstly */
	mtk_hdl_dma_stop_now(chn_base);

	mtk_hdl_dma_set_con(chn_base, control_settings);
	dma_debug("chn_base %p, control_setting 0x%x\n", chn_base,
		control_settings);

	/* dma channel work parameter settings */
	if (cfg->addr_1 == 0 || cfg->addr_2 == 0) {
		dma_err("warning src_addr 0x%x dst_addr 0x%x!\n",
			cfg->addr_1, cfg->addr_2);
	}
	mtk_hdl_dma_set_src(chn_base, cfg->addr_1);
	mtk_hdl_dma_set_dst(chn_base, cfg->addr_2);

	if (mtk_hdl_dma_set_count(chn_base, cfg->count, ctrls->transize) < 0) {
		dma_err("invalid transfer byte count %d!\n", cfg->count);
		return -DMA_EPARAM;
	}

	if (ctrls->wrap_en == true) {
		if (cfg->wrap_point > cfg->count) {
			dma_err("invalid wrap point: 0x%x\n", cfg->wrap_point);
			return -DMA_EPARAM;
		}
		mtk_hdl_dma_set_wppt(chn_base, cfg->wrap_point,
			ctrls->transize);
		if (cfg->wrap_to_addr == 0) {
			dma_err("warning wrap to addr: 0x%x\n",
				cfg->wrap_to_addr);
		}
		mtk_hdl_dma_set_wpto(chn_base, cfg->wrap_to_addr);
	}

	mtk_hdl_dma_set_bw_limiter(chn_base, cfg->bw_limiter);

	return 0;
}

static int _mtk_mhal_dma_config_halfsize_dma(struct dma_controller *controller)
{
	u32 control_settings = 0;
	void __iomem *chn_base = DMA_GET_CHN_BASE(controller->base,
						  controller->chn);
	struct dma_ctrl *ctrls = controller->ctrls;
	struct dma_config *cfg = controller->cfg;

	if (ctrls == NULL || cfg == NULL) {
		dma_err("dma control(%p) or config parameters(%p) is NULL!\n",
		    ctrls, cfg);
		return -DMA_EPARAM;
	}

	/* dma channel work mode control settings */
	if (ctrls->dir == PERI_2_MEM) {
		/* dir = 1, read from peripheral and write to memory */
		control_settings |= DMA_CON_DIR;
	} else if (ctrls->dir == MEM_2_PERI) {
		/* dir = 0, read from memory and write to peripheral */
		control_settings &= ~DMA_CON_DIR;
	}
	if (ctrls->wrap_en == true) {
		control_settings |= DMA_CON_WPEN;
		if (ctrls->wrap_side == true) {
			/* wrap_side = 1, address-wrapping
			 * on destination side
			 */
			control_settings |= DMA_CON_WPSD;
		} else {
			/* wrap_side = 0, address-wrapping
			 * on source side
			 */
			control_settings &= ~DMA_CON_WPSD;
		}
	}
	if (ctrls->int_en == true)
		control_settings |= DMA_CON_ITEN;

	if (ctrls->half_int_en == true)
		control_settings |= DMA_CON_HITEN;

	if (ctrls->bw_transfer_en == true)
		control_settings |= DMA_CON_B2W;

	control_settings |= DMA_CON_DRQ;

	if (ctrls->dst_inc_en == true)
		control_settings |= DMA_CON_DINC;
	if (ctrls->src_inc_en == true)
		control_settings |= DMA_CON_SINC;

	switch (ctrls->transize) {
	case DMA_SIZE_BYTE:
		control_settings |= DMA_CON_SIZE_BYTE;
		break;
	case DMA_SIZE_SHORT:
		control_settings |= DMA_CON_SIZE_SHORT;
		if (ctrls->burst_type == DMA_BURST_TYPE_16BEAT) {
			ctrls->burst_type = DMA_BURST_TYPE_8BEAT;
			dma_err("BUSRT_16BEAT cannot be with SIZE_SHORT, ");
			dma_err("change to 8BEAT\n");
		}
		break;
	case DMA_SIZE_LONG:
		control_settings |= DMA_CON_SIZE_LONG;
		if (ctrls->burst_type == DMA_BURST_TYPE_8BEAT ||
		    ctrls->burst_type == DMA_BURST_TYPE_16BEAT) {
			ctrls->burst_type = DMA_BURST_TYPE_4BEAT;
			dma_err("BUSRT_8BEAT and BUSRT_16BEAT cannot be ");
			dma_err("used with SIZE_LONG, change to 4BEAT!\n");
		}
		break;
	default:
		dma_err("default size is 0(DMA_CON_SIZE_BYTE)\n");
		return -DMA_EPARAM;
	}

	switch (ctrls->burst_type) {
	case DMA_BURST_TYPE_SINGLE:
		control_settings |= DMA_CON_BURST_SINGLE;
		break;
	case DMA_BURST_TYPE_4BEAT:
		control_settings |= DMA_CON_BURST_4BEAT;
		break;
	case DMA_BURST_TYPE_8BEAT:
		control_settings |= DMA_CON_BURST_8BEAT;
		break;
	case DMA_BURST_TYPE_16BEAT:
		control_settings |= DMA_CON_BURST_16BEAT;
		break;
	default:
		dma_err("unused burst-type: %d\n", ctrls->burst_type);
		return -DMA_EPARAM;
	}

	/* Before starting dma configuration, should stop dma firstly */
	mtk_hdl_dma_stop_now(chn_base);

	mtk_hdl_dma_set_con(chn_base, control_settings);
	dma_debug("chn_base %p, control_setting 0x%x\n", chn_base,
		control_settings);

	/* dma channel work parameter settings */
	if (cfg->addr_1 == 0 || cfg->addr_2 == 0) {
		dma_err("warning prog_addr 0x%x fix_addr 0x%x!\n",
			cfg->addr_1, cfg->addr_2);
	}

	mtk_hdl_dma_set_pgmaddr(chn_base, cfg->addr_1);

	if ((cfg->addr_2 & 0x3) != 0) {
		dma_err("fix addr 0x%x isn't 4byte aligned\n", cfg->addr_2);
		return -DMA_EPARAM;
	}
	mtk_hdl_dma_set_fixaddr(chn_base, cfg->addr_2);

	if (mtk_hdl_dma_set_count(chn_base, cfg->count, ctrls->transize) < 0) {
		dma_err("invalid transfer byte count %d!\n", cfg->count);
		return -DMA_EPARAM;
	}

	if (ctrls->wrap_en == true) {
		if (cfg->wrap_point > cfg->count) {
			dma_err("invalid wrap point: 0x%x\n", cfg->wrap_point);
			return -DMA_EPARAM;
		}
		mtk_hdl_dma_set_wppt(chn_base, cfg->wrap_point,
			ctrls->transize);
		if (cfg->wrap_to_addr == 0) {
			dma_err("warning wrap to addr: 0x%x\n",
				cfg->wrap_to_addr);
		}
		mtk_hdl_dma_set_wpto(chn_base, cfg->wrap_to_addr);
	}

	mtk_hdl_dma_set_bw_limiter(chn_base, cfg->bw_limiter);

	if (cfg->reload_en == true)
		mtk_hdl_dma_enable_reload(chn_base);
	else
		mtk_hdl_dma_disable_reload(chn_base);

	return 0;
}

static int _mtk_mhal_dma_config_vfifo_dma(struct dma_controller *controller)
{
	u32 control_settings = 0;
	void __iomem *chn_base = DMA_GET_CHN_BASE(controller->base,
						  controller->chn);
	struct dma_ctrl *ctrls = controller->ctrls;
	struct dma_config *cfg = controller->cfg;

	if (ctrls == NULL || cfg == NULL) {
		dma_err("dma control(%p) or config parameters(%p) is NULL!\n",
		    ctrls, cfg);
		return -DMA_EPARAM;
	}

	/* dma channel work mode control settings */
	if (ctrls->dir == PERI_2_MEM) {
		/* dir = 1, read from peripheral and write to memory */
		control_settings |= DMA_CON_DIR;
	} else if (ctrls->dir == MEM_2_PERI) {
		/* dir = 0, read from memory and write to peripheral */
		control_settings &= ~DMA_CON_DIR;
	}

	if (ctrls->int_en == true)
		control_settings |= DMA_CON_ITEN;
	if (ctrls->timeout_int_en == true)
		control_settings |= DMA_CON_TOEN;

	control_settings |= DMA_CON_DRQ;

	switch (ctrls->transize) {
	case DMA_SIZE_BYTE:
		control_settings |= DMA_CON_SIZE_BYTE;
		break;
	case DMA_SIZE_SHORT:
		control_settings |= DMA_CON_SIZE_SHORT;
		break;
	case DMA_SIZE_LONG:
		control_settings |= DMA_CON_SIZE_LONG;
		break;
	default:
		dma_err("default size is 0(DMA_CON_SIZE_BYTE)\n");
		return -DMA_EPARAM;
	}

	/* Before starting dma configuration, should stop dma firstly */
	mtk_hdl_dma_stop_now(chn_base);

	mtk_hdl_dma_set_con(chn_base, control_settings);
	dma_debug("chn_base %p, control_setting 0x%x\n", chn_base,
		control_settings);

	/* dma channel work parameter settings */
	if (cfg->addr_1 == 0 || cfg->addr_2 == 0) {
		dma_err("warning prog_addr 0x%x fix_addr 0x%x!\n",
			cfg->addr_1, cfg->addr_2);
	}

	mtk_hdl_dma_set_pgmaddr(chn_base, cfg->addr_1);

	if ((cfg->addr_2 & 0x3) != 0) {
		dma_err("fix addr 0x%x isn't 4byte aligned\n", cfg->addr_2);
		return -DMA_EPARAM;
	}
	mtk_hdl_dma_set_fixaddr(chn_base, cfg->addr_2);

	dma_debug("cfg->count %d\n", cfg->count);
	if (mtk_hdl_dma_set_count(chn_base, cfg->count, ctrls->transize) < 0) {
		dma_err("invalid fifo threshold %d.\n", cfg->count);
		return -DMA_EPARAM;
	}

	mtk_hdl_dma_set_bw_limiter(chn_base, cfg->bw_limiter);

	/* config fifo engine parameters */
	if (mtk_hdl_dma_set_ffsize(chn_base, cfg->fifo_size,
		ctrls->transize) < 0) {
		dma_err("invalid fifo length %d\n", cfg->fifo_size);
	}

	/* set alert length of VFF DMA channel */
	if (mtk_hdl_dma_set_altlen(chn_base, cfg->alert_cmp_type,
		cfg->alert_len) < 0) {
		dma_err("invalid vfifo alert_len: %d\n", cfg->alert_len);
		return -DMA_EPARAM;
	}

	mtk_hdl_dma_set_timeout_thr(chn_base, cfg->timeout_cnt);

	mtk_hdl_dma_set_swptr(chn_base, 0);

	return 0;
}

int mtk_mhal_dma_config(struct dma_controller *controller)
{

	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	if (mtk_hdl_dma_chk_run_status(controller->base,
				       controller->chn)) {
		return -DMA_EBUSY;
	}

	switch (controller->chn_type) {
	case DMA_TYPE_FULLSIZE:
		return _mtk_mhal_dma_config_fullsize_dma(controller);
	case DMA_TYPE_HALFSIZE:
		return _mtk_mhal_dma_config_halfsize_dma(controller);
	case DMA_TYPE_VFF:
		return _mtk_mhal_dma_config_vfifo_dma(controller);
	default:
		dma_err("config invalid dma channel type!\n");
		return -DMA_EPARAM;
	}
}

int mtk_mhal_dma_start(struct dma_controller *controller)
{
	void __iomem *chn_base;

	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	if (mtk_hdl_dma_chk_run_status(controller->base,
				       controller->chn)) {
		return -DMA_EBUSY;
	}

	chn_base = DMA_GET_CHN_BASE(controller->base, controller->chn);
	mtk_hdl_dma_start(chn_base);

	return 0;
}

int mtk_mhal_dma_stop(struct dma_controller *controller)
{
	void __iomem *chn_base;

	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	chn_base = DMA_GET_CHN_BASE(controller->base, controller->chn);
	mtk_hdl_dma_stop_now(chn_base);
	return 0;
}

int mtk_mhal_dma_pause(struct dma_controller *controller)
{
	if (controller == NULL) {
		osai_print("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	mtk_hdl_dma_pause(controller->base, controller->chn);
	return 0;
}

int mtk_mhal_dma_resume(struct dma_controller *controller)
{
	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	if (mtk_hdl_dma_chk_pause_status(controller->base,
					 controller->chn)) {
		mtk_hdl_dma_resume(controller->base, controller->chn);
	}
	return 0;
}

int mtk_mhal_dma_get_status(struct dma_controller *controller)
{
	u32 status = 0;

	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	if (mtk_hdl_dma_chk_int_status(controller->base,
				       controller->chn)) {
		status |= DMA_STATUS_INTERRUPT;
	}
	if (mtk_hdl_dma_chk_run_status(controller->base,
				       controller->chn)) {
		status |= DMA_STATUS_RUNNING;
	}
	if (mtk_hdl_dma_chk_pause_status(controller->base,
					 controller->chn)) {
		status |= DMA_STATUS_PAUSE;
	}

	dma_debug("dma chn %d, base %p status %d\n",
		controller->chn, controller->base, status);

	return status;
}

int mtk_mhal_dma_set_param(struct dma_controller *controller,
			   enum dma_param param, u32 value)
{
	void __iomem *chn_base;

	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	chn_base = DMA_GET_CHN_BASE(controller->base, controller->chn);
	switch (param) {
	case DMA_PARAM_FIX_ADDR:
		mtk_hdl_dma_set_fixaddr(chn_base, value);
		break;
	case DMX_PARAM_PROG_ADDR:
		mtk_hdl_dma_set_pgmaddr(chn_base, value);
		break;
	case DMA_PARAM_VFF_FIFO_SIZE:
		return mtk_hdl_dma_set_ffsize(chn_base, value,
			controller->ctrls->transize);
	case DMA_PARAM_VFF_SWPTR:
		mtk_hdl_dma_set_swptr(chn_base, value);
		break;
	default:
		dma_err("cannot set param %d\n", (u32)param);
		return -DMA_EPARAM;
	}

	return 0;
}

int mtk_mhal_dma_get_param(struct dma_controller *controller,
			   enum dma_param param)
{
	void __iomem *chn_base;

	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	chn_base = DMA_GET_CHN_BASE(controller->base, controller->chn);
	switch (param) {
	case DMA_PARAM_RLCT:
		return mtk_hdl_dma_get_remain_cnt(chn_base,
			controller->ctrls->transize);
	case DMA_PARAM_FIX_ADDR:
		return mtk_hdl_dma_get_fixaddr(chn_base);
	case DMX_PARAM_PROG_ADDR:
		return mtk_hdl_dma_get_pgmaddr(chn_base);
	case DMA_PARAM_VFF_FIFO_SIZE:
		return mtk_hdl_dma_get_vfifo_ffsize(chn_base,
			controller->ctrls->transize);
	case DMA_PARAM_VFF_FIFO_CNT:
		return mtk_hdl_dma_get_vfifo_ffcnt(chn_base,
			controller->ctrls->transize);
	case DMA_PARAM_VFF_HWPTR:
		return mtk_hdl_dma_get_hwptr(chn_base);
	case DMA_PARAM_VFF_SWPTR:
		return mtk_hdl_dma_get_swptr(chn_base);
	default:
		dma_err("cannot get param %d\n", (u32)param);
		return -DMA_EPARAM;
	}
}

static int _mtk_mhal_dma_clear_fullsize_irq_status(
	struct dma_controller *controller)
{
	void __iomem *chn_base = DMA_GET_CHN_BASE(controller->base,
						  controller->chn);
	int ret = -DMA_EPARAM;

	if (mtk_hdl_dma_chk_int(chn_base)) {
		if (controller->cfg->isr_callback_1 != NULL) {
			controller->cfg->isr_callback_1(
			    controller->cfg->isr_cb_data_1);
		}
		mtk_hdl_dma_ack_int(chn_base);
		ret = 0;
	}

	return ret;
}

static int _mtk_mhal_dma_clear_halfsize_irq_status(
	struct dma_controller *controller)
{
	void __iomem *chn_base = DMA_GET_CHN_BASE(controller->base,
						  controller->chn);
	int ret = -DMA_EPARAM;

	if (mtk_hdl_dma_chk_int(chn_base)) {
		if (controller->cfg->isr_callback_1 != NULL) {
			controller->cfg->isr_callback_1(
			    controller->cfg->isr_cb_data_1);
		}
		mtk_hdl_dma_ack_int(chn_base);
		ret = 0;
	}
	if (mtk_hdl_dma_chk_half_int(chn_base)) {
		if (controller->cfg->isr_callback_2 != NULL) {
			controller->cfg->isr_callback_2(
			    controller->cfg->isr_cb_data_2);
		}
		mtk_hdl_dma_ack_half_int(chn_base);
		ret = 0;
	}

	return ret;
}

static int _mtk_mhal_dma_clear_vfifo_irq_status(
	struct dma_controller *controller)
{
	void __iomem *chn_base = DMA_GET_CHN_BASE(controller->base,
						  controller->chn);
	int ret = -DMA_EPARAM;

	if (mtk_hdl_dma_chk_int(chn_base)) {
		if (controller->cfg->isr_callback_1 != NULL) {
			controller->cfg->isr_callback_1(
			    controller->cfg->isr_cb_data_1);
		}
		mtk_hdl_dma_ack_int(chn_base);
		ret = 0;
	}
	if (mtk_hdl_dma_chk_timeout_int(chn_base)) {
		if (controller->cfg->isr_callback_2 != NULL) {
			controller->cfg->isr_callback_2(
			    controller->cfg->isr_cb_data_2);
		}
		mtk_hdl_dma_ack_timeout_int(chn_base);
		ret = 0;
	}
	return ret;
}

int mtk_mhal_dma_clear_irq_status(struct dma_controller *controller)
{
	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	dma_debug("dma chn %d, base %p clear irq status\n",
		controller->chn, controller->base);

	switch (controller->chn_type) {
	case DMA_TYPE_FULLSIZE:
		return _mtk_mhal_dma_clear_fullsize_irq_status(controller);
	case DMA_TYPE_HALFSIZE:
		return _mtk_mhal_dma_clear_halfsize_irq_status(controller);
	case DMA_TYPE_VFF:
		return _mtk_mhal_dma_clear_vfifo_irq_status(controller);
	default:
		dma_err("invalid dma channel type clear irq status!\n");
		return -DMA_EPARAM;
	}
}

int mtk_mhal_dma_dump_reg(struct dma_controller *controller)
{
	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	mtk_hdl_dma_dump_chn_reg(
	    DMA_GET_CHN_BASE(controller->base, controller->chn));
	mtk_hdl_dma_dump_global_reg(controller->base);

	return 0;
}

int mtk_mhal_dma_clock_enable(struct dma_controller *controller)
{
	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	mtk_hdl_dma_clock_enable(controller->base, controller->chn);

	return 0;
}

int mtk_mhal_dma_clock_disable(struct dma_controller *controller)
{
	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	mtk_hdl_dma_clock_disable(controller->base, controller->chn);

	return 0;
}

int mtk_mhal_dma_reset(struct dma_controller *controller)
{
	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	mtk_hdl_dma_reset_chn(controller->base, controller->chn);

	return 0;
}

/* calculate new value of swptr according to current swptr value, fifo size
 * and updated byte length.
 */
static u32 _mtk_mhal_dma_calculate_swptr(u32 swptr, u32 ffsize,
					   u32 length_byte)
{
	u32 wrap;

	wrap = (swptr & 0x00010000);
	swptr &= 0x0000FFFF;

	/* Handle ring buffer case for swptr */
	if ((swptr + length_byte) >= ffsize) {
		swptr = (swptr + length_byte - ffsize);
		swptr &= 0x0000FFFF;
		/* If spwtr should ring, wrap bit should be oppsite. */
		wrap = 0x00010000 - wrap;
	} else {
		swptr += length_byte;
	}
	swptr |= wrap;

	return swptr;
}

int mtk_mhal_dma_update_swptr(struct dma_controller *controller,
					u32 length_byte)
{
	int ret = 0;
	u32 swptr_cr, swptr_new, ffsize;

	ffsize = mtk_mhal_dma_get_param(controller, DMA_PARAM_VFF_FIFO_SIZE);
	if (length_byte > ffsize) {
		dma_err("invalid update byte length: 0x%x\n", length_byte);
		return -DMA_EPARAM;
	}
	/* get the current value of swptr */
	swptr_cr = mtk_mhal_dma_get_param(controller, DMA_PARAM_VFF_SWPTR);
	/* calculate new value of swptr */
	swptr_new = _mtk_mhal_dma_calculate_swptr(swptr_cr, ffsize,
						length_byte);
	ret = mtk_mhal_dma_set_param(controller, DMA_PARAM_VFF_SWPTR,
				 swptr_new);

	dma_debug("ffsize %d length_byte %d swptr_cr %d swptr_new %d\n",
		ffsize,	length_byte, swptr_cr, swptr_new);

	return ret;
}

int mtk_mhal_dma_vff_read_data(struct dma_controller *controller,
					u8 *buffer, u32 length)
{
	u32 fifo_data_cnt, fifo_size, swptr, data_idx;
	u8 *fifo_addr;

	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	if (buffer == NULL || length == 0) {
		dma_err("[%s]buffer %p, length %d\n", __func__, buffer,
			   length);
		return -DMA_EPARAM;
	}

	fifo_data_cnt = mtk_mhal_dma_get_param(controller,
					DMA_PARAM_VFF_FIFO_CNT);
	fifo_size = mtk_mhal_dma_get_param(controller,
					DMA_PARAM_VFF_FIFO_SIZE);
	/* get data offset from fifo start address as swptr */
	swptr = mtk_mhal_dma_get_param(controller,
					DMA_PARAM_VFF_SWPTR) & 0xffff;
	/* get start address of fifo */
	fifo_addr = (u8 *)mtk_mhal_dma_get_param(controller,
					DMX_PARAM_PROG_ADDR);

	fifo_data_cnt = fifo_data_cnt < length ? fifo_data_cnt : length;

	for (data_idx = 0; data_idx < fifo_data_cnt; data_idx++) {
		*(buffer + data_idx) = *(fifo_addr + swptr);
		swptr++;
		if (swptr >= fifo_size)
			swptr = 0;
	}

	mtk_mhal_dma_update_swptr(controller, fifo_data_cnt);

	dma_debug("fifo_addr %p length %d fifo_data_cnt %d\n",
		fifo_addr, length, fifo_data_cnt);

	return fifo_data_cnt;
}

int mtk_mhal_dma_clr_dreq(struct dma_controller *controller)
{
	if (controller == NULL) {
		dma_err("dma controller is NULL!\n");
		return -DMA_EPTR;
	}

	mtk_hdl_dma_clr_dreq(controller->base, controller->chn);

	return 0;
}

