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

#ifndef __NVIC_H__
#define __NVIC_H__


#include "mt3620.h"
#include "mt3620_cm4_hw_memmap.h"   /* usb_merge  */


#define CA7_DBNC_CON_BASE           CA7_IRQ_BASE

#define CA7_IRQ_SENS0               (CA7_IRQ_BASE + 0x200)
#define CA7_IRQ_SENS1               (CA7_IRQ_BASE + 0x204)
#define CA7_IRQ_SENS2               (CA7_IRQ_BASE + 0x208)
#define CA7_IRQ_SENS3               (CA7_IRQ_BASE + 0x20C)

#define CA7_IRQ_EDGE2LVL0           (CA7_IRQ_BASE + 0x240)
#define CA7_IRQ_EDGE2LVL1           (CA7_IRQ_BASE + 0x244)
#define CA7_IRQ_EDGE2LVL2           (CA7_IRQ_BASE + 0x248)
#define CA7_IRQ_EDGE2LVL3           (CA7_IRQ_BASE + 0x24C)

#define CA7_IRQ_STS0                (CA7_IRQ_BASE + 0x280)
#define CA7_IRQ_STS1                (CA7_IRQ_BASE + 0x284)
#define CA7_IRQ_STS2                (CA7_IRQ_BASE + 0x288)
#define CA7_IRQ_STS3                (CA7_IRQ_BASE + 0x28C)

#define CA7_CFG_IRQ_DIS0            (CA7_IRQ_BASE + 0x600)
#define CA7_CFG_IRQ_DIS1            (CA7_IRQ_BASE + 0x604)
#define CA7_CFG_IRQ_DIS2            (CA7_IRQ_BASE + 0x608)
#define CA7_CFG_IRQ_DIS3            (CA7_IRQ_BASE + 0x60C)

#define CA7_CFG_IRQ_TEST0           (CA7_IRQ_BASE + 0x700)
#define CA7_CFG_IRQ_TEST1           (CA7_IRQ_BASE + 0x704)
#define CA7_CFG_IRQ_TEST2           (CA7_IRQ_BASE + 0x708)
#define CA7_CFG_IRQ_TEST3           (CA7_IRQ_BASE + 0x70C)

/*	#define CA7_IRQ_DIS_BASE            (CA7_IRQ_BASE + 0x600)*/
/*	#define CA7_IRQ_TEST_BASE           (CA7_IRQ_BASE + 0x700)*/
#define CA7_IRQ_TEST_CTL            (CA7_IRQ_BASE + 0x720)
#define CA7_IRQ_TEST_CTL_ENABLE     (1 << 0)



#define CM4_DBNC_CON_BASE           CM4_IRQ_CFG_BASE
#define CM4_IRQ_SENS0               (CM4_IRQ_CFG_BASE + 0x200)
#define CM4_IRQ_SENS1               (CM4_IRQ_CFG_BASE + 0x204)
#define CM4_IRQ_SENS2               (CM4_IRQ_CFG_BASE + 0x208)
#define CM4_IRQ_SENS3               (CM4_IRQ_CFG_BASE + 0x20C)
#define CM4_IRQ_SENS4               (CM4_IRQ_CFG_BASE + 0x210)

/*	WIC relative control register*/
#define CM4_WIC_SWPEND_CR0          (CM4_IRQ_CFG_BASE + 0x304)
#define CM4_WIC_SWPEND_CR1          (CM4_IRQ_CFG_BASE + 0x308)
#define CM4_WIC_SWPEND_CR2          (CM4_IRQ_CFG_BASE + 0x30C)
#define CM4_WIC_SWPEND_CR3          (CM4_IRQ_CFG_BASE + 0x310)
#define CM4_WIC_SWPEND_CR4          (CM4_IRQ_CFG_BASE + 0x314)

#define CM4_WIC_SWEN_CR0            (CM4_IRQ_CFG_BASE + 0x324)
#define CM4_WIC_SWEN_CR1            (CM4_IRQ_CFG_BASE + 0x328)
#define CM4_WIC_SWEN_CR2            (CM4_IRQ_CFG_BASE + 0x32C)
#define CM4_WIC_SWEN_CR3            (CM4_IRQ_CFG_BASE + 0x330)
#define CM4_WIC_SWEN_CR4            (CM4_IRQ_CFG_BASE + 0x334)

#if 0 /*define in mt3620_cm4_hw_memmap.h*/
#define CM4_WIC_SWCON_CR            (CM4_IRQ_CFG_BASE + 0x340)
#define CM4_WIC_SW_CLR              (CM4_IRQ_CFG_BASE + 0x344)
#define CM4_WIC_PEND_STA0           (CM4_IRQ_CFG_BASE + 0x364)
#define CM4_WIC_PEND_STA1           (CM4_IRQ_CFG_BASE + 0x368)
#define CM4_WIC_PEND_STA2           (CM4_IRQ_CFG_BASE + 0x36C)
#define CM4_WIC_PEND_STA3           (CM4_IRQ_CFG_BASE + 0x370)
#define CM4_WIC_PEND_STA4           (CM4_IRQ_CFG_BASE + 0x374)
#endif

#define CM4_WIC_EN_STA0             (CM4_IRQ_CFG_BASE + 0x384)
#define CM4_WIC_EN_STA1             (CM4_IRQ_CFG_BASE + 0x388)
#define CM4_WIC_EN_STA2             (CM4_IRQ_CFG_BASE + 0x38C)
#define CM4_WIC_EN_STA3             (CM4_IRQ_CFG_BASE + 0x390)
#define CM4_WIC_EN_STA4             (CM4_IRQ_CFG_BASE + 0x394)
#define CM4_WIC_WAKEUP_STA          (CM4_IRQ_CFG_BASE + 0x3A0)

/*	test_irq, dis_irq control register*/
#define CM4_CFG_IRQ_DIS0            (CM4_IRQ_CFG_BASE + 0x600)
#define CM4_CFG_IRQ_DIS1            (CM4_IRQ_CFG_BASE + 0x604)
#define CM4_CFG_IRQ_DIS2            (CM4_IRQ_CFG_BASE + 0x608)
#define CM4_CFG_IRQ_DIS3            (CM4_IRQ_CFG_BASE + 0x60C)
#define CM4_CFG_IRQ_DIS4            (CM4_IRQ_CFG_BASE + 0x610)
#define CM4_CFG_WIC_DIS0            (CM4_IRQ_CFG_BASE + 0x620)
#define CM4_CFG_WIC_DIS1            (CM4_IRQ_CFG_BASE + 0x624)
#define CM4_CFG_WIC_DIS2            (CM4_IRQ_CFG_BASE + 0x628)
#define CM4_CFG_WIC_DIS3            (CM4_IRQ_CFG_BASE + 0x62C)
#define CM4_CFG_WIC_DIS4            (CM4_IRQ_CFG_BASE + 0x630)
#define CM4_CFG_IRQ_TEST0           (CM4_IRQ_CFG_BASE + 0x700)
#define CM4_CFG_IRQ_TEST1           (CM4_IRQ_CFG_BASE + 0x704)
#define CM4_CFG_IRQ_TEST2           (CM4_IRQ_CFG_BASE + 0x708)
#define CM4_CFG_IRQ_TEST3           (CM4_IRQ_CFG_BASE + 0x70C)
#define CM4_CFG_IRQ_TEST4           (CM4_IRQ_CFG_BASE + 0x710)
#define CM4_IRQ_TEST_CTL            (CM4_IRQ_CFG_BASE + 0x720)
#define CM4_IRQ_TEST_CTL_ENABLE     (1 << 0)

//cm4_irq IP Soft reset
#define CM4_CFG_SOFT_RST            (CM4_IRQ_CFG_BASE + 0x900)
#define CFG_SOFT_RST_CM4_NVIC	    (1 << 2)

#define IRQ_EDGE_TRIGGER        0x00
#define IRQ_LEVEL_TRIGGER       0x01
#define IRQ_EDGE2LVL_TRIGGER    0x02

typedef enum
{
    No_Trigger = 0,
    SW_Trigger,
    HW_Trigger,
} trigger_source;


__STATIC_INLINE uint32_t NVIC_GetVectActive(void)
{
	return ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos) - 16;
}


#define local_irq_save(flag)				\
	do{						\
		flag = __get_PRIMASK();			\
		__disable_irq();			\
	}while(0);


#define local_irq_restore(flag)			\
	do {					\
		__set_PRIMASK(flag);		\
	}while(0);

#ifdef __cplusplus
extern "C" {
#endif
void NVIC_SetupVectorTable(void);
void NVIC_SWInt(unsigned int source);
void CM4_Install_NVIC(int irqn,int prior,int edgetr, NVIC_IRQ_Handler handler ,int enable);
int NVIC_Register(int irqn, NVIC_IRQ_Handler handler);
int NVIC_UnRegister(int irqn);
int NVIC_ISR_Check_and_backup(int irqn);
int NVIC_ISR_Restore(void);
unsigned int cm4_irq_sw_reset(void);
#ifdef __cplusplus
}
#endif

#endif
