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

#include <hdl_wdt.h>
#include <mhal_wdt.h>

#define wdt_warn(str, arg...)          osai_print("[mhal_wdt][W] "str, ##arg)

int mtk_mhal_wdt_enable(struct hal_wdt_dev *wdt_dev,
			u32 en)
{
	if (!wdt_dev || !wdt_dev->cm4_wdt_base)
		return -WDT_EPTR;

	mtk_hdl_wdt_set_enable(wdt_dev->cm4_wdt_base, en);
	return 0;
}

int mtk_mhal_wdt_config(struct hal_wdt_dev *wdt_dev,
			u8 irq)
{
	if (!wdt_dev || !wdt_dev->cm4_wdt_base)
		return -WDT_EPTR;

	mtk_hdl_wdt_set_irq_mode(wdt_dev->cm4_wdt_base, irq);

	return 0;
}

int mtk_mhal_wdt_set_timeout(struct hal_wdt_dev *wdt_dev,
			     u32 sec)
{
	int ret = 0;

	if (!wdt_dev || !wdt_dev->cm4_wdt_base)
		return -WDT_EPTR;

	if (sec > WDT_LENGTH_SEC_MAX) {
		wdt_warn("set_timeout: sec(%u) > MAX(%u),fix to MAX\n",
			 sec, WDT_LENGTH_SEC_MAX);
		sec = WDT_LENGTH_SEC_MAX;
		ret = -WDT_EINVAL;
	}

	mtk_hdl_wdt_set_length(wdt_dev->cm4_wdt_base, WDT_LENGTH_SEC2TICK(sec));
	mtk_hdl_wdt_restart(wdt_dev->cm4_wdt_base);

	return ret;
}

int mtk_mhal_wdt_restart(struct hal_wdt_dev *wdt_dev)
{
	if (!wdt_dev || !wdt_dev->cm4_wdt_base)
		return -WDT_EPTR;

	mtk_hdl_wdt_restart(wdt_dev->cm4_wdt_base);
	return 0;
}

int mtk_mhal_wdt_hwrst(struct hal_wdt_dev *wdt_dev)
{
	void __iomem *wdt_reg;

	if (!wdt_dev || !wdt_dev->cm4_wdt_base)
		return -WDT_EPTR;

	wdt_reg = wdt_dev->cm4_wdt_base;

	mtk_hdl_wdt_set_enable(wdt_reg, 0);
	mtk_hdl_wdt_set_length(wdt_reg, 1);
	mtk_hdl_wdt_restart(wdt_reg);
	mtk_hdl_wdt_set_irq_mode(wdt_reg, 0);
	mtk_hdl_wdt_set_enable(wdt_reg, 1);
	/* wait core reset by WDT hwrst */
	osai_delay_ms(1000);

	return -WDT_EFAULT; /* never return */
}

int mtk_mhal_wdt_swrst(struct hal_wdt_dev *wdt_dev)
{
	void __iomem *wdt_reg;

	if (!wdt_dev || !wdt_dev->cm4_wdt_base)
		return -WDT_EPTR;

	wdt_reg = wdt_dev->cm4_wdt_base;

	mtk_hdl_wdt_set_enable(wdt_reg, 0);
	mtk_hdl_wdt_set_length(wdt_reg,
		WDT_LENGTH_SEC2TICK(WDT_LENGTH_SEC_MAX));
	mtk_hdl_wdt_restart(wdt_reg);
	mtk_hdl_wdt_set_irq_mode(wdt_reg, 0);
	mtk_hdl_wdt_swrst(wdt_reg);
	/* wait core reset by WDT swrst */
	osai_delay_ms(1000);

	return -WDT_EFAULT; /* never return */
}

int mtk_mhal_wdt_get_status(struct hal_wdt_dev *wdt_dev,
			    unsigned int *rst_sta)
{
	u32 sta_val;

	if (!wdt_dev || !wdt_dev->cm4_wdt_base || !rst_sta)
		return -WDT_EPTR;

	sta_val = mtk_hdl_wdt_get_status(wdt_dev->cm4_wdt_base);
	if (sta_val & WDT_HW_RST_STA)
		*rst_sta = 2;
	else if (sta_val & WDT_SW_RST_STA)
		*rst_sta = 1;
	else
		*rst_sta = 0;

	return 0;
}
