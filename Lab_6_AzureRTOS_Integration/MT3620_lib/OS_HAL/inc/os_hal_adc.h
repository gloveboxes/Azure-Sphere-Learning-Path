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


#ifndef __OS_HAL_ADC_H__
#define __OS_HAL_ADC_H__

#include "mhal_adc.h"

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup adc
 * @{
 * This section introduces the Analog-to-Digital Converter (ADC) APIs
 * including terms and acronyms, supported features,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section OS_HAL_ADC_Terms_Chapter Terms and Acronyms
 *
 * |Terms		   |Details		|
 * |------------|-------------------------------------|
 * |\b ADC		 | ADC is an Analog-to-Digital Converter.|
 * |\b DMA		 | Direct Memory Access.|
 * |\b FIFO		 | First In, First Out.|
 *
 * @section OS_HAL_ADC_Features_Chapter Supported Features
 * See @ref MHAL_ADC_Features_Chapter for the details of  Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup adc
 * @{
 * @section OS_HAL_ADC_Driver_Usage_Chapter How to use this driver
 *
 * - \b How \b to \b develop \b user \b application \b by \b using
 *	  \b OS-HAL \b API: \n
 *	- sample code (this is the user application sample code on freeRTos):
 *	  @code
 *
 *	  - ADC one shot mode:
 *
 *        -Initialize the ADC module.
 *            - Call mtk_os_hal_adc_ctlr_init(void)
 *
 *        //One shot mode will use fifo RX data, The user needs to pass
 *        //the data storage address to driver, HW will put sampling data
 *        //to this buf, Each sampling data takes 4 bytes, bit[3:0] is the
 *        //channel number information, bit[15:4] is the sample raw data,
 *        //bit[32:16] is debug info, no need to care.
 *        //When use one shot mode, please select
 *        //adc_fsm_parameter->fifo_mode = ADC_FIFO_DIRECT.
 *        -Set one shot state machine parameters.
 *            -Call mtk_os_hal_adc_fsm_param_set(
 *            struct adc_fsm_param *adc_fsm_parameter)
 *
 *        -Start the ADC module.
 *            - Call mtk_os_hal_adc_trigger_one_shot_once(void)
 *
 *        -ADC hw is no longer in use,  to return the ADC module back
 *        to its original state.
 *            - Call mtk_os_hal_adc_ctlr_deinit(void)
 *
 *
 *	  - ADC Periodic mode with DMA:
 *
 *        -Initialize the ADC module.
 *            - Call mtk_os_hal_adc_ctlr_init(void)
 *
 *        //Periodic mode will use DMA RX data, The user needs to pass
 *        //the data storage address to driver, HW will put sampling data
 *        //to this buf, Each sampling data takes 4 bytes, bit[3:0] is the
 *        //channel number information, bit[15:4] is the sample raw data,
 *        //bit[32:16] is debug info, no need to care.
 *        //When use periodic mode, please select
 *        //adc_fsm_parameter->fifo_mode = ADC_FIFO_DMA.
 *        -Set dma state machine parameters.
 *            - Call mtk_os_hal_adc_fsm_param_set(
 *            struct adc_fsm_param *adc_fsm_parameter)
 *
 *        -Start the ADC module.
 *            - Call mtk_os_hal_adc_period_start(void)
 *
 *        //There is no need to continue sampling data, stop the ADC.
 *        -Stop the ADC module.
 *            - Call mtk_os_hal_adc_period_stop(void)
 *
 *        -ADC hw is no longer in use,  to return the ADC module back
 *        to its original state.
 *            - Call mtk_os_hal_adc_ctlr_deinit(void)
 *	@endcode
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup adc
 * @{
 */

/** @defgroup os_hal_adc_function Function
  * @{
  *	OS-HAL ADC function list information, including
  *	ADC hardware initializing and de-initializing,
  *	enabling and disabling ADC hardware output,
  *	gets ADC sample data using one shot mode.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Init ADC controller.
 *
 *  @param none.
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#ADC_EPTR , it means ctlr is NULL.\n
 *	If return value is -#ADC_EPARAMETER , it means parameter invalid.
 */
int mtk_os_hal_adc_ctlr_init(void);

/**
 * @brief  Deinit ADC controller.
 *
 *  @param none.
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#ADC_EPTR , it means ctlr is NULL.\n
 *	If return value is -#ADC_EPARAMETER , it means parameter invalid.
 */
int mtk_os_hal_adc_ctlr_deinit(void);

/**
  * @brief  Configure  ADC controller parameters.
  * @param [in] adc_fsm_parameter : ADC parameter information.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.\n
  *  If return value is -#ADC_EPARAMETER , it means parameter invalid.
  */

int mtk_os_hal_adc_fsm_param_set(struct adc_fsm_param *adc_fsm_parameter);

/**
 * @brief  Start predefined channels to sample data with one shot mode.\n
 *	Once the function is called, ADC will be triggered once, and users can\n
 *	call this api multiple times to obtain data.\n
 *
 *  @param none.
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#ADC_EPTR , it means ctlr is NULL.\n
 *	If return value is -#ADC_EPARAMETER , it means parameter invalid.
 */
int mtk_os_hal_adc_trigger_one_shot_once(void);

/**
 * @brief  Start predefined channels to sample datas with period mode.
 *
 *  @param none.
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#ADC_EPTR , it means ctlr is NULL.\n
 *	If return value is -#ADC_EPARAMETER , it means parameter invalid.
 */
int mtk_os_hal_adc_period_start(void);

/**
 * @brief  Stop predefined channels to sample datas with period mode.
 *
 *  @param none.
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#ADC_EPTR , it means ctlr is NULL.\n
 *	If return value is -#ADC_EPARAMETER , it means parameter invalid.
 */
int mtk_os_hal_adc_period_stop(void);

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
