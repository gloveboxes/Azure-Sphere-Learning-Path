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


#ifndef __HDL_ADC_H__
#define __HDL_ADC_H__

#include "mhal_osai.h"

#define DISABLE_ADC_DEBUG

#define adc_err(fmt, arg...)\
	do {\
		osai_print("[%s]: ", __func__);\
		osai_print(fmt, ##arg);\
	} while (0)

#ifndef DISABLE_ADC_DEBUG
#define adc_debug(fmt, arg...)\
	do {\
		osai_print("[%s]: ", __func__);\
		osai_print(fmt, ##arg);\
	} while (0)
#else
#define adc_debug(fmt, arg...)
#endif

#define MASK(Field)         (Field##_MASK)
#define SHFT(Field)         (Field##_SHFT)

#define ADCADDR(field)		(field##_ADDR)
#define ADCMASK(field)		(field##_MASK)
#define ADCSHFT(field)		(field##_SHFT)

#define adc_get_reg(base, field) \
	((osai_readl(base + ADCADDR(field)) & (MASK(field))) >> SHFT(field))

#define adc_mod_reg(base, field, value) \
	{void  __iomem *addr = base + ADCADDR(field); \
	osai_writel(((((u32)(value) << ADCSHFT(field)) \
	& ADCMASK(field)) | (osai_readl(addr) & ~MASK(field))), addr); }

#define CH_BIT_MAP              0xFF
/**< 0xFF mean open all 8 adc channel*/
#define ADC_FIFO_SIZE   32
/**< ADC FIFO buffer size */
#define ADC_INIT_TIME	20
/**< ADC initial stable time value */
#define ADC_CH_STABLE_TIME	8
/**< ADC channel stable time value */

#define ADC_FSM_ENABLE		1
#define ADC_FSM_DISABLE		0
#define ADC_TIME_STAMP_ENABLE		1
#define ADC_TIME_STAMP_DISABLE		0
#define ADC_DATA_SYNC_NEGATIVE_MODE		1
#define ADC_DATA_SYNC_POSITIVE_MODE		0
#define ADC_COMP_PREAMP_ENABLE		1
#define ADC_COMP_PREAMP_DISABLE		0
#define ADC_DITHERING_ENABLE		1
#define ADC_DITHERING_DISABLE		0
#define ADC_INVERT_PMU_CLK_ENABLE		1
#define ADC_INVERT_PMU_CLK_DISABLE		0
#define ADC_VREF18_SUPPORT		1
#define ADC_VREF25_SUPPORT		1
#define ADC_CLK_SRC_CONTROLLER		1
#define ADC_CLK_SRC_BUCK		0

#define ADC_RX_DMA_ENABLE		1
#define ADC_RX_DMA_DISABLE		0
#define ADC_RX_FULL_ENABLE		1
#define ADC_RX_FULL_DISABLE		0
#define ADC_RX_TIME_OUT_ENABLE		1
#define ADC_RX_TIME_OUT_DISABLE		0

/*
 * ADC register defines
 */
#define ADC_CR_SW_RST_MASK                          BIT(0)

#define ADC_CTRL_OFFSET                             (0x100)

#define ADC_FSM_EN_ADDR                             (ADC_CTRL_OFFSET + 0x0000)
#define ADC_FSM_EN_MASK                             BIT(0)
#define ADC_FSM_EN_SHFT                             0

#define ADC_REG_AVG_MODE_ADDR                       (ADC_CTRL_OFFSET + 0x0000)
#define ADC_REG_AVG_MODE_MASK                       BITS(1, 3)
#define ADC_REG_AVG_MODE_SHFT                       1

#define ADC_REG_T_CH_ADDR                           (ADC_CTRL_OFFSET + 0x0000)
#define ADC_REG_T_CH_MASK                           BITS(4, 7)
#define ADC_REG_T_CH_SHFT                           4

#define ADC_PMODE_EN_ADDR                           (ADC_CTRL_OFFSET + 0x0000)
#define ADC_PMODE_EN_MASK                           BIT(8)
#define ADC_PMODE_EN_SHFT                           8

#define ADC_REG_T_INIT_ADDR                         (ADC_CTRL_OFFSET + 0x0000)
#define ADC_REG_T_INIT_MASK                         BITS(9, 15)
#define ADC_REG_T_INIT_SHFT                         9

#define ADC_REG_CH_MAP_ADDR                         (ADC_CTRL_OFFSET + 0x0000)
#define ADC_REG_CH_MAP_MASK                         BITS(16, 31)
#define ADC_REG_CH_MAP_SHFT                         16

#define ADC_REG_PERIOD_ADDR                         (ADC_CTRL_OFFSET + 0x0004)
#define ADC_REG_PERIOD_MASK                         BITS(0, 31)
#define ADC_REG_PERIOD_SHFT                         0

#define ADC_REG_ADC_DATA_SYNC_MODE_ADDR             (ADC_CTRL_OFFSET + 0x0008)
#define ADC_REG_ADC_DATA_SYNC_MODE_MASK             BIT(20)
#define ADC_REG_ADC_DATA_SYNC_MODE_SHFT             20

#define ADC_REG_ADC_TIMESTAMP_EN_ADDR               (ADC_CTRL_OFFSET + 0x0008)
#define ADC_REG_ADC_TIMESTAMP_EN_MASK               BIT(21)
#define ADC_REG_ADC_TIMESTAMP_EN_SHFT               21

#define ADC_REG_INTERNAL_CLK_SAMPLE_DATA_EN_ADDR    (ADC_CTRL_OFFSET + 0x0008)
#define ADC_REG_INTERNAL_CLK_SAMPLE_DATA_EN_MASK    BIT(22)
#define ADC_REG_INTERNAL_CLK_SAMPLE_DATA_EN_SHFT    22

#define ADC_REG_AUXADC_COMP_DELAY_TIME_ADDR         (ADC_CTRL_OFFSET + 0x000C)
#define ADC_REG_AUXADC_COMP_DELAY_TIME_MASK         BITS(0, 1)
#define ADC_REG_AUXADC_COMP_DELAY_TIME_SHFT         0

#define ADC_REG_AUXADC_COMP_PREAMP_CURRENT_ADDR     (ADC_CTRL_OFFSET + 0x000C)
#define ADC_REG_AUXADC_COMP_PREAMP_CURRENT_MASK     BITS(2, 3)
#define ADC_REG_AUXADC_COMP_PREAMP_CURRENT_SHFT     2

#define ADC_REG_AUXADC_COMP_PREAMP_EN_ADDR          (ADC_CTRL_OFFSET + 0x000C)
#define ADC_REG_AUXADC_COMP_PREAMP_EN_MASK          BIT(4)
#define ADC_REG_AUXADC_COMP_PREAMP_EN_SHFT          4

#define ADC_REG_AUXADC_DITHERING_EN_ADDR            (ADC_CTRL_OFFSET + 0x000C)
#define ADC_REG_AUXADC_DITHERING_EN_MASK            BIT(6)
#define ADC_REG_AUXADC_DITHERING_EN_SHFT            6

#define ADC_REG_AUXADC_DITHERING_STEPS_ADDR         (ADC_CTRL_OFFSET + 0x000C)
#define ADC_REG_AUXADC_DITHERING_STEPS_MASK         BITS(8, 9)
#define ADC_REG_AUXADC_DITHERING_STEPS_SHFT         8

#define ADC_REG_AUXADC_INVERT_PMU_CLK_ADDR          (ADC_CTRL_OFFSET + 0x000C)
#define ADC_REG_AUXADC_INVERT_PMU_CLK_MASK          BIT(16)
#define ADC_REG_AUXADC_INVERT_PMU_CLK_SHFT          16

#define ADC_REG_AUXADC_ADC_CLK_SRC_ADDR             (ADC_CTRL_OFFSET + 0x000C)
#define ADC_REG_AUXADC_ADC_CLK_SRC_MASK             BIT(17)
#define ADC_REG_AUXADC_ADC_CLK_SRC_SHFT             17

#define ADC_REG_AUXADC_ADC_VCM_VREF_ADDR             (ADC_CTRL_OFFSET + 0x000C)
#define ADC_REG_AUXADC_ADC_VCM_VREF_MASK             BIT(31)
#define ADC_REG_AUXADC_ADC_VCM_VREF_SHFT             31

#define ADC_FIFO_OFFSET                             0x200
#define ADC_RBR_ADDR                                (ADC_FIFO_OFFSET + 0x0000)
#define ADC_RBR_MASK                                BITS(0, 31)
#define ADC_RBR_SHFT                                0

#define ADC_RXFEN_ADDR                              (ADC_FIFO_OFFSET + 0x0004)
#define ADC_RXFEN_MASK                              BIT(0)
#define ADC_RXFEN_SHFT                              0

#define ADC_RXTIMEFEN_ADDR                          (ADC_FIFO_OFFSET + 0x0004)
#define ADC_RXTIMEFEN_MASK                          BIT(2)
#define ADC_RXTIMEFEN_SHFT                          2

#define ADC_LSR_DR_ADDR                             (ADC_FIFO_OFFSET + 0x0014)
#define ADC_LSR_DR_MASK                             BIT(0)
#define ADC_LSR_DR_SHFT                             0

#define ADC_RX_DMA_EN_ADDR                          (ADC_FIFO_OFFSET + 0x004C)
#define ADC_RX_DMA_EN_MASK                          BIT(0)
#define ADC_RX_DMA_EN_SHFT                          0

#define ADC_TO_CNT_AUTORST_ADDR                     (ADC_FIFO_OFFSET + 0x004C)
#define ADC_TO_CNT_AUTORST_MASK                     BIT(2)
#define ADC_TO_CNT_AUTORST_SHFT                     2

#define ADC_RTOCNT_ADDR                             (ADC_FIFO_OFFSET + 0x0054)
#define ADC_RTOCNT_MASK                             BITS(0, 17)
#define ADC_RTOCNT_SHFT                             0

#define ADC_RX_TRI_LVL_ADDR                         (ADC_FIFO_OFFSET + 0x0060)
#define ADC_RX_TRI_LVL_MASK                         BITS(2, 6)
#define ADC_RX_TRI_LVL_SHFT                         2

#define ADC_RX_PTR_ADDR                             (ADC_FIFO_OFFSET + 0x00D4)
#define ADC_RX_PTR_MASK                             BITS(0, 9)
#define ADC_RX_PTR_SHFT                             0

#define ADC_RX_PTR_READ_ADDR                        (ADC_FIFO_OFFSET + 0x00D4)
#define ADC_RX_PTR_READ_MASK                        BITS(0, 4)
#define ADC_RX_PTR_READ_SHFT                        0
#define ADC_RX_PTR_WRITE_ADDR                       (ADC_FIFO_OFFSET + 0x00D4)
#define ADC_RX_PTR_WRITE_MASK                       BITS(5, 9)
#define ADC_RX_PTR_WRITE_SHFT                       5

/** @brief This enum defines adc dither funciton steps*/
typedef enum {
	ADC_DITHER_2STEPS = 0,
	ADC_DITHER_4STEPS,
	ADC_DITHER_8STEPS,
	ADC_DITHER_16STEPS
} adc_dither_steps;

/** @brief This enum defines adc comparator pre-amplifier current*/
typedef enum {
	ADC_CURRENT_40UA = 0,
	ADC_CURRENT_80UA,
	ADC_CURRENT_160UA,
	ADC_CURRENT_160MAX
} adc_comparator_current;

/** @brief This enum defines adc comparator timeing loop delay*/
typedef enum {
ADC_DELAY_3NS = 0,
	ADC_DELAY_6NS,
	ADC_DELAY_9NS,
	ADC_DELAY_12NS
} adc_comparator_delay;

#ifdef __cplusplus
extern "C" {
#endif

void mtk_hal_adc_init(void __iomem *base);

void mtk_hal_adc_uninit(void __iomem *base);

void mtk_hdl_adc_enable_clk(void __iomem *cg_base);

void mtk_hdl_adc_disable_clk(void __iomem *cg_base);

void mtk_hdl_adc_fsm_param_set(void __iomem *base,
	u16 pmode, u16 avg_mode, u16 channel_map, u32 period);

void mtk_hdl_adc_fsm_param_get(void __iomem *base,
	u16 *pmode, u16 *avg_mode, u16 *channel_map, u32 *period);

void mtk_hdl_adc_start(void __iomem *base);

void mtk_hdl_adc_stop(void __iomem *base);

void mtk_hdl_adc_reset(void __iomem *base);

void mtk_hdl_adc_dma_enable(void __iomem *base);

void mtk_hdl_adc_fifo_rx_full_enable(void __iomem *base);


void mtk_hdl_adc_trigger_level_set(void __iomem *base,
		u32 trigger_level);

void mtk_hdl_adc_periodic_mode_set(void __iomem *base,
		u32 periodic_mode);

void mtk_hdl_adc_fifo_get_length(void __iomem *base,
		u32 *length);

int mtk_hdl_adc_fifo_read_data(void __iomem *base);

#ifdef __cplusplus
}
#endif

#endif /* __HDL_ADC_H__ */

