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

#ifndef __MHAL_I2S_H__
#define __MHAL_I2S_H__

#include "mhal_osai.h"

/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup I2S
 * @{
 * This section introduces the Integrated Interchip Sound (I2S) APIs including
 * terms and acronyms, supported features, software architecture,
 * details on how to use this driver, enums, structures and functions.
 *
 * @section Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b DMA                        | Direct Memory Access.|
 * |\b FIFO                       | First In, First Out.|
 * |\b ISR                        | Interrupt Service Routine.|
 * |\b I2S                        | Integrated Interchip Sound.|
 *
 * @section MHAL_I2S_Features_Chapter Supported Features
 * - Two I2S interfaces share the same XPLL clock source.\n
 * - Slave mode, only used to communicate with the external codec.\n
 * - 16MHz master clock provided.\n
 * - Two modes supported.\n
 *     - I2S slave mode: Standard I2S protocol.\n
 *         - Stereo (2-ch)/Mono (1-ch) supported.\n
 *         - 16-bit word length supported.\n
 *         - 8/12/16/24/32/48 kHz sample rate supported.\n
 *     - TDM slave mode: TDM protocol.\n
 *         - 32/64/128 bit clocks in each FS cycle supported.\n
 *         - 4/2 channels supported.\n
 *         - 16-bit word length supported.\n
 *         - 8/12/16/24/32/48 kHz sample rate supported.\n
 *         - MSB offset: 0 ~ 127.\n
 * - For more detail, please refer to the I2S datasheet.\n
 * @}
 * @}
 */
/**
 * @addtogroup M-HAL
 * @{
 * @addtogroup I2S
 * @{
 *
 * @section MHAL_I2S_Driver_Usage_Chapter How to Use This Driver
 *
 * - Use I2S transmit/receive (Tx/Rx) data for external codec
 *   functionality. \n
 *   The users can set the initial type to "MHAL_I2S_TYPE_EXTERNAL_MODE"
 *   in mtk_mhal_i2s_cfg_type() function when connecting an external
 *   codec component. \n
 *   An interrupt will be triggered after the data transfer, using the I2S
 *   interface, is completed. A user-defined callback function is called to
 *    handle data transfer in the I2S interrupt service routine. \n
 *  - Step1:   Call #mtk_mhal_i2s_alloc_vfifo_ch to allocate DMA channel. \n
 *  - Step2:   Call #mtk_mhal_i2s_cfg_type() to configure the I2S type. \n
 *  - Step3:   Call #mtk_mhal_i2s_set_config() to configure the I2S settings.\n
 *  - Step4:   Call #mtk_mhal_i2s_cfg_tx_dma_irq_enable() and
 *             #mtk_mhal_i2s_cfg_rx_dma_irq_enable() to
 *             configure DMA interrupt and register the callback functions. \n
 *  - Step5:   Call #mtk_mhal_i2s_start_tx_vfifo() and
 *             #mtk_mhal_i2s_start_rx_vfifo() to configure and start Tx
 *             and Rx Virtual FIFO. \n
 *  - Step6:   Call #mtk_mhal_i2s_enable_audio_top(),
 *             #mtk_mhal_i2s_enable_tx() and #mtk_mhal_i2s_enable_rx()
 *             functions to start Tx and Rx link data flows. \n
 *
 * - Event handling for Tx and Rx links data transfer operations.\n
 *  - The callback functions of Tx and Rx links are always invoked
 *    in the DMA Interrupt Service Routine (ISR).
 *    It is recommended to send the events of Tx and
 *    Rx links to user-defined media task in callback functions and
 *    handle the corresponding data operations in the media task to
 *    avoid long operations in the ISR. \n
 *  - Sample code below is shown to explain how to handle events in
 *    the user-defined media task. \n
 *    @code
 *	void _mtk_os_hal_i2s0_tx_callback(void *data)
 *	{
 *		mtk_mhal_i2s_move_tx_point(&i2s0_ctlr_cfg.i2s_ctrl,
 *					   i2s0_ctlr_cfg.tx_period_len);
 *		if (i2s0_ctlr_cfg.tx_callback_func != NULL)
 *			i2s0_ctlr_cfg.tx_callback_func(
 *				i2s0_ctlr_cfg.tx_callback_data);
 *		// i2s0_ctlr_cfg.tx_callback_func(NULL) is user application.
 *		// This function triger a event to resume a handle task.
 *		// The handle task will fill data to next period size buffer.
 *	}
 *	void _mtk_os_hal_i2s0_rx_callback(void *data)
 *	{
 *		mtk_mhal_i2s_move_rx_point(&i2s0_ctlr_cfg.i2s_ctrl,
 *					   i2s0_ctlr_cfg.rx_period_len);
 *		if (i2s0_ctlr_cfg.rx_callback_func != NULL)
 *			i2s0_ctlr_cfg.rx_callback_func(
 *				i2s0_ctlr_cfg.rx_callback_data);
 *		// i2s0_ctlr_cfg.rx_callback_func(NULL) is user application.
 *		// This function triger a event to resume a handle task.
 *		// The handle task will get data on this period size buffer.
 *	}
 *    @endcode
 *
 * - \b The \b OS-HAL \b freeRTos \b driver\n
 * \b sample \b code \b is \b as \b follows: \n
 * <a href="https://github.com/MediaTek-Labs/mt3620_m4_software/blob/master/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_i2s.c"> freeRTos I2S sample code on github </a>
 *
 * @section MHAL_I2S_Function_Group_Chapter I2S Function Groups Description
 *
 * The APIs are grouped by their functionality for easier use.
 * Users can refer to the details of each function by clicking
 * the function name.
 * - \b Initialization \b and \b configuration \n
 *   The #mtk_mhal_i2s_cfg_type() function configures the I2S type, which should
 *   be set at the beginning of an I2S operation.\n
 *   The #mtk_mhal_i2s_reset() function, conversely, resets the I2S HW
 *   configuration. \n
 *   The #mtk_mhal_i2s_set_config() function configures the I2S settings
 *   providing the respective sample rate and channel number for Tx and
 *   Rx links as well as the clock mode for the whole I2S. The users should
 *   configure settings before enabling the I2S functions. \n
 *   The #mtk_mhal_i2s_start_tx_vfifo() function configures DMA Tx VFIFO \n
 *   The #mtk_mhal_i2s_start_rx_vfifo() function configures DMA Rx VFIFO \n
 *   The functions are as follows: \n
 *   - #mtk_mhal_i2s_cfg_type()
 *   - #mtk_mhal_i2s_reset()
 *   - #mtk_mhal_i2s_set_config()
 *   - #mtk_mhal_i2s_start_tx_vfifo()
 *   - #mtk_mhal_i2s_start_rx_vfifo()
 *
 * - \b Callback \b register \b functions \n
 *   Use the following functions to configure interrupt enable and
 *   register callback functions for Tx and Rx link data operation. \n
 *   These functions include: \n
 *  - #mtk_mhal_i2s_cfg_tx_dma_irq_enable()
 *  - #mtk_mhal_i2s_cfg_tx_dma_irq_enable()
 *
 * - \b Enable \b and \b disable \b functions \n
 *   Call #mtk_mhal_i2s_enable_tx() function to enable the I2S Tx link
 *   to transmit the data. \n
 *   Call #mtk_mhal_i2s_disable_tx() function to disable the I2S Tx data
 *   transmission immediately. \n
 *   Call #mtk_mhal_i2s_enable_rx() function to enable the I2S Rx link
 *   to receive data. \n
 *   Call #mtk_mhal_i2s_disable_rx() function to disable the I2S Rx link
 *   immediately. \n
 *   Call #mtk_mhal_i2s_enable_audio_top() function to enable the I2S
 *   audio top immediately. \n
 *   Call #mtk_mhal_i2s_disable_audio_top() function to disable the
 *   I2S audio top immediately. \n
 *   Call #mtk_mhal_i2s_stop_rx_vfifo() function to disable DMA Rx VFIFO. \n
 *   Call #mtk_mhal_i2s_stop_tx_vfifo() function to disable DMA Tx VFIFO. \n
 *   The functions are as follows: \n
 *   - #mtk_mhal_i2s_enable_tx()
 *   - #mtk_mhal_i2s_disable_tx()
 *   - #mtk_mhal_i2s_enable_rx()
 *   - #mtk_mhal_i2s_disable_rx()
 *   - #mtk_mhal_i2s_enable_audio_top()
 *   - #mtk_mhal_i2s_disable_audio_top()
 *   - #mtk_mhal_i2s_stop_rx_vfifo()
 *   - #mtk_mhal_i2s_stop_tx_vfifo()
 */
/** @defgroup driver_i2s_typedef Typedef
  * @{
  * This section introduces the typedef that I2S M-HAL used.
  */

/** @brief	This defines the callback function prototype.
* @brief     Usage: OS-HAL driver can use this function to register callback
 *            function.\n
 *            For the use of the DMA interrupt handle,
 *            please refer to the section -
 *            @ref Driver_Usage_Chapter.
 */

typedef void (*i2s_dma_callback_func) (void *user_data);

/**
  * @}
  */

/** @defgroup driver_i2s_define Define
  * @{
  * This section introduces the Macro definition
  * which is used as I2S M-HAL's API error return type.
  */

/** Invalid argument, it means the transfer length isn't supported. */
#define I2S_ELENGTH		2
/** Invalid argument, it means a wrong parameter is given. */
#define I2S_EPTR		1

/**
  * @}
  */

/** @defgroup hal_i2s_enum Enum
 * @{
 * This section introduces the enumerations
 * that I2S should configure before performing transfer.
*/

/** @brief I2S link channel per sample.
 * Before I2S transfer, users should make sure that the parameter is defined
 * in hal_i2s_link_config structure and configured through
 * mtk_mhal_i2s_set_config() and just used in TDM mode.
 */
typedef enum {
	/** Two channels in each frame sync cycle. */
	MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_2 = 0,
	/** Four channels in each frame sync cycle. */
	MHAL_I2S_LINK_CHANNLE_PER_SAMPLE_4 = 1
} hal_i2s_link_channel_per_sample;


/** @brief I2S bits per sample.
 * Before I2S transfer, users should make sure that the parameter is defined
 * in hal_i2s_link_config structure and configured through
 * mtk_mhal_i2s_set_config().
 */
typedef enum {
	/** 32 bits in each frame sync cycle. */
	MHAL_I2S_BITS_PER_SAMPLE_32 = 0,
	/** 64 bits in each frame sync cycle. */
	MHAL_I2S_BITS_PER_SAMPLE_64 = 1,
	/** 128 bits in each frame sync cycle. */
	MHAL_I2S_BITS_PER_SAMPLE_128 = 2
} hal_i2s_link_bits_sample;


/** @brief Enable or disable the right channel of I2S Tx sending the same data
 * as the left channel of I2S Tx.
 * Before I2S transfer, users should make sure that the parameter is defined
 * in hal_i2s_link_config structure and configured through
 * mtk_mhal_i2s_set_config().
 */
typedef enum {
	/** The right channel of Tx sends all 0 */
	MHAL_I2S_TX_MONO_DUPLICATE_DISABLE = 0,
	/** The right channel of Tx sends the same data as
	 * the left channelof I2S Tx
	 */
	MHAL_I2S_TX_MONO_DUPLICATE_ENABLE = 1
} hal_i2s_tx_mode;


/** @brief Enable or disable 2x down sample rate mode in the I2S Rx link.
 *   There is a mode that the sample rate of I2S Tx is 48kHz and
 *   the sample rate of I2S Rx is 24kHz. And in this mode, I2S Rx will
 *   drop one sample in each two input samples automatically.
 * Before I2S transfer, users should make sure the parameter is defined
 * in hal_i2s_link_config structure and configured through
 * mtk_mhal_i2s_set_config().
 */
typedef enum {
	/** Real sample rate of I2S Rx =  sample rate */
	MHAL_I2S_RX_DOWN_RATE_DISABLE = 0,
	/** Real sample rate of I2S Rx is 1/2 of the sample rate*/
	MHAL_I2S_RX_DOWN_RATE_ENABLE = 1
} hal_i2s_rx_down_rate;


/** @brief I2S channel number define.
 * Before I2S transfer, users should make sure that the parameter is defined
 * in hal_i2s_link_config structure and configured through
 * mtk_mhal_i2s_set_config().
 */
typedef enum {
	/** This value means one channel  */
	MHAL_I2S_MONO = 0,
	/** This value means two channels */
	MHAL_I2S_STEREO = 1
} hal_i2s_channel_number;

/** @brief I2S port number.
 * This parameter is defined in the mtk_i2s_ctlr structure,
 * which is used by the user to specify the I2S port.
 */
typedef enum {
	/** I2S port is i2s0 */
	MHAL_I2S0 = 0,
	/** I2S port is i2s1 */
	MHAL_I2S1 = 1
} i2s_no;
/** @brief Function enable/disable.
 * Before I2S transfer, users should make sure that the parameter is defined
 * in hal_i2s_link_config structure, which is used in lr_swap
 * and word_select_inverse.
 * They are configured through mtk_mhal_i2s_set_config().
 */
typedef enum {
	/** Function disable */
	MHAL_FN_DIS = 0,
	/** Function enable */
	MHAL_FN_EN = 1
} i2s_fnen;
/** @brief This enum defines the initial type of I2S.
 * Before I2S transfer, users should use the parameter in
 * mtk_mhal_i2s_cfg_type() function to set I2S HW initial parameter
 */

typedef enum {
	/** I2S external mode */
	MHAL_I2S_TYPE_EXTERNAL_MODE = 0,
	/** I2S external TDM mode */
	MHAL_I2S_TYPE_EXTERNAL_TDM_MODE = 1,
	/** I2S internal loopback mode */
	MHAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE = 2
} hal_i2s_initial_type;

/** @brief I2S sample rate define.
 * Before I2S transfer, users should make sure that the parameter is defined
 * in hal_i2s_link_config structure and configured through
 * mtk_mhal_i2s_set_config().
 */
typedef enum {
	/**  8000Hz  */
	MHAL_I2S_SAMPLE_RATE_8K = 0,
	/**  12000Hz */
	MHAL_I2S_SAMPLE_RATE_12K = 1,
	/**  16000Hz */
	MHAL_I2S_SAMPLE_RATE_16K = 2,
	/**  24000Hz */
	MHAL_I2S_SAMPLE_RATE_24K = 3,
	/**  32000Hz */
	MHAL_I2S_SAMPLE_RATE_32K = 4,
	/**  48000Hz */
	MHAL_I2S_SAMPLE_RATE_48K = 5,
} hal_i2s_sample_rate;

/**
 * @}
 */

/** @defgroup hal_i2s_struct Struct
 * @{
 * This section introduces the structure used by I2S OS-HAL/M-HAL/HDL.
 */


/** @brief I2S link structure.
 *         The hal_i2s_link_config structure is used to configure
 *         I2S protocol for external audio codec. It is defined in
 *         hal_i2s_config struct.
 */
struct hal_i2s_link_config {
	/** Specify the sample rate of the I2S link.
	 */
	hal_i2s_sample_rate sample_rate;
	/** Specify the channel number of the I2S link.
	 */
	hal_i2s_channel_number channel_number;
	/** Specify the channel number of each frame sync cycle
	 * of the I2S link.
	 */
	hal_i2s_link_channel_per_sample channels_per_sample;
	/** Specify the bit number of each frame sync cycle
	 * of the I2S link.
	 */
	hal_i2s_link_bits_sample bits_per_sample;
	/** Specify the delay cycle from rising edge of sample rate
	 * to first channel MSB of the I2S link.
	 */
	u32 msb_offset;
	/** Enable or disable swapping the data of the right channel and the
	 * left channelof the I2S link.
	 */
	i2s_fnen lr_swap;
	/** Enable or disable word select inverse of the I2S link.
	 */
	i2s_fnen word_select_inverse;
};


/** @brief I2S configure structure.
 *         The hal_i2s_config structure is used to configure the I2S protocol.
 *         It is used in the mtk_mhal_i2s_set_config().
 */
struct hal_i2s_config {
	/** I2S input link configuration */
	struct hal_i2s_link_config i2s_in;
	/** I2S output link configuration */
	struct hal_i2s_link_config i2s_out;
	/** Enable or disable the right channel of I2S Tx sending the same
	 * data as the left channel of I2S Tx
	 */
	hal_i2s_tx_mode tx_mode;
	/** Enable or disable the function of real sample rate of
	 *  I2S Rx being 1/2 of SR, and I2S Rx will drop one sample in each
	 *  two Rx samples automatically
	 */
	hal_i2s_rx_down_rate rx_down_rate;
};

/** @brief M-HAL privite structure.
 * It's only used by I2S to programming M-HAL.
 * OS-HAL is not need to care this structure.
 */
struct mtk_i2s_private {
	/*for recording value from setting of user*/
	hal_i2s_initial_type			i2s_initial_type;
	/* backup user config */
	struct hal_i2s_config			i2s_user_config;
	/* backup TX DMA config */
	struct osai_dma_config			i2s_txdma_cfg;
	/* backup RX DMA config */
	struct osai_dma_config			i2s_rxdma_cfg;

};

/** @brief I2S control structure.
 *         The mtk_i2s_ctlr structure is used to configure
 *         I2S port number (i2s0/i2s1),I2S port base address,
 *         I2S FIFO port address and TX/RX DMA channel number.
 */
struct mtk_i2s_ctlr {
	/** I2S port base address */
	void __iomem *base;
	/** I2S port number */
	i2s_no i2s_port;
	/** I2S TX DMA channel number */
	u8 i2s_txdma_chnum;
	/** I2S RX DMA channel  number */
	u8 i2s_rxdma_chnum;
	/** I2S TX FIFO port */
	void __iomem *i2s_tx_fifo_port;
	/** I2S RX FIFO port */
	void __iomem *i2s_rx_fifo_port;
	/** M-HAL privite structure, used by M-HAL only */
	void *mdata;
};
/**
  * @}
  */

/** @defgroup hal_i2s_function Function
 * @{
 * This section provides fixed APIs (defined as Common Interface)
 * to fully control the MediaTek I2S HW.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     Configure I2S type.
 * @brief     Usage: OS-HAL driver should call it in the initialization flow.\n
 * @param[in] ctlr : I2S port and base address.
 * @param[in] i2s_initial_type : Initial configuration type.
 *
 * @par       Example
 * @code
 *	struct hal_i2s_config i2s_config;
 *	struct mtk_i2s_ctlr i2s_ctrl;
 *
 *      // set I2S0 as external mode
 *      i2s_ctrl.base = i2s0_baseaddr;
 *      i2s_ctrl.i2s_port = MHAL_I2S0;
 *      mtk_mhal_i2s_cfg_type(&i2s_ctrl, MHAL_I2S_TYPE_EXTERNAL_MODE);
 *
 *      // configure Rx settings
 *      i2s_config.i2s_in.sample_rate = MHAL_I2S_SAMPLE_RATE_8K;
 *      i2s_config.rx_down_rate=MHAL_I2S_RX_DOWN_RATE_DISABLE;
 *	........
 *      // configure Tx settings
 *      i2s_config.i2s_out.sample_rate = MHAL_I2S_SAMPLE_RATE_8K;
 *      i2s_config.i2s_out.channel_number = MHAL_I2S_STEREO;
 *	........
 *      result = mtk_mhal_i2s_set_config(&i2s_ctrl, &i2s_config);
 * @endcode
 *
 * @return
 *      Return "0" if parameter setting is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr or config is NULL.
 *
 * @note      Set i2s_initial_type as MHAL_I2S_TYPE_EXTERNAL_MODE
 *            when using external codec component in the application.
 */
int mtk_mhal_i2s_cfg_type(struct mtk_i2s_ctlr *ctlr,
		       hal_i2s_initial_type i2s_initial_type);
/**
 * @brief     Reset the I2S hardware.
 * @brief     Usage: OS-HAL driver can use this function to reset I2S HW
 *            before the initialization flow or after disabling I2S.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if I2S reset is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_reset(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     enable the I2S clock.
 * @brief     Usage: OS-HAL driver can use this function to enable or
 *            disable MCLK output, I2S out and I2S in clock.\n
 *            MCLK output, I2S out and I2S in clock will be enable after
 *            step3 on section How to Use This Driver.
 * @param[in] ctlr : I2S port and base address.
 * @param[in] en : enable or disable clock.
 * @return
 *      Return "0" if I2S reset is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_clk_en(struct mtk_i2s_ctlr *ctlr, i2s_fnen en);
/**
 * @brief     Set the I2S configuration in detail.
 * @brief     Usage: OS-HAL driver can call this function to configure I2S.\n
 *            The mtk_mhal_i2s_set_config() function configures the
 *            I2S settings providing the respective sample rate and channel
 *            number for Tx and Rx links as well as the clock mode
 *            for the whole I2S.
 * @param[in] ctlr : I2S port and base address.
 * @param[in] config : Link configuration of the I2S module.
 * @return
 *      Return "0" if configure I2S HW is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL or configuration
 *      parameter is invalid.
 */
int mtk_mhal_i2s_set_config(struct mtk_i2s_ctlr *ctlr,
			    struct hal_i2s_config *config);
/**
 * @brief     Enable uplink FIFO and downlink FIFO of the I2S link.
 * @brief     Usage: OS-HAL driver can use this function to enable I2S FIFO.\n
 *            The function should be called before hal_i2s_enable_tx()
 *            and hal_i2s_enable_rx()
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if enable I2S top is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_enable_audio_top(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     Disable the uplink FIFO and the downlink FIFO of I2S link.
 * @brief     Usage: OS-HAL driver can use this function to disable I2S FIFO.\n
 *            The function should be called before hal_i2s_disable_tx()
 *            and hal_i2s_disable_rx()
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if disable I2S top is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_disable_audio_top(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     Config Tx VFIFO DMA interrupt enable.
 * @brief     Usage: OS-HAL driver can use this function to enable I2S Tx DMA
 *            interrupt and register callback.
 *            The Tx VFIFO DMA will trigger an interrupt when the amount of
 *            the output data in Tx VFIFO is lower than that
 *            in the Tx VFIFO threshold.\n
 *            For the use of the DMA interrupt handle,
 *            please refer to the section - @ref Driver_Usage_Chapter.
 * @param[in] ctlr : I2S port and base address.
 * @param[in] callback_func : callback function.
 * @return
 *      Return "0" if configure I2S Tx DMA interrupt enable is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_cfg_tx_dma_irq_enable(struct mtk_i2s_ctlr *ctlr,
				       i2s_dma_callback_func callback_func);
/**
 * @brief     Config Tx VFIFO DMA interrupt disable.
 * @brief     Usage: OS-HAL driver can use this function to disable I2S Tx DMA
 *            interrupt.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if configure I2S Rx DMA interrupt disable is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_cfg_tx_dma_irq_disable(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     Config Rx VFIFO DMA interrupt enable.
 * @brief     Usage: OS-HAL driver can use this function to enable I2S Rx DMA
 *            interrupt and register callback.
 *            VFIFO DMA will trigger an interrupt when the amount of
 *            the available received data in Rx VFIFO is larger than
 *            that in the Rx VFIFO threshold.
 *            For the use of the DMA interrupt handle,
 *            please refer to the section - @ref Driver_Usage_Chapter.
 * @param[in] ctlr : I2S port and base address.
 * @param[in] callback_func : callback function.
 * @return
 *      Return "0" if configure I2S Rx DMA interrupt enable is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_cfg_rx_dma_irq_enable(struct mtk_i2s_ctlr *ctlr,
				       i2s_dma_callback_func callback_func);
/**
 * @brief     Config Rx VFIFO DMA interrupt disable.
 * @brief     Usage: OS-HAL driver can use this function to disable I2S Rx DMA
 *            interrupt.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if configure I2S Rx DMA interrupt disable is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_cfg_rx_dma_irq_disable(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     Start the Tx VFIFO DMA transmit operation.
 * @brief     Usage: OS-HAL driver can use this function to configure the
 *            I2S Tx VFIFO DMA.\n
 *            The FIFO starts to pump data from the Tx VFIFO buffer
 *            into the I2S Tx FIFO when the VFIFO DMA is configured
 *            and the I2S FIFO is enabled.
 *            The VFIFO DMA will trigger an interrupt when the amount of
 *            output data in Tx VFIFO is lower than that in the
 *            Tx VFIFO threshold.
 * @param[in] ctlr : I2S port and base address.
 * @param[in] buffer : Pointer to memory buffer for Tx VFIFO.
 * @param[in] threshold : The value of Tx VFIFO threshold.
 * @param[in] buffer_length : The size of memory buffer for Tx VFIFO.
 * @return
 *      Return "0" if setup I2S Tx VFIFO DMA is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.\n
 *      Return -#I2S_ELENGTH if the threshold and buffer_length is invalid.
 */
int mtk_mhal_i2s_start_tx_vfifo(struct mtk_i2s_ctlr *ctlr,
					     u32 *buffer,
					     u32 threshold,
					     u32 buffer_length);
/**
 * @brief     Disable the Tx VFIFO DMA transmit operation.
 * @brief     Usage: OS-HAL driver can use this function to stop
 *            the I2S Tx VFIFO DMA.\n
 *            The FIFO will stop pumping data from the Tx VFIFO buffer
 *            into I2S Tx FIFO.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if stop I2S Tx VFIFO DMA is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_stop_tx_vfifo(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     Start the Rx VFIFO DMA receive operation.
 * @brief     Usage: OS-HAL driver can use this function to configure
 *            the I2S VFIFO DMA.\n
 *            The FIFO starts to pump data from I2S Rx into the Rx VFIFO buffer
 *            when VFIFO DMA is configured and the I2S FIFO is enabled.
 *            If the I2S Rx is enabled, VFIFO DMA will trigger an interrupt
 *            when the amount of available received data in Rx VFIFO
 *            is larger than that in the Rx VFIFO threshold.
 * @param[in] ctlr : I2S port and base address.
 * @param[in] buffer : The pointer to memory buffer for Rx VFIFO.
 * @param[in] threshold : The value of Rx VFIFO threshold.
 * @param[in] buffer_length : The size of array to memory buffer for Rx VFIFO.
 * @return
 *      Return "0" if setup I2S Rx VFIFO DMA is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.\n
 *      Return -#I2S_ELENGTH if the threshold and buffer_length is invalid.
 */
int mtk_mhal_i2s_start_rx_vfifo(struct mtk_i2s_ctlr *ctlr,
					     u32 *buffer,
					     u32 threshold,
					     u32 buffer_length);
/**
 * @brief     Disable the Rx VFIFO DMA receive operation.
 * @brief     Usage: OS-HAL driver can use this function to stop
 *            I2S Rx VFIFO DMA.\n
 *            The FIFO will stop to pump data from the I2S Rx into the Rx
 *            VFIFO buffer.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if stop I2S Rx VFIFO DMA is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_stop_rx_vfifo(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     Power on the I2S the output link.
 * @brief     Usage: OS-HAL driver can use this function to enable
 *            I2S Tx to transmit data.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if enable I2S Tx is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_enable_tx(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     Power off the I2S output link.
 * @brief     Usage: OS-HAL driver can use this function to disable
 *            I2S Tx to transmit data.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if disable I2S Tx is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_disable_tx(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     Power on the I2S input link.
 * @brief     Usage: OS-HAL driver can use this function to enable
 *            I2S Rx to receive data.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if enable I2S Rx is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_enable_rx(struct mtk_i2s_ctlr *ctlr);
/**
 * @brief     Power off the I2S input link.
 * @brief     Usage: OS-HAL driver can use this function to disable
 *            I2S Rx to receive data.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if disable I2S Rx is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.
 */
int mtk_mhal_i2s_disable_rx(struct mtk_i2s_ctlr *ctlr);

/**
 * @brief     Move the SW point of Tx VFIFO DMA.
 * @brief     Usage: OS-HAL driver can use this function to move DMA SW
 *            point when the output data in Tx VFIFO is lower than
 *            threshold.\n
 * @param[in] ctlr : I2S port and base address.
 * @param[in] buffer_length : move pointer count.
 * @return
 *      Return "0" if setup I2S Tx VFIFO DMA is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.\n
 */
int mtk_mhal_i2s_move_tx_point(struct mtk_i2s_ctlr *ctlr,
			       u32 buffer_length);
/**
 * @brief     Move the SW point of Rx VFIFO DMA.
 * @brief     Usage: OS-HAL driver can use this function to move DMA SW
 *            point when the input data in Rx VFIFO is larger than
 *            threshold.\n
 * @param[in] ctlr : I2S port and base address.
 * @param[in] buffer_length : move pointer count.
 * @return
 *      Return "0" if setup I2S Rx VFIFO DMA is sucessfully.\n
 *      Return -#I2S_EPTR if the ctlr is NULL.\n
 */
int mtk_mhal_i2s_move_rx_point(struct mtk_i2s_ctlr *ctlr,
			       u32 buffer_length);
/**
 * @brief     I2S allocate DMA channel.
 * @brief     Usage: OS-HAL driver can use this function to allocate
 *            DMA channel.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if allocate memory sucessfully.\n
 *      Return -#I2S_EPTR if allocate channel fail or the ctlr is NULL.
 */
int mtk_mhal_i2s_alloc_vfifo_ch(struct mtk_i2s_ctlr *ctlr);

/**
 * @brief     I2S release DMA channel.
 * @brief     Usage: OS-HAL driver can use this function to release
 *            DMA channel.
 * @param[in] ctlr : I2S port and base address.
 * @return
 *      Return "0" if allocate memory sucessfully.\n
 *      Return -#I2S_EPTR if release channel fail or the ctlr is NULL.
 */
int mtk_mhal_i2s_release_vfifo_ch(struct mtk_i2s_ctlr *ctlr);

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
#endif				/* __MHAL_I2S_H__ */
