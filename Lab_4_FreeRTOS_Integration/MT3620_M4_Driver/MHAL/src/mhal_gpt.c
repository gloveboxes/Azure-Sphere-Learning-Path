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

#include "hdl_gpt.h"
#include "mhal_gpt.h"

int mtk_mhal_gpt_set_compare(struct hal_gpt_dev *gpt_dev,
			     enum gpt_num timer_id,
			     u32 cmp_val)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	return mtk_hdl_gpt_set_compare(gpt_dev->cm4_gpt_base, (u8)timer_id,
				       cmp_val);
}

int mtk_mhal_gpt_config_mode(struct hal_gpt_dev *gpt_dev,
			     enum gpt_num timer_id,
			     enum gpt_mode mode)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	return mtk_hdl_gpt_config_mode(gpt_dev->cm4_gpt_base, (u8)timer_id,
				       (mode == GPT_REPEAT) ? 1 : 0);
}

int mtk_mhal_gpt_enable_irq(struct hal_gpt_dev *gpt_dev,
			    enum gpt_num timer_id)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	return mtk_hdl_gpt_enable_irq(gpt_dev->cm4_gpt_base, (u8)timer_id);
}

int mtk_mhal_gpt_disable_irq(struct hal_gpt_dev *gpt_dev,
			     enum gpt_num timer_id)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	return mtk_hdl_gpt_disable_irq(gpt_dev->cm4_gpt_base, (u8)timer_id);
}

int mtk_mhal_gpt_get_irq_status(struct hal_gpt_dev *gpt_dev,
				enum gpt_num timer_id,
				unsigned int *int_sta)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base || !int_sta)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	return mtk_hdl_gpt_get_irq_status(gpt_dev->cm4_gpt_base,
					  (u8)timer_id, int_sta);
}

int mtk_mhal_gpt_clear_irq_status(struct hal_gpt_dev *gpt_dev,
				  enum gpt_num timer_id)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	return mtk_hdl_gpt_clear_irq_status(gpt_dev->cm4_gpt_base,
					    (u8)timer_id);
}

int mtk_mhal_gpt_start(struct hal_gpt_dev *gpt_dev,
		       enum gpt_num timer_id)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	mtk_hdl_gpt_start(gpt_dev->cm4_gpt_base, (u8)timer_id);

	return 0;
}

int mtk_mhal_gpt_stop(struct hal_gpt_dev *gpt_dev,
		      enum gpt_num timer_id)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	mtk_hdl_gpt_stop(gpt_dev->cm4_gpt_base, (u8)timer_id);

	return 0;
}

int mtk_mhal_gpt_restart_count(struct hal_gpt_dev *gpt_dev,
			       enum gpt_num timer_id)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	mtk_hdl_gpt_restart_count(gpt_dev->cm4_gpt_base, (u8)timer_id);

	return 0;
}

int mtk_mhal_gpt_get_count(struct hal_gpt_dev *gpt_dev,
			   enum gpt_num timer_id,
			   u32 *cnt_val)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base || !cnt_val)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	*cnt_val = mtk_hdl_gpt_get_count(gpt_dev->cm4_gpt_base, (u8)timer_id);
	return 0;
}

int mtk_mhal_gpt_config_clk(struct hal_gpt_dev *gpt_dev,
			    enum gpt_num timer_id,
			    enum gpt_clk clk)
{
	if (!gpt_dev || !gpt_dev->cm4_gpt_base)
		return -GPT_EPTR;
	if (timer_id >= GPT_MAX_NUM)
		return -GPT_ENODEV;

	mtk_hdl_gpt_config_clk(gpt_dev->cm4_gpt_base, (u8)timer_id, (u8)clk);

	return 0;
}

