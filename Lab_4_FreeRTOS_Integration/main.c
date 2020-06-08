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


#ifdef OEM_AVNET
#include "lsm6dso_driver.h"
#include "lsm6dso_reg.h"
#include "i2c.h"
#endif // OEM_AVNET


#include "hw/azure_sphere_learning_path.h"


 /******************************************************************************/
 /* Configurations */
 /******************************************************************************/

enum LP_INTER_CORE_CMD
{
	LP_IC_UNKNOWN,
	LP_IC_HEARTBEAT,
	LP_IC_TEMPERATURE_PRESSURE_HUMIDITY,
	LP_IC_EVENT_BUTTON_A,
	LP_IC_EVENT_BUTTON_B,
	LP_IC_SET_DESIRED_TEMPERATURE,
	LP_IC_BLINK_RATE
};

typedef struct LP_INTER_CORE_BLOCK
{
	enum LP_INTER_CORE_CMD cmd;
	float	temperature;
	float	pressure;
	float	humidity;
	int		blinkRate;

} LP_INTER_CORE_BLOCK;

static LP_INTER_CORE_BLOCK ic_control_block;

#define UART_PORT_NUM OS_HAL_UART_ISU0
#define APP_STACK_SIZE_BYTES (1024 / 4)


static const int blinkIntervalsMs[] = { 125, 250, 500, 750, 1000, 2000 };
static int blinkIntervalIndex = 3;
static const int numBlinkIntervals = sizeof(blinkIntervalsMs) / sizeof(blinkIntervalsMs[0]);
static SemaphoreHandle_t LEDSemphr;


// Inter-core Communications
#include "mt3620-intercore.h" // Support for inter Core Communications
static const size_t payloadStart = 20;
static uint8_t buf[256];
static uint32_t dataSize;
static BufferHeader* outbound, * inbound;
static uint32_t sharedBufSize = 0;

bool HLAppReady = false;
int desired_temperature = 0.0;
int last_temperature = 0;

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

static int gpio_input(u8 gpio_no, os_hal_gpio_data* pvalue)
{
	u8 ret;

	ret = mtk_os_hal_gpio_request(gpio_no);
	if (ret != 0)
	{
		printf("request gpio[%d] fail\n", gpio_no);
		return ret;
	}
	mtk_os_hal_gpio_set_direction(gpio_no, OS_HAL_GPIO_DIR_INPUT);
	vTaskDelay(pdMS_TO_TICKS(10));

	mtk_os_hal_gpio_get_input(gpio_no, pvalue);

	ret = mtk_os_hal_gpio_free(gpio_no);
	if (ret != 0)
	{
		printf("free gpio[%d] fail\n", gpio_no);
		return ret;
	}

	return 0;
}

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

void send_inter_core_msg(void)
{
	if (HLAppReady)
	{
		memcpy((void*)&buf[payloadStart], (void*)&ic_control_block, sizeof(ic_control_block));
		dataSize = payloadStart + sizeof(ic_control_block);

		EnqueueData(inbound, outbound, sharedBufSize, buf, dataSize);
	}
}

static void ButtonTask(void* pParameters)
{
	static os_hal_gpio_data oldStateButtonA = OS_HAL_GPIO_DATA_LOW;
	static os_hal_gpio_data oldStateButtonB = OS_HAL_GPIO_DATA_LOW;
	os_hal_gpio_data value = 0;

	while (1)
	{
		// Get Button_A status
		gpio_input(BUTTON_A, &value);
		if ((value != oldStateButtonA) && (value == OS_HAL_GPIO_DATA_LOW))
		{
			blinkIntervalIndex = (blinkIntervalIndex + 1) % numBlinkIntervals;

			ic_control_block.cmd = LP_IC_EVENT_BUTTON_A;
			send_inter_core_msg();

			ic_control_block.cmd = LP_IC_BLINK_RATE;
			ic_control_block.blinkRate = blinkIntervalIndex;
			send_inter_core_msg();
		}
		oldStateButtonA = value;

		// Get Button_B status
		gpio_input(BUTTON_B, &value);
		if ((value != oldStateButtonB) && (value == OS_HAL_GPIO_DATA_LOW))
		{
			ic_control_block.cmd = LP_IC_EVENT_BUTTON_B;
			send_inter_core_msg();
		}
		oldStateButtonB = value;

		// Delay for 100ms
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

static void SetLedBlinkRateTask(void* pParameters)
{
	// ensure LEDs are turned off
	gpio_output(LED_RED, true);
	gpio_output(LED_GREEN, true);
	gpio_output(LED_BLUE, true);

	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(blinkIntervalsMs[blinkIntervalIndex]));
		xSemaphoreGive(LEDSemphr);
	}
}

static void LedTask(void* pParameters)
{
	BaseType_t rt;
	static enum LEDS previous_led = RED;

	while (1)
	{
		rt = xSemaphoreTake(LEDSemphr, portMAX_DELAY);
		if (rt == pdPASS)
		{
			if (previous_led != current_led)
			{
				gpio_output(leds[(int)previous_led], true); // turn off old current colour
				previous_led = current_led;
			}

			led_state[(int)current_led] = !led_state[(int)current_led];
			gpio_output(leds[(int)current_led], led_state[(int)current_led]);
		}
	}
}

void SetTemperatureStatus(int temperature)
{
	if (temperature == desired_temperature)
	{
		current_led = GREEN;
	}

	if (temperature < desired_temperature)
	{
		current_led = BLUE;
	}

	if (temperature > desired_temperature)
	{
		current_led = RED;
	}
}

static void RTCoreMsgTask(void* pParameters)
{
	int rand_number;

#ifdef OEM_AVNET
	mtk_os_hal_i2c_ctrl_init(i2c_port_num);		// Initialize MT3620 I2C bus
	i2c_enum();									// Enumerate I2C Bus
	i2c_init();
	lsm6dso_init(i2c_write, i2c_read);
#endif // OEM_AVNET	

	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

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
			case LP_IC_SET_DESIRED_TEMPERATURE:
				desired_temperature = round(ic_control_block.temperature);
				SetTemperatureStatus(last_temperature);
				break;
			case LP_IC_BLINK_RATE:
				blinkIntervalIndex = ic_control_block.blinkRate % numBlinkIntervals;
				break;
			case LP_IC_TEMPERATURE_PRESSURE_HUMIDITY:

#ifdef OEM_AVNET

				ic_control_block.cmd = LP_IC_TEMPERATURE_PRESSURE_HUMIDITY;
				ic_control_block.temperature = get_temperature();

				rand_number = (rand() % 20) - 10;
				ic_control_block.humidity = (float)(50.0 + rand_number);

				rand_number = (rand() % 50) - 25;
				ic_control_block.pressure = (float)(1000.0 + rand_number);

#endif // OEM_AVNET

// The Seeed Studio Developer boards do not include any sensors so create some fake telemetry
#if defined(OEM_SEEED_STUDIO) || defined (OEM_SEEED_STUDIO_MINI)

				ic_control_block.cmd = LP_IC_TEMPERATURE_PRESSURE_HUMIDITY;

				rand_number = (rand() % 10) - 5;
				ic_control_block.temperature = (float)(25.0 + rand_number);

				rand_number = (rand() % 20) - 10;
				ic_control_block.humidity = (float)(50.0 + rand_number);

				rand_number = (rand() % 50) - 25;
				ic_control_block.pressure = (float)(1000.0 + rand_number);				

#endif // OEM_SEEED_STUDIO

				send_inter_core_msg();

				last_temperature = round(ic_control_block.temperature);
				SetTemperatureStatus(last_temperature);

				break;
			default:
				break;
			}
		}

		// Delay for 100ms
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

#ifdef OEM_SEEED_STUDIO_MINI
static void VirtualButtonTask(void* pParameters)
{
	static bool toggle = false;
	printf("Button Task Started\n");

	while (1)
	{
		if (toggle)
		{
			blinkIntervalIndex = (blinkIntervalIndex + 1) % numBlinkIntervals;
			ic_control_block.cmd = LP_IC_EVENT_BUTTON_A;
			send_inter_core_msg();
		}
		else
		{
			ic_control_block.cmd = LP_IC_EVENT_BUTTON_B;
			send_inter_core_msg();

			ic_control_block.cmd = LP_IC_BLINK_RATE;
			ic_control_block.blinkRate = blinkIntervalIndex;
			send_inter_core_msg();
		}

		toggle = !toggle;

		vTaskDelay(pdMS_TO_TICKS(10000));	// 10 seconds
	}
}
#endif


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

	LEDSemphr = xSemaphoreCreateBinary();

	xTaskCreate(SetLedBlinkRateTask, "Periodic Task", APP_STACK_SIZE_BYTES, NULL, 6, NULL);
	xTaskCreate(LedTask, "LED Task", APP_STACK_SIZE_BYTES, NULL, 5, NULL);
	xTaskCreate(ButtonTask, "GPIO Task", APP_STACK_SIZE_BYTES, NULL, 4, NULL);
#ifdef OEM_SEEED_STUDIO_MINI
	xTaskCreate(VirtualButtonTask, "GPIO Task", APP_STACK_SIZE_BYTES, NULL, 4, NULL);
#endif
	xTaskCreate(RTCoreMsgTask, "RTCore Msg Task", APP_STACK_SIZE_BYTES, NULL, 2, NULL);
	vTaskStartScheduler();

	for (;;)
	{
		__asm__("wfi");
	}
}

