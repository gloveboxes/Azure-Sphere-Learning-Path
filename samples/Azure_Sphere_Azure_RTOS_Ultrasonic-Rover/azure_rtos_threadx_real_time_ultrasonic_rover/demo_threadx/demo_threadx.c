/* This is a small demo of the high-performance ThreadX kernel.  It includes examples of eight
   threads of different priorities, using a message queue, semaphore, mutex, event flags group,
   byte pool, and block pool.  */

#include "../inter_core_definition/inc/inter_core_definition.h"
#include "hw/azure_sphere_learning_path.h"
#include "mt3620-intercore.h" // Support for inter Core Communications
#include "os_hal_gpio.h"
#include "printf.h"
#include "tx_api.h"
#include "Ultrasonic/ultransonic.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

/* Convert from millisecond to ThreadX Tick value. */
#define MS_TO_TICK(ms)                                     ((ms) * (TX_TIMER_TICKS_PER_SECOND) / 1000)

bool HLAppReady = false;
float distance_left, distance_right, newDistanceLeft, newDistanceRight;
bool newDataReady = false;

enum LEDS { RED, GREEN, BLUE };

typedef struct ULTRASONIC_SENSOR
{
	float centimeters;
	int lastKnownGoodCentimeters;
	enum LEDS previous_led;
	int LEDs[];
} ULTRASONIC_SENSOR;

ULTRASONIC_SENSOR leftSensor = { .LEDs = { LED_RED_LEFT, LED_GREEN_LEFT, LED_BLUE_LEFT }, .lastKnownGoodCentimeters = -1 };
ULTRASONIC_SENSOR rightSensor = { .LEDs = { LED_RED_RIGHT, LED_GREEN_RIGHT, LED_BLUE_RIGHT }, .lastKnownGoodCentimeters = -1 };

// Inter-core Communications
LP_INTER_CORE_BLOCK ic_control_block;
const size_t payloadStart = 20;
uint8_t buf[256];
uint32_t dataSize;
BufferHeader* outbound, * inbound;
uint32_t sharedBufSize = 0;


#define DEMO_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_BLOCK_POOL_SIZE    100
#define DEMO_QUEUE_SIZE         100


/* Define the ThreadX object control blocks...  */

TX_THREAD               thread_measure_distance;
TX_THREAD				thread_intercore_messaging;
TX_THREAD				thread_update_azure_iot;
TX_BYTE_POOL            byte_pool_0;

UCHAR                   memory_area[DEMO_BYTE_POOL_SIZE];


/* Define thread prototypes.  */
void thread_measure_distance_entry(ULONG thread_input);
void thread_intercore_messaging_entry(ULONG thread_input);
void thread_update_azure_iot_entry(ULONG thread_input);


int main(void)
{

	/* Enter the ThreadX kernel.  */
	tx_kernel_enter();
}


/* Define what the initial system looks like.  */

void tx_application_define(void* first_unused_memory)
{
	CHAR* pointer;

	/* Create a byte memory pool from which to allocate the thread stacks.  */
	tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEMO_BYTE_POOL_SIZE);

	/* Put system definition stuff in here, e.g. thread creates and other assorted
	   create information.  */

	tx_byte_allocate(&byte_pool_0, (VOID**)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
	tx_thread_create(&thread_measure_distance, "ultrasonic measure", thread_measure_distance_entry, 0,
		pointer, DEMO_STACK_SIZE, 2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);


	tx_byte_allocate(&byte_pool_0, (VOID**)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
	tx_thread_create(&thread_intercore_messaging, "intercore messaging", thread_intercore_messaging_entry, 0,
		pointer, DEMO_STACK_SIZE, 2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);

	tx_byte_allocate(&byte_pool_0, (VOID**)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
	tx_thread_create(&thread_update_azure_iot, "update azure", thread_update_azure_iot_entry, 0,
		pointer, DEMO_STACK_SIZE, 2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);	
}


int gpio_output(u8 gpio_no, u8 level)
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


// https://embeddedartistry.com/blog/2017/02/17/implementing-malloc-with-threadx/
// overrides for malloc and free required for srand and rand
void *malloc(size_t size)
{
	void* ptr = NULL;

	if (size > 0)
	{
		// We simply wrap the threadX call into a standard form
		uint8_t r = tx_byte_allocate(&byte_pool_0, &ptr, size,
			TX_WAIT_FOREVER);

		if (r != TX_SUCCESS)
		{
			ptr = NULL;
		}
	}
	//else NULL if there was no size

	return ptr;
}


void free(void* ptr)
{
	if (ptr)
	{
		//We simply wrap the threadX call into a standard form
		//uint8_t r = tx_byte_release(ptr);
		tx_byte_release(ptr);
	}
}


void set_distance_indicator(ULTRASONIC_SENSOR* sensor)
{
	enum LEDS current_led = RED;

	if (!isnan(sensor->centimeters))
	{
		current_led = sensor->centimeters > 20.0 ? GREEN : sensor->centimeters > 10.0 ? BLUE : RED;

		if (sensor->previous_led != current_led)	// turn off previous LED if different from new colour
		{
			gpio_output(sensor->LEDs[(int)sensor->previous_led], true); // turn off old current colour

			sensor->previous_led = current_led;
		}

		// turn on LED colour
		gpio_output(sensor->LEDs[(int)current_led], false);

		sensor->lastKnownGoodCentimeters = (int)sensor->centimeters;

		newDataReady = true;
	}
	else	// else turn off LED set
	{
		for (size_t i = 0; i < 3; i++)
		{
			gpio_output(sensor->LEDs[i], true);
		}
	}
}


void thread_measure_distance_entry(ULONG thread_input)
{
	while (true)
	{
		leftSensor.centimeters = lp_get_distance(HCSR04_LEFT, 3000);
		set_distance_indicator(&leftSensor);

		rightSensor.centimeters = lp_get_distance(HCSR04_RIGHT, 3000);
		set_distance_indicator(&rightSensor);

		tx_thread_sleep(2); // 20 milliseconds
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


void thread_update_azure_iot_entry(ULONG thread_input)
{
	while (true)
	{
		tx_thread_sleep(MS_TO_TICK(1000));

		if (newDataReady)
		{
			// check for sensible data
			if (leftSensor.lastKnownGoodCentimeters > 0 && leftSensor.lastKnownGoodCentimeters < 20 &&
				rightSensor.lastKnownGoodCentimeters > 0 && rightSensor.lastKnownGoodCentimeters < 20)
			{
				ic_control_block.cmd = LP_IC_REPORT_DISTANCE;
				ic_control_block.distance_left = leftSensor.lastKnownGoodCentimeters;
				ic_control_block.distance_right = rightSensor.lastKnownGoodCentimeters;

				send_inter_core_msg();
			}

			newDataReady = false;
		}
	}
}


void thread_intercore_messaging_entry(ULONG thread_input)
{
	// Initialize Inter-Core Communications
	if (GetIntercoreBuffers(&outbound, &inbound, &sharedBufSize) == -1)
	{
		for (;;)
		{
			// empty.
		}
	}

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

		tx_thread_sleep(MS_TO_TICK(1000));
	}
}
