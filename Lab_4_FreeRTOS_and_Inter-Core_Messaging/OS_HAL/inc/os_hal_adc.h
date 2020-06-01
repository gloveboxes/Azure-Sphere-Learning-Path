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
 *
 *	  - ADC one shot mode:
 *
 *        -Initialize the ADC module.
 *            - Call mtk_os_hal_adc_ctlr_init(
 *            adc_pmode pmode,
 *            adc_fifo_mode fifo_mode,
 *            u16 bit_map)
 *
 *        -Set one shot state machine parameters.
 *            -Call mtk_os_hal_adc_fsm_param_set(
 *            struct adc_fsm_param *adc_fsm_parameter)
 *
 *        -Start the ADC module.
 *            - Call mtk_os_hal_adc_start(void) or
 *            mtk_os_hal_adc_start_ch(u16 ch_bit_map)
 *
 *        -Retrieve sample data for a channel.
 *            - Call mtk_os_hal_adc_one_shot_get_data(
 *            adc_channel sample_channel,
 *            u32 *data)
 *
 *        -ADC hw is no longer in use,  to return the ADC module back
 *        to its original state.
 *            - Call mtk_os_hal_adc_ctlr_deinit(void)
 *
 *
 *	  - ADC fifo mode:
 *
 *        -Initialize the ADC module.
 *            - Call mtk_os_hal_adc_ctlr_init(
 *            adc_pmode pmode,
 *            adc_fifo_mode fifo_mode,
 *            u16 bit_map)
 *
 *        -Set one shot state machine parameters.
 *            - Call mtk_os_hal_adc_fsm_param_set(
 *            struct adc_fsm_param *adc_fsm_parameter)
 *
 *        -Retrieve the sample data.
 *            - Call mtk_os_hal_adc_period_get_data(u32 (*rx_buf)[32],
 *            u32 *length)
 *
 *        -ADC hw is no longer in use,  to return the ADC module back
 *        to its original state.
 *            - Call mtk_os_hal_adc_ctlr_deinit(void)
 *
 *
 *	  - ADC dma mode:
 *
 *        -Initialize the ADC module.
 *            - Call mtk_os_hal_adc_ctlr_init(
 *            adc_pmode pmode,
 *            adc_fifo_mode fifo_mode,
 *            u16 bit_map)
 *
 *        -Set one shot state machine parameters.
 *            - Call mtk_os_hal_adc_fsm_param_set(
 *            struct adc_fsm_param *adc_fsm_parameter)
 *
 *        -Retrieve sample dat
 *            - Call mtk_os_hal_adc_period_get_data(u32 (*rx_buf)[32],
 *            u32 *length)
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
  *	gets ADC sample data using FIFO mode,
  *	gets ADC sample data using DMA mode,
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Init ADC controller.
 *
 *  @param pmode : ADC one shot or periodic mode select,
 *	0 one shot mode, 1 is  periodic mode.
 *  @param fifo_mode : ADC data transfer operation mode,
  *	0 is ADC_FIFO_DIRECT, 1 is DMA mode.
  *  @param bit_map :  ch_bit_map: ADC_CH_0 = 0x0001, ADC_CH_1 = 0x0002,
  *  ADC_CH_2 = 0x0004, ADC_CH_3 = 0x0008, ADC_CH_4 = 0x0010,
  *  ADC_CH_5 = 0x0020, ADC_CH_6 = 0x0040, ADC_CH_7 = 0x0080.\n
  *  For example: use ADC 0 and ADC1
  *  ch_bit_map = ADC_CH_0 | ADC_CH_1,\n
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#ADC_EPTR , it means ctlr is NULL.\n
 *	If return value is -#ADC_EPARAMETER , it means parameter invalid.
 */
int mtk_os_hal_adc_ctlr_init(adc_pmode pmode, adc_fifo_mode fifo_mode,
	u16 bit_map);

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
 * @brief  start predefined channels by ADC init API.
 *
 *  @param none.
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#ADC_EPTR , it means ctlr is NULL.\n
 *	If return value is -#ADC_EPARAMETER , it means parameter invalid.
 */
int mtk_os_hal_adc_start(void);

/**
 * @brief  start ADC controller by channel bit map info.
 *
 *  @param ch_bit_map: ADC_CH_0 = 0x0001, ADC_CH_1 = 0x0002,
 *	ADC_CH_2 = 0x0004, ADC_CH_3 = 0x0008, ADC_CH_4 = 0x0010,
 *	ADC_CH_5 = 0x0020, ADC_CH_6 = 0x0040, ADC_CH_7 = 0x0080.\n
 *	For example: use ADC 0 and ADC1
 *	ch_bit_map = ADC_CH_0 | ADC_CH_1,\n
 *
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#ADC_EPTR , it means ctlr is NULL.\n
 *	If return value is -#ADC_EPARAMETER , it means parameter invalid.
 */

int mtk_os_hal_adc_start_ch(u16 ch_bit_map);
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
  * @brief  This function is used to sample ADC channel voltage
  *  using one shot FIFO mode.
  * @param [in] sample_channel : ADC channel number(0~7).
  * @param [out] data : ADC sample data.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.\n
  *  If return value is -#ADC_EPARAMETER , it means parameter invalid.
  */

int mtk_os_hal_adc_one_shot_get_data(adc_channel sample_channel, u32 *data);
/**
  * @brief  This function is used to sample ADC channel voltage
  *  using period FIFO mode.
  * @param [out] rx_buf : Sampled data.
  * @param [out] length : Length of sample data.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.\n
  *  If return value is -#ADC_EPARAMETER , it means parameter invalid.
  */
int mtk_os_hal_adc_period_get_data(u32 (*rx_buf)[32], u32 *length);

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
