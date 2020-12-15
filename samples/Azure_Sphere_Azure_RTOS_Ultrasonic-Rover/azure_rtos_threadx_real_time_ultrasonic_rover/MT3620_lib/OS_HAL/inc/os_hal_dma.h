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

#ifndef __OS_HAL_DMA_H__
#define __OS_HAL_DMA_H__

#include "mhal_dma.h"

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup dma
 * @{
 * This section describes the programing api of DMA OS-HAL layer,
 * including terms and acronyms, supported features,
 * details on how to use this driver, enums and functions.
 *
 * @section OS_HAL_DMA_Terms_Chapter Terms and Acronyms
 *
 * |Terms                   |Details                             |
 * |------------------------------|--|
 * |\b DMA                        | Direct Memory Access.|
 * |\b FIFO                       | First In, First Out.|
 * |\b VFF \b DMA                 | Virtual FIFO DMA. |
 *
 * @section OS_HAL_DMA_Features_Chapter Supported Features
 * See @ref DRIVER_DMA_Features_Chapter for the details of Supported Features.
 *
 * @}
 * @}
 */

/**
 * @addtogroup OS-HAL
 * @{
 * @addtogroup dma
 * @{
 * @section OS_HAL_DMA_Driver_Usage_Chapter How to use this driver
 *
 * - \b Device \b driver \b sample \b code \b is \b as \b follows: \n
 *  - sample code (this is the user application sample code on freeRTos):
 *    @code
 *	- Allocate one DMA channel
 *	 -Call mtk_os_hal_dma_alloc_chan(enum dma_channel chn)
 *	   to allocate one DMA channel.
 *
 *	- Config one DMA channel
 *	  -Call mtk_os_hal_dma_config(enum dma_channel chn,
				struct dma_setting *setting)
 *
 *	- Start one DMA channel
 *	 -Call mtk_os_hal_dma_start(enum dma_channel chn)
 *
 *	- Stop one DMA channel
 *	 -Call mtk_os_hal_dma_stop(enum dma_channel chn)
 *
 *	- Pause one DMA channel
 *	 -Call mtk_os_hal_dma_pause(enum dma_channel chn)
 *
 *	- Resume one DMA channel
 *	 -Call mtk_os_hal_dma_resume(enum dma_channel chn)
 *
 *	- Get DMA channel status
 *	 -Call mtk_os_hal_dma_get_status(enum dma_channel chn)
 *
 *	- Register interrupt callback
 *	 -Call mtk_os_hal_dma_register_isr(enum dma_channel chn,
				dma_interrupt_callback callback,
				void *callback_data,
				enum dma_interrupt_type isr_type)
 *
 *	- Dump DMA registers
 *	 - Call  mtk_os_hal_dma_dump_register(enum dma_channel chn)
 *
 *	- Update VFF DMA channel swptr
 *	 - Call  mtk_os_hal_dma_update_swptr(enum dma_channel chn,
				u32 length_byte)
 *
 *
 *	- Set DMA parameter which is defined in the #dma_param_type
 *	 - Call  mtk_os_hal_dma_set_param(enum dma_channel chn,
				enum dma_param_type param_type, u32 value)
 *
 *	- Get DMA parameter which is defined in the #dma_param_type
 *	 - Call  mtk_os_hal_dma_get_param(enum dma_channel chn,
				enum dma_param_type param_type)
 *
 *	- Release one DMA channel
 *	 - Call  mtk_os_hal_dma_release_chan(enum dma_channel chn)
 *
 *	- Reset DMA channel
 *	 - Call  mtk_os_hal_dma_reset(enum dma_channel chn)
 *
 *	- Clear dreq signal of DMA channel
 *	 - Call  mtk_os_hal_dma_clr_dreq(enum dma_channel chn)
 *
 *    @endcode
 *
 * @}
 * @}
 */

/**
* @addtogroup OS-HAL
* @{
* @addtogroup dma
* @{
*/

/** @defgroup os_hal_dma_enum Enum
 * @{
 * This section describes the enumeration definitions in DMA driver OS-HAL
 * layer. These enumerations include DMA channels, DMA parameters type and
 * dma interrrupts type. If users want to set the value for the above
 * variables, please refer to Enumerations.
 */

/** @brief DMA channels definition.
 * This definition includes all DMA channels. Users can look up this enumeration
 * for selecting DMA channels. The DMA_M2M_CH12 channel can be used to copy data
 * from memory to memory, And other channels are specified for some peripheral
 * devices which can transfer data between memory and peripheral devices. VFF
 * DMA is Virtual FIFO DMA.
 */
enum dma_channel {
	/** HALF-SIZE DMA channel 0 as ISU0-TX */
	DMA_ISU0_TX_CH0,
	/** HALF-SIZE DMA channel 1 as ISU0-RX */
	DMA_ISU0_RX_CH1,
	/** HALF-SIZE DMA channel 2 as ISU1-TX */
	DMA_ISU1_TX_CH2,
	/** HALF-SIZE DMA channel 3 as ISU1-RX */
	DMA_ISU1_RX_CH3,
	/** HALF-SIZE DMA channel 4 as ISU2-TX */
	DMA_ISU2_TX_CH4,
	/** HALF-SIZE DMA channel 5 as ISU2-RX */
	DMA_ISU2_RX_CH5,
	/** HALF-SIZE DMA channel 6 as ISU3-TX */
	DMA_ISU3_TX_CH6,
	/** HALF-SIZE DMA channel 7 as ISU3-RX */
	DMA_ISU3_RX_CH7,
	/** HALF-SIZE DMA channel 8 as ISU4-TX */
	DMA_ISU4_TX_CH8,
	/** HALF-SIZE DMA channel 9 as ISU4-RX */
	DMA_ISU4_RX_CH9,
	/** FULL-SIZE DMA channel 12 as memory copy */
	DMA_M2M_CH12 = 12,
	/** VFF DMA channel 13 as ISU0-TX */
	VDMA_ISU0_TX_CH13,
	/** VFF DMA channel 14 as ISU0-RX */
	VDMA_ISU0_RX_CH14,
	/** VFF DMA channel 15 as ISU1-TX */
	VDMA_ISU1_TX_CH15,
	/** VFF DMA channel 16 as ISU1-RX */
	VDMA_ISU1_RX_CH16,
	/** VFF DMA channel 17 as ISU2-TX */
	VDMA_ISU2_TX_CH17,
	/** VFF DMA channel 18 as ISU2-RX */
	VDMA_ISU2_RX_CH18,
	/** VFF DMA channel 19 as ISU3-TX */
	VDMA_ISU3_TX_CH19,
	/** VFF DMA channel 20 as ISU3-RX */
	VDMA_ISU3_RX_CH20,
	/** VFF DMA channel 21 as ISU4-TX */
	VDMA_ISU4_TX_CH21,
	/** VFF DMA channel 22 as ISU4-RX */
	VDMA_ISU4_RX_CH22,
	/** VFF DMA channel 25 as I2S0-TX */
	VDMA_I2S0_TX_CH25 = 25,
	/** VFF DMA channel 26 as I2S0-RX */
	VDMA_I2S0_RX_CH26,
	/** VFF DMA channel 27 as I2S1-TX */
	VDMA_I2S1_TX_CH27,
	/** VFF DMA channel 28 as I2S1-RX */
	VDMA_I2S1_RX_CH28,
	/** VFF DMA channel 29 as ADC-RX */
	VDMA_ADC_RX_CH29,
};

/** @brief DMA parameters type.
 * This definition indicates DMA parameters which may be set or got by users.
 * For the FULL-SIZE DMA, users only can get the OS_HAL_DMA_PARAM_RLCT. For the
 * HALF-SIZE DMA, users can get OS_HAL_DMA_PARAM_RLCT and set/get
 * OS_HAL_DMA_PARAM_FIX_ADDR/OS_HAL_DMA_PARAM_PROG_ADDR. For the VFF DMA, users
 * can get OS_HAL_DMA_PARAM_VFF_FIFO_CNT/OS_HAL_DMA_PARAM_VFF_HWPTR, and set/get
 * OS_HAL_DMA_PARAM_VFF_FIFO_SIZE/OS_HAL_DMA_PARAM_VFF_SWPTR.
 */
enum dma_param_type {
	/** The remain count of data transfer, read-only for FULL-SIZE DMA
	 * and HALF-SIZE DMA.
	 */
	OS_HAL_DMA_PARAM_RLCT = 0,
	/** The fix address which indicates the data port of peripheral
	 * devices, for HALF-SIZE DMA and VFF DMA.
	 */
	OS_HAL_DMA_PARAM_FIX_ADDR = 1,
	/** The programable address which indicates the memory address,
	 * for HALF-SIZE DMA and VFF DMA.
	 */
	OS_HAL_DMA_PARAM_PROG_ADDR = 2,
	/** The Virtual FIFO Length, only for VFF DMA */
	OS_HAL_DMA_PARAM_VFF_FIFO_SIZE = 3,
	/** The byte count of data in Virtual FIFO, read-only for VFF DMA */
	OS_HAL_DMA_PARAM_VFF_FIFO_CNT = 4,
	/** The HW pointer of Virtual FIFO, read-only for VFF DMA */
	OS_HAL_DMA_PARAM_VFF_HWPTR = 5,
	/** The SW pointer of Virtual FIFO, Only for VFF DMA */
	OS_HAL_DMA_PARAM_VFF_SWPTR = 6,
};

/** @brief DMA interrupt type definition.
 * This definition indicates DMA hardware interrupts. The FULL-SIZE DMA can
 * set DMA_INT_COMPLETION to enable the interrupt of data transfer completion.
 * The HALF-SIZE DMA can set DMA_INT_COMPLETION and DMA_INT_HALF_COMPLETION.
 * The VFF DMA can set DMA_INT_VFIFO_TIMEOUT and DMA_INT_VFIFO_THRESHOLD.
 */
enum dma_interrupt_type {
	/** The interrupt of data transfer completion, for FULL-SIZE DMA and
	 * HALF-SIZE DMA.
	 */
	DMA_INT_COMPLETION = 0x1 << 0,
	/** The interrupt of the half of data transfer completion, only for
	 * HALF-SIZE DMA.
	 */
	DMA_INT_HALF_COMPLETION = 0x1 << 1,
	/** The interrupt of the Virtual FIFO data updating timeout, only for
	 * Virtual FIFO DMA.
	 */
	DMA_INT_VFIFO_TIMEOUT = 0x1 << 2,
	/** The interrupt of the Virtual FIFO data threshold, only for
	 * Virtual FIFO DMA.
	 */
	DMA_INT_VFIFO_THRESHOLD = 0x1 << 3,
};
/**
 * @}
 */

/** @defgroup os_hal_dma_typedef Typedef
 * @{
 * This section introduces the typedef used by DMA OS-HAL Layer.
 */

/** @brief This defines the function prototype of DMA interrupt service
 * routines callback.
 * Different DMA channels have different interrupts. FULL-SIZE DMA has data
 * transfer completion interrupt; HALF-SIZE DMA has data transfer completion
 * interrupt and half-completion interrupt; VFF DMA has FIFO data overflow/
 * underflow interrupt and FIFO timeout interrupt.\n
 * dma_interrupt_callback will be called in the DMA interrupt service routines
 * and it can notify users that corresponding HW event occurs.
 * This typedef describes the callback indicating what users want to do.
 *
 * @param [in] user_data : is a user defined parameter provided
 * by #mtk_os_hal_dma_register_isr().
 * @sa  #mtk_os_hal_dma_register_isr()
 */
typedef void (*dma_interrupt_callback)(void *user_data);
/**
 * @}
 */

/** @defgroup os_hal_dma_struct Struct
 * @{
 * This section introduces the structure definition in DMA OS-HAL. When
 * users want to call DMA OS-HAL API, these structures should be allocated
 * and filled by right values.
 */

/** @brief dma_wrap specifies the DMA channel settings which control the
 * address-wrap function for FULL-SIZE DMA and HALF-SIZE DMA. Users should
 * fill the right values for dma_wrap.
 */
struct dma_wrap {
	/** Flag of transfer address-wrapping fucntion. If it is enabled,
	 * wrap_point and wrap_to_addr need to be set.
	 */
	u8 wrap_en;
	/** The side for using address-wrapping: source or destination, and
	 * only one side can activate address-wrapping feature at a time.
	 */
	u8 wrap_side;
	/** Wrap point from start address. If wrap_en is enable, DMA channel
	 * read/write pointer will jump to #wrap_to_addr at #wrap_point
	 * during data transfer.
	 */
	u32 wrap_point;
	/** The address which DMA will wrap to. If wrap_en in dma_ctrl is
	 * enable, DMA channel read/write pointer will jump to #wrap_to_addr
	 * at #wrap_point during data transfer.
	 */
	u32 wrap_to_addr;
};

/** @brief dma_control_mode specifies the DMA channel settings to control the
 * DMA channel transfer mode.
 */
struct dma_control_mode {
	/** Burst-type, only for FULL-SIZE DMA and HALF-SIZE DMA.
	 * Refer to #dma_burst_type in M-HAL.
	 */
	u8 burst_type;
	/** Word to byte or byte to word transfer flag, only for
	 * HALF-SIZE DMA.
	 */
	u8 bw_transfer_en;
	/** Destination addresses automatically increase after every transfer,
	 * only for FULL-SIZE DMA and HALF-SIZE DMA.
	 */
	u8 dst_inc_en;
	/** Source addresses automatically increase after every transfer,
	 * only for FULL-SIZE DMA and HALF-SIZE DMA.
	 */
	u8 src_inc_en;
	/** Data size within the confine of a bus cycle per transfer.
	 * Refer to #dma_transize in M-HAL.
	 */
	u8 transize;
	/** Bandwidth limiter. The value range is from 0 to 255. */
	u8 bw_limiter;
	/** The setting for address-wrap function */
	struct dma_wrap wrap_settings;
};

/** @brief dma_interrupt indicates the callback function and users data
 * of DMA hardware interrupts.
 */
struct dma_interrupt {
	/** The interrupt callback function */
	dma_interrupt_callback isr_cb;
	/** The users data being used in callback function */
	void *cb_data;
};

/** @brief dma_vfifo specifies the specified settings of the VFF DMA, which are
 * only used for the VFF DMA.
 */
struct dma_vfifo {
	/** It specifies the threshold for DMA_INT_VFIFO_THRESHOLD. If the data
	 * count in FIFO is more or less than #fifo_thrsh, the VFF DMA will
	 * trigger DMA_INT_VFIFO_THRESHOLD interrupt.
	 */
	u32 fifo_thrsh;
	/** Specifies the Alert Length of Virtual FIFO DMA */
	u8 alert_cmp_type;
	/** Specifies the Compare Equation Between ALTLEN and
	 * FIFO_SIZE - FIFO_CNT
	 */
	u32 alert_len;
	/** Virtual FIFO size */
	u32 fifo_size;
	/** Interrupt will assert if there is no new data into FIFO more
	 * than n T(Bus Clock)
	 */
	u32 timeout_cnt;
};

/** @brief dma_setting specifies the DMA channel settings which control the DMA
 * channel transfer. Users should fill the right value according to application
 * scenarios.
 */
struct dma_setting {
	/** The flag of DMA hardware interrupt, please refer to
	 * #dma_interrupt_type.
	 */
	u8 interrupt_flag;
	/** Transfer direction, only for HALF-SIZE DMA
	 * and VFF DMA. Refer to #dma_dir in M-HAL.
	 */
	u8 dir;
	/** The source address of data transfer. */
	u32 src_addr;
	/** The destination address of data transfer. */
	u32 dst_addr;
	/** The total byte count of data transfer. */
	u32 count;
	/** Reload transfer count flag, only for HALF-SIZE DMA. */
	u8 reload_en;
	/** The VFF DMA specified setting. */
	struct dma_vfifo vfifo;
	/** The setting to control DMA hardware transfer mode. */
	struct dma_control_mode ctrl_mode;
};
/**
 * @}
 */

/** @defgroup os_hal_dma_function Function
 * @{
 * This section provides DMA OS-HAL APIs to use DMA channel for data transfer.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function is used to allocate one DMA channel.
 * @brief Usage: Users should call it as the first step when use DMA OS-HAL
 * APIs.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return 0 if users allocate DMA channel successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_alloc_chan(enum dma_channel chn);

/**
 * @brief This function is used to config one DMA channel.
 * @brief Usage: Used to config one DMA channel.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 * @param [in] setting : The DMA channel data transfer setting, please
 * refer to #dma_setting.
 *
 * @return
 * Return 0 if users allocate DMA channel successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_config(enum dma_channel chn, struct dma_setting *setting);

/**
 * @brief This function is used to start one DMA channel.
 * @brief Usage: Used to start one DMA channel.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return 0 if users start DMA channel successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_start(enum dma_channel chn);

/**
 * @brief This function is used to stop one DMA channel.
 * @brief Usage: Used to stop one DMA channel.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return 0 if users stop DMA channel successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_stop(enum dma_channel chn);

/**
 * @brief This function is used to pause one DMA channel.
 * @brief Usage: Used to pause one DMA channel.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return 0 if users pause DMA channel successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_pause(enum dma_channel chn);

/**
 * @brief This function is used to resume one DMA channel.
 * @brief Usage: Used to resume one DMA channel.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return 0 if users resume DMA channel successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_resume(enum dma_channel chn);

/**
 * @brief This function is used to get DMA channel status.
 * @brief Usage: Used to get DMA channel status. The DMA channel status
 * can refer to #dma_status in M-HAL.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return DMA channel status if get DMA channel status successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_get_status(enum dma_channel chn);

/**
 * @brief This function is used to register interrupt callback.
 * @brief Usage: Used to register interrupt callback.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 * @param [in] callback : The DMA interrupt callback, please refer to
 * #dma_interrupt_callback.
 * @param [in] callback_data : The users data of callback.
 * @param [in] isr_type : The interrupt type, please refer to
 * #dma_interrupt_type.
 *
 * @return
 * Return 0 if users allocate DMA channel successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_register_isr(enum dma_channel chn,
	dma_interrupt_callback callback, void *callback_data,
				enum dma_interrupt_type isr_type);

/**
 * @brief This function is used to dump DMA registers for debug.
 * @brief Usage: Used to dump DMA registers for debug.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return 0 if users dump DMA registers successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_dump_register(enum dma_channel chn);

/**
 * @brief This function is used to set DMA parameter which is
 * defined in the #dma_param_type.
 * @brief Usage: Users should call this function carefully if DMA channel
 * is running.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 * @param [in] param_type : The parameter type whose value to be set.
 * @param [in] value : The new parameter value.
 *
 * @return
 * Return 0 if users set DMA channel parameter successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_set_param(enum dma_channel chn,
	enum dma_param_type param_type, u32 value);

/**
 * @brief This function is used to get DMA parameter which is
 * defined in the #dma_param_type.
 * @brief Usage: Users should call this function carefully if DMA channel
 * is running.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 * @param [in] param_type : The parameter type whose value to be got.
 *
 * @return
 * Return the parameter value if users get DMA parameters successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_get_param(enum dma_channel chn,
	enum dma_param_type param_type);

/**
 * @brief This function is used to update VFF DMA channel swptr.
 * @brief Usage: Used to update VFF DMA channel swptr.
 * If users read data from Virtual FIFO or write data into Virtual FIFO,
 * users should update swptr to control read pointer or write pointer
 * of fifo control engine. This API is only for VFF DMA.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 * @param [in] length_byte : The updated byte count. It cannot be
 * larger than vfifo size.
 *
 * @return
 * Return 0 if users update VFF DMA channel swptr successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_update_swptr(enum dma_channel chn, u32 length_byte);

/**
 * @brief This function is used to read data from FIFO of VFF DMA channel.
 * @brief Usage: Used to read data from FIFO of VFF DMA channel. This api
 * will also update swptr after reading data. This API is only for VFF DMA.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 * @param [out] buffer : buffer for saving read data.
 * @param [in] length : byte number which users want to read. It cannot be
 * larger than vfifo size.
 * @return
 * Return the actual read data byte count if successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_vff_read_data(enum dma_channel chn, u8 *buffer, u32 length);

/**
 * @brief This function is used to release one DMA channel.
 * @brief Usage: Users should call it as the last step when use DMA OS-HAL APIs.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return 0 if users release DMA channel successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_release_chan(enum dma_channel chn);

/**
 * @brief This function is used to reset DMA channel.
 * @brief Usage: Users can call this API to reset DMA channel.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return 0 if users reset DMA channel successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_reset(enum dma_channel chn);

/**
 * @brief This function is used to clear dreq signal of DMA channel.
 * @brief Usage: This function is specified for HALF-SIZE DMA and VFF DMA.
 * @param [in] chn : The DMA channel number, please refer to #dma_channel.
 *
 * @return
 * Return 0 if users clear dreq signal successfully.\n
 * Return negative integer indicating error number when error occur.\n
 */
int mtk_os_hal_dma_clr_dreq(enum dma_channel chn);

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
