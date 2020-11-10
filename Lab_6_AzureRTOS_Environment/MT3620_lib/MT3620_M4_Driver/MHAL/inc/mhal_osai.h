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

#ifndef __MHAL_OSAI__H__
#define __MHAL_OSAI__H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "printf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * This section introduces the Macro definition
  * that OSAI provided to mapping different OS.
  */

/**
 * It's the mapping of DMA address type between different OS.
 * A dma_addr_t can hold any valid DMA address.
 */
#define dma_addr_t unsigned long

/**
 * It's the mapping of module base address type between different OS.
 */
#define __iomem

/**
 * It's the wrapper of print log function between different OS.
 */
#define osai_print printf

/**
  * This section introduces the typedef to mapping
  * signed/unsigned char/short/int/long long.
  */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#ifndef BIT
#define BIT(n)                  ((u32) 1 << (n))

/*
 * bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m, n)           (~(BIT(m) - 1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

/**
 * @brief  This function is used to delay miscrosecond.\n
 * @brief Usage:
 * It will delay us miscrosecond after call this function.
 * @param [in] us : The delay time, unit is miscrosecond.
 *
 * @return None
 */
void osai_delay_us(u32 us);

/**
 * @brief  This function is used to delay millisecond.\n
 * @brief Usage:
 * It will delay ms millisecond after call this function.
 * @param [in] ms : The delay time, unit is millisecond.
 *
 * @return None
 */
void osai_delay_ms(u32 ms);

/**
 * @brief  This function is used to read I/O memory.\n
 * @param [in] addr : The I/O memory address.
 *
 * @return The value of the address.
 */
u32 osai_readl(void __iomem *addr);

/**
 * @brief  This function is used to write I/O memory.\n
 * @param [in] data : The data needs to be written to I/O memory.
 * @param [in] addr : The I/O memory address.
 *
 * @return None.
 */
void osai_writel(u32 data, void __iomem *addr);

/**
 * @brief  This function is used to get the physical address
 * from the virtual address.
 * @param [in] vir_addr : The virtual address.
 *
 * @return Return the converted physical address.
 */
unsigned long osai_get_phyaddr(void *vir_addr);

/**
 * @brief  This function is used to sync cache data to DRAM.
 * @param [in] vir_addr : The address of the operating memory.
 * @param [in] len : The size of the operating memory.
 *
 * @return None.
 */
void osai_clean_cache(void *vir_addr, u32 len);

/**
 * @brief  This function is used to invalid cache data,
 * the next data read will from DRAM.
 * @param [in] vir_addr : The address of the operating memory.
 * @param [in] len : The size of the operating memory.
 *
 * @return None.
 */
void osai_invalid_cache(void *vir_addr, u32 len);


/** @brief DMA parameter-type definition.
 * Sometimes users want to set or get some parameters
 * of DMA channel for interaction with DMA channel during
 * data transfer.
 */
enum osai_dma_param_type {
	/** remaining length of current transfer,
	 * only for HALF-SIZE DMA.
	 */
	OSAI_DMA_PARAM_RLCT = 0,
	/** fix_addr specifies the address for peripheral;
	 * must be 4-Byte alignment; only for Virtual FIFO DMA.
	 */
	OSAI_DMA_PARAM_FIX_ADDR = 1,
	/** prog_addr specifies the memory space address;
	 * only for Virtual FIFO DMA.
	 */
	OSAI_DMX_PARAM_PROG_ADDR = 2,
	/** FIFO size, only for Virtual FIFO DMA */
	OSAI_DMA_PARAM_VFF_FIFO_SIZE = 3,
	/** Data in FIFO, only for Virtual FIFO DMA */
	OSAI_DMA_PARAM_VFF_FIFO_CNT = 4,
	/** Hardware pointer,  only for Virtual FIFO DMA */
	OSAI_DMA_PARAM_VFF_HWPTR = 5,
	/** Software pointer, only for Virtual FIFO DMA */
	OSAI_DMA_PARAM_VFF_SWPTR = 6,
};

/** @brief DMA interrupt type definition.
 * There are four kinds of interrupt notifications in various DMA channels.\n
 * The interrupt of transfer completion notifies event of the completion of
 * transfer for both FULL-SIZE DMA and HALF-SIZE DMA.\n VFF DMA (Virtual FIFO
 * DMA) has two kinds of interrupt, and one notifies event that FIFO data is
 * over or under a certain threshold,\n the other is FIFO timeout interrupt
 * which shows there is no new data into the FIFO in the previous FIFO timeout
 * bus cycles.
 */
enum osai_dma_interrupt_type {
	/** the interrupt of transfer completion */
	OSAI_DMA_INT_COMPLETION = 0x1 << 0,
	/** The interrupt of FIFO data count threshold*/
	OSAI_DMA_INT_VFIFO_TIMEOUT = 0x1 << 2,
	/** The interrupt of FIFO timeout */
	OSAI_DMA_INT_VFIFO_THRESHOLD = 0x1 << 3,
};

/** @brief DMA dma_transize definition. The transaction size indicate transfer
 * data bytes in one bus cycle.
 */
enum osai_dma_transize {
	/** Select DMA transaction size as Byte (1-byte) */
	OSAI_DMA_SIZE_BYTE = 0,
	/** Select DMA transaction size as Short (2-byte) */
	OSAI_DMA_SIZE_SHORT,
	/** Select DMA transaction size as Long (4-byte) */
	OSAI_DMA_SIZE_LONG,
};

/** @brief This defines the function prototype of DMA interrupt service
 * routines callback.
 * osai_dma_interrupt_callback will be called in the DMA interrupt service
 *  routine and it can notify users that corresponding HW event occurs.\n
 * This typedef is to describe the callback indicating what users want to do.
 *
 * @param [in] user_data is a user defined parameter setted in
 * osai_dma_register_isr()
 */
typedef void (*osai_dma_interrupt_callback)(void *user_data);

/** @brief osai_dma_config specifies the dma channel configuration
 * for data transfer.
 */
struct osai_dma_config {
	/** The flag for enable dma channel interrupt, which is
	 * combination of #osai_dma_interrupt_type
	 */
	u8 interrupt_flag;
	/** The direction of data transfer: 0 is for memory to peripheral,
	 * 1 is for peripheral to memory.
	 */
	u8 dir;
	/** The source address of data transfer */
	u32 src_addr;
	/** The destination address of data transfer */
	u32 dst_addr;
	/** It specifies total data byte count of transfer for FULL-SIZE
	 * dma and HALF-SIZE dma
	 */
	u32 count;
	/** Data size within the confine of a bus cycle per transfer.
	 * Refer to #osai_dma_transize.
	 */
	u8 transize;
	/** The threshold for the interrupt #OSAI_DMA_INT_VFIFO_THRESHOLD */
	u32 vfifo_thrsh;
	/** Virtual FIFO size, only for Virtual FIFO DMA */
	u32 vfifo_size;
	/** Interrupt will assert if there is no new data into FIFO more
	 * than n T(Bus Clock)
	 */
	u32 vfifo_timeout_cnt;
	/** For HALF-SIZE DMA, done_callback is completion isr callback.
	 * For VFF DMA, done_callback is FIFO data underflow or overflow
	 * interrupt callback(#OSAI_DMA_INT_VFIFO_THRESHOLD).
	 */
	osai_dma_interrupt_callback done_callback;
	/** For VFF DMA, excep_callback is FIFO DMA timeout interrupt callback.
	 */
	osai_dma_interrupt_callback excep_callback;
	/** The user data of done_callback */
	void *done_callback_data;
	/** The user data of excep_callback */
	void *excep_callback_data;
};

/**
 * @brief This function is used to allocate one DMA channel.
 * @brief Usage: user should call it to allocate DMA channel
 * successfully before using DMA channel to transfer data.
 * @param [in] chn : The dma channel number.
 * @return
 * Return 0 if users allocate DMA channel successfully.\n
 * Return negative value if errors occur.
 */
int osai_dma_allocate_chan(u8 chn);

/**
 * @brief This function is used to configure one DMA channel.
 * @brief Usage: user should call it before starting DMA. According to
 * the DMA channel type,\n users should set right configuration values.
 * @param [in] chn : The dma channel number.
 * @param [in] cfg_params: the dma channel configuration structure
 * osai_dma_config.
 * @return
 * Return 0 if users configure DMA channel successfully.\n
 * Return negative value if errors occur.
 */
int osai_dma_config(u8 chn, struct osai_dma_config *cfg_params);

/**
 * @brief This function is used to start one DMA channel.
 * @brief Usage: user should call it after configure DMA
 * successfully.
 * @param [in] chn : The dma channel number.
 * @return
 * Return 0 if users start DMA channel successfully.\n
 * Return negative value if errors occur.
 */
int osai_dma_start(u8 chn);

/**
 * @brief This function is used to stop one DMA channel.
 * @brief Usage: user should call it after starting DMA.
 * Calling this function will stop transfer immediately and
 * DMA channel will return idle status.
 * @param [in] chn : The dma channel number.
 * @return
 * Return 0 if users stop DMA channel successfully.\n
 * Return negative value if errors occur.
 */
int osai_dma_stop(u8 chn);

/**
 * @brief This function is used to set DMA parameter which is
 * defined in the #dma_param.
 * @brief Usage: User only can set the value of parameters defined
 * in #osai_dma_param_type.
 * @param [in] chn : The dma channel number.
 * @param [in] param_type: The type of parameter.
 * @param [in] value: The value of parameter.
 * @return
 * Return 0 if users set DMA channel parameter successfully.\n
  * Return negative value if errors occur.
 */
int osai_dma_set_param(u8 chn, enum osai_dma_param_type param_type,
			     u32 value);

/**
 * @brief This function is used to get DMA parameter which is
 * defined in the #dma_param.
 * @brief Usage: User only can get the value of parameters defined
 * in #osai_dma_param_type.
 * @param [in] chn : The dma channel number.
 * @param [in] param_type: The type of parameter.
 * @return
 * Return the parameter value if users get DMA parameters successfully.\n
 * Return negative value if errors occur.
 */
int osai_dma_get_param(u8 chn, enum osai_dma_param_type param_type);

/**
 * @brief This function is used to release DMA channel.
 * @brief Usage: If DMA channel has been done and user can not trigger it
 * again, user should call this api to release DMA channel.
 * @param [in] chn : The dma channel number.
 *
 * @return
 * Return 0 if users release DMA channel successfully.\n
 * Return negative value if errors occur.
 */
int osai_dma_release_chan(u8 chn);

/**
 * @brief This function is used to update Virtual FIFO DMA fifo swptr.
 * @brief Usage: If user read data from fifo or write data into fifo,
 * user should call this api to updata fifo swptr.
 * @param [in] chn : The dma channel number.
 * @param [in] length_byte : the updated data length.
 * @return
 * Return 0 if updata fifo swptr successfully.\n
 * Return negative value if errors occur.
 */
int osai_dma_update_vfifo_swptr(u8 chn, u32 length_byte);


/**
 * @brief This function is used to read data from Virtual FIFO.
 * @brief Usage: For RX case, if virtual fifo is full, user can
 * use this api to read data and updata fifo swptr.
 * @param [in] chn : The dma channel number.
 * @param [out] buffer: the data buffer.
 * @param [in] length: the read data length.
 * @return
 * Return actual read data count if successfully.\n
 * Return -1 value if errors occur.
 */
int osai_dma_vff_read_data(u8 chn, u8 *buffer, u32 length);

/**
 * @brief This function is used to reset one DMA channel.
 * @brief Usage: Calling this function will reset DMA channel
 * registers and DMA channel will return idle status.
 * @param [in] chn : The dma channel number.
 * @return
 * Return 0 if users reset DMA channel successfully.\n
 * Return negative value if errors occur.
 */
int osai_dma_reset(u8 chn);

/**
 * @brief This function is used to clear dreq signal of DMA channel.
 * @brief Usage: Calling this function will clear dreq signal of DMA channel.
 * Some peripheral module need to call this api before start dma transfer.
 * @param [in] chn : The dma channel number.
 * @return
 * Return 0 if users clear dreq signal of DMA channel successfully.\n
 * Return negative value if errors occur.
 */
int osai_dma_clr_dreq(u8 chn);

#ifdef __cplusplus
}
#endif

#endif
