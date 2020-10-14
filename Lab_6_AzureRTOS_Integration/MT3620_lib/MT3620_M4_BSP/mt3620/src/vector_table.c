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

#include <stdint.h>
#include "vector_table.h"

extern uint32_t StackTop;

static _Noreturn void DefaultExceptionHandler(void)
{
	for (;;) {
		// empty.
	}
}

void __attribute__((weak, alias("DefaultExceptionHandler"))) NMI_Handler(void);
void __attribute__((weak, alias("DefaultExceptionHandler"))) Hard_Fault_Handler(void);
void __attribute__((weak, alias("DefaultExceptionHandler"))) MPU_Fault_Handler(void);
void __attribute__((weak, alias("DefaultExceptionHandler"))) Bus_Fault_Handler(void);
void __attribute__((weak, alias("DefaultExceptionHandler"))) Usage_Fault_Handler(void);
void __attribute__((weak, alias("DefaultExceptionHandler"))) SVC_Handler(void);
void __attribute__((weak, alias("DefaultExceptionHandler"))) Debug_Monitor_Handler(void);
void __attribute__((weak, alias("DefaultExceptionHandler"))) PendSV_Handler(void);
void __attribute__((weak, alias("DefaultExceptionHandler"))) SystmTick_Handler(void);

uintptr_t __isr_vector[116] __attribute__((section(".vector_table"))) __attribute__((used)) = {
	[0] = (uintptr_t)&StackTop,				/* Top of Stack */
	[1] = (uintptr_t)Reset_Handler,			/* Reset Handler */
	[2] = (uintptr_t)NMI_Handler,			/* NMI Handler */
	[3] = (uintptr_t)Hard_Fault_Handler,	/* Hard Fault Handler */
	[4] = (uintptr_t)MPU_Fault_Handler,		/* MPU Fault Handler */
	[5] = (uintptr_t)Bus_Fault_Handler,		/* Bus Fault Handler */
	[6] = (uintptr_t)Usage_Fault_Handler,	/* Usage Fault Handler */
	[11] = (uintptr_t)SVC_Handler,			/* SVCall Handler */
	[12] = (uintptr_t)Debug_Monitor_Handler,/* Debug Monitor Handler */
	[14] = (uintptr_t)PendSV_Handler,		/* PendSV Handler */
	[15] = (uintptr_t)SystmTick_Handler,	/* SysTick Handler */

	[(16)... (115)] = (uintptr_t)DefaultExceptionHandler
};

#ifdef M4_ENABLE_XIP_FLASH
uintptr_t __isr_vector_tcm[116] __attribute__((section(".vector_table_tcm")));

void relocate_vector_table(uintptr_t *vector_start,
			   uintptr_t *vector_end,
			   uintptr_t *new_address)
{
	uintptr_t *src, *dest;

	src = vector_start;
	dest = new_address;

	if ((dest - src) & 0xFFFFFF00) {
		while (src < vector_end)
			*dest++ = *src++;
	}
}
#endif

