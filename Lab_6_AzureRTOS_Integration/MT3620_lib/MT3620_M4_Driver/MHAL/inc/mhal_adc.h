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


#ifndef __MHAL_ADC_H__
#define __MHAL_ADC_H__

#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup ADC
 * @{
 * This section describes the programming interfaces of the Analog-to-Digital
 * Converter (ADC) M-HAL driver, including terms and acronyms,
 * supported features, software architecture, details on how to
 * use this driver, ADC define, enums, functions, structures and typedef.
 *
 * @section MHAL_ADC_Terms_Chapter Terms and Acronyms
 *
 * |Terms		   |Details		|
 * |------------|-------------------------------------|
 * |\b ADC		 | ADC is an Analog-to-Digital Converter.|
 * |\b DMA		 | Direct Memory Access.|
 * |\b FIFO		 | First In, First Out.|
 *
 * @section MHAL_ADC_Features_Chapter Supported Features
 * The ADC is a Successive Approximation ADC and the resolution
 * of ADC is 12 bits.
 * - \b One \b Shot \b Mode: \n
 *   Get sample data using One shot mode.
 *
 * - \b Periodic \b Mode: \n
 *   Get sample data using period mode with DMA.
 *
 * @}
 * @}
 */

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup ADC
 * @{
 * @section MHAL_ADC_Driver_Usage_Chapter How to Use This Driver
 * - \b SW \b Architecture: \n
 *	See @ref MHAL_Overview_2_Chapter for details of the SW architecture.
 *	This section describes the definition of APIs and provides
 *	an example on FreeRTOS to show how to use these APIs to develop
 *	an OS-related driver.\n
 *
 * - \b The \b OS-HAL \b freeRTos \b driver\n
 *    \b sample \b code \b is \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_adc.c"> freeRTos ADC sample code on github </a>
 *
 * @}
 * @}
 */
/**
  * @addtogroup M-HAL
  * @{
  * @addtogroup ADC
  * @{
  */
/** @defgroup driver_adc_def Define
 * @{
 * M-HAL ADC define list information, maximum channel number that ADC
 * supports, ADC ring buffer size and error return value.
 */

#define ADC_EPTR		1
/**< Invalid argument, it means the pointer is NULL */
#define ADC_EPARAMETER	2
/**< Invalid argument, it means the parameter invalid */
#define ADC_ENOMEM		3
/**< Out of memory, it means memory malloc fail */
#define ADC_EFAULT		4
/**< Bad address */
#define ADC_EAGAIN		5
/**< Try again */

/**
  *@}
  */

/***************************************************************************
   * enums
   **************************************************************************/
/** @defgroup driver_adc_enum Enum
  * @{
  *	M-HAL ADC enumeration define list information, including
  *	define of the ADC one shot or periodic mode enumeration,
  *	define of the ADC finite state machine sampling times enumeration,
  *	define of the ADC operation mode enumeration(FIFO or DMA mode),
  *	define of the ADC channel number enumeration,
  *	define of the ADC interrupt enable enumeration.
  */
 /** @brief This enum defines ADC one shot or periodic mode */
typedef enum {
	ADC_PMODE_ONE_TIME = 0,
	/**< ADC one shot mode*/
	ADC_PMODE_PERIODIC
	/**< ADC periodic mode*/
} adc_pmode;

 /** @brief This enum defines ADC finite state machine sampling times */
typedef enum {
	ADC_AVG_1_SAMPLE = 0,
	/**< ADC state machine samples 1 time*/
	ADC_AVG_2_SAMPLE,
	/**< ADC state machine samples 2 times*/
	ADC_AVG_4_SAMPLE,
	/**< ADC state machine samples 4 times*/
	ADC_AVG_8_SAMPLE,
	/**< ADC state machine samples 8 times*/
	ADC_AVG_16_SAMPLE,
	/**< ADC state machine samples 16 times*/
	ADC_AVG_32_SAMPLE,
	/**< ADC state machine samples 32 times*/
	ADC_AVG_64_SAMPLE
	/**< ADC state machine samples 64 times*/
} adc_avg_mode;

 /** @brief This enum defines ADC operation mode: FIFO or DMA mode */
typedef enum {
	ADC_FIFO_DIRECT = 0,
	/**< ADC FIFO sample*/
	ADC_FIFO_DMA
	/**< ADC DMA sample*/
} adc_fifo_mode;

/** @brief This enum defines ADC interrupt enable*/
typedef enum {
	ADC_FIFO_IER_BOTH = 0,
	/**< ADC rxfull and  timeout interrupt enable */
	ADC_FIFO_IER_RXFULL,
	/**< ADC rxfull interrupt enable */
	ADC_FIFO_IER_TIMEOUT
	/**< ADC timeout interrupt enable */
} adc_fifo_ier_mode;

/** @brief This enum defines ADC channel bit map*/
typedef enum {
	ADC_BIT0 = 0x0001,
	/**< ADC channel0 bit map*/
	ADC_BIT1 = 0x0002,
	/**< ADC channel1 bit map*/
	ADC_BIT2 = 0x0004,
	/**< ADC channel2 bit map*/
	ADC_BIT3 = 0x0008,
	/**< ADC channel3 bit map*/
	ADC_BIT4 = 0x0010,
	/**< ADC channel4 bit map*/
	ADC_BIT5 = 0x0020,
	/**< ADC channel5 bit map*/
	ADC_BIT6 = 0x0040,
	/**< ADC channel6 bit map*/
	ADC_BIT7 = 0x0080,
	/**< ADC channel7 bit map*/
} adc_channel_bit_map;

/** @brief This enum defines ADC channel number*/
typedef enum {
	ADC_CHANNEL_0 = 0,
	/**< ADC channel 0 */
	ADC_CHANNEL_1 = 1,
	/**< ADC channel 1 */
	ADC_CHANNEL_2 = 2,
	/**< ADC channel 2 */
	ADC_CHANNEL_3 = 3,
	/**< ADC channel 3 */
	ADC_CHANNEL_4 = 4,
	/**< ADC channel 4 */
	ADC_CHANNEL_5 = 5,
	/**< ADC channel 5*/
	ADC_CHANNEL_6 = 6,
	/**< ADC channel 6*/
	ADC_CHANNEL_7 = 7,
	/**< ADC channel 7*/
	ADC_CHANNEL_MAX
	/**< ADC max channel(invalid) */
} adc_channel;

/**
  * @}
  */

/** @defgroup driver_adc_typedef Typedef
  * @{
  *	M-HAL ADC typedef define list information, including
  *	define of the ADC OS-HAL user's callback API typedef.
  */

/** @brief This defines the callback function prototype.
  * @brief	   Usage: OS-HAL driver can use this function to register
  * callback function.
  */

typedef void (*adc_dma_callback_func) (void *user_data);

/**
  * @}
  */
/** @defgroup driver_adc_struct Struct
  * @{
  *	M-HAL ADC structure list information, including
  *	ADC controller state machine related parameters,
  *	ADC DMA mode related parameters,
  *	ADC controller structure which used to store the hardware register
  *	base address, clock, Rx done callback API.
  */

/** @brief This structure defines ADC related parameters,\n
  * such as ADC mode, sample times, channel map, and interrupt mode.
  */
struct adc_fsm_param {
	adc_pmode	pmode;
	/**< ADC mode, one shot or periodic mode.*/
	u16 channel_map;
	/**< ADC bit map, REG_CH_MAP[7:0]=8'0000_0011, enable CH0 & CH1*/
	u32 sample_rate;
	/**< Sample rate used in periodic mode, for example, the sampling\n
	 * rate can be set as 10K, ch_num * sample_rate <= 90K.\n
	 * Please note due to the sample_rate calculation formula, there\n
	 * might be some deviation. The following is the exact value:\n
	 *     Configuration Value : Real Sampling Rate (when 1 channel)\n
	 *     48000 : 48.78KHz\n
	 *     44100 : 44.44KHz\n
	 *     32000 : 32.26KHz\n
	 *     24000 : 24.10KHz\n
	 *     22050 : 22.22KHz\n
	 *     16000 : 16.00KHz\n
	 *     12000 : 12.05KHz\n
	 *     11025 : 11.05KHz\n
	 *     10000 : 10.00KHz\n
	 *      8000 :  8.00KHz\n
	 *      1000 :  1.00KHz\n
	 *       100 :  0.10KHz\n
	 *        10 :  0.01KHz\n
	 *         1 :  0.001KHz\n
	 * The calculation formula of sample_rate(TS) is as follows:\n
	 * TS = ADC_CLOCK/{(REG_T_INIT  + 1) + [((REG_T_CH + 2) +\n
	 * (avg_num + 1))] * ch_num + (REG_PERIOD + 1)}\n
	 * ADC_CLOCK select 2M, ADC initial stable time(REG_T_INIT) set as 20,\n
	 * channel stable time(REG_T_CH) set as 8, sample average number(avg_num)\n
	 * can select be set to 1, 2, 4, 8, 16, 32, 64. (REG_PERIOD)ADC clock\n
	 * cycle count period, unit as clock cycle, max value 0xFFFFFFFF,\n
	 * 1 < ch_num < 8.
	 */
	adc_fifo_mode fifo_mode;
	/**< ADC operation mode: FIFO or DMA mode, one shot mode use\n
	 * ADC_FIFO_DIRECT, periodic mode use ADC_FIFO_DMA.
	 */
	u32 *vfifo_addr;
	/**< ADC Virtual FIFO address for periodic or one shot mode,\n
	  * record the data storage the SRAM address passed by user, HW will\n
	  * put sampling data to this buffer.\n
	  * Each sampling data takes 4 bytes, bit[3:0] is the channel\n
	  * number information, bit[15:4] is the sample raw data, user can use\n
	  * the following formula to convert raw data to voltage:\n
	  * voltage = raw data * 2500 / 4096, val bit[32:16] is debug info,\n
	  * no need to care.
	  */
	u32 vfifo_len;
	/**< ADC Virtual FIFO length.\n
	  * Periodic Mode: vfifo_len is the total number of the ADC sampling data\n
	  *     ex, vfifo_len=16 means the buffer could store totally 16 ADC sampling data (16x4=64 Bytes)\n
	  *     ex, vfifo_len=1024 means the buffer could store totally 1024 ADC sampling data (1024x4=4096 Bytes)\n
	  * One Shot Mode: vfifo_len is the number of enabled channels
	  */
	u32 rx_period_len;
	/**< ADC rx period length.\n
	  * Periodic Mode: rx_period_len is the duration for ADC driver to invoke RTApp callback function.\n
	  *     ex, rx_period_len=4 means the callback function is invoked whenever 4 sampling data captured.\n
	  *     ex, rx_period_len=256 means the callback function is invoked whenever 256 sampling data captured.\n
	  * One Shot Mode: rx_period_len is the number of enabled channels.
	  */
	adc_fifo_ier_mode ier_mode;
	/**< ADC interrupt mode, config ADC_FIFO_IER_RXFULL irq enable */
	adc_dma_callback_func rx_callback_func;
	/**< RX DMA callback function */
	void *rx_callback_data;
	/**< RX DMA callback data */
};

/** @brief Interface to ADC, used to store the hardware register base address,\n
 * clock, Rx done callback API, etc.
 */
struct mtk_adc_controller {
	/** ADC controller base address */
	void __iomem *base;
	/** Clock gate base address */
	void __iomem *cg_base;
	/** ADC fsm related parameters */
	struct adc_fsm_param *adc_fsm_parameter;
	/** ADC hardware is sampling data */
	u16 adc_processing;
	/** Rx DMA physical address */
	dma_addr_t	rx_addr;
	/** ADC DMA channel number */
	u8 dma_channel;
};

/**
  * @}
  */


/** @defgroup driver_adc_function Function
  * @{
  *	M-HAL ADC function list information,
  *	ADC hardware initializing and de-initializing,
  *	setting and getting ADC parameters set,
  *	enabling and disabling ADC hardware output,
  *	gets ADC sample data using FIFO mode,
  *	gets ADC sample data using DMA mode,
  *	enabling and disabling clock.
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  This function is used to initialize an ADC hardware controller.
  * @brief Usage: The main sequence is as bellow:\n
  *  1.Call this function after allocating ADC controller.\n
  *  2.Before operating ADC hardware, this API should be called firstly.
  * @param [in] ctlr : Abstract an ADC controller.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.
  */
int mtk_mhal_adc_init(struct mtk_adc_controller *ctlr);

/**
  * @brief  This function is used to de-initialize an ADC\n
  *  hardware controller.
  * @brief Usage: This function should be called before releasing ADC\n
  *  controller.
  * @param [in] ctlr : Abstract an ADC controller.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.
  */
int mtk_mhal_adc_deinit(struct mtk_adc_controller *ctlr);
/**
  * @brief  This function is used to configure  ADC controller parameters.
  * @brief Usage: OS-HAL calls this API to set ADC hardware,\n
  *  and this function should be called before ADC start or\n
  *  ADC start channel.
  * @param [in] ctlr : Abstract an ADC controller.
  * @param [in] adc_fsm_parameter : ADC parameter information.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.\n
  *  If return value is -#ADC_EPARAMETER , it means parameter invalid.
  */

int mtk_mhal_adc_fsm_param_set(struct mtk_adc_controller *ctlr,
		struct adc_fsm_param *adc_fsm_parameter);

/**
  * @brief This function is used to start the predefined channels by ADC\n
  *  parameter configre API, please refer to the structure\n
  *  channel_map for detail info.
  * @brief Usage: OS-HAL calls this API to start ADC hardware,\n
  *  and this function should be called after setting ADC parameters.
  * @param [in] ctlr : Abstract an ADC controller.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.
  */
int mtk_mhal_adc_start(struct mtk_adc_controller *ctlr);

/**
  * @brief  This function is used to stop ADC controller.
  * @brief Usage: The main sequence of the API is as bellow:\n
  *  1.This function should be called after setting ADC parameters.\n
  *  2.This function should be called before ADC de-initializing.
  * @param [in] ctlr : Abstract an ADC controller.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.
  */
int mtk_mhal_adc_stop(struct mtk_adc_controller *ctlr);

/**
 * @brief This function is used to enable ADC clock.
 * @brief Usage: It must be called before operating hardware;\n
 *	otherwise, the access to ADC register will fail. The function should\n
 *	be called in the sequence as bellow:\n
 *	alloc_controller-> enable_clk->adc_init.
 * @param [in] ctlr :  Abstract an ADC controller.
 * @return
 *	If return value is 0, it means success.\n
 *	If return value is -#ADC_EPTR , it means ctlr is NULL.
 */
int mtk_mhal_adc_enable_clk(struct mtk_adc_controller *ctlr);

/**
  * @brief This function is used to disable ADC clock.
  * @brief Usage: When Hardware is not in use, advise to disable clock.
  * @param [in] ctlr :  Abstract an ADC controller.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.
 */
int mtk_mhal_adc_disable_clk(struct mtk_adc_controller *ctlr);
/**
  * @brief This function is used to read FIFO data from HW.
  * @brief Usage: It must be called in irq function with FIFO mode.
  * @param [in] ctlr :  Abstract an ADC controller.
  * @return
  *  If return value is 0, it means success.\n
  *  If return value is -#ADC_EPTR , it means ctlr is NULL.
 */
int mtk_mhal_adc_fifo_handle_rx(struct mtk_adc_controller *ctlr);

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

/**
* @}
* @}
*/
#endif /* __MHAL_ADC_H__ */

