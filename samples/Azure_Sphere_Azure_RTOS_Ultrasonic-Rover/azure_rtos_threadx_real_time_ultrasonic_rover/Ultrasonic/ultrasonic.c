#include "ultransonic.h"

void lp_write_reg32(uintptr_t baseAddr, size_t offset, uint32_t value)
{
	*(volatile uint32_t*)(baseAddr + offset) = value;
}

uint32_t lp_read_reg32(uintptr_t baseAddr, size_t offset)
{
	return *(volatile uint32_t*)(baseAddr + offset);
}

void lp_gpt3_wait_microseconds(int microseconds)
{
	// GPT3_INIT = initial counter value
	lp_write_reg32(GPT_BASE, 0x54, 0x0);

	// GPT3_CTRL
	uint32_t ctrlOn = 0x0;
	ctrlOn |= (0x19) << 16; // OSC_CNT_1US (default value)
	ctrlOn |= 0x1;          // GPT3_EN = 1 -> GPT3 enabled
	lp_write_reg32(GPT_BASE, 0x50, ctrlOn);

	// GPT3_CNT
	while (lp_read_reg32(GPT_BASE, 0x58) < microseconds)
	{
		// empty.
	}

	// GPT_CTRL -> disable timer
	lp_write_reg32(GPT_BASE, 0x50, 0x0);
}

bool lp_read_input(u8 pin)
{
	os_hal_gpio_data value = 0;
	mtk_os_hal_gpio_get_input(pin, &value);
	return value == OS_HAL_GPIO_DATA_HIGH;
}

float lp_get_distance(u8 pin, unsigned long timeoutMicroseconds)
{
	uint32_t pulseBegin, pulseEnd;

	mtk_os_hal_gpio_set_direction(pin, OS_HAL_GPIO_DIR_OUTPUT);	// set for output
	mtk_os_hal_gpio_set_output(pin, OS_HAL_GPIO_DATA_LOW);		// pull low
	lp_gpt3_wait_microseconds(2);

	mtk_os_hal_gpio_set_output(pin, OS_HAL_GPIO_DATA_HIGH);		// pull high
	lp_gpt3_wait_microseconds(5);

	// GPT3_CTRL - starts microsecond resolution clock
	uint32_t ctrlOn = 0x0;
	ctrlOn |= (0x19) << 16; // OSC_CNT_1US (default value)
	ctrlOn |= 0x1;          // GPT3_EN = 1 -> GPT3 enabled
	lp_write_reg32(GPT_BASE, 0x50, ctrlOn);

	mtk_os_hal_gpio_set_direction(pin, OS_HAL_GPIO_DIR_INPUT);	// set for input

	while (lp_read_input(pin))		// wait for any previous pulse to end
	{
		if (lp_read_reg32(GPT_BASE, 0x58) > timeoutMicroseconds)
		{
			lp_write_reg32(GPT_BASE, 0x50, 0x0);	// GPT_CTRL -> disable timer
			return NAN;
		}
	}

	while (!lp_read_input(pin))		// wait for the pulse to start
	{
		pulseBegin = lp_read_reg32(GPT_BASE, 0x58);
		if (lp_read_reg32(GPT_BASE, 0x58) > timeoutMicroseconds)
		{
			lp_write_reg32(GPT_BASE, 0x50, 0x0);	// GPT_CTRL -> disable timer
			return NAN;
		}
	}

	pulseBegin = lp_read_reg32(GPT_BASE, 0x58);

	while (lp_read_input(pin))		// wait for the pulse to stop
	{
		if (lp_read_reg32(GPT_BASE, 0x58) > timeoutMicroseconds)
		{
			lp_write_reg32(GPT_BASE, 0x50, 0x0);	// GPT_CTRL -> disable timer
			return NAN;
		}
	}

	pulseEnd = lp_read_reg32(GPT_BASE, 0x58);

	lp_write_reg32(GPT_BASE, 0x50, 0x0);	// GPT_CTRL -> disable timer

	return (pulseEnd - pulseBegin) / 58.0; //  (29 / 2);
}
