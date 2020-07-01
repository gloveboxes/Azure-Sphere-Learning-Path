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

#ifndef __MT3620_CONF_H__
#define __MT3620_CONF_H__


#define OSC_FREQUENCY		40000000
#if (CFG_FPGA == 0)
#define CPU_FREQUENCY		40000000	/* HCLK : CPU = 1: 1*/
#else
#define CPU_FREQUENCY		8000000
#endif
#define __NVIC_PRIO_BITS	3	/*!< uses 4 Bits for the Priority Levels    */
#define __FPU_PRESENT             1       /*!< FPU present */


#ifdef M4SC_ENABLE
#define CM4_IRQ_MAX     132
#else
#define CM4_IRQ_MAX     77
#endif

typedef enum IRQn
{
	/****  CM4 internal exceptions  **********/
	Reset_IRQn                  = -15,	/* Reset */
	NonMaskableInt_IRQn         = -14,	/* NMI */
	HardFault_IRQn              = -13,	/* HarFault */
	MemoryManagement_IRQn       = -12,	/* Memory Management */
	BusFault_IRQn               = -11,	/* Bus Fault  */
	UsageFault_IRQn             = -10,	/* Usage Fault */
	SVC_IRQn                    = -5,	/* SV Call*/
	DebugMonitor_IRQn           = -4,	/* Debug Monitor */
	PendSV_IRQn                 = -2,	/* Pend SV */
	SysTick_IRQn                = -1,	/* System Tick */

	/****  MT3620 specific external/peripheral interrupt ****/
	PERI0_IRQn                  = 0,
	PERI1_IRQn                  = 1,
    IRQ_MAX                     = CM4_IRQ_MAX,
} IRQn_Type;

#include "core_cm4.h"                  /* Core Peripheral Access Layer */

/* move to feature_xx.mk
#if defined(M4SC_ENABLE)
#define NVIC_RAM_VECTOR_ADDRESS   (0x20000000)  // Location of vectors in RAM
#define NVIC_FLASH_VECTOR_ADDRESS (0x90000000)  // Initial vector position in flash
#elif defined(M4IO1_ENABLE)
#define NVIC_RAM_VECTOR_ADDRESS   (0x80100000)  // Location of vectors in RAM
#define NVIC_FLASH_VECTOR_ADDRESS (0x90100000)  // Initial vector position in flash
#elif defined(M4IO2_ENABLE)
#define NVIC_RAM_VECTOR_ADDRESS   (0x80200000)  // Location of vectors in RAM
#define NVIC_FLASH_VECTOR_ADDRESS (0x90200000)  // Initial vector position in flash
#else
#define NVIC_RAM_VECTOR_ADDRESS   (0x20000000)  // Location of vectors in RAM
#define NVIC_FLASH_VECTOR_ADDRESS (0x10000000)  // Initial vector position in flash
#endif
*/

#include "irq.h"                  /*include irq number define header file */


/* IRQ Priority */
#define DEFAULT_PRI             5
#define HIGHEST_PRI             1
#define CM4_UART1_PRI			DEFAULT_PRI
#define CM4_DMA_PRI			    DEFAULT_PRI
#define CM4_HIF_PRI			    DEFAULT_PRI
#define CM4_I2C1_PRI			DEFAULT_PRI
#define CM4_I2C2_PRI			DEFAULT_PRI
#define CM4_UART2_PRI			DEFAULT_PRI
#define CM4_MTK_CRYPTO_PRI		DEFAULT_PRI
#define CM4_SF_PRI			    DEFAULT_PRI
#define CM4_EINT_PRI			DEFAULT_PRI
#define CM4_BTIF_PRI			DEFAULT_PRI
#define CM4_WDT_PRI			    DEFAULT_PRI
#define CM4_AHB_MON_PRI			DEFAULT_PRI
#define CM4_SPI_SLAVE_PRI		DEFAULT_PRI
#define CM4_N9_WDT_PRI			DEFAULT_PRI
#define CM4_ADC_PRI			    DEFAULT_PRI
#define CM4_IRDA_TX_PRI			DEFAULT_PRI
#define CM4_IRDA_RX_PRI			DEFAULT_PRI
#define CM4_USB_VBUS_ON_PRI		DEFAULT_PRI
#define CM4_USB_VBUS_OFF_PRI	DEFAULT_PRI
#define CM4_SPIS_CS_WAKEUP_PRI	DEFAULT_PRI
#define CM4_GPT3_PRI			DEFAULT_PRI
#define CM4_PRI21_PRI			DEFAULT_PRI
#define CM4_AUDIO_PRI			DEFAULT_PRI
#define CM4_HIF_DMA_PRI			DEFAULT_PRI
#define CM4_GPT_PRI			    DEFAULT_PRI
#define CM4_ADC_COMP_PRI		DEFAULT_PRI
#define CM4_AUDIO_WAKE_PRI	    DEFAULT_PRI
#define CM4_SPIM_PRI			DEFAULT_PRI
#define CM4_USB_PRI			    DEFAULT_PRI
#define CM4_UDMA_PRI			DEFAULT_PRI
#define CM4_TRNG_PRI            DEFAULT_PRI

#define CM4_WIC_INT_WAKE_UP_PRI  HIGHEST_PRI
#define CM4_WIC_0_PRI            DEFAULT_PRI
#define CM4_WIC_1_PRI            DEFAULT_PRI
#define CM4_WIC_2_PRI            DEFAULT_PRI
#define CM4_WIC_3_PRI            DEFAULT_PRI
#define CM4_WIC_4_PRI            DEFAULT_PRI
#define CM4_WIC_5_PRI            DEFAULT_PRI
#define CM4_WIC_6_PRI            DEFAULT_PRI
#define CM4_WIC_7_PRI            DEFAULT_PRI
#define CM4_WIC_8_PRI            DEFAULT_PRI
#define CM4_WIC_9_PRI            DEFAULT_PRI
#define CM4_WIC_10_PRI           DEFAULT_PRI
#define CM4_WIC_11_PRI           DEFAULT_PRI
#define CM4_WIC_12_PRI           DEFAULT_PRI
#define CM4_WIC_13_PRI           DEFAULT_PRI
#define CM4_WIC_14_PRI           DEFAULT_PRI
#define CM4_WIC_15_PRI           DEFAULT_PRI
#define CM4_WIC_16_PRI           DEFAULT_PRI
#define CM4_WIC_17_PRI           DEFAULT_PRI
#define CM4_WIC_18_PRI           DEFAULT_PRI
#define CM4_WIC_19_PRI           DEFAULT_PRI
#define CM4_WIC_20_PRI           DEFAULT_PRI
#define CM4_WIC_21_PRI           DEFAULT_PRI
#define CM4_WIC_22_PRI           DEFAULT_PRI
#define CM4_WIC_23_PRI           DEFAULT_PRI

#define CM4_EINT_0_PRI           DEFAULT_PRI
#define CM4_EINT_1_PRI           DEFAULT_PRI
#define CM4_EINT_2_PRI           DEFAULT_PRI
#define CM4_EINT_3_PRI           DEFAULT_PRI
#define CM4_EINT_4_PRI           DEFAULT_PRI
#define CM4_EINT_5_PRI           DEFAULT_PRI
#define CM4_EINT_6_PRI           DEFAULT_PRI
#define CM4_EINT_7_PRI           DEFAULT_PRI
#define CM4_EINT_8_PRI           DEFAULT_PRI
#define CM4_EINT_9_PRI           DEFAULT_PRI
#define CM4_EINT_10_PRI          DEFAULT_PRI
#define CM4_EINT_11_PRI          DEFAULT_PRI
#define CM4_EINT_12_PRI          DEFAULT_PRI
#define CM4_EINT_13_PRI          DEFAULT_PRI
#define CM4_EINT_14_PRI          DEFAULT_PRI
#define CM4_EINT_15_PRI          DEFAULT_PRI
#define CM4_EINT_16_PRI          DEFAULT_PRI
#define CM4_EINT_17_PRI          DEFAULT_PRI
#define CM4_EINT_18_PRI          DEFAULT_PRI
#define CM4_EINT_19_PRI          DEFAULT_PRI
#define CM4_EINT_20_PRI          DEFAULT_PRI
#define CM4_EINT_21_PRI          DEFAULT_PRI
#define CM4_EINT_22_PRI          DEFAULT_PRI
#define CM4_EINT_23_PRI          DEFAULT_PRI
#define CM4_EINT_24_PRI          DEFAULT_PRI
#define CM4_EINT_25_PRI          DEFAULT_PRI
#define CM4_EINT_26_PRI          DEFAULT_PRI
#define CM4_EINT_27_PRI          DEFAULT_PRI
#define CM4_EINT_28_PRI          DEFAULT_PRI
#define CM4_EINT_29_PRI          DEFAULT_PRI
#define CM4_EINT_30_PRI          DEFAULT_PRI
#define CM4_EINT_31_PRI          DEFAULT_PRI
#define CM4_EINT_32_PRI          DEFAULT_PRI
#define CM4_EINT_33_PRI          DEFAULT_PRI
#define CM4_EINT_34_PRI          DEFAULT_PRI
#define CM4_EINT_35_PRI          DEFAULT_PRI
#define CM4_EINT_36_PRI          DEFAULT_PRI
#define CM4_EINT_37_PRI          DEFAULT_PRI
#define CM4_EINT_38_PRI          DEFAULT_PRI
#define CM4_EINT_39_PRI          DEFAULT_PRI

#define CM4_RTC_ALARM_PRI        DEFAULT_PRI


#ifdef __cplusplus
extern "C" {
#endif
typedef void (*NVIC_IRQ_Handler)(void);
extern int NVIC_Register(int irqn, NVIC_IRQ_Handler handler);
extern int NVIC_UnRegister(int irqn);

#ifdef __cplusplus
}
#endif

#include "nvic.h"
#endif
