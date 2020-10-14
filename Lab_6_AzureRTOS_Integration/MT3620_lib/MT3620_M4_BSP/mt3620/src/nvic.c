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


#include "nvic.h"
#include "mem_util.h"
#include "timer.h"
#include "cirq_common.h"
#include "type_def.h"
#include "vector_table.h"

volatile uint32_t sys_tick_in_ms = 0;
void SystmTick_Handler(void)
{
	sys_tick_in_ms++;
	// #ifdef OSAI_FREERTOS
	extern void SysTick_Handler(void);
	SysTick_Handler();
	// #endif
}

int NVIC_Register(int irqn, NVIC_IRQ_Handler handler)
{
	NVIC_DisableIRQ(irqn);
	NVIC_ClearPendingIRQ(irqn);

#ifdef M4_ENABLE_XIP_FLASH
	__isr_vector_tcm[irqn+16] = (unsigned long) handler;
#else
	__isr_vector[irqn+16] = (unsigned long) handler;
#endif
	__asm volatile( "dsb" );
	__asm volatile( "isb" );
	return 0;
}

int NVIC_UnRegister(int irqn)
{
	NVIC_DisableIRQ(irqn);
	NVIC_ClearPendingIRQ(irqn);

#ifdef M4_ENABLE_XIP_FLASH
	__isr_vector_tcm[irqn+16] = NULL;
#else
	__isr_vector[irqn+16] = NULL;
#endif
	__asm volatile( "dsb" );
	__asm volatile( "isb" );
	return 0;
}

void NVIC_SetupVectorTable(void)
{
#ifdef M4_ENABLE_XIP_FLASH
	relocate_vector_table(&__vector_table_start__,
			      &__vector_table_end__,
			      __isr_vector_tcm);
	SCB->VTOR = (uint32_t)__isr_vector_tcm;
#else
	SCB->VTOR = (uint32_t)__isr_vector;
#endif
	__asm volatile( "dsb" );
	__asm volatile( "isb" );

    // Set M4 core to 197.6MHz (@26MHz)
	DRV_Reg32(CM4_HCLK_CK_CTRL) &= 0xFFFF00FF;
	DRV_Reg32(CM4_HCLK_CK_CTRL) |= 0x00000200;

	// Enable System Tick
	SysTick_Config(200000);
}

void NVIC_SWInt(unsigned int source)
{
	NVIC->STIR = source;
}

NVIC_IRQ_Handler backup_handler = NULL;
int backup_irqn = 0;
int NVIC_ISR_Check_and_backup(int irqn)
{
#ifdef M4_ENABLE_XIP_FLASH
	backup_handler = (NVIC_IRQ_Handler) __isr_vector_tcm[irqn + 16];
#else
    backup_handler = (NVIC_IRQ_Handler) __isr_vector[irqn + 16];
#endif
    backup_irqn = irqn;
    return 0;
}
int NVIC_ISR_Restore(void)
{
    int result= -1;

    result = NVIC_Register(backup_irqn, backup_handler);
    return result;
}

void CM4_Install_NVIC(int irqn,int prior,int edgetr, NVIC_IRQ_Handler handler ,int enable)
{
    NVIC_Register(irqn, handler);

    if(prior <= 0)
        NVIC_SetPriority(irqn, DEFAULT_PRI);
    else
        NVIC_SetPriority(irqn, prior);

    if(edgetr == IRQ_EDGE2LVL_TRIGGER) {
        cirq_irq_ctl_set_level(irqn);
        cirq_gpio_irq_ctl_set_edge2lvl(irqn);
    } else if(edgetr == IRQ_EDGE_TRIGGER) {
        cirq_irq_ctl_set_edge(irqn);
    } else if(edgetr == IRQ_LEVEL_TRIGGER) {
        cirq_irq_ctl_set_level(irqn);
    }

    if(enable == TRUE)
        NVIC_EnableIRQ(irqn);
}
//cm4_irq IP SW reset : start
void cm4_irq_set_sw_reset(void)
{
    DRV_Reg32(CM4_CFG_SOFT_RST) |= CFG_SOFT_RST_CM4_NVIC;
}

void cm4_irq_clear_sw_reset(void)
{
    DRV_Reg32(CM4_CFG_SOFT_RST) &= ~CFG_SOFT_RST_CM4_NVIC;
}

unsigned int cm4_irq_sw_reset(void)
{
   uint32_t vector;

   cm4_irq_set_sw_reset();

   for (vector = 0; vector <= CM4_IRQ_MAX; vector++) {
        NVIC_DisableIRQ(vector);
        NVIC_ClearPendingIRQ(vector);
   }

   cm4_irq_clear_sw_reset();

   return 0;
}
//cm4_irq IP SW reset : end


