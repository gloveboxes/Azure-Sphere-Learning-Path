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

#include "hdl_eint.h"

static void __iomem *_mtk_hdl_eint_get_offset(u32 eint_num,
				void __iomem *eint_base)
{
	void __iomem *offset;

	offset = (void __iomem *)(eint_base + ((eint_num) * 4));
	return offset;
}

void mtk_hdl_eint_enable_debounce(u32 eint_num,
				void __iomem *eint_base)
{
	u32 reg_val;
	void __iomem *offset = _mtk_hdl_eint_get_offset(eint_num, eint_base);

	eint_debug("%s: eint_base: %x, eint num: %d\n",
				__func__, (u32)eint_base, eint_num);
	reg_val = osai_readl(offset);
	reg_val |= EINT_DEBOUNCE_EN_MASK;
	osai_writel(reg_val, offset);
}

void mtk_hdl_eint_disable_debounce(u32 eint_num,
				void __iomem *eint_base)
{
	u32 reg_val;
	void __iomem *offset = _mtk_hdl_eint_get_offset(eint_num, eint_base);

	eint_debug("%s: eint_base: %x, eint num: %d\n",
				__func__, (u32)eint_base, eint_num);
	reg_val = osai_readl(offset);
	reg_val &= ~EINT_DEBOUNCE_EN_MASK;
	osai_writel(reg_val, offset);
}

void mtk_hdl_eint_set_polarity(u32 eint_num,
				u32 pol, void __iomem *eint_base)
{
	u32 reg_val;
	void __iomem *offset = _mtk_hdl_eint_get_offset(eint_num, eint_base);

	eint_debug("%s: eint_base: %x, eint num: %d, pol: %d\n",
				__func__, (u32)eint_base, eint_num, pol);
	reg_val = osai_readl(offset);
	if (pol == IRQ_NEGATIVE_POLARITY)
		reg_val &= ~EINT_DEBOUNCE_POLARITY_MASK;
	else
		reg_val |= EINT_DEBOUNCE_POLARITY_MASK;
	osai_writel(reg_val, offset);
}

void mtk_hdl_eint_set_dual(u32 eint_num, u32 dual,
				void __iomem *eint_base)
{
	u32 reg_val;
	void __iomem *offset = _mtk_hdl_eint_get_offset(eint_num, eint_base);

	eint_debug("%s: eint_base: %x, eint num: %d, dual: %d\n",
				__func__, (u32)eint_base, eint_num, dual);
	reg_val = osai_readl(offset);
	if (dual == 0)
		reg_val &= ~EINT_DEBOUNCE_DUAL_MASK;
	else
		reg_val |= EINT_DEBOUNCE_DUAL_MASK;
	osai_writel(reg_val, offset);
}

void mtk_hdl_eint_set_prescaler(u32 eint_num, u32 prescaler,
				void __iomem *eint_base)
{
	u32 reg_val;
	void __iomem *offset = _mtk_hdl_eint_get_offset(eint_num, eint_base);

	eint_debug("%s: eint_base: %x, eint num: %d, prescaler: %d\n",
				__func__, (u32)eint_base, eint_num, prescaler);
	reg_val = osai_readl(offset);
	reg_val &= ~EINT_DEBOUNCE_PRESCALER_MASK;
	reg_val |= (EINT_DEBOUNCE_PRESCALER_MASK &
				(prescaler << EINT_DEBOUNCE_PRESCALER_SHFT));
	osai_writel(reg_val, offset);
}

void mtk_hdl_eint_set_cnt(u32 eint_num, u32 cnt,
				void __iomem *eint_base)
{
	u32 reg_val;
	void __iomem *offset = _mtk_hdl_eint_get_offset(eint_num, eint_base);

	eint_debug("%s: eint_base: %x, eint num: %d, cnt: %d\n",
				__func__, (u32)eint_base, eint_num, cnt);
	reg_val = osai_readl(offset);
	reg_val &= ~EINT_DEBOUNCE_CNT_MASK;
	cnt = cnt & EINT_DEBOUNCE_CNT_MASK;
	reg_val |= cnt;
	osai_writel(reg_val, offset);
}
