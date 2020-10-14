/*
 * (C) 2005-2019 MediaTek Inc. All rights reserved.
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

#ifndef __MHAL_GPIOIF_H__
#define __MHAL_GPIOIF_H__
#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup GPIOIF
 * @{
 * This section introduces the General-Purpose Inputs-Outputs Interface
 * (GPIOIF) APIs including terms and acronyms, supported features,
 * software architecture details on how to use this driver,
 * GPIOIF groups, enums and functions.
 *
 * @section MHAL_GPIOIF_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b GPIOIF  | General-Purpose Inputs-Outputs Interface.|
 * |\b FIFO                       | First In, First Out.|
 *
 * @section MHAL_GPIOIF_Features_Chapter Supported features
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup GPIOIF
 * @{\n
 *
 *  - Support two hardware counter modes: Event Counter Mode
 *    and Capture Counter Mode. The hardware counter can be incremented,
 *    decremented and sampled from GPIO_0 and GPIO_1 with GPIO_2 being
 *    an opational reset pin. In hardware counter mode, GPIO_3 is not involved
 *    and hence is left for legacy input/output GPIO feature.\n
 *  - Support Event Counter Mode:\n
 *    - Direction mode.
 *       Event is the signal change of GPIO_0, the increment/decrement
 *       definition depends on GPIO_1 which is also called
 *       Direction indicator.\n
 *    - Up Down mode.
 *       The signal change of GPIO_0 and GPIO_1 are opposite Events.
 *       For example, GPIO_0 represents increment Event and
 *       GPIO_1 stands for decrement Event\n
 *    - Quadrature mode.
 *       Event is defined based on the order of singal change of GPIO_0 and
 *       GPIO_1. For example, GPIO_0 Rising edge before GPIO_1 Rising edge
 *       is considered to be an increment Event; Meanwhile, GPIO_0 Rising
 *       edge after GPIO_1 Rising edge will be a decrement Event.\n
 *  - Support Capture Counter Mode:\n
 *    - Support one 8-entry Capture FIFO for each GPIO.
 *       Capture means to push the counter value into Capture FIFO
 *       on the signal change of GPIO_0 and GPIO_1.\n
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup GPIOIF
 * @{
 * @section MHAL_GPIOIF_Driver_Usage_Chapter How to Use This Driver
 *
 * - \b SW \b Architecture: \n
 * See @ref MHAL_Overview_2_Chapter for the details of SW architecture.
 *
 *	This section describes the definition of APIs and provides
 *	an example on freeRTos to show how to use these APIs to develop
 *	an OS-related GPIOIF driver.\n
 *
 * - \b The \b OS-HAL \b freeRTos \b driver \b sample \b code \b is
 *    \b as \b follows:\n
 *	- Sample code (GPIOIF is special IP.
 *       There is not any framework on freeRTos or linux,
 *       so this sample code provides APIs to User Application):
 * @par       Example
 * @code
 *
 *#include "nvic.h"
 *#include "os_hal_gpioif.h"
 *#include "os_hal_gpio.h"
 *
 *#define CM4_GPIOIF_GRP0_BASE		0x38010000
 *#define CM4_GPIOIF_GRP1_BASE		0x38020000
 *#define CM4_GPIOIF_GRP2_BASE		0x38030000
 *#define CM4_GPIOIF_GRP3_BASE		0x38040000
 *#define CM4_GPIOIF_GRP4_BASE		0x38050000
 *#define CM4_GPIOIF_GRP5_BASE		0x38060000
 *
 *#define CM4_GPIOIF_CG_BASE		0x30030000
 *
 *#define MTK_GPIOIF_MAX_VAL	0xffffffff
 *
 *static unsigned long gpioif_base_addr[MTK_GPIOIF_MAX_GRP_NUM] = {
 *	 CM4_GPIOIF_GRP0_BASE,
 *	 CM4_GPIOIF_GRP1_BASE,
 *	 CM4_GPIOIF_GRP2_BASE,
 *	 CM4_GPIOIF_GRP3_BASE,
 *	 CM4_GPIOIF_GRP4_BASE,
 *	 CM4_GPIOIF_GRP5_BASE,
 *};
 *
 *struct mtk_gpioif_controller_rtos {
 *	 struct mtk_gpioif_controller *ctlr;
 *	 void *user_data;
 *	 gpioif_int_callback int_callback;
 *};
 *
 *static struct mtk_gpioif_controller_rtos
 *	 g_gpioif_ctlr_rtos[MTK_GPIOIF_MAX_GRP_NUM];
 *
 *static struct mtk_gpioif_config g_gpioif_mconfig[MTK_GPIOIF_MAX_GRP_NUM];
 *static struct mtk_gpioif_int_count g_gpioif_int_cnt[MTK_GPIOIF_MAX_GRP_NUM];
 *static struct mtk_gpioif_controller g_gpioif_ctlr[MTK_GPIOIF_MAX_GRP_NUM];
 *
 *static struct mtk_gpioif_controller_rtos *_mtk_os_hal_gpioif_get_ctlr(
 *	 u8 group)
 *{
 *	 if (group >= MTK_GPIOIF_MAX_GRP_NUM) {
 *		 printf("gpioif get group[%d] fail\n", group);
 *		 return NULL;
 *	 }
 *
 *	 return &g_gpioif_ctlr_rtos[group];
 *}
 *
 *
 *int mtk_os_hal_gpioif_int_callback_register(u8 group,
 *	 gpioif_int_callback callback, void *user_data)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *
 *	 if (!ctlr_rtos || !callback || !user_data)
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr_rtos->user_data = user_data;
 *	 ctlr_rtos->int_callback = callback;
 *
 *	 return 0;
 *}
 *
 *static int _mtk_os_hal_gpioif_irq_handler(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 printf("now in gpioif%d_irq_handler\n", group);
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 mtk_mhal_gpioif_isr_handle(ctlr);
 *	 ctlr_rtos->int_callback(ctlr_rtos->user_data);
 *
 *	 return 0;
 *}
 *
 *static void _mtk_os_hal_gpioif_grp0_irq_event(void)
 *{
 *	 _mtk_os_hal_gpioif_irq_handler(0);
 *}
 *
 *static void _mtk_os_hal_gpioif_grp1_irq_event(void)
 *{
 *	 _mtk_os_hal_gpioif_irq_handler(1);
 *}
 *
 *static void _mtk_os_hal_gpioif_grp2_irq_event(void)
 *{
 *	 _mtk_os_hal_gpioif_irq_handler(2);
 *}
 *
 *static void _mtk_os_hal_gpioif_grp3_irq_event(void)
 *{
 *	 _mtk_os_hal_gpioif_irq_handler(3);
 *}
 *
 *static void _mtk_os_hal_gpioif_grp4_irq_event(void)
 *{
 *	 _mtk_os_hal_gpioif_irq_handler(4);
 *}
 *
 *static void _mtk_os_hal_gpioif_grp5_irq_event(void)
 *{
 *	 _mtk_os_hal_gpioif_irq_handler(5);
 *}
 *
 *static int _mtk_os_hal_gpioif_request_irq(u8 group)
 *{
 *	 switch (group) {
 *	 case 0:
 *		 CM4_Install_NVIC(CM4_IRQ_GPIO_G0_CNT, DEFAULT_PRI,
 *				  IRQ_LEVEL_TRIGGER,
 *				  _mtk_os_hal_gpioif_grp0_irq_event,
 *				  TRUE);
 *		 break;
 *	 case 1:
 *		 CM4_Install_NVIC(CM4_IRQ_GPIO_G1_CNT, DEFAULT_PRI,
 *				  IRQ_LEVEL_TRIGGER,
 *				  _mtk_os_hal_gpioif_grp1_irq_event,
 *				  TRUE);
 *		 break;
 *	 case 2:
 *		 CM4_Install_NVIC(CM4_IRQ_GPIO_G2_CNT, DEFAULT_PRI,
 *				  IRQ_LEVEL_TRIGGER,
 *				  _mtk_os_hal_gpioif_grp2_irq_event,
 *				  TRUE);
 *		 break;
 *	 case 3:
 *		 CM4_Install_NVIC(CM4_IRQ_GPIO_G3_CNT, DEFAULT_PRI,
 *				  IRQ_LEVEL_TRIGGER,
 *				  _mtk_os_hal_gpioif_grp3_irq_event,
 *				  TRUE);
 *		 break;
 *	 case 4:
 *		 CM4_Install_NVIC(CM4_IRQ_GPIO_G4_CNT, DEFAULT_PRI,
 *				  IRQ_LEVEL_TRIGGER,
 *				  _mtk_os_hal_gpioif_grp4_irq_event,
 *				  TRUE);
 *		 break;
 *	 case 5:
 *		 CM4_Install_NVIC(CM4_IRQ_GPIO_G5_CNT, DEFAULT_PRI,
 *				  IRQ_LEVEL_TRIGGER,
 *				  _mtk_os_hal_gpioif_grp5_irq_event,
 *				  TRUE);
 *		 break;
 *	 }
 *
 *	 return 0;
 *}
 *
 *static int _mtk_os_hal_gpioif_free_irq(u8 group)
 *{
 *	 switch (group) {
 *	 case 0:
 *		 NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G0_CNT);
 *		 break;
 *	 case 1:
 *		 NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G1_CNT);
 *		 break;
 *	 case 2:
 *		 NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G2_CNT);
 *		 break;
 *	 case 3:
 *		 NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G3_CNT);
 *		 break;
 *	 case 4:
 *		 NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G4_CNT);
 *		 break;
 *	 case 5:
 *		 NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_GPIO_G5_CNT);
 *		 break;
 *	 }
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_read_gpio_event_count(u8 group, u32 *pvalue)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 mtk_mhal_gpioif_read_gpio_event_count(ctlr, pvalue);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_read_reset_val(u8 group, u32 *pvalue)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 mtk_mhal_gpioif_read_reset_val(ctlr, pvalue);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_read_low_limit_val(u8 group, u32 *pvalue)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 mtk_mhal_gpioif_read_low_limit_val(ctlr, pvalue);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_read_high_limit_val(u8 group, u32 *pvalue)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 mtk_mhal_gpioif_read_high_limit_val(ctlr, pvalue);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_capture_fifo_init(u8 group)
 *{
 *	 u32 i = 0;
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 for (i = 0; i < MAX_FIFO_ENTRY_CNT ; i++) {
 *		 ctlr->int_cnt->cap_fifo0[i] = 0;
 *		 ctlr->int_cnt->cap_fifo1[i] = 0;
 *	 }
 *	 ctlr->int_cnt->cap_fifo0_count = 0;
 *	 ctlr->int_cnt->cap_fifo1_count = 0;
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_interrupt_count_init(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 ctlr->int_cnt->int_bit = IRQ_GPIOIF_ALL_ENABLE;
 *	 ctlr->int_cnt->int_event_low_count = 0;
 *	 ctlr->int_cnt->int_event_high_count = 0;
 *	 ctlr->int_cnt->int_gpio2_rst_done_count = 0;
 *	 ctlr->int_cnt->int_event_over_count = 0;
 *	 ctlr->int_cnt->int_event_uf_count = 0;
 *	 ctlr->int_cnt->int_cap_f0_full_count = 0;
 *	 ctlr->int_cnt->int_cap_f1_full_count = 0;
 *	 ctlr->int_cnt->int_reset_cap_f0_full_count = 0;
 *	 ctlr->int_cnt->int_reset_cap_f1_full_count = 0;
 *	 ctlr->int_cnt->int_cap_f0_np_count = 0;
 *	 ctlr->int_cnt->int_cap_f1_np_count = 0;
 *	 ctlr->int_cnt->int_cap_f0_p_count = 0;
 *	 ctlr->int_cnt->int_cap_f1_p_count = 0;
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_set_direction_mode(
 *	 u8 group, u8 control_setting, u32 low_limit, u32 high_limit,
 *	 u32 reset_value, u8 clock_source)
 *{
 *	 mhal_gpioif_event_capture_setting setting;
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((control_setting > MAX_CTRL_SETTING)
 *		 || (clock_source >= MHAL_GPIOIF_CLOCK_MAX)
 *		 || (low_limit >= MTK_GPIOIF_MAX_VAL)
 *		 || (high_limit >= MTK_GPIOIF_MAX_VAL)
 *		 || (reset_value >= MTK_GPIOIF_MAX_VAL))
 *		 return -GPIOIF_EINVAL;
 *
 *	 setting.control_setting = control_setting;
 *	 setting.reset_value = reset_value;
 *	 setting.low_limit = low_limit;
 *	 setting.high_limit = high_limit;
 *	 ctlr->mconfig->setting = &setting;
 *	 ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
 *	 ctlr->mconfig->event_cnt_mode = MHAL_GPIOIF_DIRECTION_MODE;
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 ctlr->mconfig->mode = MHAL_GPIOIF_EVENT_COUNTER_MODE;
 *
 *	 mtk_mhal_gpioif_select_clock_source(ctlr);
 *	 mtk_mhal_gpioif_counter_clock_setting(ctlr, 0x1);
 *	 mtk_mhal_gpioif_event_counter_setting(ctlr);
 *	 mtk_mhal_gpioif_enable_event_counter(ctlr);
 *	 mtk_mhal_gpioif_software_reset(ctlr);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_set_updown_mode(
 *	 u8 group, u8 control_setting, u32 low_limit, u32 high_limit,
 *	 u32 reset_value, u8 clock_source)
 *{
 *	 mhal_gpioif_event_capture_setting setting;
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((control_setting > MAX_CTRL_SETTING)
 *		 || (clock_source >= MHAL_GPIOIF_CLOCK_MAX)
 *		 || (low_limit >= MTK_GPIOIF_MAX_VAL)
 *		 || (high_limit >= MTK_GPIOIF_MAX_VAL)
 *		 || (reset_value >= MTK_GPIOIF_MAX_VAL))
 *		 return -GPIOIF_EINVAL;
 *
 *	 setting.control_setting = control_setting;
 *	 setting.reset_value = reset_value;
 *	 setting.low_limit = low_limit;
 *	 setting.high_limit = high_limit;
 *	 ctlr->mconfig->setting = &setting;
 *	 ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
 *	 ctlr->mconfig->event_cnt_mode = MHAL_GPIOIF_UP_DOWN_MODE;
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 ctlr->mconfig->mode = MHAL_GPIOIF_EVENT_COUNTER_MODE;
 *
 *	 mtk_mhal_gpioif_select_clock_source(ctlr);
 *	 mtk_mhal_gpioif_counter_clock_setting(ctlr, 0x1);
 *	 mtk_mhal_gpioif_event_counter_setting(ctlr);
 *	 mtk_mhal_gpioif_enable_event_counter(ctlr);
 *	 mtk_mhal_gpioif_software_reset(ctlr);
 *
 *	 return 0;
 *}
 *int mtk_os_hal_gpioif_set_quadrature_mode(
 *	 u8 group, u8 control_setting, u32 low_limit,
 *	 u32 high_limit, u32 reset_value, u8 clock_source)
 *{
 *	 mhal_gpioif_event_capture_setting setting;
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((control_setting > MAX_CTRL_SETTING)
 *		 || (clock_source >= MHAL_GPIOIF_CLOCK_MAX)
 *		 || (low_limit > MTK_GPIOIF_MAX_VAL)
 *		 || (high_limit > MTK_GPIOIF_MAX_VAL)
 *		 || (reset_value > MTK_GPIOIF_MAX_VAL))
 *		 return -GPIOIF_EINVAL;
 *
 *	 setting.control_setting = control_setting;
 *	 setting.reset_value = reset_value;
 *	 setting.low_limit = low_limit;
 *	 setting.high_limit = high_limit;
 *	 ctlr->mconfig->setting = &setting;
 *	 ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
 *	 ctlr->mconfig->event_cnt_mode = MHAL_GPIOIF_QUA_MODE;
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 ctlr->mconfig->mode = MHAL_GPIOIF_EVENT_COUNTER_MODE;
 *
 *	 mtk_mhal_gpioif_select_clock_source(ctlr);
 *	 mtk_mhal_gpioif_counter_clock_setting(ctlr, 0x1);
 *	 mtk_mhal_gpioif_event_counter_setting(ctlr);
 *	 mtk_mhal_gpioif_enable_event_counter(ctlr);
 *	 mtk_mhal_gpioif_software_reset(ctlr);
 *
 *	 return 0;
 *}
 *int mtk_os_hal_gpioif_set_capture_mode(
 *	 u8 group, u8 edge_type_gpio_0, u8 edge_type_gpio_1, u8 clock_source)
 *{
 *	 mhal_gpioif_event_capture_setting setting;
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((clock_source >= MHAL_GPIOIF_CLOCK_MAX)
 *		 || (edge_type_gpio_0 >= MHAL_GPIOIF_GPIO_0_EDGE_TYPE_MAX)
 *		 || (edge_type_gpio_1 >= MHAL_GPIOIF_GPIO_1_EDGE_TYPE_MAX))
 *		 return -GPIOIF_EINVAL;
 *
 *	 setting.edge_type_gpio_0 = edge_type_gpio_0;
 *	 setting.edge_type_gpio_1 = edge_type_gpio_1;
 *	 ctlr->mconfig->setting = &setting;
 *	 ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 ctlr->mconfig->mode = MHAL_GPIOIF_CAP_COUNTER_MODE;
 *
 *	 mtk_mhal_gpioif_select_clock_source(ctlr);
 *	 mtk_mhal_gpioif_counter_clock_setting(ctlr, 0x1);
 *	 mtk_mhal_gpioif_capture_counter_setting(ctlr);
 *	 mtk_mhal_gpioif_enable_capture_counter(ctlr);
 *	 mtk_mhal_gpioif_software_reset(ctlr);
 *
 *	 return 0;
 *}
 *int mtk_os_hal_gpioif_interrupt_control(
 *	 u8 group, u8 enable, u8 clear, u32 bit)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((enable > 1) || (clear > 1) || (bit > MTK_GPIOIF_MAX_VAL))
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 if (enable)
 *		 mtk_mhal_gpioif_interrupt_bit_wise(ctlr,
 *			 bit, 0x1);
 *	 else
 *		 mtk_mhal_gpioif_interrupt_bit_wise(ctlr,
 *			 0x0, 0x0);
 *
 *	 if (clear) {
 *		 mtk_mhal_gpioif_interrupt_bit_wise(ctlr,
 *			 bit, 0x1);
 *		 mtk_mhal_gpioif_interrupt_bit_wise(ctlr,
 *			 0x0, 0x0);
 *	 }
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_interrupt_bit_wise(u8 group, u32 bit, u8 enable)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((enable > 1) || (bit > MTK_GPIOIF_MAX_VAL))
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 return mtk_mhal_gpioif_interrupt_bit_wise(ctlr, bit, enable);
 *}
 *
 *int mtk_os_hal_gpioif_limit_comparator(
 *	 u8 group, u8 sa_limit_v, u8 interrupt_limit_v)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((sa_limit_v >= MHAL_GPIOIF_SA_MAX)
 *		 || (interrupt_limit_v >= MHAL_GPIOIF_INTERRUPT_MAX))
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 ctlr->mconfig->sa_mode_lim =
 *		 (mhal_gpioif_sa_mode_limit_select)sa_limit_v;
 *	 ctlr->mconfig->inter_lim =
 *		 (mhal_gpioif_interrupt_limit_select)interrupt_limit_v;
 *
 *	 mtk_mhal_gpioif_limit_comparator(ctlr);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_de_glitch(
 *	 u8 group, u8 gpio, u8 enable, u32 min_p, u32 init_v)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((gpio > 2)  || (enable > 1) || (init_v > 1)
 *		 || (min_p > MAX_MIN_PURSE_WIDTH))
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 mtk_mhal_gpioif_de_glitch(ctlr, gpio,
 *		 enable, min_p, init_v);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_global_cr_reset(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 mtk_mhal_gpioif_global_reset(ctlr);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_hardware_reset(u8 group, u8 mode, u8 active_reset)
 *{
 *	 u32 value = 0;
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((active_reset > 1) || (mode >= MHAL_GPIOIF_MODE_MAX))
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 ctlr->mconfig->mode = (mhal_gpioif_mode)mode;
 *	 mtk_mhal_gpioif_hardware_reset_by_gpio_2(ctlr, active_reset);
 *
 *	 if (mode == MHAL_GPIOIF_EVENT_COUNTER_MODE) {
 *		 mtk_mhal_gpioif_read_gpio_event_count(ctlr,
 *		 &value);
 *		 printf("event counter value 0x%x\n", value);
 *	 } else if (mode == MHAL_GPIOIF_CAP_COUNTER_MODE) {
 *		 mtk_mhal_gpioif_read_gpio_cap_fifo0_value(ctlr,
 *		 &value);
 *		 printf("capture counter value 0x%x\n", value);
 *	 } else if (mode == MHAL_GPIOIF_EVENT_CAP_COUNTER_MODE) {
 *		 mtk_mhal_gpioif_read_gpio_event_count(ctlr,
 *		 &value);
 *		 printf("event counter value 0x%x\n", value);
 *		 mtk_mhal_gpioif_read_gpio_cap_fifo0_value(ctlr,
 *		 &value);
 *		 printf("capture counter fifo0 value 0x%x\n", value);
 *		 mtk_mhal_gpioif_read_gpio_cap_fifo1_value(ctlr,
 *		 &value);
 *		 printf("capture counter fifo1 value 0x%x\n", value);
 *	 }
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_select_clock_source(u8 group, u8 clock_source)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if (clock_source >= MHAL_GPIOIF_CLOCK_MAX)
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr->mconfig->clk = (mhal_gpioif_source_clock)clock_source;
 *	 mtk_mhal_gpioif_select_clock_source(ctlr);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_counter_clock_setting(u8 group, u8 enable)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if (enable > 1)
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *
 *	 mtk_mhal_gpioif_counter_clock_setting(ctlr, enable);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_software_reset(u8 group, u8 mode)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if (mode >= MHAL_GPIOIF_MODE_MAX)
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 ctlr->mconfig->mode = (mhal_gpioif_mode)mode;
 *
 *	 mtk_mhal_gpioif_software_reset(ctlr);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_enable_event_counter(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *
 *	 mtk_mhal_gpioif_enable_event_counter(ctlr);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_event_counter_setting(
 *	 u8 group, u8 control_setting, u32 low_limit, u32 high_limit,
 *	 u32 reset_value, u8 event_cnt_mode)
 *{
 *	 mhal_gpioif_event_capture_setting setting;
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if ((control_setting > MAX_CTRL_SETTING)
 *		 || (event_cnt_mode >= MHAL_GPIOIF_EVNET_COUNTER_MODE_MAX)
 *		 || (low_limit > MTK_GPIOIF_MAX_VAL)
 *		 || (high_limit > MTK_GPIOIF_MAX_VAL)
 *		 || (reset_value > MTK_GPIOIF_MAX_VAL))
 *		 return -GPIOIF_EINVAL;
 *
 *	 setting.control_setting = control_setting;
 *	 setting.reset_value = reset_value;
 *	 setting.low_limit = low_limit;
 *	 setting.high_limit = high_limit;
 *	 ctlr->mconfig->setting = &setting;
 *	 ctlr->mconfig->event_cnt_mode =
 *		 (mhal_gpioif_event_counter_mode)event_cnt_mode;
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *
 *	 mtk_mhal_gpioif_event_counter_setting(ctlr);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_read_gpio_cap_fifo0_value(u8 group, u32 *pvalue)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 return mtk_mhal_gpioif_read_gpio_cap_fifo0_value(ctlr, pvalue);
 *}
 *
 *int mtk_os_hal_gpioif_read_gpio_cap_fifo1_value(u8 group, u32 *pvalue)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *	 return mtk_mhal_gpioif_read_gpio_cap_fifo1_value(ctlr, pvalue);
 *}
 *
 *int mtk_os_hal_gpioif_dump_int(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 printf("int_cnt=0x%x\n", ctlr->int_cnt->int_bit);
 *	 printf("int_event_low_count=%d\n",
 *		 ctlr->int_cnt->int_event_low_count);
 *	 printf("int_event_high_count=%d\n",
 *		 ctlr->int_cnt->int_event_high_count);
 *	 printf("int_gpio2_rst_done_count=%d\n",
 *		 ctlr->int_cnt->int_gpio2_rst_done_count);
 *	 printf("int_event_over_count=%d\n",
 *		 ctlr->int_cnt->int_event_over_count);
 *	 printf("int_event_uf_count=%d\n",
 *		 ctlr->int_cnt->int_event_uf_count);
 *	 printf("int_cap_f0_full_count=%d\n",
 *		 ctlr->int_cnt->int_cap_f0_full_count);
 *	 printf("int_cap_f1_full_count=%d\n",
 *		 ctlr->int_cnt->int_cap_f1_full_count);
 *	 printf("int_reset_cap_f0_full_count=%d\n",
 *		 ctlr->int_cnt->int_reset_cap_f0_full_count);
 *	 printf("int_reset_cap_f1_full_count=%d\n",
 *		 ctlr->int_cnt->int_reset_cap_f1_full_count);
 *	 printf("int_cap_f0_np_count=%d\n",
 *		 ctlr->int_cnt->int_cap_f0_np_count);
 *	 printf("int_cap_f1_np_count=%d\n",
 *		 ctlr->int_cnt->int_cap_f1_np_count);
 *	 printf("int_cap_f0_p_count=%d\n",
 *		 ctlr->int_cnt->int_cap_f0_p_count);
 *	 printf("int_cap_f1_p_count =%d\n",
 *		 ctlr->int_cnt->int_cap_f1_p_count);
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_event_low_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_event_low_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_event_high_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_event_high_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_gpio2_rst_done_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_gpio2_rst_done_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_event_over_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_event_over_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_event_uf_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_event_uf_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_cap_f0_full_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_cap_f0_full_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_cap_f1_full_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_cap_f1_full_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_reset_cap_f0_full_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_reset_cap_f0_full_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_reset_cap_f1_full_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_reset_cap_f1_full_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_cap_f0_np_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_cap_f0_np_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_cap_f1_np_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_cap_f1_np_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_cap_f0_p_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_cap_f0_p_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_int_cap_f1_p_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->int_cap_f1_p_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_cap_fifo0_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->cap_fifo0_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_cap_fifo1_count(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 return ctlr->int_cnt->cap_fifo1_count;
 *}
 *
 *int mtk_os_hal_gpioif_get_cap_fifo0_val(u8 group, u32 idex)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if (idex >= MAX_FIFO_ENTRY_CNT)
 *		 return -GPIOIF_EINVAL;
 *
 *	 return ctlr->int_cnt->cap_fifo0[idex];
 *}
 *
 *int mtk_os_hal_gpioif_get_cap_fifo1_val(u8 group, u32 idex)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if (idex >= MAX_FIFO_ENTRY_CNT)
 *		 return -GPIOIF_EINVAL;
 *
 *	 return ctlr->int_cnt->cap_fifo1[idex];
 *}
 *
 *int mtk_os_hal_gpioif_ctlr_init(u8 group)
 *{
 *	 struct mtk_gpioif_controller_rtos *ctlr_rtos =
 *		 _mtk_os_hal_gpioif_get_ctlr(group);
 *	 struct mtk_gpioif_controller *ctlr;
 *
 *	 if (!ctlr_rtos)
 *		 return -GPIOIF_EINVAL;
 *	 ctlr = ctlr_rtos->ctlr;
 *
 *	 if (group >= MTK_GPIOIF_MAX_GRP_NUM)
 *		 return -GPIOIF_EINVAL;
 *
 *	 ctlr_rtos->ctlr = &g_gpioif_ctlr[group];
 *	 ctlr = ctlr_rtos->ctlr;
 *	 ctlr->base[group] = (void __iomem *)gpioif_base_addr[group];
 *	 ctlr->reg_clk_base = (void __iomem *)CM4_GPIOIF_CG_BASE;
 *	 ctlr->mconfig = &g_gpioif_mconfig[group];
 *	 ctlr->int_cnt = &g_gpioif_int_cnt[group];
 *	 ctlr->mconfig->group = (mhal_gpioif_group)group;
 *
 *	 mtk_os_hal_gpioif_interrupt_count_init(group);
 *	 mtk_os_hal_gpioif_capture_fifo_init(group);
 *
 *	 _mtk_os_hal_gpioif_request_irq(group);
 *
 *
 *	 return 0;
 *}
 *
 *int mtk_os_hal_gpioif_ctlr_deinit(u8 group)
 *{
 *
 *	 _mtk_os_hal_gpioif_free_irq(group);
 *
 *	 return 0;
 *}
 *
 *
 * @endcode
 *
 * - \b How \b to \b develop \b user \b application \b by \b using
 *    \b OS-HAL \b API: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *    @code
 *
 *	//1. Call this function to initialize GPIOIF HW
 *	mtk_os_hal_gpioif_ctlr_init(group);
 *    //2. Define user struct and interrupt callback function
 *	struct mtk_gpioif_test {
 *		u32 group;
 *	};
 *
 *	int mtk_os_hal_gpioif_int_callback(void *data)
 *	{
 *		struct mtk_gpioif_test *gpioif_test = data;
 *
 *		return 0;
 *	}
 *    - Set GPIOIF Event Counter Direction mode:
 *      -Call mtk_os_hal_gpioif_int_callback_register(3,
 *               mtk_os_hal_gpioif_int_callback, (void *)gpioif_test)
 *        to register callback function.
 *      -Call mtk_os_hal_gpioif_hardware_reset(group, mode, active_reset)
 *        to enable hardware reset of GPIOIF event counter.
 *      -Call mtk_os_hal_gpioif_de_glitch(group, gpio, enable, min_p, init_v)
 *        to add GPIOIF GPIO_0 & GPIO_1 & GPIO_2 of one group de-glitch setting.
 *	  -Call mtk_os_hal_gpioif_limit_comparator(group, sa_limit_v,
 *              interrupt_limit_v)
 *        to set different interrupt mode or saturation mode.
 *	  -Call mtk_os_hal_gpioif_set_direction_mode(group, control_setting,
 *        low_limit, high_limit, reset_value, clock_source)
 *        to set GPIOIF Direction mode setting.
 *	  -Call mtk_os_hal_gpioif_interrupt_control(group, enable, clear, bit)
 *        to clear all interrupt then enable all interrupt.
 *      -Input high/low singal to GPIOIF GPIO_0 and GPIO_1,
 *        the interrupt callback function will be called
 *        when interrupt is asserted.
 *
 *    - Set GPIOIF Event Counter Up Down mode:
 *      -Call mtk_os_hal_gpioif_int_callback_register(3,
 *              mtk_os_hal_gpioif_int_callback, (void *)gpioif_test)
 *        to register callback function.
 *      -Call mtk_os_hal_gpioif_hardware_reset(group, mode, active_reset)
 *        to enable hardware reset of GPIOIF event counter.
 *      -Call mtk_os_hal_gpioif_de_glitch(group, gpio, enable, min_p, init_v)
 *        to add GPIOIF GPIO_0 & GPIO_1 & GPIO_2 of one group de-glitch setting.
 *	  -Call mtk_os_hal_gpioif_limit_comparator(group, sa_limit_v,
 *              interrupt_limit_v)
 *        to set different interrupt mode or saturation mode.
 *	  -Call mtk_os_hal_gpioif_set_updown_mode(group, control_setting,
 *              low_limit, high_limit, reset_value, clock_source)
 *        to set GPIOIF Up Down mode setting.
 *	  -Call mtk_os_hal_gpioif_interrupt_control(group, enable, clear, bit)
 *        to clear all interrupt then enable all interrupt.
 *      -Input high/low singal to GPIOIF GPIO_0 and GPIO_1,
 *        the interrupt callback function will be called
 *        when interrupt is asserted.
 *
 *    - Set GPIOIF Event Counter Quadrature mode:
 *      -Call mtk_os_hal_gpioif_int_callback_register(3,
 *              mtk_os_hal_gpioif_int_callback, (void *)gpioif_test)
 *        to register callback function.
 *      -Call mtk_os_hal_gpioif_hardware_reset(group, mode, active_reset)
 *        to enable hardware reset of GPIOIF event counter.
 *      -Call mtk_os_hal_gpioif_de_glitch(group, gpio, enable, min_p, init_v)
 *        to add GPIOIF GPIO_0 & GPIO_1 & GPIO_2 of one group de-glitch setting.
 *	  -Call mtk_os_hal_gpioif_limit_comparator(group, sa_limit_v,
 *              interrupt_limit_v)
 *        to set different interrupt mode or saturation mode.
 *	  -Call mtk_os_hal_gpioif_set_quadrature_mode(group, control_setting,
 *              low_limit, high_limit, reset_value, clock_source)
 *        to set GPIOIF Quadrature mode setting.
 *	  -Call mtk_os_hal_gpioif_interrupt_control(group, enable, clear, bit)
 *        to clear all interrupt then enable all interrupt.
 *      -Input high/low singal to GPIOIF GPIO_0 and GPIO_1,
 *        the interrupt callback function will be called
 *        when interrupt is asserted.
 *
 *    - Set GPIOIF Capture Counter Mode:
 *      -Call mtk_os_hal_gpioif_int_callback_register(3,
 *              mtk_os_hal_gpioif_int_callback, (void *)gpioif_test)
 *        to register callback function.
 *      -Call mtk_os_hal_gpioif_hardware_reset(group, mode, active_reset)
 *        to enable hardware reset of GPIOIF event counter.
 *      -Call mtk_os_hal_gpioif_de_glitch(group, gpio, enable, min_p, init_v)
 *        to add GPIOIF GPIO_0 & GPIO_1 & GPIO_2 of one group de-glitch setting.
 *	  -Call mtk_os_hal_gpioif_set_capture_mode(group, edge_type_gpio_0,
 *              edge_type_gpio_1, clock_source)
 *        to set GPIOIF Capture Counter Mode setting.
 *	  -Call mtk_os_hal_gpioif_set_quadrature_mode(group, control_setting,
 *              low_limit, high_limit, reset_value, clock_source)
 *        to set GPIOIF Quadrature mode setting.
 *	  -Call mtk_os_hal_gpioif_interrupt_control(group, enable, clear, bit)
 *        to clear all interrupt then enable all interrupt.
 *      -Input high/low singal to GPIOIF GPIO_0 and GPIO_1,
 *        the interrupt callback function will be called
 *        when interrupt is asserted.
 *
 *    @endcode
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup GPIOIF
 * @{
 */

/** @defgroup driver_GPIOIF_define Define
  * @{
  * This section introduces the Macro definition.
  * It is used as GPIOIF M-HAL's API error return type
  * and GPIOIF parameter judgemental value.
  */

/** Indicate that a wrong parameter is given */
#define GPIOIF_EINVAL	1
/** Invalid argument, it means the pointer is NULL */
#define GPIOIF_EPTR		2

/** This defines GPIOIF Capture Mode maximum available FIFO entry counter */
#define MAX_FIFO_ENTRY_CNT	16

/** This defines maximum control edge type number of GPIOIF Event Counter */
#define MAX_CTRL_SETTING	6

/** This defines maximum minimum purse width of GPIO_0 de-glitch */
#define MAX_MIN_PURSE_WIDTH	0x3fff

/**  GPIOIF interrupt when event counter reaches low limit */
#define IRQ_GPIOIF_EVENT_C_R_LOW		(0)
/**  GPIOIF interrupt when event counter reaches highlimit */
#define IRQ_GPIOIF_EVENT_C_R_HIGH		(1)
/**  GPIOIF interrupt when GPIO_2 reset asserted */
#define IRQ_GPIOIF_RESET_DONE			(2)
/**  GPIOIF interrupt when event counter overflow */
#define IRQ_GPIOIF_EVENT_C_OVERFLOW		(8)
/**  GPIOIF interrupt when event counter underflow */
#define IRQ_GPIOIF_EVENT_C_UNDERFLOW	(9)
/**  GPIOIF interrupt when Writing capture value
  * to capture FIFO 0 which is full(GPIO_0)
  */
#define IRQ_GPIOIF_W_CFIFO0_WHEN_FULL	(12)
/**  GPIOIF interrupt when Writing capture value
  * to capture FIFO 1 which is full(GPIO_1)
  */
#define IRQ_GPIOIF_W_CFIFO1_WHEN_FULL	(13)
/**  GPIOIF interrupt when Writing GPIO_2 reset notification
  * to capture FIFO 0 which is full(GPIO_0)
  */
#define IRQ_GPIOIF_W_GPIO_2_RESET_CFIFO0_WHEN_FULL	(14)
/**  GPIOIF interrupt when Writing GPIO_2 reset notification
  * to capture FIFO 1 which is full(GPIO_1)
  */
#define IRQ_GPIOIF_W_GPIO_2_RESET_CFIFO1_WHEN_FULL	(15)
/**  GPIOIF interrupt when capture FIFO 0 which is not empty */
#define IRQ_GPIOIF_CFIFO0_NOT_EMPTY		(16)
/**  GPIOIF interrupt when capture FIFO 1 which is not empty */
#define IRQ_GPIOIF_CFIFO1_NOT_EMPTY		(17)
/**  GPIOIF interrupt when read capture FIFO 0 which is empty */
#define IRQ_GPIOIF_R_CFIFO0_WHENEMPTY	(24)
/**  GPIOIF interrupt when read capture FIFO 1 which is empty */
#define IRQ_GPIOIF_R_CFIFO1_WHENEMPTY	(25)

#define IRQ_GPIOIF_ALL_ENABLE	(1 << IRQ_GPIOIF_EVENT_C_R_LOW) |\
	(1 << IRQ_GPIOIF_EVENT_C_R_HIGH) |\
	(1 << IRQ_GPIOIF_RESET_DONE) |\
	(1 << IRQ_GPIOIF_EVENT_C_OVERFLOW) |\
	(1 << IRQ_GPIOIF_EVENT_C_UNDERFLOW) |\
	(1 << IRQ_GPIOIF_W_CFIFO0_WHEN_FULL) |\
	(1 << IRQ_GPIOIF_W_CFIFO1_WHEN_FULL) |\
	(1 << IRQ_GPIOIF_W_GPIO_2_RESET_CFIFO0_WHEN_FULL) |\
	(1 << IRQ_GPIOIF_W_GPIO_2_RESET_CFIFO1_WHEN_FULL) |\
	(1 << IRQ_GPIOIF_CFIFO0_NOT_EMPTY) |\
	(1 << IRQ_GPIOIF_CFIFO1_NOT_EMPTY) |\
	(1 << IRQ_GPIOIF_R_CFIFO0_WHENEMPTY) |\
	(1 << IRQ_GPIOIF_R_CFIFO1_WHENEMPTY)

/**
  * @}
  */

/** @defgroup driver_gpioif_enum Enum
  * @{
  * This section introduces the enumerations
  * that GPIOIF should configure when calling GPIOIF APIs.
  */

/**
 * @brief This enum defines GPIOIF clock source selection.\n
 */
typedef enum {
	/** GPIOIF Embedded XTAL KHz(26MHz) clock source */
	MHAL_GPIOIF_CLOCK_26MHZ = 0,
	/** GPIOIF Embedded 32KHz clock source */
	MHAL_GPIOIF_CLOCK_32KHZ = 1,
	/** GPIOIF AXI1 MHz(125MHz) clock source */
	MHAL_GPIOIF_CLOCK_125MHZ = 2,
	/** GPIOIF 200MHz clock source */
	MHAL_GPIOIF_CLOCK_200MHZ = 3,
	/**  GPIOIF maximum clock source <invalid> */
	MHAL_GPIOIF_CLOCK_MAX
} mhal_gpioif_source_clock;

/**
 * @brief This enum defines GPIOIF group selection.\n
 */
typedef enum {
	/** GPIOIF group0(gpio0~3) */
	MHAL_GPIOIF_GROUP_0 = 0,
	/** GPIOIF group1(gpio4~7) */
	MHAL_GPIOIF_GROUP_1 = 1,
	/** GPIOIF group2(gpio8~11) */
	MHAL_GPIOIF_GROUP_2 = 2,
	/** GPIOIF group3(gpio12~15) */
	MHAL_GPIOIF_GROUP_3 = 3,
	/** GPIOIF group4(gpio16~19) */
	MHAL_GPIOIF_GROUP_4 = 4,
	/** GPIOIF group5(gpio20~23) */
	MHAL_GPIOIF_GROUP_5 = 5,
	/**  GPIOIF maximum group <invalid> */
	MHAL_GPIOIF_GROUP_MAX
} mhal_gpioif_group;

/**
 * @brief This enum defines GPIOIF Event Counter Mode.\n
 */
typedef enum {
	/** GPIOIF Direction mode */
	MHAL_GPIOIF_DIRECTION_MODE = 0,
	/** GPIOIF Up Down mode */
	MHAL_GPIOIF_UP_DOWN_MODE = 1,
	/** GPIOIF Quadrature mode */
	MHAL_GPIOIF_QUA_MODE = 2,
	/**  GPIOIF maximum Event Counter Mode <invalid> */
	MHAL_GPIOIF_EVNET_COUNTER_MODE_MAX
} mhal_gpioif_event_counter_mode;

/**
 * @brief This enum defines GPIOIF counter mode which can support.\n
 */
typedef enum {
	/** GPIOIF Event Counter Mode */
	MHAL_GPIOIF_EVENT_COUNTER_MODE = 0,
	/** GPIOIF Capture Counter Mode */
	MHAL_GPIOIF_CAP_COUNTER_MODE = 1,
	/** GPIOIF Both Event Counter Mode and Capture Counter Mode */
	MHAL_GPIOIF_EVENT_CAP_COUNTER_MODE = 2,
	/**  GPIOIF maximum supported Mode  <invalid> */
	MHAL_GPIOIF_MODE_MAX
} mhal_gpioif_mode;

/**
 * @brief This enum defines GPIOIF saturation mode selection of
 *    GPIO event counter when reaching limit value .\n
 */
typedef enum {
	/** Not saturare on limit value */
	MHAL_GPIOIF_NOT_SA_LIMIT_V = 0,
	/** Saturare on low limit value */
	MHAL_GPIOIF_SA_LOW_LIMIT_V = 1,
	/** Saturare on high limit value */
	MHAL_GPIOIF_SA_HIGH_LIMIT_V = 2,
	/** Back to reset value when the counter reaches low limit value */
	MHAL_GPIOIF_SA_RESET_LOW_LIMIT_V = 3,
	/** Back to reset value when the counter reaches high limit value */
	MHAL_GPIOIF_SA_RESET_HIGH_LIMIT_V = 4,
	/**  GPIOIF maximum saturare mode limit selection <invalid> */
	MHAL_GPIOIF_SA_MAX
} mhal_gpioif_sa_mode_limit_select;


/**
 * @brief This enum defines GPIOIF interrupt mode selection of
 *    GPIO event counter.\n
 */
typedef enum {
	/** Not interrupt */
	MHAL_GPIOIF_NOT_INTERRUPT = 0,
	/** Interrupt issued when the counter reaches low limit value */
	MHAL_GPIOIF_INTERRUPT_LOW_LIMIT_V = 1,
	/** Interrupt issued when the counter reaches high limit value */
	MHAL_GPIOIF_INTERRUPT_HIGH_LIMIT_V = 2,
	/** Interrupt issued when the counter reaches
	* both low and high limit value
	*/
	MHAL_GPIOIF_INTERRUPT_BOTH_V = 3,
	/**  GPIOIF maximum interrupt mode limit selection <invalid> */
	MHAL_GPIOIF_INTERRUPT_MAX
} mhal_gpioif_interrupt_limit_select;

/**
 * @brief This enum defines the GPIOIF edge type of GPIO_0 capture.\n
 */
typedef enum {
	/** Capture on rising edge */
	MHAL_GPIOIF_GPIO_0_RISING = 0,
	/** Capture on falling edge */
	MHAL_GPIOIF_GPIO_0_FALLING = 1,
	/** Capture on bith rising or falling edge */
	MHAL_GPIOIF_GPIO_0_BOTH_RISING_FALLING = 2,
	/** Capture on rising edge(reserved) */
	MHAL_GPIOIF_GPIO_0_RISING_RESERVED = 3,
	/**  GPIOIF maximum edge type <invalid> */
	MHAL_GPIOIF_GPIO_0_EDGE_TYPE_MAX
} mhal_gpioif_gpio_0_edge_type;

/**
 * @brief This enum defines the GPIOIF edge type of GPIO_1 capture.\n
 */
typedef enum {
	/** Capture on rising edge */
	MHAL_GPIOIF_GPIO_1_RISING = 0,
	/** Capture on falling edge */
	MHAL_GPIOIF_GPIO_1_FALLING = 1,
	/** Capture on bith rising or falling edge */
	MHAL_GPIOIF_GPIO_1_BOTH_RISING_FALLING = 2,
	/** Capture on rising edge(reserved) */
	MHAL_GPIOIF_GPIO_1_RISING_RESERVED = 3,
	/**  GPIOIF maximum edge type <invalid> */
	MHAL_GPIOIF_GPIO_1_EDGE_TYPE_MAX
} mhal_gpioif_gpio_1_edge_type;

/**
 * @}
 */

/** @defgroup driver_gpioif_struct Struct
  * @{
  * This section introduces the structure used by
  * GPIOIF OS-HAL/M-HAL/HDL.
  */

/** @brief This structure defines the GPIOIF related parameters */
typedef struct {
	/** Control edge type of GPIOIF Event Counter(1~6) */
	u8 control_setting;
	/** Definition of increment/decremnet GPIO pin for GPIOIF Event Counter
	 * in quadrature mode, up down mode or direction mode(0/1)
	 */
	u8 updown;
	/** Reset value for GPIOIF Event Counter */
	u32 reset_value;
	/** Low limit value for GPIOIF Event Counter */
	u32 low_limit;
	/** High limit value for GPIOIF Event Counter */
	u32 high_limit;
	/** Define edge type of GPIO_0 capture(0~3) */
	u32 edge_type_gpio_0;
	/** Define edge type of GPIO_1 capture(0~3) */
	u32 edge_type_gpio_1;
} mhal_gpioif_event_capture_setting;

/** @brief The common configuration can be set for the GPIOIF HW. */
struct mtk_gpioif_config {
	/**  GPIOIF clock source */
	mhal_gpioif_source_clock clk;
	/** GPIOIF group */
	mhal_gpioif_group group;
	/** GPIOIF Event Counter Mode */
	mhal_gpioif_event_counter_mode event_cnt_mode;
	/** GPIOIF mode */
	mhal_gpioif_mode mode;
	/** GPIOIF saturation mode selection of
	* GPIO event counter when reaching limit value
	*/
	mhal_gpioif_sa_mode_limit_select sa_mode_lim;
	/** GPIOIF interrupt mode selection of GPIO event counter */
	mhal_gpioif_interrupt_limit_select inter_lim;
	/**  GPIOIF edge type of GPIO_0 */
	mhal_gpioif_gpio_0_edge_type gpio_0_edge_type;
	/**  GPIOIF edge type of GPIO_1 */
	mhal_gpioif_gpio_1_edge_type gpio_1_edge_type;
	/**  GPIOIF event and capture setting */
	mhal_gpioif_event_capture_setting *setting;
};

/** @brief This structure defines the GPIOIF interrupt count related
 * parameters
 */
struct mtk_gpioif_int_count {
	/**  GPIOIF interrupt bit value */
	u32 int_bit;
	/**  GPIOIF interrupt event low count value */
	u32 int_event_low_count;
	/**  GPIOIF interrupt event high count value */
	u32 int_event_high_count;
	/**  GPIOIF gpio2 interrupt reset done count value */
	u32 int_gpio2_rst_done_count;
	/**  GPIOIF interrupt event overflow count value */
	u32 int_event_over_count;
	/**  GPIOIF interrupt event underflow count value */
	u32 int_event_uf_count;
	/**  GPIOIF interrupt capture fifo0 count value */
	u32 int_cap_f0_full_count;
	/**  GPIOIF interrupt capture fifo1 count value */
	u32 int_cap_f1_full_count;
	/**  GPIOIF interrupt reset capture fifo0 count value */
	u32 int_reset_cap_f0_full_count;
	/**  GPIOIF interrupt reset capture fifo1 count value */
	u32 int_reset_cap_f1_full_count;
	/**  GPIOIF interrupt capture fifo0 not empty count value */
	u32 int_cap_f0_np_count;
	/**  GPIOIF interrupt capture fifo1 not empty count value */
	u32 int_cap_f1_np_count;
	/**  GPIOIF interrupt capture fifo0 empty count value */
	u32 int_cap_f0_p_count;
	/**  GPIOIF interrupt capture fifo1 empty count value */
	u32 int_cap_f1_p_count;
	/**  GPIOIF store capture fifo0 value */
	u32	cap_fifo0[MAX_FIFO_ENTRY_CNT];
	/**  GPIOIF store capture fifo1 value */
	u32	cap_fifo1[MAX_FIFO_ENTRY_CNT];
	/**  GPIOIF read capture fifo0 count value */
	u32	cap_fifo0_count;
	/**  GPIOIF read capture fifo1 count value */
	u32	cap_fifo1_count;
};


/** @brief This structure defines the GPIOIF related property*/
struct mtk_gpioif_controller {
	/** GPIOIF controller base address */
	void __iomem *base[MHAL_GPIOIF_GROUP_MAX];
	/** GPIOIF controller clk base address */
	void __iomem	*reg_clk_base;
	/** GPIOIF controller group0~group5 irq number */
	int  irq[MHAL_GPIOIF_GROUP_MAX];
	/** GPIOIF configure property*/
	struct mtk_gpioif_config *mconfig;
	struct mtk_gpioif_int_count *int_cnt;
};

/**
 * @}
 */

/** @defgroup driver_gpioif_function Function
  * @{
  * This section provides Fixed APIs(defined as Common Interface)
  * to fully control the MediaTek GPIOIF HW.
  */

/**
 * @brief  This function is used to select the GPIOIF clock source.
 * @brief Usage: OS-HAL driver should call it
 *    to select the GPIOIF clock source.
 * @param [in] ctlr : GPIOIF controller used with the device.
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_select_clock_source(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to set event counter mode setting
 * @brief Usage: OS-HAL driver should call it
 *    to set event counter mode setting.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_event_counter_setting(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to set capture mode counter setting
 * @brief Usage: OS-HAL driver should call it
 *    to set capture mode counter setting.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_capture_counter_setting(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to enable event counter mode.
 * @brief Usage: OS-HAL driver should call it
 *    to enable event counter mode.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_enable_event_counter(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to disable event counter mode.
 * @brief Usage: OS-HAL driver should call it
 *    to disable event counter mode.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_disable_event_counter(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to enable capture counter mode.
 * @brief Usage: OS-HAL driver should call it
 *    to enable capture counter mode.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_enable_capture_counter(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to disable capture counter mode.
 * @brief Usage: OS-HAL driver should call it
 *    to disable capture counter mode.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_disable_capture_counter(struct mtk_gpioif_controller *ctlr);


/**
 * @brief  This function is used to perform software reset of
 *    the GPIO event counter.
 * @brief Usage: OS-HAL driver should call it
 *    to perform software reset of the GPIO event counter.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_software_reset(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to enable/disable bit-wise interrupt.
 * @brief Usage: OS-HAL driver should call it
 *    to enable/disable interrupt by bit-wise.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [in] bit : Different bit , different interrupt case
 * @param [in] enable :
 *    0:No interrupt is issued when interrupt status is asserted;
 *    1:Interrupt is issued when interrupt status is asserted.
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_interrupt_bit_wise(
	struct mtk_gpioif_controller *ctlr, u32 bit, u8 enable);

/**
 * @brief  This function uses GPIO_2 pin to
 *    reset GPIOIF event counter.
 * @brief Usage: OS-HAL driver should call it
 *    to enable hardware reset of GPIOIF event counter.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [in] active_reset : 0: low; 1: high.
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_hardware_reset_by_gpio_2(
	struct mtk_gpioif_controller *ctlr, int active_reset);

/**
 * @brief This function is used to read the GPIO event counter value.
 * @brief Usage: OS-HAL driver should call it
 *    to read the GPIO event counter.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [out] pvalue : Read GPIO event counter value
 *    and save it to address pvalue
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_read_gpio_event_count(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue);

/**
 * @brief This function is used to read the GPIO capture fifo0 counter.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [out] pvalue : Read register value and save it to address pvalue
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_read_gpio_cap_fifo0_value(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue);

/**
 * @brief This function is used to read the GPIO capture fifo1 counter.
 * @brief Usage: OS-HAL driver should call it
 *    to read the capture fifo1 counter in the capture function mode.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [out] pvalue : Read register value and save it to address pvalue
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_read_gpio_cap_fifo1_value(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue);

/**
 * @brief  This function is used to set GPIOIF de-glitch setting.
 * @brief Usage: OS-HAL driver should call it
 *    to add GPIOIF de-glitch setting.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [in] gpio : GPIO number(0~2)
 * @param [in] enable : 0: disable;  1: enable.
 * @param [in] min_p :
 *    Minimum pulse width of the GPIOIF de-glitch circuit(0~16383)
 * @param [in] init_v :
 *    Initial output value of the GPIOIF de-glitch circuit(0 or 1)
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_de_glitch(
	struct mtk_gpioif_controller *ctlr, int gpio, int enable,
	unsigned int min_p, unsigned int init_v);

/**
 * @brief This function is used to set interrupt mode or
 *    saturation mode of the GPIO event counter.
 * @brief Usage: OS-HAL driver should call it in GPIO event counter
 *    to set different interrupt mode or saturation mode.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_limit_comparator(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to reset all GPIOIF group globally and
 *    make the register of the GPIOIF group to default value.
 * @brief Usage: OS-HAL driver should call it
 *    to enable GPIOIF global software reset.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_global_reset(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to enable/disable GPIOIF source clock.
 * @brief Usage: OS-HAL driver should call it
 *    to enable/disable GPIOIF counter clock source.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [in] enable : clock is enable or not(0:disable; 1:enable).
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */
int mtk_mhal_gpioif_counter_clock_setting(
	struct mtk_gpioif_controller *ctlr, int enable);

/**
 * @brief This function is used to handle GPIOIF group0~5 interrupt.
 * @brief Usage: OS-HAL driver should call it
 *    to handle GPIOIF group0~5 interrupt.
 * @param [in] ctlr : GPIOIF controller used with the device
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */

int mtk_mhal_gpioif_isr_handle(struct mtk_gpioif_controller *ctlr);

/**
 * @brief This function is used to read reset value of GPIOIF Event Counter.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [out] pvalue : Read register value and save it to address pvalue
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */

int mtk_mhal_gpioif_read_reset_val(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue);

/**
 * @brief This function is used to read low limit value of GPIOIF Event Counter.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [out] pvalue : Read register value and save it to address pvalue
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */

int mtk_mhal_gpioif_read_low_limit_val(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue);

/**
 * @brief This function is used to read high limit value
 *    of GPIOIF Event Counter.
 * @param [in] ctlr : GPIOIF controller used with the device
 * @param [out] pvalue : Read register value and save it to address pvalue
 *
 * @par       Example
 * See @ref MHAL_GPIOIF_Driver_Usage_Chapter to use this function.
 *
 * @return To indicate whether this function call is successful or not.\n
 *    If the return value is -#GPIOIF_EINVAL,
 *    it means that a wrong parameter is given,
 *    and the parameter must be verified.\n
 *    If the return value is 0, it means success.\n
 *
 */

int mtk_mhal_gpioif_read_high_limit_val(
	struct mtk_gpioif_controller *ctlr, unsigned int *pvalue);

/**
* @}
*/

/**
* @}
* @}
*/

#endif
