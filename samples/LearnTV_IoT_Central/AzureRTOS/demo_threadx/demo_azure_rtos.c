#include "hw/azure_sphere_learning_path.h"
#include "i2c.h"
#include "lsm6dso_driver.h"
#include "lsm6dso_reg.h"
#include "mt3620-intercore.h"
#include "os_hal_gpio.h"
#include "os_hal_uart.h"
#include "printf.h"
#include "tx_api.h"
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "intercore_contract.h"

#define DEMO_STACK_SIZE 1024
#define DEMO_BYTE_POOL_SIZE 9120
#define DEMO_BLOCK_POOL_SIZE 100
#define DEMO_QUEUE_SIZE 100

// resources for inter core messaging
static uint8_t buf[256];
static uint32_t dataSize;
static BufferHeader *outbound, *inbound;
static uint32_t sharedBufSize = 0;
static const size_t payloadStart = 20;

LP_INTER_CORE_BLOCK ic_control_block;

enum LEDS
{
	RED,
	GREEN,
	BLUE
};

static enum LEDS current_led = RED;
static int leds[] = {LED_RED, LED_GREEN, LED_BLUE};
static bool led_state[] = {false, false, false};

static const int blinkIntervalsMs[] = {125, 250, 500, 750, 1000, 2000};
static int blinkIntervalIndex = 3;
static const int numBlinkIntervals = sizeof(blinkIntervalsMs) / sizeof(blinkIntervalsMs[0]);

bool highLevelReady = false;
int desired_temperature = 0.0;
int last_temperature = 0;

// Define the ThreadX object control blocks...
TX_THREAD tx_thread_inter_core;
TX_THREAD tx_thread_read_button;
TX_THREAD tx_thread_read_sensor;
TX_THREAD tx_thread_blink_led;
TX_EVENT_FLAGS_GROUP event_flags_0;
TX_BYTE_POOL byte_pool_0;
TX_BLOCK_POOL block_pool_0;
UCHAR memory_area[DEMO_BYTE_POOL_SIZE];

// Define thread prototypes.
void thread_inter_core(ULONG thread_input);
void thread_read_sensor(ULONG thread_input);
void thread_blink_led(ULONG thread_blink);
void thread_button(ULONG thread_blink);
int gpio_output(u8 gpio_no, u8 level);
int gpio_input(u8 gpio_no, os_hal_gpio_data *pvalue);

int main()
{
	tx_kernel_enter(); // Enter the Azure RTOS kernel.
}

// Define what the initial system looks like.
void tx_application_define(void *first_unused_memory)
{
	CHAR *pointer;

	tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEMO_BYTE_POOL_SIZE); // Create a byte memory pool from which to allocate the thread stacks

	tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);	   // Allocate the stack for inter core thread
	tx_thread_create(&tx_thread_inter_core, "thread inter core", thread_inter_core, 0, // Create inter core message thread
					 pointer, DEMO_STACK_SIZE, 4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

	tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT); // Allocate the stack for thread_blink_led thread
	tx_thread_create(&tx_thread_blink_led, "thread blink led", thread_blink_led, 0, // Create button press thread */
					 pointer, DEMO_STACK_SIZE, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

	tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);		  // Allocate the stack for read sensor thread
	tx_thread_create(&tx_thread_read_sensor, "thread read sensor", thread_read_sensor, 0, // Create read sensor thread */
					 pointer, DEMO_STACK_SIZE, 4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

	tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);	 // Allocate the stack for read sensor thread
	tx_thread_create(&tx_thread_read_button, "thread read sensor", thread_button, 0, // Create read sensor thread */
					 pointer, DEMO_STACK_SIZE, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

	tx_event_flags_create(&event_flags_0, "event flags 0"); // Create event flag for thread sync
}

// https://embeddedartistry.com/blog/2017/02/17/implementing-malloc-with-threadx/
// overrides for malloc and free required for srand and rand
void *malloc(size_t size)
{
	void *ptr = NULL;

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

void free(void *ptr)
{
	if (ptr)
	{
		//We simply wrap the threadX call into a standard form
		tx_byte_release(ptr);
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

void send_inter_core_msg(void)
{
	if (highLevelReady)
	{
		memcpy((void *)&buf[payloadStart], (void *)&ic_control_block, sizeof(ic_control_block));
		dataSize = payloadStart + sizeof(ic_control_block);

		EnqueueData(inbound, outbound, sharedBufSize, buf, dataSize);
	}
}

void thread_inter_core(ULONG thread_input)
{
	UINT status;

	if (GetIntercoreBuffers(&outbound, &inbound, &sharedBufSize) == -1)
	{ // Initialize Inter-Core Communications
		for (;;)
		{ // empty.
		}
	}

	// This thread monitors inter core messages.
	while (1)
	{
		dataSize = sizeof(buf);
		int r = DequeueData(outbound, inbound, sharedBufSize, buf, &dataSize);

		if (r == 0 && dataSize > payloadStart)
		{
			highLevelReady = true;

			memcpy((void *)&ic_control_block, (void *)&buf[payloadStart], sizeof(ic_control_block));

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
				// Set event flag 0 to wakeup threads read sensor and blink led
				status = tx_event_flags_set(&event_flags_0, 0x1, TX_OR);

				if (status != TX_SUCCESS)
					break;
				break;
			default:
				break;
			}
		}

		tx_thread_sleep(10);
	}
}

void thread_blink_led(ULONG thread_blink)
{
	static enum LEDS previous_led = RED;

	while (1)
	{
		if (previous_led != current_led)
		{
			gpio_output(leds[(int)previous_led], true); // turn off old current colour
			previous_led = current_led;
		}

		led_state[(int)current_led] = !led_state[(int)current_led];
		gpio_output(leds[(int)current_led], led_state[(int)current_led]);

		tx_thread_sleep(blinkIntervalsMs[blinkIntervalIndex] / 10);
	}
}

#if !defined(OEM_SEEED_STUDIO_MINI)
void thread_button(ULONG thread_blink)
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
		tx_thread_sleep(10);
	}
}
#endif

#if defined(OEM_SEEED_STUDIO_MINI)
void thread_button(ULONG thread_blink)
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

			ic_control_block.cmd = LP_IC_BLINK_RATE;
			ic_control_block.blinkRate = blinkIntervalIndex;
			send_inter_core_msg();
		}
		else
		{
			ic_control_block.cmd = LP_IC_EVENT_BUTTON_B;
			send_inter_core_msg();
		}

		toggle = !toggle;

		// Delay for 10 seconds. 100 ticks = 1 seconds
		tx_thread_sleep(1000);
	}
}
#endif

void thread_read_sensor(ULONG thread_input)
{
	UINT status;
	ULONG actual_flags;
	int rand_number;

#ifdef OEM_AVNET

	mtk_os_hal_i2c_ctrl_init(i2c_port_num); // Initialize MT3620 I2C bus
	i2c_enum();								// Enumerate I2C Bus

	if (i2c_init())
	{
		return;
	}

	// LSM6DSO Init - the accelerometer calibration has been commented out for faster start up
	if (lsm6dso_init(i2c_write, i2c_read))
	{
		return;
	}

#endif // OEM_AVNET

	srand((unsigned int)time(NULL)); // seed the random number generator for fake telemetry

	while (true)
	{
		// waits here until flag set in inter core thread
		status = tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER); // wait until event set in the thread_inter_core thread

		if ((status != TX_SUCCESS) || (actual_flags != 0x1))
			break;

		if (highLevelReady)
		{
#ifdef OEM_AVNET

			ic_control_block.cmd = LP_IC_TEMPERATURE_PRESSURE_HUMIDITY;
			ic_control_block.temperature = get_temperature();

			rand_number = (rand() % 20) - 10;
			ic_control_block.humidity = (float)(50.0 + rand_number);

			rand_number = (rand() % 50) - 25;
			ic_control_block.pressure = (float)(1000.0 + rand_number);

#endif // OEM_AVNET

			// The Seeed Studio Developer boards do not include any sensors so create some fake telemetry
#if !defined(OEM_AVNET)

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
		}
	}
}

int gpio_output(u8 gpio_no, u8 level)
{
	mtk_os_hal_gpio_set_direction(gpio_no, OS_HAL_GPIO_DIR_OUTPUT);
	mtk_os_hal_gpio_set_output(gpio_no, level);

	return 0;
}

int gpio_input(u8 gpio_no, os_hal_gpio_data *pvalue)
{
	mtk_os_hal_gpio_set_direction(gpio_no, OS_HAL_GPIO_DIR_INPUT);
	//vTaskDelay(pdMS_TO_TICKS(10));
	tx_thread_sleep(1);

	mtk_os_hal_gpio_get_input(gpio_no, pvalue);

	return 0;
}