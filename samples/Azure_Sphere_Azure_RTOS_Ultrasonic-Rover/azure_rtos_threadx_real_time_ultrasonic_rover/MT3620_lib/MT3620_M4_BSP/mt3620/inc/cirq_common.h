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

/**
 * @file cirq_common.h
 *
 *
 *
 */


#ifndef __CIRQ_COMMON_H__
#define __CIRQ_COMMON_H__

#ifdef M4SC_ENABLE
#define ENABLE_GPIO_IRQ_EDGE2LEVEL
#endif

struct gpio_irq_hsp2ca7 {
    const unsigned int  vector_id;
    const unsigned int  ca7_mapping_id;
};

int cirq_gpio_irq_ctl_set_level(uint32_t vector);
int cirq_gpio_irq_ctl_set_edge(uint32_t vector);
int cirq_gpio_irq_ctl_set_edge2lvl(uint32_t vector);
int cirq_gpio_irq_ctl_clr_edge2lvl(uint32_t vector);
int cirq_gpio_irq_ctl_ack_edge2lvl(uint32_t vector);
int cirq_irq_test_set_trigger(uint32_t cpu, uint32_t vector);
int cirq_irq_test_clr_trigger(uint32_t cpu, uint32_t vector);
int cirq_irq_dis_set_trigger(uint32_t cpu, uint32_t vector);
int cirq_irq_dis_clr_trigger(uint32_t cpu, uint32_t vector);
int cirq_irq_ctl_set_level(uint32_t vector);
int cirq_irq_ctl_set_edge(uint32_t vector);
int cirq_cm4_wic_enable(uint32_t vector);
int cirq_cm4_wic_disable(uint32_t vector);

#endif /* __CIRQ_COMMON_H__ */

