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

#ifdef OSAI_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif

#include "nvic.h"
#include "os_hal_i2c.h"
#include "os_hal_dma.h"

#ifndef OSAI_ENABLE_DMA
#define PIO_I2C_MAX_LEN 8
#endif

#define ISU0_I2C_BASE	0x38070200
#define ISU1_I2C_BASE	0x38080200
#define ISU2_I2C_BASE	0x38090200
#define ISU3_I2C_BASE	0x380a0200
#define ISU4_I2C_BASE	0x380b0200

#define ISU0_CG_BASE	0x38070000
#define ISU1_CG_BASE	0x38080000
#define ISU2_CG_BASE	0x38090000
#define ISU3_CG_BASE	0x380a0000
#define ISU4_CG_BASE	0x380b0000

static unsigned long i2c_base_addr[OS_HAL_I2C_ISU_MAX] = {
	ISU0_I2C_BASE,
	ISU1_I2C_BASE,
	ISU2_I2C_BASE,
	ISU3_I2C_BASE,
	ISU4_I2C_BASE,
};

static unsigned long cg_base_addr[OS_HAL_I2C_ISU_MAX] = {
	ISU0_CG_BASE,
	ISU1_CG_BASE,
	ISU2_CG_BASE,
	ISU3_CG_BASE,
	ISU4_CG_BASE,
};

static int i2c_dma_chan[OS_HAL_I2C_ISU_MAX][2] = {
	/* [0]:tx, [1]:rx */
	{DMA_ISU0_TX_CH0, DMA_ISU0_RX_CH1},
	{DMA_ISU1_TX_CH2, DMA_ISU1_RX_CH3},
	{DMA_ISU2_TX_CH4, DMA_ISU2_RX_CH5},
	{DMA_ISU3_TX_CH6, DMA_ISU3_RX_CH7},
	{DMA_ISU4_TX_CH8, DMA_ISU4_RX_CH9},
};

/**
 * this os special i2c structure, need mapping it to mtk_i2c_controller
 */
struct mtk_i2c_ctrl_rtos {
	struct mtk_i2c_controller *i2c;

	/* the type based on OS */
#ifdef OSAI_FREERTOS
	QueueHandle_t xfer_completion;
#else
	volatile u8 xfer_completion;
#endif
};

static struct mtk_i2c_ctrl_rtos g_i2c_ctrl_rtos[OS_HAL_I2C_ISU_MAX];
struct mtk_i2c_controller g_i2c_ctrl[OS_HAL_I2C_ISU_MAX];
struct mtk_i2c_private g_i2c_mdata[OS_HAL_I2C_ISU_MAX];

static void _mtk_os_hal_i2c_irq_handler(int bus_num)
{
	u8 ret = 0;
	struct mtk_i2c_ctrl_rtos *ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];
	struct mtk_i2c_controller *i2c = ctrl_rtos->i2c;
#ifdef OSAI_FREERTOS
	BaseType_t x_higher_priority_task_woken = pdFALSE;
#endif
	ret = mtk_mhal_i2c_irq_handle(i2c);

	/* 1. FIFO mode: return completion done in I2C irq handler
	 * 2. DMA mode: return completion done in DMA irq handler
	 */
	if (!ret) {
#ifdef OSAI_FREERTOS
		xSemaphoreGiveFromISR(ctrl_rtos->xfer_completion,
				      &x_higher_priority_task_woken);
		portYIELD_FROM_ISR(x_higher_priority_task_woken);
#else
		ctrl_rtos->xfer_completion++;
#endif
	}
}

static void _mtk_os_hal_i2c0_irq_event(void)
{
	_mtk_os_hal_i2c_irq_handler(0);
}

static void _mtk_os_hal_i2c1_irq_event(void)
{
	_mtk_os_hal_i2c_irq_handler(1);
}

static void _mtk_os_hal_i2c2_irq_event(void)
{
	_mtk_os_hal_i2c_irq_handler(2);
}

static void _mtk_os_hal_i2c3_irq_event(void)
{
	_mtk_os_hal_i2c_irq_handler(3);
}

static void _mtk_os_hal_i2c4_irq_event(void)
{
	_mtk_os_hal_i2c_irq_handler(4);
}

static void _mtk_os_hal_i2c_request_irq(int bus_num)
{
	switch (bus_num) {
	case OS_HAL_I2C_ISU0:
		CM4_Install_NVIC(CM4_IRQ_ISU_G0_I2C, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_i2c0_irq_event,
				 TRUE);
		break;
	case OS_HAL_I2C_ISU1:
		CM4_Install_NVIC(CM4_IRQ_ISU_G1_I2C, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_i2c1_irq_event,
				 TRUE);
		break;
	case OS_HAL_I2C_ISU2:
		CM4_Install_NVIC(CM4_IRQ_ISU_G2_I2C, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_i2c2_irq_event,
				 TRUE);
		break;
	case OS_HAL_I2C_ISU3:
		CM4_Install_NVIC(CM4_IRQ_ISU_G3_I2C, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_i2c3_irq_event,
				 TRUE);
		break;
	case OS_HAL_I2C_ISU4:
		CM4_Install_NVIC(CM4_IRQ_ISU_G4_I2C, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_i2c4_irq_event,
				 TRUE);
		break;
	}
}

static void _mtk_os_hal_i2c_free_irq(int bus_num)
{
	switch (bus_num) {
	case OS_HAL_I2C_ISU0:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G0_I2C);
		break;
	case OS_HAL_I2C_ISU1:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G1_I2C);
		break;
	case OS_HAL_I2C_ISU2:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G2_I2C);
		break;
	case OS_HAL_I2C_ISU3:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G3_I2C);
		break;
	case OS_HAL_I2C_ISU4:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G4_I2C);
		break;
	}
}

static int _mtk_os_hal_i2c_dma_done_callback(void *data)
{
#ifdef OSAI_FREERTOS
	BaseType_t x_higher_priority_task_woken = pdFALSE;
	struct mtk_i2c_ctrl_rtos *ctrl_rtos;

	ctrl_rtos = (struct mtk_i2c_ctrl_rtos *)data;

	/* while using DMA mode, release semaphore in this callback */
	xSemaphoreGiveFromISR(ctrl_rtos->xfer_completion,
			      &x_higher_priority_task_woken);
	portYIELD_FROM_ISR(x_higher_priority_task_woken);

	return 0;
#else
	struct mtk_i2c_ctrl_rtos *ctrl_rtos = data;

	ctrl_rtos->xfer_completion++;
	return 0;
#endif
}

static int _mtk_os_hal_i2c_wait_for_completion_timeout(
	struct mtk_i2c_ctrl_rtos *ctrl_rtos, int time_ms)
{
#ifdef OSAI_FREERTOS
	if (pdTRUE != xSemaphoreTake(ctrl_rtos->xfer_completion,
				     time_ms / portTICK_RATE_MS))
		return -1;
#else
	extern volatile u32 sys_tick_in_ms;
	uint32_t start_tick = sys_tick_in_ms;

	while (ctrl_rtos->xfer_completion == 0) {
		if (sys_tick_in_ms - start_tick > time_ms)
			return -1;
	}
	ctrl_rtos->xfer_completion--;
#endif

	return 0;
}

int _mtk_os_hal_i2c_transfer(struct mtk_i2c_ctrl_rtos *ctrl_rtos, int bus_num)
{
	int ret = I2C_OK;
	struct mtk_i2c_controller *i2c;

	i2c = ctrl_rtos->i2c;

	ret = mtk_mhal_i2c_trigger_transfer(i2c);
	if (ret) {
		printf("i2c%d trigger transfer fail\n", bus_num);
		goto err_exit;
	}

	ret = _mtk_os_hal_i2c_wait_for_completion_timeout(ctrl_rtos,
						  i2c->timeout);
	if (ret) {
		printf("Take i2c%d Semaphore timeout!\n", bus_num);
		ret = -I2C_ETIMEDOUT;
	} else
		ret = mtk_mhal_i2c_result_handle(i2c);

	if (ret) {
		mtk_mhal_i2c_dump_register(i2c);
		mtk_mhal_i2c_init_hw(i2c);
	}

err_exit:

	return ret;
}

int mtk_os_hal_i2c_ctrl_init(i2c_num bus_num)
{
	struct mtk_i2c_ctrl_rtos *ctrl_rtos;
	struct mtk_i2c_controller *i2c;
	int ret = 0;

	if (bus_num >= OS_HAL_I2C_ISU_MAX)
		return -I2C_EINVAL;

	ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];

	i2c = &g_i2c_ctrl[bus_num];
	i2c->base = (void __iomem *)i2c_base_addr[bus_num];
	i2c->cg_base = (void __iomem *)cg_base_addr[bus_num];
	i2c->mdata = &g_i2c_mdata[bus_num];
	i2c->dma_tx_chan = i2c_dma_chan[bus_num][0];
	i2c->dma_rx_chan = i2c_dma_chan[bus_num][1];

	mtk_mhal_i2c_dma_done_callback_register(i2c,
					      _mtk_os_hal_i2c_dma_done_callback,
					     (void *)ctrl_rtos);

	ctrl_rtos->i2c = i2c;
#ifdef OSAI_FREERTOS
	if (!ctrl_rtos->xfer_completion)
		ctrl_rtos->xfer_completion = xSemaphoreCreateBinary();
#else
	ctrl_rtos->xfer_completion = 0;
#endif


	ret = mtk_mhal_i2c_request_dma(i2c);
	if (ret < 0) {
		printf("I2C%d request dma channel fail, ret = %d\n",
				bus_num, ret);
		return ret;
	}

	_mtk_os_hal_i2c_request_irq(bus_num);
	mtk_mhal_i2c_enable_clk(i2c);

	return 0;
}

int mtk_os_hal_i2c_ctrl_deinit(i2c_num bus_num)
{
	struct mtk_i2c_ctrl_rtos *ctrl_rtos;
	struct mtk_i2c_controller *i2c;

	if (bus_num >= OS_HAL_I2C_ISU_MAX)
		return -I2C_EINVAL;

	ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];
	i2c = ctrl_rtos->i2c;

	if (!i2c) {
		printf("i2c%d *i2c is NULL Pointer\n", bus_num);
		return -I2C_EPTR;
	}

#ifdef OSAI_FREERTOS
	if (ctrl_rtos->xfer_completion) {
		vSemaphoreDelete(ctrl_rtos->xfer_completion);
		ctrl_rtos->xfer_completion = NULL;
	}
#else
	ctrl_rtos->xfer_completion = 0;
#endif

	_mtk_os_hal_i2c_free_irq(bus_num);
	mtk_mhal_i2c_release_dma(i2c);
	mtk_mhal_i2c_disable_clk(i2c);

	i2c = NULL;
	ctrl_rtos->i2c = i2c;

	return 0;
}

int mtk_os_hal_i2c_speed_init(i2c_num bus_num, enum i2c_speed_kHz speed)
{
	int ret = I2C_OK;
	struct mtk_i2c_ctrl_rtos *ctrl_rtos;
	struct mtk_i2c_controller *i2c;

	if (bus_num >= OS_HAL_I2C_ISU_MAX)
		return -I2C_EINVAL;

	ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];

	i2c = ctrl_rtos->i2c;
	if (!i2c) {
		printf("i2c%d *i2c is NULL Pointer\n", bus_num);
		return -I2C_EPTR;
	}

	ret = mtk_mhal_i2c_init_speed(i2c, speed);
	if (ret)
		printf("i2c%d init speed fail\n", bus_num);

	return ret;
}

int mtk_os_hal_i2c_read(i2c_num bus_num, u8 device_addr, u8 *buffer, u16 len)
{
	struct i2c_msg msgs;
	struct mtk_i2c_ctrl_rtos *ctrl_rtos;
	struct mtk_i2c_controller *i2c;
	int ret = I2C_OK;

	if (bus_num >= OS_HAL_I2C_ISU_MAX)
		return -I2C_EINVAL;

#ifndef OSAI_ENABLE_DMA
	if (len > PIO_I2C_MAX_LEN) {
		printf("Error! buf length should be less than or equal to %d\n", PIO_I2C_MAX_LEN);
		return -I2C_EINVAL;
	}
#endif

	ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];

	i2c = ctrl_rtos->i2c;
	if (!i2c) {
		printf("i2c%d *i2c is NULL Pointer\n", bus_num);
		return -I2C_EPTR;
	}

	i2c->msg_num = 1;
	i2c->dma_en = false;
	i2c->i2c_mode = I2C_MASTER_MODE;
	i2c->timeout = 2000;
	i2c->irq_stat = 0;

	msgs.addr = device_addr;
	msgs.flags = I2C_MASTER_RD;
	msgs.len = len;
	msgs.buf = buffer;

	i2c->msg = &msgs;

	ret = _mtk_os_hal_i2c_transfer(ctrl_rtos, bus_num);
	if (ret)
		printf("i2c%d read fail\n", bus_num);

	return ret;
}

int   mtk_os_hal_i2c_write(i2c_num bus_num, u8 device_addr, u8 *buffer, u16 len)
{
	struct i2c_msg msgs;
	struct mtk_i2c_ctrl_rtos *ctrl_rtos;
	struct mtk_i2c_controller *i2c;
	int ret = I2C_OK;

	if (bus_num >= OS_HAL_I2C_ISU_MAX)
		return -I2C_EINVAL;

#ifndef OSAI_ENABLE_DMA
	if (len > PIO_I2C_MAX_LEN) {
		printf("Error! buf length should be less than or equal to %d\n", PIO_I2C_MAX_LEN);
		return -I2C_EINVAL;
	}
#endif

	ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];

	i2c = ctrl_rtos->i2c;
	if (!i2c) {
		printf("i2c%d *i2c is NULL Pointer\n", bus_num);
		return -I2C_EPTR;
	}

	i2c->msg_num = 1;
	i2c->dma_en = false;
	i2c->i2c_mode = I2C_MASTER_MODE;
	i2c->timeout = 2000;
	i2c->irq_stat = 0;

	msgs.addr = device_addr;
	msgs.flags = I2C_MASTER_WR;
	msgs.len = len;
	msgs.buf = buffer;

	i2c->msg = &msgs;

	ret = _mtk_os_hal_i2c_transfer(ctrl_rtos, bus_num);
	if (ret)
		printf("i2c%d write fail\n", bus_num);

	return ret;
}

int mtk_os_hal_i2c_write_read(i2c_num bus_num, u8 device_addr,
			      u8 *wr_buf, u8 *rd_buf, u16 wr_len, u16 rd_len)
{
	struct i2c_msg msgs[2];
	struct mtk_i2c_ctrl_rtos *ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];
	struct mtk_i2c_controller *i2c;
	int ret = I2C_OK;

	if (bus_num >= OS_HAL_I2C_ISU_MAX)
		return -I2C_EINVAL;

#ifndef OSAI_ENABLE_DMA
	if (wr_len > PIO_I2C_MAX_LEN || rd_len > PIO_I2C_MAX_LEN) {
		printf("Error! buf length should be less than or equal to %d\n", PIO_I2C_MAX_LEN);
		return -I2C_EINVAL;
	}
#endif

	ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];

	i2c = ctrl_rtos->i2c;
	if (!i2c) {
		printf("i2c%d *i2c is NULL Pointer\n", bus_num);
		return -I2C_EPTR;
	}

	i2c->msg_num = 2;
	i2c->dma_en = false;
	i2c->i2c_mode = I2C_MASTER_MODE;
	i2c->timeout = 2000;
	i2c->irq_stat = 0;

	msgs[0].addr = device_addr;
	msgs[0].flags = I2C_MASTER_WR;
	msgs[0].len = wr_len;
	msgs[0].buf = wr_buf;

	msgs[1].addr = device_addr;
	msgs[1].flags = I2C_MASTER_RD;
	msgs[1].len = rd_len;
	msgs[1].buf = rd_buf;

	i2c->msg = &msgs[0];

	ret = _mtk_os_hal_i2c_transfer(ctrl_rtos, bus_num);
	if (ret)
		printf("i2c%d write fail\n", bus_num);

	return ret;
}

int mtk_os_hal_i2c_set_slave_addr(i2c_num bus_num, u8 slv_addr)
{
	int ret = I2C_OK;
	struct mtk_i2c_ctrl_rtos *ctrl_rtos;
	struct mtk_i2c_controller *i2c;

	if (bus_num >= OS_HAL_I2C_ISU_MAX)
		return -I2C_EINVAL;

	ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];

	i2c = ctrl_rtos->i2c;
	if (!i2c) {
		printf("i2c%d *i2c is NULL Pointer\n", bus_num);
		return -I2C_EPTR;
	}

	i2c->i2c_mode = I2C_SLAVE_MODE;

	ret = mtk_mhal_i2c_init_slv_addr(i2c, slv_addr);
	if (ret)
		printf("i2c%d init slv_addr fail\n", bus_num);

	return ret;
}

int mtk_os_hal_i2c_slave_tx(i2c_num bus_num, u8 *buffer, u16 len, u32 time_out)
{
	struct i2c_msg msgs;
	struct mtk_i2c_ctrl_rtos *ctrl_rtos;
	struct mtk_i2c_controller *i2c;
	int ret = I2C_OK;

	if (bus_num >= OS_HAL_I2C_ISU_MAX)
		return -I2C_EINVAL;

#ifndef OSAI_ENABLE_DMA
	if (len > PIO_I2C_MAX_LEN) {
		printf("Error! buf length should be less than or equal to %d\n", PIO_I2C_MAX_LEN);
		return -I2C_EINVAL;
	}
#endif

	ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];

	i2c = ctrl_rtos->i2c;
	if (!i2c) {
		printf("i2c%d *i2c is NULL Pointer\n", bus_num);
		return -I2C_EPTR;
	}

	i2c->msg_num = 1;
	i2c->dma_en = false;
	i2c->i2c_mode = I2C_SLAVE_MODE;
	i2c->irq_stat = 0;
	i2c->timeout = time_out;

	msgs.flags = I2C_SLAVE_TX;
	msgs.len = len;
	msgs.buf = buffer;

	i2c->msg = &msgs;

	ret = _mtk_os_hal_i2c_transfer(ctrl_rtos, bus_num);
	if (ret)
		printf("i2c%d slave TX fail\n", bus_num);

	return ret;
}

int mtk_os_hal_i2c_slave_rx(i2c_num bus_num, u8 *buffer, u16 len, u32 time_out)
{

	struct i2c_msg msgs;
	struct mtk_i2c_ctrl_rtos *ctrl_rtos;
	struct mtk_i2c_controller *i2c;
	int ret = I2C_OK;

	if (bus_num >= OS_HAL_I2C_ISU_MAX)
		return -I2C_EINVAL;

#ifndef OSAI_ENABLE_DMA
	if (len > PIO_I2C_MAX_LEN) {
		printf("Error! buf length should be less than or equal to %d\n", PIO_I2C_MAX_LEN);
		return -I2C_EINVAL;
	}
#endif

	ctrl_rtos = &g_i2c_ctrl_rtos[bus_num];

	i2c = ctrl_rtos->i2c;
	if (!i2c) {
		printf("i2c%d *i2c is NULL Pointer\n", bus_num);
		return -I2C_EPTR;
	}

	i2c->msg_num = 1;
	i2c->dma_en = false;
	i2c->i2c_mode = I2C_SLAVE_MODE;
	i2c->irq_stat = 0;
	i2c->timeout = time_out;
	i2c->msg = &msgs;

	memset(&msgs, 0, sizeof(struct i2c_msg));

	msgs.flags = I2C_SLAVE_RX;
	msgs.buf = buffer;
	msgs.len = len;

	ret = _mtk_os_hal_i2c_transfer(ctrl_rtos, bus_num);
	if (ret) {
		printf("i2c%d slave RX fail\n", bus_num);
		return ret;
	}

	return 0;
}

