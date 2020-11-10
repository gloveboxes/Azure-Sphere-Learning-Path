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

#ifndef __HDL_MBOX_H__
#define __HDL_MBOX_H__

/**
 * @file hdl_mbox.h
 *
 * Mailbox hardware driver for cm4 cores (IO0/IO1)
 */
#include <mhal_osai.h>

/*
 * Constants and Macros
 */
#define MBOX_CHANNEL_FIFO_DEPTH		(15)
#define MBOX_NUM_SW_INT			(8)

/* Channel definition */
#define MBOX_HSP_CA7_NORMAL_CHANNEL	(0)
#define MBOX_HSP_CA7_SECURE_CHANNEL	(1)

#define MBOX_IO_CA7_CHANNEL		(0)
#define MBOX_IO0_IO1_CHANNEL		(1)


#define MBOX_INT_MASK				(0x1)
#define	MBOX_INT_RD_OFFSET			(3U)
#define	MBOX_INT_NF_OFFSET			(2U)
#define	MBOX_INT_WR_OFFSET			(1U)
#define	MBOX_INT_NE_OFFSET			(0U)

#define MBOX_THRS_MASK				(0x0F)

#ifdef __cplusplus
extern "C" {
#endif

void mtk_hdl_mbox_reset(void __iomem *base);

void mtk_hdl_mbox_set_ne_thrs(void __iomem *base, u32 value);

void mtk_hdl_mbox_get_ne_thrs(void __iomem *base, u32 *value);

void mtk_hdl_mbox_set_nf_thrs(void __iomem *base, u32 value);

void mtk_hdl_mbox_get_nf_thrs(void __iomem *base, u32 *value);

void mtk_hdl_mbox_sem_post(void __iomem *base);

void mtk_hdl_mbox_sem_pend(void __iomem *base, u32 *value);

void mtk_hdl_mbox_trig_sw_int(void __iomem *base, u32 id);

void mtk_hdl_mbox_sw_int_en(void __iomem *base, u32 value);

void mtk_hdl_mbox_get_sw_int_en(void __iomem *base, u32 *value);

void mtk_hdl_mbox_sw_int_ctrl(void __iomem *base, int enable, u32 id);

void mtk_hdl_mbox_get_sw_int_sts(void __iomem *base, u32 *value);

void mtk_hdl_mbox_clear_sw_int_sts(void __iomem *base, u32 mask);

void mtk_hdl_mbox_int_en(void __iomem *base, u32 enable, u32 mask);

void mtk_hdl_mbox_get_int_en(void __iomem *base, u32 *value);

void mtk_hdl_mbox_get_int_sts(void __iomem *base, u32 *value);

void mtk_hdl_mbox_clear_int_sts(void __iomem *base, u32 mask);

void mtk_hdl_mbox_get_post_fifo_cnt(void __iomem *base, u32 *value);

void mtk_hdl_mbox_get_acpt_fifo_cnt(void __iomem *base, u32 *value);

void mtk_hdl_mbox_data_fifo_post(void __iomem *base, u32 data);

void mtk_hdl_mbox_cmd_fifo_post(void __iomem *base, u32 cmd);

void mtk_hdl_mbox_data_fifo_acpt(void __iomem *base, u32 *data);

void mtk_hdl_mbox_cmd_fifo_acpt(void __iomem *base, u32 *cmd);

#ifdef __cplusplus
}
#endif

#endif /* __HDL_MBOX_H__ */
