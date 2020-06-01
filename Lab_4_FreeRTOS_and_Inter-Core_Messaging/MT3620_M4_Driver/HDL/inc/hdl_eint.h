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

#ifndef __EINT_H__
#define __EINT_H__

#include "mhal_osai.h"

/* debug help MACRO */
#define MTK_EINT_DEBUG_ENABLE 0

#define eint_err(fmt, arg...)	osai_print(fmt, ##arg)
#if MTK_EINT_DEBUG_ENABLE
#define eint_debug(fmt, arg...)	osai_print(fmt, ##arg)
#else
#define eint_debug(fmt, arg...)
#endif

#define EINT_DEBOUNCE_EN_MASK				0x00000200
#define EINT_DEBOUNCE_EN_SHFT				9
#define EINT_DEBOUNCE_POLARITY_MASK			0x00000100
#define EINT_DEBOUNCE_POLARITY_SHFT			8
#define EINT_DEBOUNCE_DUAL_MASK				0x00000080
#define EINT_DEBOUNCE_DUAL_SHFT				7
#define EINT_DEBOUNCE_PRESCALER_MASK			0x00000070
#define EINT_DEBOUNCE_PRESCALER_SHFT			4
#define EINT_DEBOUNCE_CNT_MASK				0x0000000F
#define EINT_DEBOUNCE_CNT_SHFT				0

#define IRQ_NEGATIVE_POLARITY				0x00
#define IRQ_POSITIVE_POLARITY				0x01
#define IRQ_DUAL_ENABLE					0x00
#define IRQ_DUAL_DISABLE				0x01

#ifdef __cplusplus
extern "C" {
#endif

extern void mtk_hdl_eint_enable_debounce(u32 eint_num,
				void __iomem *eint_base);
extern void mtk_hdl_eint_disable_debounce(u32 eint_num,
				void __iomem *eint_base);
extern void mtk_hdl_eint_set_polarity(u32 eint_num,
				u32 pol, void __iomem *eint_base);
extern void mtk_hdl_eint_set_dual(u32 eint_num, u32 dual,
				void __iomem *eint_base);
extern void mtk_hdl_eint_set_prescaler(u32 eint_num,
				u32 prescaler, void __iomem *eint_base);
extern void mtk_hdl_eint_set_cnt(u32 eint_num,
				u32 cnt, void __iomem *eint_base);

#ifdef __cplusplus
}
#endif

#endif /* __EINT_H__ */

