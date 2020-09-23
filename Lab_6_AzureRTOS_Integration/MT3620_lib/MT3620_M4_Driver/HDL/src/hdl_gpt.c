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

/* GPT0 & GPT1: down + irq (1k, 32k) */
#define GPT_ISR				0x000
#define GPT_IER				0x004
#define GPTx_INT_MASK(id)		(0x0001 << (id))
#define GPTs_INT_MASK			0x0003

#define GPTx_CTRL(id)			(0x010 + ((id) << 4))
#define GPT_RESTART			(0x0001 << 3)
#define GPT_SPEED(sp)			((sp & 0x1) << 2)
#define GPT_SPEED_1K			GPT_SPEED(0)
#define GPT_SPEED_32K			GPT_SPEED(1)
#define GPT_MODE(mode)			((mode & 0x1) << 1)
#define GPT_MODE_ONESHOT		GPT_MODE(0)
#define GPT_MODE_REPEAT			GPT_MODE(1)
#define GPT_EN				(0x0001 << 0)
#define GPTx_ICNT(id)			(0x0014 + ((id) << 4))
#define GPTx_CNT(id)			(0x0040 + ((id) << 2))

/* GPT2: up + freerun (1k, 32k) */
#define GPT2_CTRL			(0x030)
#define GPT2_SPEED(sp)			((sp & 0x1) << 1)
#define GPT2_SPEED_1K			GPT2_SPEED(0)
#define GPT2_SPEED_32K			GPT2_SPEED(1)
#define GPT2_CNT			(0x034)

/* GPT3: up + irq (1M) */
#define GPT3_CTRL			(0x050)
#define GPT3_CLKSRC_26M			(26000000)
#define GPT3_OSC_CNT_1US(tick)		(((tick) & 0x3f) << 16)
#define GPT3_OSC_CNT_MASK		GPT3_OSC_CNT_1US(0x3f)
#define GPT3_ICLR			(0x0001 << 15)
#define GPT3_INIT			(0x054)
#define GPT3_CNT			(0x058)
#define GPT3_EXPIRE			(0x05c)

/* GPT4: up + freerun (bus: fpga 40M, soc 160M) */
#define GPT4_CTRL			(0x060)
#define GPT4_SPEED(sp)			GPT2_SPEED(sp)
#define GPT4_SPEED_H_BUS		GPT4_SPEED(0)
#define GPT4_SPEED_BUS			GPT4_SPEED(1)
#define GPT4_INIT			(0x064)
#define GPT4_CNT			(0x068)

#define GPT_TIMER_MAX_NUM		5

/** error return: need to be consistent with mhal_gpt.h */
/** Invalid argument. It means the input pointer is NULL */
#define GPT_EPTR		1
/** Invalid argument. It means the HW unit does not exist. */
#define GPT_ENODEV		2
/**
  * Permission denied.
  * It means the configuration is not supported by HW design.
  */
#define GPT_EACCES		3

#define gpt_read_reg(__base, __offset)		osai_readl((__base) + \
							(__offset))
#define gpt_write_reg(__base, __offset, __val)	osai_writel((__val), (__base) \
							+ (__offset))

static const u32 GPT_CTRL_LIST[GPT_TIMER_MAX_NUM] = {
					GPTx_CTRL(0), GPTx_CTRL(1), GPT2_CTRL,
					GPT3_CTRL, GPT4_CTRL};
static const u32 GPT_CNT_LIST[GPT_TIMER_MAX_NUM] = {
					GPTx_CNT(0), GPTx_CNT(1), GPT2_CNT,
					GPT3_CNT, GPT4_CNT};

static inline u32 _mtk_hdl_gpt_cm4_reg_read(void __iomem *gpt_reg_base,
					    u32 offset)
{
	return (u32)gpt_read_reg(gpt_reg_base, offset);
}

static inline void _mtk_hdl_gpt_cm4_reg_write(void __iomem *gpt_reg_base,
					      u32 offset, u32 val)
{
	gpt_write_reg(gpt_reg_base, offset, val);
}

int mtk_hdl_gpt_set_compare(void __iomem *gpt_reg_base,
			    u8 timer_id,
			    u32 cmp_val)
{
	if (timer_id <= 1) {
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPTx_ICNT(timer_id),
					   cmp_val);
		return 0;
	} else if (timer_id == 3) {
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT3_EXPIRE, cmp_val);
		return 0;
	}

	return -GPT_EACCES;
}

int mtk_hdl_gpt_config_mode(void __iomem *gpt_reg_base,
			    u8 timer_id,
			    u8 auto_repeat)
{
	u32 val;

	if (timer_id <= 1) {
		val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base,
						GPT_CTRL_LIST[timer_id]);
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base,
					   GPT_CTRL_LIST[timer_id],
					   (auto_repeat) ?
					   (val | GPT_MODE_REPEAT) :
					   (val & (~GPT_MODE_REPEAT)));
		return 0;
	} else if (timer_id == 3) {
		if (!auto_repeat)
			return 0;
	}

	return -GPT_EACCES;
}

int mtk_hdl_gpt_enable_irq(void __iomem *gpt_reg_base,
			   u8 timer_id)
{
	u32 val;

	if (timer_id <= 1) {
		val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base, GPT_IER);
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT_IER,
					   (val | GPTx_INT_MASK(timer_id)));
		return 0;
	}

	return -GPT_EACCES;
}

int mtk_hdl_gpt_disable_irq(void __iomem *gpt_reg_base,
			    u8 timer_id)
{
	u32 val;

	/* disable & clear int */
	if (timer_id <= 1) {
		val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base, GPT_IER);
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT_IER,
					   (val & (~GPTx_INT_MASK(timer_id))));
		return 0;
	}

	return -GPT_EACCES;
}

int mtk_hdl_gpt_get_irq_status(void __iomem *gpt_reg_base,
			       u8 timer_id,
			       unsigned int *int_sta)
{
	u32 gpt_status;

	if (timer_id <= 1) {
		gpt_status = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base, GPT_ISR);
		if (gpt_status & GPTx_INT_MASK(timer_id))
			*int_sta = 1;
		else
			*int_sta = 0;

		return 0;
	}

	*int_sta = 0;

	return -GPT_EACCES;
}

int mtk_hdl_gpt_clear_irq_status(void __iomem *gpt_reg_base,
				 u8 timer_id)
{
	u32 gpt_status;

	if (timer_id <= 1) {
		gpt_status = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base, GPT_ISR);
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT_ISR,
					gpt_status & GPTx_INT_MASK(timer_id));

	} else if (timer_id == 3) {
		/* non reg to check gpt3 triggers interrupt */
		gpt_status = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base, GPT3_CTRL);
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT3_CTRL,
					   gpt_status | GPT3_ICLR);
		osai_delay_us(5);
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT3_CTRL,
					   gpt_status & (~GPT3_ICLR));
	} else
		return -GPT_EACCES;

	return 0;
}

void mtk_hdl_gpt_start(void __iomem *gpt_reg_base,
		       u8 timer_id)
{
	u32 val;

	val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base, GPT_CTRL_LIST[timer_id]);
	_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT_CTRL_LIST[timer_id],
				   (val | GPT_EN));
}

void mtk_hdl_gpt_stop(void __iomem *gpt_reg_base,
		      u8 timer_id)
{
	u32 val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base,
					    GPT_CTRL_LIST[timer_id]);

	_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT_CTRL_LIST[timer_id],
				   (val & (~GPT_EN)));
}

void mtk_hdl_gpt_restart_count(void __iomem *gpt_reg_base,
			       u8 timer_id)
{
	u32 val;

	val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base, GPT_CTRL_LIST[timer_id]);
	if (timer_id <= 1) {
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base,
					   GPT_CTRL_LIST[timer_id],
					   (val | GPT_RESTART));

	} else {
		if (val & GPT_EN) {
			_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base,
						   GPT_CTRL_LIST[timer_id],
						   (val & (~GPT_EN)));
			osai_delay_us(2);
			_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base,
						   GPT_CTRL_LIST[timer_id],
						   val);
		}
	}
}

u32 mtk_hdl_gpt_get_count(void __iomem *gpt_reg_base,
			  u8 timer_id)
{
	return _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base, GPT_CNT_LIST[timer_id]);
}

void mtk_hdl_gpt_config_clk(void __iomem *gpt_reg_base,
			    u8 timer_id,
			    u8 clk_src)
{
	u32 val;
	u32 osc_cnt_1us;

	if (clk_src > 1)
		clk_src = 1;

	if (timer_id <= 1) {
		val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base,
						GPT_CTRL_LIST[timer_id]) &
						(~GPT_SPEED(1));
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base,
					   GPT_CTRL_LIST[timer_id],
					   (val | GPT_SPEED(clk_src)));

	} else if (timer_id == 2) {
		val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base,
						GPT_CTRL_LIST[timer_id]) &
						(~GPT2_SPEED(1));
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base,
					   GPT_CTRL_LIST[timer_id],
					   (val | GPT2_SPEED(clk_src)));

	} else if (timer_id == 3) {
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT3_INIT, 0);

		val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base,
						GPT_CTRL_LIST[timer_id]) &
						(~GPT3_OSC_CNT_MASK);

		osc_cnt_1us = (GPT3_CLKSRC_26M / 1000000) - 1;

		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base,
				GPT_CTRL_LIST[timer_id],
				(val | GPT3_OSC_CNT_1US(osc_cnt_1us)));

	} else if (timer_id == 4) {
		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base, GPT4_INIT, 0);
		val = _mtk_hdl_gpt_cm4_reg_read(gpt_reg_base,
						GPT_CTRL_LIST[timer_id]) &
						(~GPT4_SPEED(1));

		_mtk_hdl_gpt_cm4_reg_write(gpt_reg_base,
					   GPT_CTRL_LIST[timer_id],
					   (val | GPT4_SPEED(clk_src)));
	}
}
