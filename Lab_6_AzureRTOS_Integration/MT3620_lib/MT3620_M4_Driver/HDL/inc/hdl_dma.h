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

#ifndef __HDL_DMA_MASTER__H__
#define __HDL_DMA_MASTER__H__

#include "mhal_osai.h"

/**
 * @addtogroup HDL
 * @{
 * @addtogroup DMA
 * @{
 * This section describes hardware layer apis of DMA.
 */

/**
 * @defgroup driver_macro_define
 * @{
 * This section is macro define.
 */

/**
 * @brief DMA debug help Macro.
 */
#define MTK_DMA_DEBUG_ENABLE 0
#define dma_err(fmt, arg...)	osai_print(fmt, ##arg)
#if MTK_DMA_DEBUG_ENABLE
#define dma_debug(fmt, arg...)	osai_print(fmt, ##arg)
#else
#define dma_debug(fmt, arg...)
#endif

#define DMA_CHANNEL_AMOUNT 30

/**
 * @brief DMA Global Registers.
 */
#define DMA_GLB_REG_OFFSET (0x4000)
#define DMA_GLBSTA0(dma_base) ((dma_base) + 0x0000 + DMA_GLB_REG_OFFSET)
#define DMA_GLBSTA1(dma_base) ((dma_base) + 0x0004 + DMA_GLB_REG_OFFSET)
#define DMA_CH_RST(dma_base) ((dma_base) + 0x0008 + DMA_GLB_REG_OFFSET)
#define DMA_GLO_CON(dma_base) ((dma_base) + 0x000C + DMA_GLB_REG_OFFSET)

#define DMA_CH_EN_STA(dma_base) ((dma_base) + 0x0020 + DMA_GLB_REG_OFFSET)
#define DMA_CH_EN_SET(dma_base) ((dma_base) + 0x0024 + DMA_GLB_REG_OFFSET)
#define DMA_CH_EN_CLR(dma_base) ((dma_base) + 0x0028 + DMA_GLB_REG_OFFSET)

#define DMA_GLBPAUSE(dma_base) ((dma_base) + 0x0030 + DMA_GLB_REG_OFFSET)
#define DMA_GLBSTAPAUSE(dma_base) ((dma_base) + 0x0034 + DMA_GLB_REG_OFFSET)
#define DMA_SET_DREQ(dma_base) ((dma_base) + 0x0050 + DMA_GLB_REG_OFFSET)
#define DMA_CLR_DREQ(dma_base) ((dma_base) + 0x0058 + DMA_GLB_REG_OFFSET)

/**
 * @brief DMA Channel Registers.
 */
#define DMA_CHN_REG_OFFSET 0x0100
#define DMA_GET_CHN_BASE(dma_base, chn)                                        \
	((dma_base) + (DMA_CHN_REG_OFFSET * (chn)))
#define DMA_SRC(chn_base) ((chn_base) + 0x0000)
#define DMA_DST(chn_base) ((chn_base) + 0x0004)
#define DMA_WPPT(chn_base) ((chn_base) + 0x0008)
#define DMA_WPTO(chn_base) ((chn_base) + 0x000C)
#define DMA_COUNT(chn_base) ((chn_base) + 0x0010)
#define DMA_CON(chn_base) ((chn_base) + 0x0014)
#define DMA_START(chn_base) ((chn_base) + 0x0018)
#define DMA_INTSTA(chn_base) ((chn_base) + 0x001C)
#define DMA_ACKINT(chn_base) ((chn_base) + 0x0020)
#define DMA_RLCT(chn_base) ((chn_base) + 0x0024)
#define DMA_LIMITER(chn_base) ((chn_base) + 0x0028)
#define DMA_PGMADDR(chn_base) ((chn_base) + 0x002C)
#define DMA_FFCNT(chn_base) ((chn_base) + 0x0038)
#define DMA_FFSTA(chn_base) ((chn_base) + 0x003C)
#define DMA_ALTLEN(chn_base) ((chn_base) + 0x0040)
#define DMA_FFSIZE(chn_base) ((chn_base) + 0x0044)
#define DMA_TO(chn_base) ((chn_base) + 0x0050)
#define DMA_HWPTR(chn_base) ((chn_base) + 0x0058)
#define DMA_SWPTR(chn_base) ((chn_base) + 0x005C)
#define DMA_FIXADDR(chn_base) ((chn_base) + 0x0060)

/**
 * @brief DMA Registers Mask.
 */

#define DMA_GLBSTA_RUN(n)	(0x00000001 << (((n)%16) << 1))
#define DMA_GLBSTA_IT(n)	(0x00000002 << (((n)%16) << 1))
#define DMA_SW_RESET   (0x00000001)

/**
 * @brief DMA_WPPT 0x0008
 */
#define DMA_WPPT_MSK 0x0000FFFF

/**
 * DMA_COUNT 0x0010
 */
/** reload len flag(only for halfsize dma)*/
#define DMA_COUNT_RLLEN_BIT 0x00010000
#define DMA_COUNT_LEN_MSK 0x0000FFFF

/**
 * @brief DMA_CON 0x0014
 */
#define DMA_CON_SIZE_MASK 0x00000003
#define DMA_CON_SIZE_BYTE 0x00000000
#define DMA_CON_SIZE_SHORT 0x00000001
#define DMA_CON_SIZE_LONG 0x00000002
#define DMA_CON_SINC 0x00000004
#define DMA_CON_DINC 0x00000008
#define DMA_CON_DRQ 0x00000010
#define DMA_CON_B2W 0x00000020
#define DMA_CON_BURST_SINGLE 0x00000000
#define DMA_CON_BURST_4BEAT 0x00000200
#define DMA_CON_BURST_8BEAT 0x00000400
#define DMA_CON_BURST_16BEAT 0x00000600
#define DMA_CON_HITEN 0x00002000
#define DMA_CON_TOEN 0x00004000
#define DMA_CON_ITEN 0x00008000
#define DMA_CON_WPSD 0x00010000
#define DMA_CON_WPEN 0x00020000
#define DMA_CON_DIR 0x00040000

/**
 * @brief DMA_START 0x0018
 */
#define DMA_START_BIT 0x00008000
#define DMA_STOP_BIT 0x00000000

/**
 * @brief DMA_INTSTA 0x001C
 */
/** half-completion interrupt status*/
#define DMA_HINTSTA_BIT 0x00002000
/** total completion or FIFO data underflow / overflow
 * interrupt status
 */
#define DMA_INTSTA_BIT 0x00008000
/** fifo timeout interrupt status */
#define DMA_TOINTSTA_BIT 0x00010000

/**
 * @brief DMA_ACKINT 0x0020
 * refer to DMA_INTSTA
 */
#define DMA_ACKHINT_BIT 0x00002000
#define DMA_ACKINT_BIT 0x00008000
#define DMA_ACKTOINT_BIT 0x00010000

/**
 * @brief DMA_RLCT 0x0024
 */
#define DMA_RLCT_MASK 0x0000FFFF

/**
 * @brief DMA_LIMITER 0x0028
 */
#define DMA_LIMITER_MASK 0x000000FF

/**
 * @brief DMA_FFCNT 0x0038
 */
#define DMA_FFCNT_MSK 0x0000FFFF

/**
 * @brief DMA_FFSTA 0x003C
 */
#define DMA_FFSTA_FULL 0x0001
#define DMA_FFSTA_EMPTY 0x0002
#define DMA_FFSTA_ALERT 0x0004

/**
 * @brief DMA_ALTLEN 0x0040
 */
#define DMA_ALTLEN_ALTSCM 0x80000000
#define DMA_ALTLEN_MSK 0x0000FFFF

/**
 * @brief DMA_FFSIZE 0x0044
 */
#define DMA_FFSIZE_MSK 0x0000FFFF

/**
 * @brief DMA_HWPTR 0x0058
 */
#define DMA_HWPTR_WARP 0x00010000
#define DMA_HWPTR_MSK 0x0000FFFF

/**
 * @brief DMA_SWPTR 0x005C
 */
#define DMA_SWPTR_WARP 0x00010000
#define DMA_SWPTR_MSK 0x0000FFFF

/**
 * @defgroup driver_function_declaration
 * @{
 * This section is function declaration.
 */

#ifdef __cplusplus
extern "C" {
#endif

void mtk_hdl_dma_set_src(void __iomem *chn_base, u32 src_addr);
void mtk_hdl_dma_set_dst(void __iomem *chn_base, u32 dst_addr);
void mtk_hdl_dma_set_wppt(void __iomem *chn_base, u32 wppt, u8 transize);
void mtk_hdl_dma_set_wpto(void __iomem *chn_base, u32 wpto);
int mtk_hdl_dma_set_count(void __iomem *chn_base, u32 count, u8 transize);
void mtk_hdl_dma_set_con(void __iomem *chn_base, u32 control_settings);
u32 mtk_hdl_dma_get_remain_cnt(void __iomem *chn_base, u8 transize);
void mtk_hdl_dma_set_bw_limiter(void __iomem *chn_base, u32 bandwidth_limiter);
void mtk_hdl_dma_set_pgmaddr(void __iomem *chn_base, u32 prog_addr);
u32 mtk_hdl_dma_get_pgmaddr(void __iomem *chn_base);
int mtk_hdl_dma_set_altlen(void __iomem *chn_base, u8 altscm, u32 altlen);
int mtk_hdl_dma_set_ffsize(void __iomem *chn_base, u32 ffsize, u8 transize);
void mtk_hdl_dma_set_timeout_thr(void __iomem *chn_base, u32 ff_timeout);
u32 mtk_hdl_dma_get_hwptr(void __iomem *chn_base);
void mtk_hdl_dma_set_swptr(void __iomem *chn_base, u32 swptr);
u32 mtk_hdl_dma_get_swptr(void __iomem *chn_base);
void mtk_hdl_dma_set_fixaddr(void __iomem *chn_base, u32 fix_addr);
u32 mtk_hdl_dma_get_fixaddr(void __iomem *chn_base);
u32 mtk_hdl_dma_get_vfifo_ffcnt(void __iomem *chn_base, u8 transize);
u32 mtk_hdl_dma_get_vfifo_ffsize(void __iomem *chn_base, u8 transize);
void mtk_hdl_dma_enable_reload(void __iomem *chn_base);
void mtk_hdl_dma_disable_reload(void __iomem *chn_base);
void mtk_hdl_dma_start(void __iomem *chn_base);
void mtk_hdl_dma_stop_now(void __iomem *chn_base);
void mtk_hdl_dma_ack_int(void __iomem *chn_base);
void mtk_hdl_dma_ack_half_int(void __iomem *chn_base);
void mtk_hdl_dma_ack_timeout_int(void __iomem *chn_base);
u8 mtk_hdl_dma_chk_run_status(void __iomem *dma_base, u8 chn);
u8 mtk_hdl_dma_chk_int_status(void __iomem *dma_base, u8 chn);
u8 mtk_hdl_dma_chk_int(void __iomem *chn_base);
u8 mtk_hdl_dma_chk_half_int(void __iomem *chn_base);
u8 mtk_hdl_dma_chk_timeout_int(void __iomem *chn_base);
void mtk_hdl_dma_dump_chn_reg(void __iomem *chn_base);
void mtk_hdl_dma_dump_global_reg(void __iomem *dma_base);
u32 mtk_hdl_dma_chk_pause_status(void __iomem *dma_base, u8 chn);
void mtk_hdl_dma_pause(void __iomem *dma_base, u8 chn);
void mtk_hdl_dma_resume(void __iomem *dma_base, u8 chn);
void mtk_hdl_dma_reset_chn(void __iomem *dma_base, u8 chn);
void mtk_hdl_dma_set_dreq(void __iomem *dma_base, u8 chn);
void mtk_hdl_dma_clr_dreq(void __iomem *dma_base, u8 chn);
void mtk_hdl_dma_clock_enable(void __iomem *dma_base, u8 chn);
void mtk_hdl_dma_clock_disable(void __iomem *dma_base, u8 chn);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

/**
 * @}
 * @}
 */

#endif
