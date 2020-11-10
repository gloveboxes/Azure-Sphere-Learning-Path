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

#define WDT_MODE			(0x0030)
#define WDT_MODE_KEY			(0x2200)
#define WDT_MODE_IRQ_OTHER_EN		(0x0010)
#define WDT_MODE_IRQ			(0x0008)
#define WDT_MODE_ENABLE			(0x0001)
#define WDT_MODE_MASK			(0x0019)

#define WDT_LENGTH			(0x0034)
#define WDT_LENGTH_KEY			(0x0008)
#define WDT_LENGTH_MAX			(0x07ff)
#define WDT_LENGTH_TIMEOUT(_tick)	(((_tick) & WDT_LENGTH_MAX) << 5)

#define WDT_RESTART			(0x0038)
#define WDT_RESTART_KEY			(0x1971)

#define WDT_STA				(0x003c)
#define WDT_STA_WDT			WDT_HW_RST_STA	/* hw timeout */
#define WDT_STA_SW_WDT			WDT_SW_RST_STA	/* sw setting */

#define WDT_INTERVAL			(0x0040)
#define WDT_INTERVAL_LENGTH_MASK	(0x0fff)

#define WDT_SWRST			(0x0044)
#define WDT_SWRST_KEY			(0x1209)

#define WDT_SWINT			(0x0080)
#define WDT_SWINT_SW_INT		(0x80000000)
#define WDT_SWINT_SW_INT_CLR		(0x40000000)

#define wdt_read_reg(__base, __offset)		osai_readl((__base) + \
							(__offset))
#define wdt_write_reg(__base, __offset, __val)	osai_writel((__val), (__base) \
							+ (__offset))

static inline u32 _mtk_hdl_wdt_cm4_reg_read(void __iomem *wdt_reg_base,
					    u32 offset)
{
	return (u32)wdt_read_reg(wdt_reg_base, offset);
}

static inline void _mtk_hdl_wdt_cm4_reg_write(void __iomem *wdt_reg_base,
					      u32 offset, u32 val)
{
	wdt_write_reg(wdt_reg_base, offset, val);
	osai_delay_us(100);
}

void mtk_hdl_wdt_set_enable(void __iomem *wdt_reg_base, u32 en)
{
	u32 val = _mtk_hdl_wdt_cm4_reg_read(wdt_reg_base, WDT_MODE);

	if (en)
		val |= WDT_MODE_ENABLE;
	else
		val &= (~(WDT_MODE_ENABLE));

	val &= (~(WDT_MODE_IRQ_OTHER_EN));

	_mtk_hdl_wdt_cm4_reg_write(wdt_reg_base, WDT_MODE, val | WDT_MODE_KEY);
}

void mtk_hdl_wdt_set_irq_mode(void __iomem *wdt_reg_base, u32 irq)
{
	u32 val;

	val = _mtk_hdl_wdt_cm4_reg_read(wdt_reg_base, WDT_MODE);

	if (irq)
		val |= WDT_MODE_IRQ;
	else
		val &= (~(WDT_MODE_IRQ));

	_mtk_hdl_wdt_cm4_reg_write(wdt_reg_base, WDT_MODE, val | WDT_MODE_KEY);
}

void mtk_hdl_wdt_set_length(void __iomem *wdt_reg_base, u32 tick)
{
	u32 val;

	if (!tick)
		tick = 1;

	val = WDT_LENGTH_TIMEOUT(tick) | WDT_LENGTH_KEY;

	_mtk_hdl_wdt_cm4_reg_write(wdt_reg_base, WDT_LENGTH, val);
}

void mtk_hdl_wdt_restart(void __iomem *wdt_reg_base)
{
	_mtk_hdl_wdt_cm4_reg_write(wdt_reg_base, WDT_RESTART, WDT_RESTART_KEY);
}


u32 mtk_hdl_wdt_get_status(void __iomem *wdt_reg_base)
{
	return (_mtk_hdl_wdt_cm4_reg_read(wdt_reg_base, WDT_STA) &
		(WDT_STA_WDT | WDT_STA_SW_WDT));
}

void mtk_hdl_wdt_swrst(void __iomem *wdt_reg_base)
{
	_mtk_hdl_wdt_cm4_reg_write(wdt_reg_base, WDT_SWRST, WDT_SWRST_KEY);
}
