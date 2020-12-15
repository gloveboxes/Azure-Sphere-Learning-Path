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

#include "mt3620.h"
#include "mhal_osai.h"
#include "os_hal_dma.h"

void osai_delay_us(u32 us)
{
	u32 current_tick;
	u32 delta_tick;
	u32 target_tick;

	if (us >= 1000) {
		osai_delay_ms(us / 1000);
	} else {
		current_tick = SysTick->VAL;
		delta_tick = us*(SysTick->LOAD) / 1000;
		if (current_tick > delta_tick) {
			target_tick = current_tick - delta_tick;
			while ((SysTick->VAL) > target_tick)
				;
		} else {
			target_tick = SysTick->LOAD + current_tick - delta_tick;
			while (SysTick->VAL > 100)
				;
			while (SysTick->VAL > target_tick)
				;
		}
		return;
	}
}

#ifdef OSAI_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
void osai_delay_ms(u32 ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}
#else
void osai_delay_ms(u32 ms)
{
	extern volatile u32 sys_tick_in_ms;
	u32 current_ms = sys_tick_in_ms;
	u32 target_ms = current_ms + ms;

	if (target_ms >= current_ms) {
		while (sys_tick_in_ms < target_ms)
			;
	} else {
		/* delay until system_tic_in_ms over flow */
		while (sys_tick_in_ms >= current_ms)
			;
		/* delay until target_ms */
		while (sys_tick_in_ms < target_ms)
			;
	}
}
#endif

u32 osai_readl(void __iomem *addr)
{
	return *(volatile u32 *)(addr);
}

void osai_writel(u32 data, void __iomem *addr)
{
	*(volatile u32 *)(addr) = data;
}

unsigned long osai_get_phyaddr(void *vir_addr)
{
	return (unsigned long) vir_addr;
}

#ifdef OSAI_ENABLE_DMA
/* sync cache data to dram */
void osai_clean_cache(void *vir_addr, u32 len)
{

}

/* invalid cache data, next data read will from dram */
void osai_invalid_cache(void *vir_addr, u32 len)
{

}

int osai_dma_allocate_chan(u8 chn)
{
	return mtk_os_hal_dma_alloc_chan((enum dma_channel) chn);
}

int osai_dma_config(u8 chn, struct osai_dma_config *cfg_params)
{
	int ret = 0;
	struct dma_setting settings;

	memset(&settings, 0, sizeof(settings));

	settings.interrupt_flag = cfg_params->interrupt_flag;
	settings.dir = cfg_params->dir;
	settings.src_addr = cfg_params->src_addr;
	settings.dst_addr = cfg_params->dst_addr;
	settings.count = cfg_params->count;
	settings.vfifo.fifo_size = cfg_params->vfifo_size;
	settings.vfifo.fifo_thrsh = cfg_params->vfifo_thrsh;
	settings.vfifo.timeout_cnt = cfg_params->vfifo_timeout_cnt;
	settings.ctrl_mode.transize = cfg_params->transize;
	if (cfg_params->interrupt_flag & OSAI_DMA_INT_COMPLETION) {
		ret = mtk_os_hal_dma_register_isr((enum dma_channel) chn,
					cfg_params->done_callback,
					cfg_params->done_callback_data,
					DMA_INT_COMPLETION);
	}
	if (cfg_params->interrupt_flag & OSAI_DMA_INT_VFIFO_THRESHOLD) {
		ret = mtk_os_hal_dma_register_isr((enum dma_channel) chn,
					cfg_params->done_callback,
					cfg_params->done_callback_data,
					DMA_INT_VFIFO_THRESHOLD);
	}
	if (cfg_params->interrupt_flag & OSAI_DMA_INT_VFIFO_TIMEOUT) {
		ret = mtk_os_hal_dma_register_isr((enum dma_channel) chn,
					cfg_params->excep_callback,
					cfg_params->excep_callback_data,
					DMA_INT_VFIFO_TIMEOUT);
	}

	if (ret < 0)
		return -1;

	return mtk_os_hal_dma_config((enum dma_channel) chn, &settings);
}

int osai_dma_start(u8 chn)
{
	return mtk_os_hal_dma_start((enum dma_channel) chn);
}

int osai_dma_stop(u8 chn)
{
	return mtk_os_hal_dma_stop((enum dma_channel) chn);
}

int osai_dma_set_param(u8 chn, enum osai_dma_param_type param_type,
		       u32 value)
{
	return mtk_os_hal_dma_set_param((enum dma_channel) chn,
					(enum dma_param_type)param_type, value);
}
int osai_dma_get_param(u8 chn, enum osai_dma_param_type param_type)
{

	return mtk_os_hal_dma_get_param((enum dma_channel) chn,
					(enum dma_param_type)param_type);
}

int osai_dma_release_chan(u8 chn)
{
	return mtk_os_hal_dma_release_chan((enum dma_channel) chn);
}

int osai_dma_get_status(u8 chn)
{
	return mtk_os_hal_dma_get_status((enum dma_channel) chn);
}

int osai_dma_update_vfifo_swptr(u8 chn, u32 length_byte)
{
	return mtk_os_hal_dma_update_swptr((enum dma_channel) chn, length_byte);
}

int osai_dma_vff_read_data(u8 chn, u8 *buffer, u32 length)
{
	return mtk_os_hal_dma_vff_read_data((enum dma_channel) chn,
					    buffer, length);
}

int osai_dma_reset(u8 chn)
{
	return mtk_os_hal_dma_reset((enum dma_channel) chn);
}

int osai_dma_clr_dreq(u8 chn)
{
	return mtk_os_hal_dma_clr_dreq((enum dma_channel) chn);
}
#else
void osai_clean_cache(void *vir_addr, u32 len)
{
}
void osai_invalid_cache(void *vir_addr, u32 len)
{
}
int osai_dma_allocate_chan(u8 chn)
{
	return 0;
}
int osai_dma_config(u8 chn, struct osai_dma_config *cfg_params)
{
	return 0;
}
int osai_dma_start(u8 chn)
{
	return 0;
}
int osai_dma_stop(u8 chn)
{
	return 0;
}
int osai_dma_set_param(u8 chn, enum osai_dma_param_type param_type,
						u32 value)
{
	return 0;
}
int osai_dma_get_param(u8 chn, enum osai_dma_param_type param_type)
{
	return 0;
}
int osai_dma_release_chan(u8 chn)
{
	return 0;
}
int osai_dma_get_status(u8 chn)
{
	return 0;
}
int osai_dma_update_vfifo_swptr(u8 chn, u32 length_byte)
{
	return 0;
}
int osai_dma_vff_read_data(u8 chn, u8 *buffer, u32 length)
{
	return 0;
}
int osai_dma_reset(u8 chn)
{
	return 0;
}
int osai_dma_clr_dreq(u8 chn)
{
	return 0;
}
#endif
