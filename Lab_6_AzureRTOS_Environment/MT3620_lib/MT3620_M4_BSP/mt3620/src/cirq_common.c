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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type_def.h"
#include "irq.h"
#include "nvic.h"
#include "mt3620.h"
#include "cirq_common.h"


#define GIC_IRQ_MIN             32
#define GIC_IRQ_MAX             143

#define CA7_IRQ_GPIO_G0_0	    49
#define CA7_IRQ_GPIO_G0_1	    50
#define CA7_IRQ_GPIO_G0_2	    51
#define CA7_IRQ_GPIO_G0_3	    52
#define CA7_IRQ_GPIO_G1_0	    93
#define CA7_IRQ_GPIO_G1_1	    94
#define CA7_IRQ_GPIO_G1_2	    95
#define CA7_IRQ_GPIO_G1_3	    96
#define CA7_IRQ_GPIO_G2_0	    97
#define CA7_IRQ_GPIO_G2_1	    98
#define CA7_IRQ_GPIO_G2_2	    99
#define CA7_IRQ_GPIO_G2_3	    100
#define CA7_IRQ_GPIO_G3_0	    101
#define CA7_IRQ_GPIO_G3_1	    102
#define CA7_IRQ_GPIO_G3_2	    103
#define CA7_IRQ_GPIO_G3_3	    104
#define CA7_IRQ_GPIO_G4_0	    105
#define CA7_IRQ_GPIO_G4_1	    106
#define CA7_IRQ_GPIO_G4_2	    107
#define CA7_IRQ_GPIO_G4_3	    108
#define CA7_IRQ_GPIO_G5_0	    109
#define CA7_IRQ_GPIO_G5_1	    110
#define CA7_IRQ_GPIO_G5_2	    111
#define CA7_IRQ_GPIO_G5_3	    112

struct gpio_irq_hsp2ca7 gpio_irq_tbl[] = {
    {CM4_IRQ_GPIO_G0_0, CA7_IRQ_GPIO_G0_0},
    {CM4_IRQ_GPIO_G0_1, CA7_IRQ_GPIO_G0_1},
    {CM4_IRQ_GPIO_G0_2, CA7_IRQ_GPIO_G0_2},
    {CM4_IRQ_GPIO_G0_3, CA7_IRQ_GPIO_G0_3},
    {CM4_IRQ_GPIO_G1_0, CA7_IRQ_GPIO_G1_0},
    {CM4_IRQ_GPIO_G1_1, CA7_IRQ_GPIO_G1_1},
    {CM4_IRQ_GPIO_G1_2, CA7_IRQ_GPIO_G1_2},
    {CM4_IRQ_GPIO_G1_3, CA7_IRQ_GPIO_G1_3},
    {CM4_IRQ_GPIO_G2_0, CA7_IRQ_GPIO_G2_0},
    {CM4_IRQ_GPIO_G2_1, CA7_IRQ_GPIO_G2_1},
    {CM4_IRQ_GPIO_G2_2, CA7_IRQ_GPIO_G2_2},
    {CM4_IRQ_GPIO_G2_3, CA7_IRQ_GPIO_G2_3},
    {CM4_IRQ_GPIO_G3_0, CA7_IRQ_GPIO_G3_0},
    {CM4_IRQ_GPIO_G3_1, CA7_IRQ_GPIO_G3_1},
    {CM4_IRQ_GPIO_G3_2, CA7_IRQ_GPIO_G3_2},
    {CM4_IRQ_GPIO_G3_3, CA7_IRQ_GPIO_G3_3},
    {CM4_IRQ_GPIO_G4_0, CA7_IRQ_GPIO_G4_0},
    {CM4_IRQ_GPIO_G4_1, CA7_IRQ_GPIO_G4_1},
    {CM4_IRQ_GPIO_G4_2, CA7_IRQ_GPIO_G4_2},
    {CM4_IRQ_GPIO_G4_3, CA7_IRQ_GPIO_G4_3},
    {CM4_IRQ_GPIO_G5_0, CA7_IRQ_GPIO_G5_0},
    {CM4_IRQ_GPIO_G5_1, CA7_IRQ_GPIO_G5_1},
    {CM4_IRQ_GPIO_G5_2, CA7_IRQ_GPIO_G5_2},
    {CM4_IRQ_GPIO_G5_3, CA7_IRQ_GPIO_G5_3}
};



int cirq_gpio_irq_ctl_set_level(uint32_t vector)
{
    struct gpio_irq_hsp2ca7 *irqn = &gpio_irq_tbl[0];
    int i;

    if ((vector < CM4_IRQ_GPIO_G0_0) || (vector > CM4_IRQ_GPIO_G5_3))
        return -1;

    for (i = 0; i < sizeof(gpio_irq_tbl)/sizeof(const struct gpio_irq_hsp2ca7); i++) {
       if (vector == irqn->vector_id)
            vector = irqn->ca7_mapping_id - GIC_IRQ_MIN;
       irqn++;
    }

	if(vector >= 0 && vector <= 31)
		*((volatile unsigned long *)CA7_IRQ_SENS0) = (*((volatile unsigned long *)CA7_IRQ_SENS0)) | ((0x1UL) << (vector-0));
	else if(vector >= 32 && vector <= 63)
		*((volatile unsigned long *)CA7_IRQ_SENS1) = (*((volatile unsigned long *)CA7_IRQ_SENS1)) | ((0x1UL) << (vector-32));
	else if(vector >= 64 && vector <= 95)
		*((volatile unsigned long *)CA7_IRQ_SENS2) = (*((volatile unsigned long *)CA7_IRQ_SENS2)) | ((0x1UL) << (vector-64));
	else if(vector >= 96 && vector <= 127)
		*((volatile unsigned long *)CA7_IRQ_SENS3) = (*((volatile unsigned long *)CA7_IRQ_SENS3)) | ((0x1UL) << (vector-96));

	return 0;
}


int cirq_gpio_irq_ctl_set_edge(uint32_t vector)
{
    struct gpio_irq_hsp2ca7 *irqn = &gpio_irq_tbl[0];
    int i;

    if ((vector < CM4_IRQ_GPIO_G0_0) || (vector > CM4_IRQ_GPIO_G5_3))
        return -1;

    for (i = 0; i < sizeof(gpio_irq_tbl)/sizeof(const struct gpio_irq_hsp2ca7); i++) {
       if (vector == irqn->vector_id)
           vector = irqn->ca7_mapping_id - GIC_IRQ_MIN;
       irqn++;
    }


	if(vector >= 0 && vector <= 31)
		*((volatile unsigned long *)CA7_IRQ_SENS0) = (*((volatile unsigned long *)CA7_IRQ_SENS0)) & ~((0x1UL) << (vector-0));
	else if(vector >= 32 && vector <= 63)
		*((volatile unsigned long *)CA7_IRQ_SENS1) = (*((volatile unsigned long *)CA7_IRQ_SENS1)) & ~((0x1UL) << (vector-32));
	else if(vector >= 64 && vector <= 95)
		*((volatile unsigned long *)CA7_IRQ_SENS2) = (*((volatile unsigned long *)CA7_IRQ_SENS2)) & ~((0x1UL) << (vector-64));
	else if(vector >= 96 && vector <= 127)
		*((volatile unsigned long *)CA7_IRQ_SENS3) = (*((volatile unsigned long *)CA7_IRQ_SENS3)) & ~((0x1UL) << (vector-96));

	return 0;
}


int cirq_gpio_irq_ctl_set_edge2lvl(uint32_t vector)
{
    struct gpio_irq_hsp2ca7 *irqn = &gpio_irq_tbl[0];
    int i;

    if ((vector < CM4_IRQ_GPIO_G0_0) || (vector > CM4_IRQ_GPIO_G5_3))
        return -1;

    for (i = 0; i < sizeof(gpio_irq_tbl)/sizeof(const struct gpio_irq_hsp2ca7); i++) {
        if (vector == irqn->vector_id)
             vector = irqn->ca7_mapping_id - GIC_IRQ_MIN;
       irqn++;
    }


	if(vector >= 0 && vector <= 31)
		*((volatile unsigned long *)CA7_IRQ_EDGE2LVL0) = (*((volatile unsigned long *)CA7_IRQ_EDGE2LVL0)) | ((0x1UL) << (vector-0));
	else if(vector >= 32 && vector <= 63)
		*((volatile unsigned long *)CA7_IRQ_EDGE2LVL1) = (*((volatile unsigned long *)CA7_IRQ_EDGE2LVL1)) | ((0x1UL) << (vector-32));
	else if(vector >= 64 && vector <= 95)
		*((volatile unsigned long *)CA7_IRQ_EDGE2LVL2) = (*((volatile unsigned long *)CA7_IRQ_EDGE2LVL2)) | ((0x1UL) << (vector-64));
	else if(vector >= 96 && vector <= 127)
		*((volatile unsigned long *)CA7_IRQ_EDGE2LVL3) = (*((volatile unsigned long *)CA7_IRQ_EDGE2LVL3)) | ((0x1UL) << (vector-96));

    return 0;
}


int cirq_gpio_irq_ctl_clr_edge2lvl(uint32_t vector)
{
    struct gpio_irq_hsp2ca7 *irqn = &gpio_irq_tbl[0];
    int i;

    if ((vector < CM4_IRQ_GPIO_G0_0) || (vector > CM4_IRQ_GPIO_G5_3))
        return -1;

    for (i = 0; i < sizeof(gpio_irq_tbl)/sizeof(const struct gpio_irq_hsp2ca7); i++) {
       if (vector == irqn->vector_id)
           vector = irqn->ca7_mapping_id - GIC_IRQ_MIN;
       irqn++;
    }


	if(vector >= 0 && vector <= 31)
		*((volatile unsigned long *)CA7_IRQ_EDGE2LVL0) = (*((volatile unsigned long *)CA7_IRQ_EDGE2LVL0)) & ~((0x1UL) << (vector-0));
	else if(vector >= 32 && vector <= 63)
		*((volatile unsigned long *)CA7_IRQ_EDGE2LVL1) = (*((volatile unsigned long *)CA7_IRQ_EDGE2LVL1)) & ~((0x1UL) << (vector-32));
	else if(vector >= 64 && vector <= 95)
		*((volatile unsigned long *)CA7_IRQ_EDGE2LVL2) = (*((volatile unsigned long *)CA7_IRQ_EDGE2LVL2)) & ~((0x1UL) << (vector-64));
	else if(vector >= 96 && vector <= 127)
		*((volatile unsigned long *)CA7_IRQ_EDGE2LVL3) = (*((volatile unsigned long *)CA7_IRQ_EDGE2LVL3)) & ~((0x1UL) << (vector-96));

	return 0;
}

int cirq_gpio_irq_ctl_ack_edge2lvl(uint32_t vector)
{
    struct gpio_irq_hsp2ca7 *irqn = &gpio_irq_tbl[0];
    int i;

    if ((vector < CM4_IRQ_GPIO_G0_0) || (vector > CM4_IRQ_GPIO_G5_3))
        return -1;

    for (i = 0; i < sizeof(gpio_irq_tbl)/sizeof(const struct gpio_irq_hsp2ca7); i++) {
       if (vector == irqn->vector_id)
           vector = irqn->ca7_mapping_id - GIC_IRQ_MIN;
       irqn++;
    }

	if(vector >= 0 && vector <= 31)
		*((volatile unsigned long *)CA7_IRQ_STS0) = (*((volatile unsigned long *)CA7_IRQ_STS0)) | ((0x1UL) << (vector-0));
	else if(vector >= 32 && vector <= 63)
		*((volatile unsigned long *)CA7_IRQ_STS1) = (*((volatile unsigned long *)CA7_IRQ_STS1)) | ((0x1UL) << (vector-32));
	else if(vector >= 64 && vector <= 95)
		*((volatile unsigned long *)CA7_IRQ_STS2) = (*((volatile unsigned long *)CA7_IRQ_STS2)) | ((0x1UL) << (vector-64));
	else if(vector >= 96 && vector <= 127)
		*((volatile unsigned long *)CA7_IRQ_STS3) = (*((volatile unsigned long *)CA7_IRQ_STS3)) | ((0x1UL) << (vector-96));

    return 0;
}


int cirq_irq_test_set_trigger(uint32_t cpu, uint32_t vector)
{

    if (cpu == 0) {
        if(vector >= 0 && vector <= 31)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST0) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST0)) | ((0x1UL) << (vector-0));
        else if(vector >= 32 && vector <= 63)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST1) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST1)) | ((0x1UL) << (vector-32));
        else if(vector >= 64 && vector <= 95)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST2) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST2)) | ((0x1UL) << (vector-64));
        else if(vector >= 96 && vector <= 127)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST3) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST3)) | ((0x1UL) << (vector-96));
#ifdef M4SC_ENABLE
        else if(vector >= 128 && vector <= 159)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST4) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST4)) | ((0x1UL) << (vector-128));
#endif
    } else {

        struct gpio_irq_hsp2ca7 *irqn = &gpio_irq_tbl[0];
        int i;

        if ((vector < CM4_IRQ_GPIO_G0_0) || (vector > CM4_IRQ_GPIO_G5_3))
            return -1;

        for (i = 0; i < sizeof(gpio_irq_tbl)/sizeof(const struct gpio_irq_hsp2ca7); i++) {
           if (vector == irqn->vector_id)
               vector = irqn->ca7_mapping_id - GIC_IRQ_MIN;
           irqn++;
        }

        if(vector >= 0 && vector <= 31)
            *((volatile unsigned long *)CA7_CFG_IRQ_TEST0) = (*((volatile unsigned long *)CA7_CFG_IRQ_TEST0)) | ((0x1UL) << (vector-0));
        else if(vector >= 32 && vector <= 63)
            *((volatile unsigned long *)CA7_CFG_IRQ_TEST1) = (*((volatile unsigned long *)CA7_CFG_IRQ_TEST1)) | ((0x1UL) << (vector-32));
        else if(vector >= 64 && vector <= 95)
            *((volatile unsigned long *)CA7_CFG_IRQ_TEST2) = (*((volatile unsigned long *)CA7_CFG_IRQ_TEST2)) | ((0x1UL) << (vector-64));
        else if(vector >= 96 && vector <= 127)
            *((volatile unsigned long *)CA7_CFG_IRQ_TEST3) = (*((volatile unsigned long *)CA7_CFG_IRQ_TEST3)) | ((0x1UL) << (vector-96));
    }

    return 0;
}


int cirq_irq_test_clr_trigger(uint32_t cpu, uint32_t vector)
{
    if (cpu == 0) {
        if(vector >= 0 && vector <= 31)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST0) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST0)) & ~((0x1UL) << (vector-0));
        else if(vector >= 32 && vector <= 63)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST1) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST1)) & ~((0x1UL) << (vector-32));
        else if(vector >= 64 && vector <= 95)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST2) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST2)) & ~((0x1UL) << (vector-64));
        else if(vector >= 96 && vector <= 127)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST3) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST3)) & ~((0x1UL) << (vector-96));
#ifdef M4SC_ENABLE
        else if(vector >= 128 && vector <= 159)
            *((volatile unsigned long *)CM4_CFG_IRQ_TEST4) = (*((volatile unsigned long *)CM4_CFG_IRQ_TEST4)) & ~((0x1UL) << (vector-128));
#endif
    } else {

        struct gpio_irq_hsp2ca7 *irqn = &gpio_irq_tbl[0];
        int i;

        if ((vector < CM4_IRQ_GPIO_G0_0) || (vector > CM4_IRQ_GPIO_G5_3))
            return -1;

        for (i = 0; i < sizeof(gpio_irq_tbl)/sizeof(const struct gpio_irq_hsp2ca7); i++) {
           if (vector == irqn->vector_id)
               vector = irqn->ca7_mapping_id - GIC_IRQ_MIN;
           irqn++;
        }


    	if(vector >= 0 && vector <= 31)
    		*((volatile unsigned long *)CA7_CFG_IRQ_TEST0) = (*((volatile unsigned long *)CA7_CFG_IRQ_TEST0)) & ~((0x1UL) << (vector-0));
    	else if(vector >= 32 && vector <= 63)
    		*((volatile unsigned long *)CA7_CFG_IRQ_TEST1) = (*((volatile unsigned long *)CA7_CFG_IRQ_TEST1)) & ~((0x1UL) << (vector-32));
    	else if(vector >= 64 && vector <= 95)
    		*((volatile unsigned long *)CA7_CFG_IRQ_TEST2) = (*((volatile unsigned long *)CA7_CFG_IRQ_TEST2)) & ~((0x1UL) << (vector-64));
    	else if(vector >= 96 && vector <= 127)
    		*((volatile unsigned long *)CA7_CFG_IRQ_TEST3) = (*((volatile unsigned long *)CA7_CFG_IRQ_TEST3)) & ~((0x1UL) << (vector-96));
    }
    return 0;
}


int cirq_irq_dis_set_trigger(uint32_t cpu, uint32_t vector)
{

    if (cpu == 0) {
        if(vector >= 0 && vector <= 31)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS0) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS0)) | ((0x1UL) << (vector-0));
        else if(vector >= 32 && vector <= 63)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS1) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS1)) | ((0x1UL) << (vector-32));
        else if(vector >= 64 && vector <= 95)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS2) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS2)) | ((0x1UL) << (vector-64));
        else if(vector >= 96 && vector <= 127)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS3) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS3)) | ((0x1UL) << (vector-96));
#ifdef M4SC_ENABLE
        else if(vector >= 128 && vector <= 159)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS4) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS4)) | ((0x1UL) << (vector-128));
#endif
    } else {
        if(vector >= 0 && vector <= 31)
            *((volatile unsigned long *)CA7_CFG_IRQ_DIS0) = (*((volatile unsigned long *)CA7_CFG_IRQ_DIS0)) | ((0x1UL) << (vector-0));
        else if(vector >= 32 && vector <= 63)
            *((volatile unsigned long *)CA7_CFG_IRQ_DIS1) = (*((volatile unsigned long *)CA7_CFG_IRQ_DIS1)) | ((0x1UL) << (vector-32));
        else if(vector >= 64 && vector <= 95)
            *((volatile unsigned long *)CA7_CFG_IRQ_DIS2) = (*((volatile unsigned long *)CA7_CFG_IRQ_DIS2)) | ((0x1UL) << (vector-64));
        else if(vector >= 96 && vector <= 127)
            *((volatile unsigned long *)CA7_CFG_IRQ_DIS3) = (*((volatile unsigned long *)CA7_CFG_IRQ_DIS3)) | ((0x1UL) << (vector-96));
    }
    return 0;
}

int cirq_irq_dis_clr_trigger(uint32_t cpu, uint32_t vector)
{

    if (cpu == 0) {
        if(vector >= 0 && vector <= 31)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS0) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS0)) & ~((0x1UL) << (vector-0));
        else if(vector >= 32 && vector <= 63)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS1) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS1)) & ~((0x1UL) << (vector-32));
        else if(vector >= 64 && vector <= 95)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS2) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS2)) & ~((0x1UL) << (vector-64));
        else if(vector >= 96 && vector <= 127)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS3) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS3)) & ~((0x1UL) << (vector-96));
#ifdef M4SC_ENABLE
        else if(vector >= 128 && vector <= 159)
            *((volatile unsigned long *)CM4_CFG_IRQ_DIS4) = (*((volatile unsigned long *)CM4_CFG_IRQ_DIS4)) & ~((0x1UL) << (vector-128));
#endif
    } else {
        if(vector >= 0 && vector <= 31)
            *((volatile unsigned long *)CA7_CFG_IRQ_DIS0) = (*((volatile unsigned long *)CA7_CFG_IRQ_DIS0)) & ~((0x1UL) << (vector-0));
        else if(vector >= 32 && vector <= 63)
            *((volatile unsigned long *)CA7_CFG_IRQ_DIS1) = (*((volatile unsigned long *)CA7_CFG_IRQ_DIS1)) & ~((0x1UL) << (vector-32));
        else if(vector >= 64 && vector <= 95)
            *((volatile unsigned long *)CA7_CFG_IRQ_DIS2) = (*((volatile unsigned long *)CA7_CFG_IRQ_DIS2)) & ~((0x1UL) << (vector-64));
        else if(vector >= 96 && vector <= 127)
            *((volatile unsigned long *)CA7_CFG_IRQ_DIS3) = (*((volatile unsigned long *)CA7_CFG_IRQ_DIS3)) & ~((0x1UL) << (vector-96));
    }

    return 0;
}

int cirq_irq_ctl_set_level(uint32_t vector)
{
	if(vector >= 0 && vector <= 31)
		*((volatile unsigned long *)CM4_IRQ_SENS0) = (*((volatile unsigned long *)CM4_IRQ_SENS0)) | ((0x1UL) << (vector-0));
	else if(vector >= 32 && vector <= 63)
		*((volatile unsigned long *)CM4_IRQ_SENS1) = (*((volatile unsigned long *)CM4_IRQ_SENS1)) | ((0x1UL) << (vector-32));
	else if(vector >= 64 && vector <= 95)
		*((volatile unsigned long *)CM4_IRQ_SENS2) = (*((volatile unsigned long *)CM4_IRQ_SENS2)) | ((0x1UL) << (vector-64));
	else if(vector >= 96 && vector <= 127)
		*((volatile unsigned long *)CM4_IRQ_SENS3) = (*((volatile unsigned long *)CM4_IRQ_SENS3)) | ((0x1UL) << (vector-96));
#ifdef M4SC_ENABLE
	else if(vector >= 128 && vector <= 159)
		*((volatile unsigned long *)CM4_IRQ_SENS4) = (*((volatile unsigned long *)CM4_IRQ_SENS4)) | ((0x1UL) << (vector-128));
#endif
	return 0;
}

int cirq_irq_ctl_set_edge(uint32_t vector)
{
	if(vector >= 0 && vector <= 31)
		*((volatile unsigned long *)CM4_IRQ_SENS0) = (*((volatile unsigned long *)CM4_IRQ_SENS0)) & ~((0x1UL) << (vector-0));
	else if(vector >= 32 && vector <= 63)
		*((volatile unsigned long *)CM4_IRQ_SENS1) = (*((volatile unsigned long *)CM4_IRQ_SENS1)) & ~((0x1UL) << (vector-32));
	else if(vector >= 64 && vector <= 95)
		*((volatile unsigned long *)CM4_IRQ_SENS2) = (*((volatile unsigned long *)CM4_IRQ_SENS2)) & ~((0x1UL) << (vector-64));
	else if(vector >= 96 && vector <= 127)
		*((volatile unsigned long *)CM4_IRQ_SENS3) = (*((volatile unsigned long *)CM4_IRQ_SENS3)) & ~((0x1UL) << (vector-96));
#ifdef M4SC_ENABLE
    else if(vector >= 128 && vector <= 159)
		*((volatile unsigned long *)CM4_IRQ_SENS4) = (*((volatile unsigned long *)CM4_IRQ_SENS4)) & ~((0x1UL) << (vector-128));
#endif
	return 0;
}

int cirq_cm4_wic_enable(uint32_t vector)
{
    if(vector >= 0 && vector <= 31)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS0) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS0)) & ~((0x1UL) << (vector-0));
    else if(vector >= 32 && vector <= 63)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS1) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS1)) & ~((0x1UL) << (vector-32));
    else if(vector >= 64 && vector <= 95)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS2) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS2)) & ~((0x1UL) << (vector-64));
    else if(vector >= 96 && vector <= 127)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS3) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS3)) & ~((0x1UL) << (vector-96));
#ifdef M4SC_ENABLE
    else if(vector >= 128 && vector <= 159)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS4) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS4)) & ~((0x1UL) << (vector-128));
#endif

    return 0;
}

int cirq_cm4_wic_disable(uint32_t vector)
{
    if(vector >= 0 && vector <= 31)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS0) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS0)) | ((0x1UL) << (vector-0));
    else if(vector >= 32 && vector <= 63)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS1) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS1)) | ((0x1UL) << (vector-32));
    else if(vector >= 64 && vector <= 95)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS2) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS2)) | ((0x1UL) << (vector-64));
    else if(vector >= 96 && vector <= 127)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS3) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS3)) | ((0x1UL) << (vector-96));
#ifdef M4SC_ENABLE
    else if(vector >= 128 && vector <= 159)
        *((volatile unsigned long *)CM4_CFG_WIC_DIS4) = (*((volatile unsigned long *)CM4_CFG_WIC_DIS4)) | ((0x1UL) << (vector-128));
#endif

    return 0;
}

