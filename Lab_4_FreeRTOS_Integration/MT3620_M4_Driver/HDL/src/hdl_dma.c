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

#include "hdl_dma.h"

static u32 _mtk_hdl_dma_byte_to_count(u32 byte_cnt, u8 transize)
{
	u32 count_unit = 1;

	switch (transize) {
	case DMA_CON_SIZE_LONG:
		count_unit = 4;
		break;
	case DMA_CON_SIZE_SHORT:
		count_unit = 2;
		break;
	case DMA_CON_SIZE_BYTE:
		count_unit = 1;
		break;
	}

	if (byte_cnt == 0)
		return 0;
	else
		return (byte_cnt - 1) / count_unit + 1;
}

static u32 _mtk_hdl_dma_count_to_byte(u32 count, u8 transize)
{
	u32 count_unit = 1;

	switch (transize) {
	case DMA_CON_SIZE_LONG:
		count_unit = 4;
		break;
	case DMA_CON_SIZE_SHORT:
		count_unit = 2;
		break;
	case DMA_CON_SIZE_BYTE:
		count_unit = 1;
		break;
	}

	return count * count_unit;
}

void mtk_hdl_dma_set_src(void __iomem *chn_base, u32 src_addr)
{
	osai_writel(src_addr, DMA_SRC(chn_base));
}

void mtk_hdl_dma_set_dst(void __iomem *chn_base, u32 dst_addr)
{
	osai_writel(dst_addr, DMA_DST(chn_base));
}

void mtk_hdl_dma_set_wppt(void __iomem *chn_base, u32 wppt, u8 transize)
{
	wppt = _mtk_hdl_dma_byte_to_count(wppt, transize);
	osai_writel(wppt & DMA_WPPT_MSK, DMA_WPPT(chn_base));
}

void mtk_hdl_dma_set_wpto(void __iomem *chn_base, u32 wpto)
{
	osai_writel(wpto, DMA_WPTO(chn_base));
}

int mtk_hdl_dma_set_count(void __iomem *chn_base, u32 count, u8 transize)
{
	u32 value = osai_readl(DMA_COUNT(chn_base));

	count = _mtk_hdl_dma_byte_to_count(count, transize);
	if ((count > DMA_COUNT_LEN_MSK) || (count == 0))
		return -1;

	value &= ~DMA_COUNT_LEN_MSK;
	value |= (count & DMA_COUNT_LEN_MSK);
	osai_writel(value, DMA_COUNT(chn_base));
	return 0;
}

void mtk_hdl_dma_set_con(void __iomem *chn_base, u32 control_settings)
{
	osai_writel(control_settings, DMA_CON(chn_base));
}

u32 mtk_hdl_dma_get_remain_cnt(void __iomem *chn_base, u8 transize)
{
	u32 remain_cnt = osai_readl(DMA_RLCT(chn_base)) & DMA_RLCT_MASK;

	return _mtk_hdl_dma_count_to_byte(remain_cnt, transize);
}

void mtk_hdl_dma_set_bw_limiter(void __iomem *chn_base,
					     u32 bandwidth_limiter)
{
	osai_writel(bandwidth_limiter & DMA_LIMITER_MASK,
		    DMA_LIMITER(chn_base));
}

void mtk_hdl_dma_set_pgmaddr(void __iomem *chn_base, u32 prog_addr)
{
	osai_writel(prog_addr, DMA_PGMADDR(chn_base));
}

u32 mtk_hdl_dma_get_pgmaddr(void __iomem *chn_base)
{
	return osai_readl(DMA_PGMADDR(chn_base));
}

int mtk_hdl_dma_set_altlen(void __iomem *chn_base, u8 altscm, u32 altlen)
{
	u32 value = osai_readl(DMA_ALTLEN(chn_base));

	if (altscm == 1)
		value |= DMA_ALTLEN_ALTSCM;
	else
		value &= ~DMA_ALTLEN_ALTSCM;

	if (altlen > DMA_ALTLEN_MSK)
		return -1;

	value &= ~DMA_ALTLEN_MSK;
	value |= (altlen & DMA_ALTLEN_MSK);
	osai_writel(value, DMA_ALTLEN(chn_base));
	return 0;
}

int mtk_hdl_dma_set_ffsize(void __iomem *chn_base, u32 ffsize, u8 transize)
{
	ffsize = _mtk_hdl_dma_byte_to_count(ffsize, transize);

	if (ffsize > DMA_FFSIZE_MSK)
		return -1;
	osai_writel(ffsize & DMA_FFSIZE_MSK, DMA_FFSIZE(chn_base));

	return 0;
}

void mtk_hdl_dma_set_timeout_thr(void __iomem *chn_base, u32 ff_timeout)
{
	osai_writel(ff_timeout, DMA_TO(chn_base));
}

u32 mtk_hdl_dma_get_hwptr(void __iomem *chn_base)
{
	return osai_readl(DMA_HWPTR(chn_base)) &
	       (DMA_HWPTR_WARP | DMA_HWPTR_MSK);
}

void mtk_hdl_dma_set_swptr(void __iomem *chn_base, u32 swptr)
{
	u32 value = osai_readl(DMA_SWPTR(chn_base));

	value &= ~DMA_SWPTR_WARP;
	value |= (swptr & DMA_SWPTR_WARP);
	value &= ~DMA_SWPTR_MSK;
	value |= (swptr & DMA_SWPTR_MSK);
	osai_writel(value, DMA_SWPTR(chn_base));
}

u32 mtk_hdl_dma_get_swptr(void __iomem *chn_base)
{
	return osai_readl(DMA_SWPTR(chn_base)) &
	       (DMA_SWPTR_WARP | DMA_SWPTR_MSK);
}

void mtk_hdl_dma_set_fixaddr(void __iomem *chn_base, u32 fix_addr)
{
	osai_writel(fix_addr, DMA_FIXADDR(chn_base));
}

u32 mtk_hdl_dma_get_fixaddr(void __iomem *chn_base)
{
	return osai_readl(DMA_FIXADDR(chn_base));
}

u32 mtk_hdl_dma_get_vfifo_ffcnt(void __iomem *chn_base, u8 transize)
{
	u32 ffcnt = osai_readl(DMA_FFCNT(chn_base)) & DMA_FFCNT_MSK;

	return _mtk_hdl_dma_count_to_byte(ffcnt, transize);
}

u32 mtk_hdl_dma_get_vfifo_ffsize(void __iomem *chn_base, u8 transize)
{
	u32 ffsize = osai_readl(DMA_FFSIZE(chn_base)) & DMA_FFSIZE_MSK;

	return _mtk_hdl_dma_count_to_byte(ffsize, transize);
}

/* only for halfsize dma */
void mtk_hdl_dma_enable_reload(void __iomem *chn_base)
{
	u32 value = osai_readl(DMA_COUNT(chn_base));

	value |= DMA_COUNT_RLLEN_BIT;
	osai_writel(value, DMA_COUNT(chn_base));
}

/* only for halfsize dma */
void mtk_hdl_dma_disable_reload(void __iomem *chn_base)
{
	u32 value = osai_readl(DMA_COUNT(chn_base));

	value &= ~DMA_COUNT_RLLEN_BIT;
	osai_writel(value, DMA_COUNT(chn_base));
}

void mtk_hdl_dma_start(void __iomem *chn_base)
{
	osai_writel(DMA_START_BIT, DMA_START(chn_base));
}

void mtk_hdl_dma_stop_now(void __iomem *chn_base)
{
	osai_writel(DMA_STOP_BIT, DMA_START(chn_base));
}

void mtk_hdl_dma_ack_int(void __iomem *chn_base)
{
	osai_writel(DMA_ACKINT_BIT, DMA_ACKINT(chn_base));
}

void mtk_hdl_dma_ack_half_int(void __iomem *chn_base)
{
	osai_writel(DMA_ACKHINT_BIT, DMA_ACKINT(chn_base));
}

void mtk_hdl_dma_ack_timeout_int(void __iomem *chn_base)
{
	osai_writel(DMA_ACKTOINT_BIT, DMA_ACKINT(chn_base));
}

u8 mtk_hdl_dma_chk_run_status(void __iomem *dma_base, u8 chn)
{
	if (chn >= 16) {
		return (osai_readl(DMA_GLBSTA1(dma_base)) &
			DMA_GLBSTA_RUN(chn)) != 0;
	} else {
		return (osai_readl(DMA_GLBSTA0(dma_base)) &
			DMA_GLBSTA_RUN(chn)) != 0;
	}
}

u8 mtk_hdl_dma_chk_int_status(void __iomem *dma_base, u8 chn)
{
	if (chn >= 16) {
		return (osai_readl(DMA_GLBSTA1(dma_base)) &
			DMA_GLBSTA_IT(chn)) != 0;
	} else {
		return (osai_readl(DMA_GLBSTA0(dma_base)) &
			DMA_GLBSTA_IT(chn)) != 0;
	}
}

u8 mtk_hdl_dma_chk_int(void __iomem *chn_base)
{
	return (osai_readl(DMA_INTSTA(chn_base)) &
		DMA_INTSTA_BIT) == DMA_INTSTA_BIT;
}

u8 mtk_hdl_dma_chk_half_int(void __iomem *chn_base)
{
	return (osai_readl(DMA_INTSTA(chn_base)) &
		DMA_HINTSTA_BIT) == DMA_HINTSTA_BIT;
}

u8 mtk_hdl_dma_chk_timeout_int(void __iomem *chn_base)
{
	return (osai_readl(DMA_INTSTA(chn_base)) &
		DMA_TOINTSTA_BIT) == DMA_TOINTSTA_BIT;
}

void mtk_hdl_dma_dump_chn_reg(void __iomem *chn_base)
{
	void __iomem *start_reg = chn_base;
	u32 chn_reg_size = DMA_CHN_REG_OFFSET;
	u32 reg_idx;

	for (reg_idx = 0; reg_idx < chn_reg_size; reg_idx += 4) {
		if (reg_idx % 16 == 0)
			dma_err("\n[%p]", start_reg + reg_idx);
		dma_err("0x%08x ", osai_readl(start_reg + reg_idx));
	}
	dma_err("\n");
}

void mtk_hdl_dma_dump_global_reg(void __iomem *dma_base)
{
	void __iomem *start_reg = dma_base + DMA_GLB_REG_OFFSET;
	u32 global_reg_size = 0x11C;
	u32 reg_idx;

	for (reg_idx = 0; reg_idx < global_reg_size; reg_idx += 4) {
		if (reg_idx % 16 == 0)
			dma_err("\n[%p]", start_reg + reg_idx);
		dma_err("0x%08x ", osai_readl(start_reg + reg_idx));
	}
	dma_err("\n");
}

u32 mtk_hdl_dma_chk_pause_status(void __iomem *dma_base, u8 chn)
{
	return (osai_readl(DMA_GLBSTAPAUSE(dma_base)) &
		(u32)(1 << chn)) >> chn;
}

void mtk_hdl_dma_pause(void __iomem *dma_base, u8 chn)
{
	osai_writel((osai_readl(DMA_GLBPAUSE(dma_base)) | (u32)(1 << chn)),
		    DMA_GLBPAUSE(dma_base));
}

void mtk_hdl_dma_resume(void __iomem *dma_base, u8 chn)
{
	osai_writel((osai_readl(DMA_GLBPAUSE(dma_base)) & (u32)(~(1 << chn))),
		     DMA_GLBPAUSE(dma_base));
}

void mtk_hdl_dma_reset_chn(void __iomem *dma_base, u8 chn)
{
	u32 value = osai_readl(DMA_CH_RST(dma_base));

	value &= ~(1 << chn);
	osai_writel(value, DMA_CH_RST(dma_base));

	value |= (1 << chn);
	osai_writel(value, DMA_CH_RST(dma_base));
}

/** @brief  Manual configure the DMA throttle to create a DREQ
 *
 *  @param  channel : the number of operating channel
 *
 *  @return void
 *
 *  @note   Manual mode for handshanke control to set a DREQ
 */
void mtk_hdl_dma_set_dreq(void __iomem *dma_base, u8 chn)
{
	u32 data = 0;

	osai_writel(data, DMA_SET_DREQ(dma_base));
	data |= (1 << (chn));
	osai_writel(data, DMA_SET_DREQ(dma_base));
	data = 0;
	osai_writel(data, DMA_SET_DREQ(dma_base));
}

/** @brief  Manual configure the DMA throttle to clear a DREQ
 *
 *  @param  channel : the number of operating channel
 *
 *  @return void
 *
 *  @note   Manual mode for handshanke control to clear a DREQ
 */
void mtk_hdl_dma_clr_dreq(void __iomem *dma_base, u8 chn)
{
	u32 data = 0;

	osai_writel(data, DMA_CLR_DREQ(dma_base));
	data |= (1 << (chn));
	osai_writel(data, DMA_CLR_DREQ(dma_base));
	data = 0;
	osai_writel(data, DMA_CLR_DREQ(dma_base));
}

void mtk_hdl_dma_clock_enable(void __iomem *dma_base, u8 chn)
{
	u32 val = 0;

	val = osai_readl(DMA_CH_EN_SET(dma_base));
	val |= (1 << chn);
	osai_writel(val, DMA_CH_EN_SET(dma_base));
	dma_debug("%s: %p, 0x%08x\n", __func__,
	(DMA_CH_EN_SET(dma_base)), (u32)(val));
}

void mtk_hdl_dma_clock_disable(void __iomem *dma_base, u8 chn)
{
	u32 val;

	val = osai_readl(DMA_CH_EN_CLR(dma_base));
	val |= (1 << chn);
	osai_writel(val, DMA_CH_EN_CLR(dma_base));
	dma_debug("%s: %p, 0x%08x\n", __func__,
	(DMA_CH_EN_CLR(dma_base)), (u32)(val));
}
