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

/*
Date: March 2020
This software has been modified by Dave Glover
Updated: Added blink and inter-core communications
*/


#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "printf.h"
#include "mt3620.h"

#include "os_hal_gpio.h"
#include "os_hal_uart.h"

#include "semphr.h"
#include "../oem/board.h"


/******************************************************************************/
/* Configurations */
/******************************************************************************/


#define UART_PORT_NUM OS_HAL_UART_ISU0

#define APP_STACK_SIZE_BYTES (1024 / 4)


static const int blinkIntervalsMs[] = { 75, 125, 250, 500, 1000, 2000 };
static int blinkIntervalIndex = 0;
static const int numBlinkIntervals = sizeof(blinkIntervalsMs) / sizeof(blinkIntervalsMs[0]);
static SemaphoreHandle_t LEDSemphr;
static bool BuiltInLedOn = false;


// Inter-core Communications
#include "mt3620-intercore.h" // Support for inter Core Communications
static const size_t payloadStart = 20;
static uint8_t buf[256];
static uint32_t dataSize;
static bool buttonA_Pressed = false;
static bool buttonB_Pressed = false;
static BufferHeader* outbound, * inbound;
static uint32_t sharedBufSize = 0;


/******************************************************************************/
/* Application Hooks */
/******************************************************************************/
// Hook for "stack over flow".
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
	printf("%s: %s\n", __func__, pcTaskName);
}

// Hook for "memory allocation failed".
void vApplicationMallocFailedHook(void)
{
	printf("%s\n", __func__);
}

// Hook for "printf".
void _putchar(char character)
{
	mtk_os_hal_uart_put_char(UART_PORT_NUM, character);
	if (character == '\n')
		mtk_os_hal_uart_put_char(UART_PORT_NUM, '\r');
}

/******************************************************************************/
/* Functions */
/******************************************************************************/
static int gpio_output(u8 gpio_no, u8 level)
{
	int ret;

	ret = mtk_os_hal_gpio_request(gpio_no);
	if (ret != 0) {
		printf("request gpio[%d] fail\n", gpio_no);
		return ret;
	}
	mtk_os_hal_gpio_set_direction(gpio_no, OS_HAL_GPIO_DIR_OUTPUT);
	mtk_os_hal_gpio_set_output(gpio_no, level);
	ret = mtk_os_hal_gpio_free(gpio_no);
	if (ret != 0) {
		printf("free gpio[%d] fail\n", gpio_no);
		return ret;
	}
	return 0;
}

static int gpio_input(u8 gpio_no, os_hal_gpio_data* pvalue)
{
	u8 ret;

	ret = mtk_os_hal_gpio_request(gpio_no);
	if (ret != 0) {
		printf("request gpio[%d] fail\n", gpio_no);
		return ret;
	}
	mtk_os_hal_gpio_set_direction(gpio_no, OS_HAL_GPIO_DIR_INPUT);
	vTaskDelay(pdMS_TO_TICKS(10));

	mtk_os_hal_gpio_get_input(gpio_no, pvalue);

	ret = mtk_os_hal_gpio_free(gpio_no);
	if (ret != 0) {
		printf("free gpio[%d] fail\n", gpio_no);
		return ret;
	}

	return 0;
}

static void ButtonTask(void* pParameters)
{
	os_hal_gpio_data value = 0;

	printf("GPIO Task Started\n");
	while (1) {

		// Get Button_A status
		gpio_input(BUTTON_A, &value);

		if (value == OS_HAL_GPIO_DATA_LOW) {
			blinkIntervalIndex = (blinkIntervalIndex + 1) % numBlinkIntervals;
			buttonA_Pressed = true;
		}

		// Get Button_B status
		gpio_input(BUTTON_B, &value);

		if (value == OS_HAL_GPIO_DATA_LOW) {
			buttonB_Pressed = true;
		}

		// Delay for 100ms
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

static void PeriodicTask(void* pParameters)
{
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(blinkIntervalsMs[blinkIntervalIndex]));
		xSemaphoreGive(LEDSemphr);
	}
}

static void LedTask(void* pParameters)
{
	BaseType_t rt;

	while (1) {
		rt = xSemaphoreTake(LEDSemphr, portMAX_DELAY);
		if (rt == pdPASS) {
			BuiltInLedOn = !BuiltInLedOn;
			gpio_output(LED1, BuiltInLedOn);
		}
	}
}

static void RTCoreMsgTask(void* pParameters)
{
	bool HLAppReady = false;

	while (1) {
		dataSize = sizeof(buf);
		int r = DequeueData(outbound, inbound, sharedBufSize, buf, &dataSize);

		//if (!(r == -1 || dataSize < payloadStart)) {
		if (r == 0 && dataSize > payloadStart) {
			HLAppReady = true;
		}

		if (buttonA_Pressed && HLAppReady) {
			const char msg[] = "ButtonA";
			strncpy((char*)buf + payloadStart, msg, sizeof buf - payloadStart);
			dataSize = payloadStart + sizeof msg - 1;

			EnqueueData(inbound, outbound, sharedBufSize, buf, dataSize);
			buttonA_Pressed = false;
		}

		if (buttonB_Pressed && HLAppReady) {
			const char msg[] = "ButtonB";
			strncpy((char*)buf + payloadStart, msg, sizeof buf - payloadStart);
			dataSize = payloadStart + sizeof msg - 1;

			EnqueueData(inbound, outbound, sharedBufSize, buf, dataSize);
			buttonB_Pressed = false;
		}

		// Delay for 100ms
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

_Noreturn void RTCoreMain(void)
{
	// Setup Vector Table
	NVIC_SetupVectorTable();

	// Init UART
	mtk_os_hal_uart_ctlr_init(UART_PORT_NUM);
	printf("\nFreeRTOS GPIO Demo\n");


	// Initialize Inter-Core Communications
	if (GetIntercoreBuffers(&outbound, &inbound, &sharedBufSize) == -1) {
		for (;;) {
			// empty.
		}
	}

	LEDSemphr = xSemaphoreCreateBinary();

	xTaskCreate(PeriodicTask, "Periodic Task", APP_STACK_SIZE_BYTES, NULL, 6, NULL);
	xTaskCreate(LedTask, "LED Task", APP_STACK_SIZE_BYTES, NULL, 5, NULL);
	xTaskCreate(ButtonTask, "GPIO Task", APP_STACK_SIZE_BYTES, NULL, 4, NULL);
	xTaskCreate(RTCoreMsgTask, "RTCore Msg Task", APP_STACK_SIZE_BYTES, NULL, 2, NULL);
	vTaskStartScheduler();

	for (;;) {
		__asm__("wfi");
	}
}

