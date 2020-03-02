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

#ifndef __OS_HAL_I2S_H__
#define __OS_HAL_I2S_H__

#include "mhal_i2s.h"

typedef struct {
	/** Protocol mode */
	hal_i2s_initial_type		i2s_initial_type;
	/** Sample rate */
	hal_i2s_sample_rate		sample_rate;
	/** Word length */
	hal_i2s_link_bits_sample	bits_per_sample;
	/** I2S channel number */
	hal_i2s_channel_number		channel_number;
	/** TDM channel number */
	hal_i2s_link_channel_per_sample	channels_per_sample;
	/** TDM MSB offset */
	unsigned int			msb_offset;
	/** LRCK inverse */
	i2s_fnen			word_select_inverse;
	/** LR swap */
	i2s_fnen			lr_swap;
	/** Right channel data as the left channel of I2S Tx */
	hal_i2s_tx_mode			tx_mode;
	/** RX down rate*/
	hal_i2s_rx_down_rate		rx_down_rate;
	/** TX buffer point */
	unsigned int			*tx_buffer_addr;
	/** TX buffer length (unit:BYTE) */
	unsigned int			tx_buffer_len;
	/** TX period length (unit:BYTE) */
	unsigned int			tx_period_len;
	/** RX buffer point */
	unsigned int			*rx_buffer_addr;
	/** RX buffer length (unit:BYTE) */
	unsigned int			rx_buffer_len;
	/** RX period length (unit:BYTE) */
	unsigned int			rx_period_len;
	/** TX DMA callback function */
	i2s_dma_callback_func		tx_callback_func;
	/** TX callback data */
	void				*tx_callback_data;
	/** TX DMA callback function */
	i2s_dma_callback_func		rx_callback_func;
	/** RX callback data */
	void				*rx_callback_data;
} audio_parameter;

/**
 * @brief     Set the I2S configuration.
 * @brief     Usage: User can call this function to configure I2S.\n
 *            The mtk_os_hal_config_i2s() function configures the
 *            I2S and DMA for I2S settings and start up DMA for I2S.
 * @param[in] i2s_port : enum i2s_no.
 * @param[in] parameter : struct audio_parameter.
 * @return
 *      Return "0" if configure I2S and DMA for I2S are sucessfully.\n
 *      Return -#I2S_EPTR if the i2s_port is over MTK_I2S_MAX_PORT_NUMBER or
 *      parameter is invalid.
 */
int mtk_os_hal_config_i2s(i2s_no i2s_port, audio_parameter *parameter);

/**
 * @brief     Enable I2S.
 * @brief     Usage: User can call this function to start I2S TX and RX
 * @param[in] i2s_port : enum i2s_no.
 * @return
 *      Return "0" if start I2S TX and RX are sucessfully.\n
 *      Return "-#I2S_EPTR" if the i2s_port is over MTK_I2S_MAX_PORT_NUMBER
 *      or enable I2S TX and RX are fail.
 */
int mtk_os_hal_enable_i2s(i2s_no i2s_port);

/**
 * @brief     Disable I2S.
 * @brief     Usage: User can call this function to start I2S TX, RX and DMA
 * @param[in] i2s_port : enum i2s_no.
 * @return
 *      Return "0" if stop I2S TX, RX and DMA are sucessfully.\n
 *      Return "-#I2S_EPTR" if the i2s_port is over MTK_I2S_MAX_PORT_NUMBER
 *      or stop I2S TX, RX and DMA are fail.
 */
int mtk_os_hal_disable_i2s(i2s_no i2s_port);

/**
 * @brief     request I2S use.
 * @brief     Usage: User can call this function to request the DMA channels
 *            and I2S PINMUX.
 * @param[in] i2s_port : enum i2s_no.
 * @return
 *      Return "0" if request I2S PINMUX and DMA channels are sucessfully.\n
 *      Return "-#I2S_EPTR" if the i2s_port is over MTK_I2S_MAX_PORT_NUMBER
 *      or DMA channel request fail
 */
int mtk_os_hal_request_i2s(i2s_no i2s_port);

/**
 * @brief     Free I2S request.
 * @brief     Usage: User can call this function to release the DMA channels
 *            and I2S PINMUX request.
 * @param[in] i2s_port : enum i2s_no.
 * @return
 *      Return "0" if free PINMUX and DMA channel are sucessfully.\n
 *      Return "-#I2S_EPTR" if the i2s_port is over MTK_I2S_MAX_PORT_NUMBER
 *      or DMA channel release is fail
 */
int mtk_os_hal_free_i2s(i2s_no i2s_port);

#endif  /*__OS_HAL_I2S_H__*/
