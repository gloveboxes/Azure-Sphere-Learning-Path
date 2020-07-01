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

#include "mhal_eint.h"
#include "hdl_eint.h"

#define DEBOUNCE_MAX_TIME		128

static void _mtk_mhal_eint_calc_deb_param(u32 debounce_time,
			u32 *prescaler, u32 *cnt)
{
	u32 const threshold = 10;
	u32 target_count = debounce_time * 8;
	u32 tmp = 0;

	for (tmp = 0; tmp < 7; tmp++) {
		if (target_count <= threshold)
			break;
		target_count = target_count / 2;
	}
	*prescaler = tmp;
	*cnt = target_count;
}

int mtk_mhal_eint_enable_debounce(eint_number eint_num,
			void __iomem *eint_base)
{
	if (!eint_base) {
		eint_err("%s eint_base is NULL.\n", __func__);
		return -EINT_EFAULT;
	}

	if (eint_num >= HAL_EINT_NUMBER_MAX) {
		eint_err("%s eint_num is invaild.\n", __func__);
		return -EINT_EINVAL;
	}

	mtk_hdl_eint_enable_debounce((u32)eint_num, eint_base);

	return 0;
}

int mtk_mhal_eint_disable_debounce(eint_number eint_num,
			void __iomem *eint_base)
{
	if (!eint_base) {
		eint_err("%s eint_base is NULL\n", __func__);
		return -EINT_EFAULT;
	}

	if (eint_num >= HAL_EINT_NUMBER_MAX) {
		eint_err("%s eint_num is invaild.\n", __func__);
		return -EINT_EINVAL;
	}

	mtk_hdl_eint_disable_debounce((u32)eint_num, eint_base);

	return 0;
}

int mtk_mhal_eint_set_polarity(eint_number eint_num, u32 pol,
			void __iomem *eint_base)
{
	if (!eint_base) {
		eint_err("%s eint_base is NULL\n", __func__);
		return -EINT_EFAULT;
	}

	if (eint_num >= HAL_EINT_NUMBER_MAX) {
		eint_err("%s eint_num is invaild.\n", __func__);
		return -EINT_EINVAL;
	}

	if (pol > 1) {
		eint_err("%s pol is invaild.\n", __func__);
		return -EINT_EINVAL;
	}

	mtk_hdl_eint_set_polarity((u32)eint_num, pol, eint_base);

	return 0;

}

int mtk_mhal_eint_set_dual(eint_number eint_num, u32 dual,
			void __iomem *eint_base)
{
	if (!eint_base) {
		eint_err("%s eint_base is NULL\n", __func__);
		return -EINT_EFAULT;
	}

	if (eint_num >= HAL_EINT_NUMBER_MAX) {
		eint_err("%s eint_num is invaild.\n", __func__);
		return -EINT_EINVAL;
	}

	if (dual > 1) {
		eint_err("%s dual is invaild.\n", __func__);
		return -EINT_EINVAL;
	}

	mtk_hdl_eint_set_dual((u32)eint_num, dual, eint_base);

	return 0;
}

int mtk_mhal_eint_set_debounce(eint_number eint_num,
		u32 debounce_time, void __iomem *eint_base)
{
	u32 prescaler, cnt;

	if (!eint_base) {
		eint_err("%s eint_base is NULL\n", __func__);
		return -EINT_EFAULT;
	}

	if (eint_num >= HAL_EINT_NUMBER_MAX) {
		eint_err("%s eint_num is invaild.\n", __func__);
		return -EINT_EINVAL;
	}

	if (debounce_time > DEBOUNCE_MAX_TIME)
		debounce_time = DEBOUNCE_MAX_TIME;

	_mtk_mhal_eint_calc_deb_param(debounce_time, &prescaler, &cnt);
	mtk_hdl_eint_set_prescaler((u32)eint_num, prescaler, eint_base);
	mtk_hdl_eint_set_cnt((u32)eint_num, cnt, eint_base);

	return 0;
}
