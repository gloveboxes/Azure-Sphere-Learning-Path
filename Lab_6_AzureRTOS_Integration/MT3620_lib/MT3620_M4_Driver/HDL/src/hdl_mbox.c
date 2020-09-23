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

#include "hdl_mbox.h"


/*
 * Register Map
 */
#define MBOX_REG_VER					(0x00)
#define MBOX_GEN_CTRL					(0x04)
#define MBOX_SOFT_RST					(0x1 << 0)
#define MBOX_SOFT_RST_MYSELF				(0x1 << 1)

#define MBOX_SW_INT_PORT				(0x14)
#define MBOX_SW_INT_EN					(0x18)
#define MBOX_SW_INT_STS					(0x1C)

#define MBOX_NF_THRS_OFFSET				(0x30)
#define MBOX_NE_THRS_OFFSET				(0x34)

#define MBOX_INT_EN_OFFSET				(0x38)
#define MBOX_INT_STS_OFFSET				(0x3C)

#define MBOX_CMD_POST_OFFSET				(0x40)
#define MBOX_DATA_POST_OFFSET				(0x44)
#define MBOX_POST_CNT_OFFSET				(0x48)

#define MBOX_CMD_ACPT_OFFSET				(0x50)
#define MBOX_DATA_ACPT_OFFSET				(0x54)
#define MBOX_ACPT_CNT_OFFSET				(0x58)

#define MBOX_SEM_PORT_OFFSET				(0x60)

/*
 * Semaphore status, only used between CA7 and CM4
 */
enum mbox_sem_status {
	MBOX_SEM_UNLOCK,
	MBOX_SEM_CM4_LOCK,
	MBOX_SEM_CA7_LOCK,
	MBOX_SEM_UNDEF,
};

/*
 * Hardware Raw Operations
 */
static inline u32 _mtk_hdl_mbox_readl(void __iomem *base, u32 reg_offset)
{
	return osai_readl((void *)(base + reg_offset));
}

static inline void _mtk_hdl_mbox_writel(void __iomem *base,
			u32 value, u32 reg_offset)
{
	osai_writel(value, (void *)(base + reg_offset));
}

/*
 * Semaphore Operations
 */
void mtk_hdl_mbox_sem_post(void __iomem *base)
{
	_mtk_hdl_mbox_writel(base, MBOX_SEM_UNLOCK, MBOX_SEM_PORT_OFFSET);
}

void mtk_hdl_mbox_sem_pend(void __iomem *base, u32 *value)
{
	*value = _mtk_hdl_mbox_readl(base, MBOX_SEM_PORT_OFFSET);
}

/*
 * Post/Acpt FIFO Operations
 */
void mtk_hdl_mbox_data_fifo_post(void __iomem *base, u32 data)
{
	_mtk_hdl_mbox_writel(base, data, MBOX_DATA_POST_OFFSET);
}

void mtk_hdl_mbox_cmd_fifo_post(void __iomem *base, u32 cmd)
{
	_mtk_hdl_mbox_writel(base, cmd, MBOX_CMD_POST_OFFSET);
}

void mtk_hdl_mbox_get_post_fifo_cnt(void __iomem *base, u32 *value)
{
	*value = _mtk_hdl_mbox_readl(base, MBOX_POST_CNT_OFFSET);
}

void mtk_hdl_mbox_data_fifo_acpt(void __iomem *base, u32 *data)
{
	*data = _mtk_hdl_mbox_readl(base, MBOX_DATA_ACPT_OFFSET);
}

void mtk_hdl_mbox_cmd_fifo_acpt(void __iomem *base, u32 *cmd)
{
	*cmd = _mtk_hdl_mbox_readl(base, MBOX_CMD_ACPT_OFFSET);
}

void mtk_hdl_mbox_get_acpt_fifo_cnt(void __iomem *base, u32 *value)
{
	*value = _mtk_hdl_mbox_readl(base, MBOX_ACPT_CNT_OFFSET);
}

/*
 * NF/NE Threshold Operations
 */
void mtk_hdl_mbox_get_nf_thrs(void __iomem *base, u32 *value)
{
	*value = _mtk_hdl_mbox_readl(base, MBOX_NF_THRS_OFFSET);
}

void mtk_hdl_mbox_set_nf_thrs(void __iomem *base, u32 value)
{
	_mtk_hdl_mbox_writel(base, (value & MBOX_THRS_MASK),
		MBOX_NF_THRS_OFFSET);
}

void mtk_hdl_mbox_get_ne_thrs(void __iomem *base, u32 *value)
{
	*value = _mtk_hdl_mbox_readl(base, MBOX_NE_THRS_OFFSET);
}

void mtk_hdl_mbox_set_ne_thrs(void __iomem *base, u32 value)
{
	_mtk_hdl_mbox_writel(base, (value & MBOX_THRS_MASK),
		MBOX_NE_THRS_OFFSET);
}

/*
 * SW Interrupt Operations
 */
void mtk_hdl_mbox_trig_sw_int(void __iomem *base, u32 id)
{
	u32 value;

	value = (0x1 << id);

	_mtk_hdl_mbox_writel(base, value, MBOX_SW_INT_PORT);
}

void mtk_hdl_mbox_sw_int_en(void __iomem *base, u32 value)
{
	_mtk_hdl_mbox_writel(base, value, MBOX_SW_INT_EN);
}

void mtk_hdl_mbox_get_sw_int_en(void __iomem *base, u32 *value)
{
	*value = _mtk_hdl_mbox_readl(base, MBOX_SW_INT_EN);
}

void mtk_hdl_mbox_sw_int_ctrl(void __iomem *base, int enable, u32 id)
{
	u32 value;

	value = _mtk_hdl_mbox_readl(base, MBOX_SW_INT_EN);

	if (enable)
		value |= (0x1 << id);
	else
		value &= ~(0x1 << id);

	_mtk_hdl_mbox_writel(base, value, MBOX_SW_INT_EN);
}

void mtk_hdl_mbox_get_sw_int_sts(void __iomem *base, u32 *value)
{
	*value = _mtk_hdl_mbox_readl(base, MBOX_SW_INT_STS);
}

void mtk_hdl_mbox_clear_sw_int_sts(void __iomem *base, u32 mask)
{
	_mtk_hdl_mbox_writel(base, mask, MBOX_SW_INT_STS);
}

/*
 * Interrupt Operations
 */
void mtk_hdl_mbox_int_en(void __iomem *base, u32 enable, u32 mask)
{
	u32 value;

	value = _mtk_hdl_mbox_readl(base, MBOX_INT_EN_OFFSET);
	if (enable)
		value |= mask;
	else
		value &= ~mask;
	_mtk_hdl_mbox_writel(base, value, MBOX_INT_EN_OFFSET);
}

void mtk_hdl_mbox_get_int_en(void __iomem *base, u32 *value)
{
	*value = _mtk_hdl_mbox_readl(base, MBOX_INT_EN_OFFSET);
}

void mtk_hdl_mbox_get_int_sts(void __iomem *base, u32 *value)
{
	*value = _mtk_hdl_mbox_readl(base, MBOX_INT_STS_OFFSET);
}

void mtk_hdl_mbox_clear_int_sts(void __iomem *base, u32 mask)
{
	_mtk_hdl_mbox_writel(base, mask, MBOX_INT_STS_OFFSET);
}

/*
 * Driver Interface
 */


void mtk_hdl_mbox_reset(void __iomem *base)
{
	_mtk_hdl_mbox_writel(base, MBOX_SOFT_RST_MYSELF, MBOX_GEN_CTRL);
}

