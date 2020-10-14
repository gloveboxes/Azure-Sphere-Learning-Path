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

#include "nvic.h"

#include "os_hal_dma.h"
#include "os_hal_spim.h"

#define ISU0_SPIM_BASE			0x38070300
#define ISU1_SPIM_BASE			0x38080300
#define ISU2_SPIM_BASE			0x38090300
#define ISU3_SPIM_BASE			0x380a0300
#define ISU4_SPIM_BASE			0x380b0300

static unsigned long spim_base_addr[OS_HAL_SPIM_ISU_MAX] = {
	ISU0_SPIM_BASE,
	ISU1_SPIM_BASE,
	ISU2_SPIM_BASE,
	ISU3_SPIM_BASE,
	ISU4_SPIM_BASE,
};

static int spim_dma_chan[OS_HAL_SPIM_ISU_MAX][2] = {
	/* [0]:tx, [1]:rx */
	{DMA_ISU0_TX_CH0, DMA_ISU0_RX_CH1},
	{DMA_ISU1_TX_CH2, DMA_ISU1_RX_CH3},
	{DMA_ISU2_TX_CH4, DMA_ISU2_RX_CH5},
	{DMA_ISU3_TX_CH6, DMA_ISU3_RX_CH7},
	{DMA_ISU4_TX_CH8, DMA_ISU4_RX_CH9},
};

#define ISU0_CG_BASE	0x38070000
#define ISU1_CG_BASE	0x38080000
#define ISU2_CG_BASE	0x38090000
#define ISU3_CG_BASE	0x380a0000
#define ISU4_CG_BASE	0x380b0000

static unsigned long cg_base_addr[OS_HAL_SPIM_ISU_MAX] = {
	ISU0_CG_BASE,
	ISU1_CG_BASE,
	ISU2_CG_BASE,
	ISU3_CG_BASE,
	ISU4_CG_BASE,
};

/**
 * this os special spi structure, need mapping it to mtk_spi_controller
 */
struct mtk_spi_controller_rtos {
	struct mtk_spi_controller *ctlr;

	/* the type based on OS */
	volatile u8 xfer_completion;

	/* used for async API */
	spi_usr_complete_callback complete;
	void *context;
};

static struct mtk_spi_controller_rtos g_spim_ctlr_rtos[OS_HAL_SPIM_ISU_MAX];
static struct mtk_spi_controller g_spim_ctlr[OS_HAL_SPIM_ISU_MAX];
static struct mtk_spi_private g_spim_mdata[OS_HAL_SPIM_ISU_MAX];

static unsigned char
	tmp_tx_buffer[OS_HAL_SPIM_ISU_MAX][MTK_SPIM_DMA_BUFFER_BYTES] __attribute__((section(".sysram")));
static unsigned char
	tmp_rx_buffer[OS_HAL_SPIM_ISU_MAX][MTK_SPIM_DMA_BUFFER_BYTES] __attribute__((section(".sysram")));

static struct mtk_spi_controller_rtos *
	_mtk_os_hal_spim_get_ctlr(spim_num bus_num)
{
	if (bus_num > OS_HAL_SPIM_ISU_MAX - 1) {
		printf("invalid, bus_num should be 0~%d\n",
			OS_HAL_SPIM_ISU_MAX - 1);
		return NULL;
	}

	return &g_spim_ctlr_rtos[bus_num];
}

int mtk_os_hal_spim_dump_reg(spim_num bus_num)
{
	struct mtk_spi_controller_rtos *ctlr_rtos;
	struct mtk_spi_controller *ctlr;

	ctlr_rtos = _mtk_os_hal_spim_get_ctlr(bus_num);
	if (!ctlr_rtos)
		return -1;

	ctlr = ctlr_rtos->ctlr;

	mtk_mhal_spim_enable_clk(ctlr);
	mtk_mhal_spim_dump_reg(ctlr);
	mtk_mhal_spim_disable_clk(ctlr);

	return 0;
}

static int _mtk_os_hal_spim_irq_handler(spim_num bus_num)
{
	struct mtk_spi_controller_rtos *ctlr_rtos;
	struct mtk_spi_controller *ctlr;
	struct mtk_spi_transfer *curr_xfer;


	ctlr_rtos = _mtk_os_hal_spim_get_ctlr(bus_num);
	if (!ctlr_rtos)
		return -1;

	ctlr = ctlr_rtos->ctlr;
	curr_xfer = ctlr->current_xfer;

	mtk_mhal_spim_clear_irq_status(ctlr);

	if (!curr_xfer->use_dma)
		mtk_mhal_spim_fifo_handle_rx(ctlr, curr_xfer);

	/* 1. FIFO mode: return completion done in SPI irq handler
	 * 2. DMA mode: return rx completion done in DMA irq handler
	 */
	if (!curr_xfer->use_dma || (curr_xfer->tx_buf && !curr_xfer->rx_buf)) {
		if (ctlr_rtos->complete) {
			/* async xfer */
			ctlr_rtos->complete(ctlr_rtos->context);
			ctlr_rtos->complete = NULL;
			ctlr_rtos->context = NULL;
			mtk_mhal_spim_disable_clk(ctlr_rtos->ctlr);
		} else {
			/* sync xfer */
			ctlr_rtos->xfer_completion++;
		}
	}

	return 0;
}

static void _mtk_os_hal_spim0_irq_event(void)
{
	_mtk_os_hal_spim_irq_handler(OS_HAL_SPIM_ISU0);
}

static void _mtk_os_hal_spim1_irq_event(void)
{
	_mtk_os_hal_spim_irq_handler(OS_HAL_SPIM_ISU1);
}

static void _mtk_os_hal_spim2_irq_event(void)
{
	_mtk_os_hal_spim_irq_handler(OS_HAL_SPIM_ISU2);
}

static void _mtk_os_hal_spim3_irq_event(void)
{
	_mtk_os_hal_spim_irq_handler(OS_HAL_SPIM_ISU3);
}

static void _mtk_os_hal_spim4_irq_event(void)
{
	_mtk_os_hal_spim_irq_handler(OS_HAL_SPIM_ISU4);
}

static void _mtk_os_hal_spim_request_irq(int bus_num)
{
	switch (bus_num) {
	case 0:
		CM4_Install_NVIC(CM4_IRQ_ISU_G0_SPIM, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_spim0_irq_event,
				 TRUE);
		break;
	case 1:
		CM4_Install_NVIC(CM4_IRQ_ISU_G1_SPIM, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_spim1_irq_event,
				 TRUE);
		break;
	case 2:
		CM4_Install_NVIC(CM4_IRQ_ISU_G2_SPIM, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_spim2_irq_event,
				 TRUE);
		break;
	case 3:
		CM4_Install_NVIC(CM4_IRQ_ISU_G3_SPIM, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_spim3_irq_event,
				 TRUE);
		break;
	case 4:
		CM4_Install_NVIC(CM4_IRQ_ISU_G4_SPIM, DEFAULT_PRI,
				 IRQ_LEVEL_TRIGGER, _mtk_os_hal_spim4_irq_event,
				 TRUE);
		break;
	}
}

static void _mtk_os_hal_spim_free_irq(int bus_num)
{
	switch (bus_num) {
	case 0:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G0_SPIM);
		break;
	case 1:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G1_SPIM);
		break;
	case 2:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G2_SPIM);
		break;
	case 3:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G3_SPIM);
		break;
	case 4:
		NVIC_DisableIRQ((IRQn_Type)CM4_IRQ_ISU_G4_SPIM);
		break;
	}
}

static int _mtk_os_hal_spim_dma_done_callback(void *data)
{
	struct mtk_spi_controller_rtos *ctlr_rtos = data;

	if (ctlr_rtos->complete) {
		ctlr_rtos->complete(ctlr_rtos->context);
		ctlr_rtos->complete = NULL;
		ctlr_rtos->context = NULL;
		mtk_mhal_spim_disable_clk(ctlr_rtos->ctlr);
	} else {
		/* while using DMA mode to do sync xfer,
		 * release semaphore in this callback
		 */
		ctlr_rtos->xfer_completion++;
	}

	return 0;
}

int mtk_os_hal_spim_ctlr_init(spim_num bus_num)
{
	struct mtk_spi_controller_rtos *ctlr_rtos;
	struct mtk_spi_controller *ctlr;

	ctlr_rtos = _mtk_os_hal_spim_get_ctlr(bus_num);
	if (!ctlr_rtos)
		return -1;

	memset(ctlr_rtos, 0, sizeof(struct mtk_spi_controller_rtos));

	/* Must init first here  */
	ctlr_rtos->ctlr = &g_spim_ctlr[bus_num];
	ctlr = ctlr_rtos->ctlr;
	ctlr->mdata = &g_spim_mdata[bus_num];

	ctlr->dma_tmp_tx_buf = tmp_tx_buffer[bus_num];
	ctlr->dma_tmp_rx_buf = tmp_rx_buffer[bus_num];

	ctlr->base = (void __iomem *)spim_base_addr[bus_num];
	ctlr->cg_base = (void __iomem *)cg_base_addr[bus_num];

	ctlr->dma_tx_chan = spim_dma_chan[bus_num][0];
	ctlr->dma_rx_chan = spim_dma_chan[bus_num][1];

	ctlr_rtos->xfer_completion = 0;

	mtk_mhal_spim_dma_done_callback_register(ctlr,
					 _mtk_os_hal_spim_dma_done_callback,
					 (void *)ctlr_rtos);

	mtk_mhal_spim_allocate_dma_chan(ctlr);

	_mtk_os_hal_spim_request_irq(bus_num);

	return 0;
}

int mtk_os_hal_spim_ctlr_deinit(spim_num bus_num)
{
	struct mtk_spi_controller_rtos *ctlr_rtos;
	struct mtk_spi_controller *ctlr;

	ctlr_rtos = _mtk_os_hal_spim_get_ctlr(bus_num);
	if (!ctlr_rtos)
		return -1;

	ctlr = ctlr_rtos->ctlr;

	_mtk_os_hal_spim_free_irq(bus_num);
	mtk_mhal_spim_release_dma_chan(ctlr);

	ctlr_rtos->ctlr = NULL;

	return 0;
}

static int _mtk_os_hal_spim_wait_for_completion_timeout(
				struct mtk_spi_controller_rtos
				*ctlr_rtos, int time_ms)
{
	while(ctlr_rtos->xfer_completion == 0) {}
	ctlr_rtos->xfer_completion--;
	return 0;
}

int mtk_os_hal_spim_transfer(spim_num bus_num,
			     struct mtk_spi_config *config,
			     struct mtk_spi_transfer *xfer)
{
	struct mtk_spi_controller_rtos *ctlr_rtos;
	struct mtk_spi_controller *ctlr;
	int ret;

	ctlr_rtos = _mtk_os_hal_spim_get_ctlr(bus_num);
	if (!ctlr_rtos)
		return -1;

	ctlr = ctlr_rtos->ctlr;

	mtk_mhal_spim_enable_clk(ctlr);

	mtk_mhal_spim_prepare_hw(ctlr, config);
	mtk_mhal_spim_prepare_transfer(ctlr, xfer);

	if (xfer->use_dma)
		ret = mtk_mhal_spim_dma_transfer_one(ctlr, xfer);
	else
		ret = mtk_mhal_spim_fifo_transfer_one(ctlr, xfer);

	if (ret) {
		printf("spi master transfer one fail.\n");
		goto err_xfer_fail;
	}

	ret = _mtk_os_hal_spim_wait_for_completion_timeout(ctlr_rtos, 1000);
	if (ret)
		printf("Take spi master Semaphore timeout!\n");

err_xfer_fail:
	mtk_mhal_spim_disable_clk(ctlr);

	return ret;
}

int mtk_os_hal_spim_async_transfer(spim_num bus_num,
				   struct mtk_spi_config *config,
				   struct mtk_spi_transfer *xfer,
				   spi_usr_complete_callback complete,
				   void *context)
{
	struct mtk_spi_controller_rtos *ctlr_rtos;
	struct mtk_spi_controller *ctlr;
	int ret;

	ctlr_rtos = _mtk_os_hal_spim_get_ctlr(bus_num);
	if (!ctlr_rtos)
		return -1;

	/* async user private function */
	ctlr_rtos->complete = complete;
	ctlr_rtos->context = context;

	ctlr = ctlr_rtos->ctlr;

	mtk_mhal_spim_enable_clk(ctlr);

	mtk_mhal_spim_prepare_hw(ctlr, config);
	mtk_mhal_spim_prepare_transfer(ctlr, xfer);

	if (xfer->use_dma)
		ret = mtk_mhal_spim_dma_transfer_one(ctlr, xfer);
	else
		ret = mtk_mhal_spim_fifo_transfer_one(ctlr, xfer);

	if (ret) {
		printf("spi master async one fail.\n");
		ctlr_rtos->complete = NULL;
		ctlr_rtos->context = NULL;
		mtk_mhal_spim_disable_clk(ctlr);
	}

	return ret;
}
