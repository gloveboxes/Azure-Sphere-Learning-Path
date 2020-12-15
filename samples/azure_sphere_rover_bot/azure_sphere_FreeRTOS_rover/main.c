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
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "FreeRTOS.h"
#include "task.h"
#include "printf.h"
#include "mt3620.h"

#include "os_hal_gpio.h"
#include "os_hal_uart.h"

#include "semphr.h"

#include "../Shared/inc/inter_core_definition.h"
#include "hw/azure_sphere_learning_path.h"
#include "mt3620-intercore.h" // Support for inter Core Communications

 /******************************************************************************/
 /* Configurations */
 /******************************************************************************/


static LP_INTER_CORE_BLOCK ic_control_block;

#define UART_PORT_NUM OS_HAL_UART_ISU0
#define APP_STACK_SIZE_BYTES (1024 / 4)

static SemaphoreHandle_t GPT3Semphr;

// Inter-core Communications
static const size_t payloadStart = 20;
static uint8_t buf[256];
static uint32_t dataSize;
static BufferHeader* outbound, * inbound;
static uint32_t sharedBufSize = 0;

bool HLAppReady = false;
float distance_left, distance_right, newDistanceLeft, newDistanceRight;
bool newDataReady = false;

static const uintptr_t GPT_BASE = 0x21030000;

enum LEDS
{
	RED,
	GREEN,
	BLUE
};

static enum LEDS current_led = RED;
static int leds[] = { LED_RED, LED_GREEN, LED_BLUE };
static bool led_state[] = { false, false, false };


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


//https://embeddedartistry.com/blog/2018/01/15/implementing-malloc-with-freertos/

/*
malloc and free overrides are here to support srand and rand for random number generator for fake telemetry
srand and rand call malloc to allocate 24 bytes on the heap and this override directs mallow to pull memory from the FreeRTOS heap
*/

void* malloc(size_t size)
{
	void* ptr = NULL;

	if (size > 0)
	{
		// We simply wrap the FreeRTOS call into a standard form
		ptr = pvPortMalloc(size);
	} // else NULL if there was an error

	return ptr;
}

void free(void* ptr)
{
	if (ptr)
	{
		// We simply wrap the FreeRTOS call into a standard form
		vPortFree(ptr);
	}
}

static int gpio_output(u8 gpio_no, u8 level)
{
	int ret;

	ret = mtk_os_hal_gpio_request(gpio_no);
	if (ret != 0)
	{
		printf("request gpio[%d] fail\n", gpio_no);
		return ret;
	}
	mtk_os_hal_gpio_set_direction(gpio_no, OS_HAL_GPIO_DIR_OUTPUT);
	mtk_os_hal_gpio_set_output(gpio_no, level);
	ret = mtk_os_hal_gpio_free(gpio_no);
	if (ret != 0)
	{
		printf("free gpio[%d] fail\n", gpio_no);
		return ret;
	}
	return 0;
}

void send_inter_core_msg(void)
{
	if (HLAppReady)
	{
		memcpy((void*)&buf[payloadStart], (void*)&ic_control_block, sizeof(ic_control_block));
		dataSize = payloadStart + sizeof(ic_control_block);

		EnqueueData(inbound, outbound, sharedBufSize, buf, dataSize);
	}
}

void WriteReg32(uintptr_t baseAddr, size_t offset, uint32_t value)
{
	*(volatile uint32_t*)(baseAddr + offset) = value;
}

uint32_t ReadReg32(uintptr_t baseAddr, size_t offset)
{
	return *(volatile uint32_t*)(baseAddr + offset);
}

void Gpt3_WaitUs(int microseconds)
{
	// GPT3_INIT = initial counter value
	WriteReg32(GPT_BASE, 0x54, 0x0);

	// GPT3_CTRL
	uint32_t ctrlOn = 0x0;
	ctrlOn |= (0x19) << 16; // OSC_CNT_1US (default value)
	ctrlOn |= 0x1;          // GPT3_EN = 1 -> GPT3 enabled
	WriteReg32(GPT_BASE, 0x50, ctrlOn);

	// GPT3_CNT
	while (ReadReg32(GPT_BASE, 0x58) < microseconds)
	{
		// empty.
	}

	// GPT_CTRL -> disable timer
	WriteReg32(GPT_BASE, 0x50, 0x0);
}

bool readInput(u8 pin)
{
	os_hal_gpio_data value = 0;
	mtk_os_hal_gpio_get_input(pin, &value);
	return value == OS_HAL_GPIO_DATA_HIGH;
}

float get_distance(u8 pin, unsigned long timeoutMicroseconds)
{
	uint32_t pulseBegin, pulseEnd;

	mtk_os_hal_gpio_set_direction(pin, OS_HAL_GPIO_DIR_OUTPUT);	// set for output
	mtk_os_hal_gpio_set_output(pin, OS_HAL_GPIO_DATA_LOW);		// pull low
	Gpt3_WaitUs(2);

	mtk_os_hal_gpio_set_output(pin, OS_HAL_GPIO_DATA_HIGH);		// pull high
	Gpt3_WaitUs(5);

	// GPT3_CTRL - starts microsecond resolution clock
	uint32_t ctrlOn = 0x0;
	ctrlOn |= (0x19) << 16; // OSC_CNT_1US (default value)
	ctrlOn |= 0x1;          // GPT3_EN = 1 -> GPT3 enabled
	WriteReg32(GPT_BASE, 0x50, ctrlOn);

	mtk_os_hal_gpio_set_direction(pin, OS_HAL_GPIO_DIR_INPUT);	// set for input

	while (readInput(pin))		// wait for any previous pulse to end
	{
		if (ReadReg32(GPT_BASE, 0x58) > timeoutMicroseconds)
		{
			return NAN;
		}
	}

	while (!readInput(pin))		// wait for the pulse to start
	{
		pulseBegin = ReadReg32(GPT_BASE, 0x58);
		if (ReadReg32(GPT_BASE, 0x58) > timeoutMicroseconds)
		{
			return NAN;
		}
	}

	pulseBegin = ReadReg32(GPT_BASE, 0x58);

	while (readInput(pin))		// wait for the pulse to stop
	{
		if (ReadReg32(GPT_BASE, 0x58) > timeoutMicroseconds)
		{
			return NAN;
		}
	}

	pulseEnd = ReadReg32(GPT_BASE, 0x58);

	// GPT_CTRL -> disable timer
	WriteReg32(GPT_BASE, 0x50, 0x0);

	return (pulseEnd - pulseBegin) / 58.0; //  29 / 2;
}

void SetDistanceIndicator(int centimeters)
{
	static enum LEDS previous_led = RED;

	current_led = centimeters > 20 ? GREEN : centimeters > 10 ? BLUE : RED;

	if (previous_led != current_led)
	{
		gpio_output(leds[(int)previous_led], true); // turn off old current colour
		previous_led = current_led;
	}

	gpio_output(leds[(int)current_led], false);
}

static void MeasureDistance(void* pParameters)
{
	while (true)
	{
		distance_left = get_distance(HCSR04_LEFT, 3000);
		distance_right = get_distance(HCSR04_RIGHT, 3000);

		if (!isnan(distance_left))
		{
			SetDistanceIndicator((int)distance_left);
			newDistanceLeft = distance_left;
			newDataReady = true;
		}
		else
		{
			for (size_t i = 0; i < 3; i++)
			{
				gpio_output(leds[i], true);
			}
		}

		if (!isnan(distance_right))
		{
			newDistanceRight = distance_right;
			newDataReady = true;
		}


		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

static void UpdateAzureIoT(void* pParameters)
{
	while (true)
	{
		vTaskDelay(pdMS_TO_TICKS(3000));

		if (newDataReady)
		{
			ic_control_block.cmd = LP_IC_REPORT_DISTANCE;
			ic_control_block.distance_left = (int)newDistanceLeft;
			ic_control_block.distance_right = (int)newDistanceRight;

			send_inter_core_msg();

			newDataReady = false;
		}
	}
}

static void RTCoreMsgTask(void* pParameters)
{
	while (1)
	{
		dataSize = sizeof(buf);
		int r = DequeueData(outbound, inbound, sharedBufSize, buf, &dataSize);

		if (r == 0 && dataSize > payloadStart)
		{
			HLAppReady = true;

			memcpy((void*)&ic_control_block, (void*)&buf[payloadStart], sizeof(ic_control_block));

			switch (ic_control_block.cmd)
			{
			case LP_IC_HEARTBEAT:
				break;
			default:
				break;
			}
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
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
	if (GetIntercoreBuffers(&outbound, &inbound, &sharedBufSize) == -1)
	{
		for (;;)
		{
			// empty.
		}
	}

	GPT3Semphr = xSemaphoreCreateMutex();

	xTaskCreate(MeasureDistance, "Measure Distance Task", APP_STACK_SIZE_BYTES, NULL, 7, NULL);
	xTaskCreate(UpdateAzureIoT, "Update Azure IoT Task", APP_STACK_SIZE_BYTES, NULL, 5, NULL);

	xTaskCreate(RTCoreMsgTask, "RTCore Msg Task", APP_STACK_SIZE_BYTES, NULL, 2, NULL);
	vTaskStartScheduler();

	for (;;)
	{
		__asm__("wfi");
	}
}

