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
#include "FreeRTOS.h"
#include <semphr.h>
#endif

#include "nvic.h"
#include "irq.h"
#include "os_hal_mbox.h"

/* debug help MACRO */
#define MTK_MBOX_DEBUG_ENABLE 0

#define OS_MBOX_ERROR(fmt, arg...)	printf("[os_mbox][error] %s(L:%d) "fmt,\
						__func__, __LINE__, ##arg)
#if MTK_MBOX_DEBUG_ENABLE
#define OS_MBOX_DEBUG(fmt, arg...)	printf("[os_mbox][debug] %s(L:%d) "fmt,\
						__func__, __LINE__, ##arg)
#else
#define OS_MBOX_DEBUG(fmt, arg...)
#endif

/* interrupt mapping */
#define MBOX_M4_VECTOR_FIFO0			CM4_IRQ_M42A7N_FIFO
#define MBOX_M4_VECTOR_FIFO1			CM4_IRQ_M42M4_FIFO

#define MBOX_M4_VECTOR_CH0_SW			CM4_IRQ_A7N2M4_SW
#define MBOX_M4_VECTOR_CH1_SW			CM4_IRQ_M42M4_SW

#define MBOX_M4_VECTOR_CH0_WR			CM4_IRQ_A7N2M4_WR
#define MBOX_M4_VECTOR_CH1_WR			CM4_IRQ_M42M4_WR

#define MBOX_M4_VECTOR_CH0_RD			CM4_IRQ_M42A7N_RD
#define MBOX_M4_VECTOR_CH1_RD			CM4_IRQ_M42M4_RD

#define MBOX_M4_VECTOR_CH0_NE			CM4_IRQ_A7N2M4_NE
#define MBOX_M4_VECTOR_CH1_NE			CM4_IRQ_M42M4_NE

#define MBOX_M4_VECTOR_CH0_NF			CM4_IRQ_M42A7N_NF
#define MBOX_M4_VECTOR_CH1_NF			CM4_IRQ_M42M4_NF


#define CH0_MBOX_BASE			0x21050000
#define CH1_MBOX_BASE			0x21060000

static unsigned long mbox_base_addr[OS_HAL_MBOX_CH_MAX] = {
	CH0_MBOX_BASE,
	CH1_MBOX_BASE,
};

static IRQn_Type wr_vectors[OS_HAL_MBOX_CH_MAX] = {
					(IRQn_Type)MBOX_M4_VECTOR_CH0_WR,
					(IRQn_Type)MBOX_M4_VECTOR_CH1_WR};
static IRQn_Type rd_vectors[OS_HAL_MBOX_CH_MAX] = {
					(IRQn_Type)MBOX_M4_VECTOR_CH0_RD,
					(IRQn_Type)MBOX_M4_VECTOR_CH1_RD};
static IRQn_Type ne_vectors[OS_HAL_MBOX_CH_MAX] = {
					(IRQn_Type)MBOX_M4_VECTOR_CH0_NE,
					(IRQn_Type)MBOX_M4_VECTOR_CH1_NE};
static IRQn_Type nf_vectors[OS_HAL_MBOX_CH_MAX] = {
					(IRQn_Type)MBOX_M4_VECTOR_CH0_NF,
					(IRQn_Type)MBOX_M4_VECTOR_CH1_NF};
static IRQn_Type sw_vectors[OS_HAL_MBOX_CH_MAX] = {
					(IRQn_Type)MBOX_M4_VECTOR_CH0_SW,
					(IRQn_Type)MBOX_M4_VECTOR_CH1_SW};

struct os_hal_mbox_channel {
#ifdef OSAI_FREERTOS
	SemaphoreHandle_t sem_fifo_read;
	SemaphoreHandle_t sem_fifo_write;
	SemaphoreHandle_t sem_sw_int;
	SemaphoreHandle_t sem_fifo_int;
#else
	volatile u8 sem_fifo_read;
	volatile u8 sem_fifo_write;
	volatile u8 sem_sw_int;
	volatile u8 sem_fifo_int;
#endif

	u32 swint_en;
	mtk_os_hal_mbox_cb swint_cb;
	mtk_os_hal_mbox_cb fifo_cb;

	struct mbox_fifo_event event;
	struct mbox_fifo_event mask;
	struct mbox_swint_info swint;

	void __iomem *base;
	int enable;
};

struct os_hal_mbox_device {
	struct os_hal_mbox_channel	channels[OS_HAL_MBOX_CH_MAX];
};

static struct os_hal_mbox_device mbox;
static struct os_hal_mbox_device *mbox_dev = &mbox;

static struct os_hal_mbox_channel *_mtk_os_hal_mbox_get_channel(
				mbox_channel_t channel)
{
	if (channel >= OS_HAL_MBOX_CH_MAX) {
		OS_MBOX_ERROR("channel:%d out of range, should be:0~%d\n",
			channel, OS_HAL_MBOX_CH_MAX-1);
		return NULL;
	}
	if (!mbox_dev->channels[channel].enable) {
		OS_MBOX_ERROR("channel %d is not open\n", channel);
		return NULL;
	}

	return &mbox_dev->channels[channel];
}

static void _mtk_os_hal_mbox_sw_int_irq_handler(mbox_channel_t channel)
{
	struct os_hal_mbox_channel *ch;
	struct mtk_os_hal_mbox_cb_data cb_data;
	void __iomem *base;
	u32 status = 0;

	ch = _mtk_os_hal_mbox_get_channel(channel);
	if (ch == NULL) {
		OS_MBOX_ERROR("get channel failed\n");
		return;
	}
	base = ch->base;
	mtk_mhal_mbox_ioctl(base, MBOX_IOGET_SWINT_EN, &(ch->swint_en));
	mtk_mhal_mbox_ioctl(base, MBOX_IOGET_SWINT_STS, &status);
	ch->swint.channel = channel;
	ch->swint.swint_sts = ch->swint_en & status;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_SWINT, &status);

	cb_data.swint.channel = ch->swint.channel;
	cb_data.swint.swint_sts = ch->swint.swint_sts;

	if (cb_data.swint.swint_sts) {
#ifdef OSAI_FREERTOS
		xSemaphoreTakeFromISR(ch->sem_sw_int, NULL);
#else
		while (ch->sem_sw_int == 0)
			;
		ch->sem_sw_int--;
#endif
		if (ch->swint_cb != NULL)
			ch->swint_cb(&cb_data);
#ifdef OSAI_FREERTOS
		xSemaphoreGiveFromISR(ch->sem_sw_int, NULL);
#else
		ch->sem_sw_int++;
#endif
	}
}

static void _mtk_os_hal_mbox_ca7_sw_int_irq_handler(void)
{
	_mtk_os_hal_mbox_sw_int_irq_handler(OS_HAL_MBOX_CH0);
}

static void _mtk_os_hal_mbox_io_sw_int_irq_handler(void)
{
	_mtk_os_hal_mbox_sw_int_irq_handler(OS_HAL_MBOX_CH1);
}

static void _mtk_os_hal_mbox_fifo_irq_handler(mbox_channel_t channel)
{
	struct os_hal_mbox_channel *ch;
	struct mtk_os_hal_mbox_cb_data cb_data;
	void __iomem *base;
	struct mbox_fifo_event status;
	struct mbox_int_arg int_ctrl;

	ch = _mtk_os_hal_mbox_get_channel(channel);
	if (ch == NULL) {
		OS_MBOX_ERROR("get channel failed\n");
		return;
	}
	base = ch->base;
	mtk_mhal_mbox_ioctl(base, MBOX_IOGET_INT_EN, &(ch->mask));
	mtk_mhal_mbox_ioctl(base, MBOX_IOGET_INT_STS, &status);

	ch->event.channel = channel;
	ch->event.rd_int = ch->mask.rd_int && status.rd_int;
	ch->event.nf_sts = ch->mask.nf_sts && status.nf_sts;
	ch->event.wr_int = ch->mask.wr_int && status.wr_int;
	ch->event.ne_sts = ch->mask.ne_sts && status.ne_sts;

	if (ch->event.rd_int) {
		int_ctrl.type = MBOX_INT_TYPE_RD;
		mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	}

	if (status.nf_sts)
		NVIC_DisableIRQ(nf_vectors[channel]);

	if (ch->event.wr_int) {
		int_ctrl.type = MBOX_INT_TYPE_WR;
		mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	}

	if (status.ne_sts)
		NVIC_DisableIRQ(ne_vectors[channel]);

	memcpy(&(cb_data.event), &(ch->event), sizeof(struct mbox_fifo_event));

	if (cb_data.event.rd_int || cb_data.event.nf_sts
		|| cb_data.event.wr_int || cb_data.event.ne_sts) {
#ifdef OSAI_FREERTOS
		xSemaphoreTakeFromISR(ch->sem_fifo_int, NULL);
#else
		while (ch->sem_fifo_int == 0)
			;
		ch->sem_fifo_int--;
#endif
		if (ch->fifo_cb != NULL)
			ch->fifo_cb(&cb_data);
#ifdef OSAI_FREERTOS
		xSemaphoreGiveFromISR(ch->sem_fifo_int, NULL);
#else
		ch->sem_fifo_int++;
#endif
	}
}

static void _mtk_os_hal_mbox_ca7_fifo_irq_handler(void)
{
	_mtk_os_hal_mbox_fifo_irq_handler(OS_HAL_MBOX_CH0);
}

static void _mtk_os_hal_mbox_io_fifo_irq_handler(void)
{
	_mtk_os_hal_mbox_fifo_irq_handler(OS_HAL_MBOX_CH1);
}

int mtk_os_hal_mbox_sw_int_register_cb(mbox_channel_t channel,
			mtk_os_hal_mbox_cb cb, u32 irq_status)
{
	struct os_hal_mbox_channel *ch;
	void __iomem *base;
	u32 SW_INT_MASK = ((1 << MBOX_NUM_SW_INT) - 1);

	ch = _mtk_os_hal_mbox_get_channel(channel);
	if (ch == NULL) {
		OS_MBOX_ERROR("get channel failed\n");
		return -MBOX_EPTR;
	}

	if (cb == NULL) {
		OS_MBOX_ERROR("cb is null\n");
		return -MBOX_EPTR;
	}

	if (irq_status > SW_INT_MASK) {
		OS_MBOX_ERROR("invalid irq_status:0x%x\n", irq_status);
		return -MBOX_EDEFAULT;
	}

	base = ch->base;

#ifdef OSAI_FREERTOS
	xSemaphoreTake(ch->sem_sw_int, portMAX_DELAY);
#else
	while (ch->sem_sw_int == 0)
		;
	ch->sem_sw_int--;
#endif
	ch->swint_cb = cb;
	ch->swint_en = irq_status;

	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_SWINT, &SW_INT_MASK);

	NVIC_DisableIRQ(sw_vectors[channel]);
	if (channel == OS_HAL_MBOX_CH0)
		NVIC_Register(sw_vectors[channel],
			_mtk_os_hal_mbox_ca7_sw_int_irq_handler);
	else if (channel == OS_HAL_MBOX_CH1)
		NVIC_Register(sw_vectors[channel],
			_mtk_os_hal_mbox_io_sw_int_irq_handler);
	NVIC_SetPriority(sw_vectors[channel], 5);
	NVIC_EnableIRQ(sw_vectors[channel]);

	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_SWINT_EN, &irq_status);

#ifdef OSAI_FREERTOS
	xSemaphoreGive(ch->sem_sw_int);
#else
	ch->sem_sw_int++;
#endif

	return MBOX_OK;
}

int mtk_os_hal_mbox_sw_int_unregister_cb(mbox_channel_t channel)
{
	struct os_hal_mbox_channel *ch;
	void __iomem *base;
	u32 SW_INT_MASK = ((1 << MBOX_NUM_SW_INT) - 1);

	ch = _mtk_os_hal_mbox_get_channel(channel);
	if (ch == NULL) {
		OS_MBOX_ERROR("get channel failed\n");
		return -MBOX_EPTR;
	}
	base = ch->base;

#ifdef OSAI_FREERTOS
	xSemaphoreTake(ch->sem_sw_int, portMAX_DELAY);
#else
	while (ch->sem_sw_int == 0)
		;
	ch->sem_sw_int--;
#endif
	ch->swint_cb = NULL;
	ch->swint_en = 0x0;

	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_SWINT_EN, &(ch->swint_en));
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_SWINT, &SW_INT_MASK);
	NVIC_DisableIRQ(sw_vectors[channel]);

#ifdef OSAI_FREERTOS
	xSemaphoreGive(ch->sem_sw_int);
#else
	ch->sem_sw_int++;
#endif

	return MBOX_OK;
}

int mtk_os_hal_mbox_fifo_register_cb(mbox_channel_t channel,
			mtk_os_hal_mbox_cb cb, struct mbox_fifo_event *mask)
{
	struct os_hal_mbox_channel *ch;
	void __iomem *base;
	struct mbox_int_arg int_ctrl;

	ch = _mtk_os_hal_mbox_get_channel(channel);
	if (ch == NULL) {
		OS_MBOX_ERROR("get channel failed\n");
		return -MBOX_EPTR;
	}

	if (cb == NULL) {
		OS_MBOX_ERROR("cb is null\n");
		return -MBOX_EPTR;
	}

	if (mask == NULL) {
		OS_MBOX_ERROR("mask is null\n");
		return -MBOX_EPTR;
	}

	base = ch->base;

#ifdef OSAI_FREERTOS
	xSemaphoreTake(ch->sem_fifo_int, portMAX_DELAY);
#else
	while (ch->sem_fifo_int == 0)
		;
	ch->sem_fifo_int--;
#endif
	ch->fifo_cb = cb;

	NVIC_DisableIRQ(wr_vectors[channel]);
	NVIC_DisableIRQ(rd_vectors[channel]);
	NVIC_DisableIRQ(ne_vectors[channel]);
	NVIC_DisableIRQ(nf_vectors[channel]);

	int_ctrl.type = MBOX_INT_TYPE_NE;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	int_ctrl.enable = mask->ne_sts;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_INT_EN, &int_ctrl);

	int_ctrl.type = MBOX_INT_TYPE_WR;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	int_ctrl.enable = mask->wr_int;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_INT_EN, &int_ctrl);

	int_ctrl.type = MBOX_INT_TYPE_NF;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	int_ctrl.enable = mask->nf_sts;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_INT_EN, &int_ctrl);

	int_ctrl.type = MBOX_INT_TYPE_RD;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	int_ctrl.enable = mask->rd_int;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_INT_EN, &int_ctrl);

	if (channel == OS_HAL_MBOX_CH0) {
		CM4_Install_NVIC(wr_vectors[channel], 5, IRQ_EDGE_TRIGGER,
			_mtk_os_hal_mbox_ca7_fifo_irq_handler, mask->wr_int);
		CM4_Install_NVIC(rd_vectors[channel], 5, IRQ_EDGE_TRIGGER,
			_mtk_os_hal_mbox_ca7_fifo_irq_handler, mask->rd_int);
		CM4_Install_NVIC(ne_vectors[channel], 5, IRQ_LEVEL_TRIGGER,
			_mtk_os_hal_mbox_ca7_fifo_irq_handler, mask->ne_sts);
		CM4_Install_NVIC(nf_vectors[channel], 5, IRQ_LEVEL_TRIGGER,
			_mtk_os_hal_mbox_ca7_fifo_irq_handler, mask->nf_sts);
	} else if (channel == OS_HAL_MBOX_CH1) {
		CM4_Install_NVIC(wr_vectors[channel], 5, IRQ_EDGE_TRIGGER,
			_mtk_os_hal_mbox_io_fifo_irq_handler, mask->wr_int);
		CM4_Install_NVIC(rd_vectors[channel], 5, IRQ_EDGE_TRIGGER,
			_mtk_os_hal_mbox_io_fifo_irq_handler, mask->rd_int);
		CM4_Install_NVIC(ne_vectors[channel], 5, IRQ_LEVEL_TRIGGER,
			_mtk_os_hal_mbox_io_fifo_irq_handler, mask->ne_sts);
		CM4_Install_NVIC(nf_vectors[channel], 5, IRQ_LEVEL_TRIGGER,
			_mtk_os_hal_mbox_io_fifo_irq_handler, mask->nf_sts);
	}

#ifdef OSAI_FREERTOS
	xSemaphoreGive(ch->sem_fifo_int);
#else
	ch->sem_fifo_int++;
#endif

	return MBOX_OK;
}

int mtk_os_hal_mbox_fifo_unregister_cb(mbox_channel_t channel)
{
	struct os_hal_mbox_channel *ch;
	void __iomem *base;
	struct mbox_int_arg int_ctrl;

	ch = _mtk_os_hal_mbox_get_channel(channel);
	if (ch == NULL) {
		OS_MBOX_ERROR("get channel failed\n");
		return -MBOX_EPTR;
	}
	base = ch->base;

#ifdef OSAI_FREERTOS
	xSemaphoreTake(ch->sem_fifo_int, portMAX_DELAY);
#else
	while (ch->sem_fifo_int == 0)
		;
	ch->sem_fifo_int--;
#endif

	ch->fifo_cb = NULL;

	int_ctrl.enable = 0;

	int_ctrl.type = MBOX_INT_TYPE_NE;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_INT_EN, &int_ctrl);

	int_ctrl.type = MBOX_INT_TYPE_WR;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_INT_EN, &int_ctrl);

	int_ctrl.type = MBOX_INT_TYPE_NF;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_INT_EN, &int_ctrl);

	int_ctrl.type = MBOX_INT_TYPE_RD;
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT, &int_ctrl);
	mtk_mhal_mbox_ioctl(base, MBOX_IOSET_INT_EN, &int_ctrl);

	NVIC_DisableIRQ(wr_vectors[channel]);
	NVIC_DisableIRQ(rd_vectors[channel]);
	NVIC_DisableIRQ(ne_vectors[channel]);
	NVIC_DisableIRQ(nf_vectors[channel]);

#ifdef OSAI_FREERTOS
	xSemaphoreGive(ch->sem_fifo_int);
#else
	ch->sem_fifo_int++;
#endif

	return MBOX_OK;
}

int mtk_os_hal_mbox_open_channel(mbox_channel_t channel)
{
	struct os_hal_mbox_channel *ch;
	void __iomem *base;

	if (channel >= OS_HAL_MBOX_CH_MAX) {
		OS_MBOX_ERROR("channel:%d out of range, should be:0~%d\n",
			channel, OS_HAL_MBOX_CH_MAX-1);
		return -MBOX_EDEFAULT;
	}

	ch = &mbox_dev->channels[channel];

	if (ch->enable) {
		OS_MBOX_DEBUG(" channel %d already opened\n", channel);
		return MBOX_OK;
	}

	ch->base = (void __iomem *)mbox_base_addr[channel];
	base = ch->base;

#ifdef OSAI_FREERTOS
	vSemaphoreCreateBinary(ch->sem_sw_int);
	vSemaphoreCreateBinary(ch->sem_fifo_int);
	vSemaphoreCreateBinary(ch->sem_fifo_read);
	vSemaphoreCreateBinary(ch->sem_fifo_write);
#else
	ch->sem_sw_int = 1;
	ch->sem_fifo_int = 1;
	ch->sem_fifo_read = 1;
	ch->sem_fifo_write = 1;
#endif

	mtk_mhal_mbox_reset(base);
	ch->enable = 1;

	return MBOX_OK;
}

int mtk_os_hal_mbox_close_channel(mbox_channel_t channel)
{
	struct os_hal_mbox_channel *ch;

	if (channel >= OS_HAL_MBOX_CH_MAX) {
		OS_MBOX_ERROR("channel:%d out of range, should be:0~%d\n",
			channel, OS_HAL_MBOX_CH_MAX-1);
		return -MBOX_EDEFAULT;
	}

	ch = &mbox_dev->channels[channel];

	if (!ch->enable) {
		OS_MBOX_DEBUG(" channel %d already closed\n", channel);
		return MBOX_OK;
	}

#ifdef OSAI_FREERTOS
	vSemaphoreDelete(ch->sem_sw_int);
	vSemaphoreDelete(ch->sem_fifo_int);
	vSemaphoreDelete(ch->sem_fifo_read);
	vSemaphoreDelete(ch->sem_fifo_write);
#else
	ch->sem_sw_int = 0;
	ch->sem_fifo_int = 0;
	ch->sem_fifo_read = 0;
	ch->sem_fifo_write = 0;
#endif

	ch->base = NULL;
	ch->enable = 0;

	return MBOX_OK;
}

int mtk_os_hal_mbox_fifo_read(mbox_channel_t channel,
			struct mbox_fifo_item *buf,
			mbox_tr_type_t type)
{
	int ret;
	struct os_hal_mbox_channel *ch;
	struct mbox_int_arg int_ctrl;
	void __iomem *base;

	ch = _mtk_os_hal_mbox_get_channel(channel);
	if (ch == NULL) {
		OS_MBOX_ERROR("get channel failed\n");
		return -MBOX_EPTR;
	}
	base = ch->base;

#ifdef OSAI_FREERTOS
	if (pdTRUE != xSemaphoreTake(ch->sem_fifo_read, portMAX_DELAY)) {
		OS_MBOX_ERROR("take sem_fifo_read fail\n");
		return -MBOX_EDEFAULT;
	}
#else
	extern volatile u32 sys_tick_in_ms;
	uint32_t start_tick = sys_tick_in_ms;

	while (ch->sem_fifo_read == 0) {
		if (sys_tick_in_ms - start_tick > 60000)
			return -1;
	}
	ch->sem_fifo_read--;
#endif

	ret = mtk_mhal_mbox_fifo_read(base, buf, type);

	if (ret == MBOX_OK) {
		mtk_mhal_mbox_ioctl(base, MBOX_IOGET_INT_EN, &ch->mask);
		if (ch->mask.ne_sts) {
			int_ctrl.type = MBOX_INT_TYPE_NE;
			mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT,
				&int_ctrl);
			NVIC_EnableIRQ(ne_vectors[channel]);
		}
	}

#ifdef OSAI_FREERTOS
	xSemaphoreGive(ch->sem_fifo_read);
#else
	ch->sem_fifo_read++;
#endif

	return ret;
}

int mtk_os_hal_mbox_fifo_write(mbox_channel_t channel,
			const struct mbox_fifo_item *buf,
			mbox_tr_type_t type)
{
	int ret;
	struct os_hal_mbox_channel *ch;
	struct mbox_int_arg int_ctrl;
	void __iomem *base;

	ch = _mtk_os_hal_mbox_get_channel(channel);
	if (ch == NULL) {
		OS_MBOX_ERROR("get channel failed\n");
		return -MBOX_EPTR;
	}
	base = ch->base;

#ifdef OSAI_FREERTOS
	if (pdTRUE != xSemaphoreTake(ch->sem_fifo_write, portMAX_DELAY)) {
		OS_MBOX_ERROR("take sem_fifo_write fail\n");
		return -MBOX_EDEFAULT;
	}
#else
	extern volatile u32 sys_tick_in_ms;
	uint32_t start_tick = sys_tick_in_ms;

	while (ch->sem_fifo_write == 0) {
		if (sys_tick_in_ms - start_tick > 60000)
			return -1;
	}
	ch->sem_fifo_write--;
#endif

	ret = mtk_mhal_mbox_fifo_write(base, buf, type);

	if (ret == MBOX_OK) {
		mtk_mhal_mbox_ioctl(base, MBOX_IOGET_INT_EN, &ch->mask);
		if (ch->mask.nf_sts) {
			int_ctrl.type = MBOX_INT_TYPE_NF;
			mtk_mhal_mbox_ioctl(base, MBOX_IOSET_CLEAR_INT,
				&int_ctrl);
			NVIC_EnableIRQ(nf_vectors[channel]);
		}
	}

#ifdef OSAI_FREERTOS
	xSemaphoreGive(ch->sem_fifo_write);
#else
	ch->sem_fifo_write++;
#endif

	return ret;
}

int mtk_os_hal_mbox_ioctl(mbox_channel_t channel,
			mbox_ioctl_t ctrl, void *arg)
{
	int ret;
	struct os_hal_mbox_channel *ch;
	void __iomem *base;

	ch = _mtk_os_hal_mbox_get_channel(channel);
	if (ch == NULL) {
		OS_MBOX_ERROR("get channel failed\n");
		return -MBOX_EPTR;
	}
	base = ch->base;

	if (ctrl == MBOX_IOGET_POST_FIFO_CNT) {
#ifdef OSAI_FREERTOS
		if (pdTRUE !=
			xSemaphoreTake(ch->sem_fifo_write, portMAX_DELAY)) {
			OS_MBOX_ERROR("take sem_fifo_write fail\n");
			return -MBOX_EDEFAULT;
		}
#else
	extern volatile u32 sys_tick_in_ms;
	uint32_t start_tick = sys_tick_in_ms;

	while (ch->sem_fifo_write == 0) {
		if (sys_tick_in_ms - start_tick > 60000)
			return -1;
	}
	ch->sem_fifo_write--;
#endif
	} else if (ctrl == MBOX_IOGET_ACPT_FIFO_CNT) {
#ifdef OSAI_FREERTOS
		if (pdTRUE !=
			xSemaphoreTake(ch->sem_fifo_read, portMAX_DELAY)) {
			OS_MBOX_ERROR("take sem_fifo_read fail\n");
			return -MBOX_EDEFAULT;
		}
#else
	extern volatile u32 sys_tick_in_ms;
	uint32_t start_tick = sys_tick_in_ms;

	while (ch->sem_fifo_read == 0) {
		if (sys_tick_in_ms - start_tick > 60000)
			return -1;
	}
	ch->sem_fifo_read--;
#endif
	}

	ret = mtk_mhal_mbox_ioctl(base, ctrl, arg);

	if (ctrl == MBOX_IOGET_POST_FIFO_CNT)
#ifdef OSAI_FREERTOS
		xSemaphoreGive(ch->sem_fifo_write);
#else
		ch->sem_fifo_write++;
#endif
	else if (ctrl == MBOX_IOGET_ACPT_FIFO_CNT)
#ifdef OSAI_FREERTOS
		xSemaphoreGive(ch->sem_fifo_read);
#else
		ch->sem_fifo_read++;
#endif
	return ret;
}


