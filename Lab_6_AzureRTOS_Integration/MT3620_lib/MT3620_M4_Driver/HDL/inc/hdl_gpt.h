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

#ifndef __HDL_GPT_H__
#define __HDL_GPT_H__

#include "mhal_osai.h"

#ifdef __cplusplus
extern "C" {
#endif

int mtk_hdl_gpt_set_compare(void __iomem *gpt_reg_base,
			    u8 timer_id,
			    u32 cmp_val);
int mtk_hdl_gpt_config_mode(void __iomem *gpt_reg_base,
			    u8 timer_id,
			    u8 auto_repeat);
int mtk_hdl_gpt_enable_irq(void __iomem *gpt_reg_base,
			   u8 timer_id);
int mtk_hdl_gpt_disable_irq(void __iomem *gpt_reg_base,
			    u8 timer_id);
int mtk_hdl_gpt_get_irq_status(void __iomem *gpt_reg_base,
			       u8 timer_id,
			       unsigned int *int_sta);
int mtk_hdl_gpt_clear_irq_status(void __iomem *gpt_reg_base,
				 u8 timer_id);
void mtk_hdl_gpt_start(void __iomem *gpt_reg_base,
		       u8 timer_id);
void mtk_hdl_gpt_stop(void __iomem *gpt_reg_base,
		      u8 timer_id);
void mtk_hdl_gpt_restart_count(void __iomem *gpt_reg_base,
			       u8 timer_id);
u32 mtk_hdl_gpt_get_count(void __iomem *gpt_reg_base,
			  u8 timer_id);
void mtk_hdl_gpt_config_clk(void __iomem *gpt_reg_base,
			    u8 timer_id,
			    u8 clk_src);

#ifdef __cplusplus
}
#endif

#endif /* __HDL_GPT_H__ */
