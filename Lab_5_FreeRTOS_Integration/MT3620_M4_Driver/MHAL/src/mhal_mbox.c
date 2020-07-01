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

#include "mhal_mbox.h"
#include "hdl_mbox.h"

/* debug help MACRO */
#define MTK_MBOX_DEBUG_ENABLE 0

#define mbox_err(fmt, arg...)	osai_print("[mbox][error] %s(L:%d) "fmt, \
						__func__, __LINE__, ##arg)
#if MTK_MBOX_DEBUG_ENABLE
#define mbox_debug(fmt, arg...)	osai_print("[mbox][debug] %s(L:%d) "fmt, \
						__func__, __LINE__, ##arg)
#else
#define mbox_debug(fmt, arg...)
#endif


int mtk_mhal_mbox_reset(void __iomem *base)
{
	if (!base) {
		mbox_err("base is null\n");
		return -MBOX_EPTR;
	}
	mtk_hdl_mbox_reset(base);

	return MBOX_OK;
}

int mtk_mhal_mbox_fifo_read(void __iomem *base, struct mbox_fifo_item *buf,
			mbox_tr_type_t type)
{
	u32 count;

	if (!base) {
		mbox_err("base is null\n");
		return -MBOX_EPTR;
	}
	if (!buf) {
		mbox_err("buf is null\n");
		return -MBOX_EPTR;
	}
	if (type >= MBOX_TR_MAX) {
		mbox_err("type is out of range, type:%d\n", type);
		return -MBOX_EDEFAULT;
	}

	mtk_hdl_mbox_get_acpt_fifo_cnt(base, &count);
	if (count == 0)
		return -MBOX_EEMPTY;

	if (type == MBOX_TR_DATA_CMD)
		mtk_hdl_mbox_data_fifo_acpt(base, &buf->data);

	 mtk_hdl_mbox_cmd_fifo_acpt(base, &buf->cmd);

	return MBOX_OK;
}

int mtk_mhal_mbox_fifo_write(void __iomem *base,
			const struct mbox_fifo_item *buf,
			mbox_tr_type_t type)
{
	u32 count;

	if (!base) {
		mbox_err("base is null\n");
		return -MBOX_EPTR;
	}
	if (!buf) {
		mbox_err("buf is null\n");
		return -MBOX_EPTR;
	}
	if (type >= MBOX_TR_MAX) {
		mbox_err("type is out of range, type:%d\n", type);
		return -MBOX_EDEFAULT;
	}

	mtk_hdl_mbox_get_post_fifo_cnt(base, &count);
	if (count == MBOX_CHANNEL_FIFO_DEPTH)
		return -MBOX_EFULL;

	if (type == MBOX_TR_DATA_CMD)
		mtk_hdl_mbox_data_fifo_post(base, buf->data);

	mtk_hdl_mbox_cmd_fifo_post(base, buf->cmd);

	return MBOX_OK;
}

static inline u32 _mtk_mhal_mbox_int_mask(enum mbox_int_type type)
{
	u32 mask = MBOX_INT_MASK;

	switch (type) {
	case MBOX_INT_TYPE_RD:
		mask <<= MBOX_INT_RD_OFFSET;
		break;
	case MBOX_INT_TYPE_NF:
		mask <<= MBOX_INT_NF_OFFSET;
		break;
	case MBOX_INT_TYPE_WR:
		mask <<= MBOX_INT_WR_OFFSET;
		break;
	case MBOX_INT_TYPE_NE:
		mask <<= MBOX_INT_NE_OFFSET;
		break;
	default:
		mask = 0;
		break;
	}

	return mask;
}

static inline void _mtk_mhal_mbox_event(struct mbox_fifo_event *event, u32 mask)
{
	event->rd_int = (MBOX_INT_MASK << MBOX_INT_RD_OFFSET) & mask ? 1 : 0;
	event->wr_int = (MBOX_INT_MASK << MBOX_INT_WR_OFFSET) & mask ? 1 : 0;
	event->ne_sts = (MBOX_INT_MASK << MBOX_INT_NE_OFFSET) & mask ? 1 : 0;
	event->nf_sts = (MBOX_INT_MASK << MBOX_INT_NF_OFFSET) & mask ? 1 : 0;
}

int mtk_mhal_mbox_ioctl(void __iomem *base,
			mbox_ioctl_t ctrl, void *arg)
{
	struct mbox_swint_arg *swint_ctrl;
	struct mbox_int_arg *int_ctrl;
	struct mbox_fifo_event *event;
	u32 mask;
	u32 status;

	if (!base) {
		mbox_err("base is null\n");
		return -MBOX_EPTR;
	}
	if (ctrl != MBOX_IOSET_SEM_POST
		&& ctrl <= MBOX_IOGET_ACPT_FIFO_CNT && !arg) {
		mbox_err("arg is null\n");
		return -MBOX_EPTR;
	}

	switch (ctrl) {
	case MBOX_IOSET_NE_THRS:
		mtk_hdl_mbox_set_ne_thrs(base, *(u32 *)arg);
		break;
	case MBOX_IOGET_NE_THRS:
		mtk_hdl_mbox_get_ne_thrs(base, (u32 *)arg);
		break;
	case MBOX_IOSET_NF_THRS:
		mtk_hdl_mbox_set_nf_thrs(base, *(u32 *)arg);
		break;
	case MBOX_IOGET_NF_THRS:
		mtk_hdl_mbox_get_nf_thrs(base, (u32 *)arg);
		break;

	case MBOX_IOSET_SEM_POST:
		mtk_hdl_mbox_sem_post(base);
		break;
	case MBOX_IOSET_SEM_PEND:
		mtk_hdl_mbox_sem_pend(base, (u32 *)arg);
		if (*((u32 *)arg) >= MBOX_SEM_UNDEF)
			return -MBOX_EDEFAULT;
		break;

	case MBOX_IOSET_SWINT_TRIG:
		if (*((u32 *)arg) >= MBOX_NUM_SW_INT) {
			mbox_err("invalid sw int trig:%u, should be 0~%d\n",
				*(u32 *)arg, MBOX_NUM_SW_INT-1);
			return -MBOX_EDEFAULT;
		}
		mtk_hdl_mbox_trig_sw_int(base, *(u32 *)arg);
		break;
	case MBOX_IOSET_SWINT_EN:
		if (*((u32 *)arg) >= (1<<MBOX_NUM_SW_INT)) {
			mbox_err(
			"invalid sw int en:0x%x, should be 0x0~0x%x\n",
				*(u32 *)arg, (1<<MBOX_NUM_SW_INT)-1);
			return -MBOX_EDEFAULT;
		}
		mtk_hdl_mbox_sw_int_en(base, *(u32 *)arg);
		break;
	case MBOX_IOGET_SWINT_EN:
		mtk_hdl_mbox_get_sw_int_en(base, (u32 *)arg);
		break;
	case MBOX_IOSET_SWINT_CTRL:
		swint_ctrl = (struct mbox_swint_arg *)arg;
		if (swint_ctrl->id >= MBOX_NUM_SW_INT) {
			mbox_err("invalid sw int ctrl:%u, should be 0~%d\n",
				swint_ctrl->id, MBOX_NUM_SW_INT-1);
			return -MBOX_EDEFAULT;
		}
		mtk_hdl_mbox_sw_int_ctrl(base,
			swint_ctrl->enable, swint_ctrl->id);
		break;
	case MBOX_IOGET_SWINT_STS:
		mtk_hdl_mbox_get_sw_int_sts(base, (u32 *)arg);
		break;
	case MBOX_IOSET_CLEAR_SWINT:
		if (*((u32 *)arg) >= (1<<MBOX_NUM_SW_INT)) {
			mbox_err(
			"invalid sw int clear:0x%x, should be 0x0~0x%x\n",
				*(u32 *)arg, (1<<MBOX_NUM_SW_INT)-1);
			return -MBOX_EDEFAULT;
		}
		mtk_hdl_mbox_clear_sw_int_sts(base, *(u32 *)arg);
		break;

	case MBOX_IOSET_INT_EN:
		int_ctrl = (struct mbox_int_arg *)arg;
		mask = _mtk_mhal_mbox_int_mask(int_ctrl->type);
		if (!mask) {
			mbox_err("invalid int type:%u, should be 0~%u\n",
				int_ctrl->type, MBOX_INT_TYPE_MAX-1);
			return -MBOX_EDEFAULT;
		}
		mtk_hdl_mbox_int_en(base, int_ctrl->enable, mask);
		break;
	case MBOX_IOGET_INT_EN:
		event = (struct mbox_fifo_event *)arg;
		mtk_hdl_mbox_get_int_en(base, &mask);
		_mtk_mhal_mbox_event(event, mask);
		break;
	case MBOX_IOGET_INT_STS:
		event = (struct mbox_fifo_event *)arg;
		mtk_hdl_mbox_get_int_sts(base, &status);
		_mtk_mhal_mbox_event(event, status);
		break;
	case MBOX_IOSET_CLEAR_INT:
		int_ctrl = (struct mbox_int_arg *)arg;
		mask = _mtk_mhal_mbox_int_mask(int_ctrl->type);
		if (!mask) {
			mbox_err("invalid int type:%u, should be 0~%u\n",
				int_ctrl->type, MBOX_INT_TYPE_MAX-1);
			return -MBOX_EDEFAULT;
		}
		mtk_hdl_mbox_clear_int_sts(base, mask);
		break;

	case MBOX_IOGET_POST_FIFO_CNT:
		mtk_hdl_mbox_get_post_fifo_cnt(base, (u32 *)arg);
		break;
	case MBOX_IOGET_ACPT_FIFO_CNT:
		mtk_hdl_mbox_get_acpt_fifo_cnt(base, (u32 *)arg);
		break;

	default:
		mbox_err("ctrl is out of range, ctrl:%d, should be 0~%d\n",
			ctrl, MBOX_IOGET_ACPT_FIFO_CNT);
		return -MBOX_EDEFAULT;
	}

	return MBOX_OK;
}
