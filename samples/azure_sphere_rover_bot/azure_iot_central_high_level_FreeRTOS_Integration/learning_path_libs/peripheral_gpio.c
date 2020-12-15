#include "peripheral_gpio.h"

LP_PERIPHERAL_GPIO** _peripheralSet = NULL;
size_t _peripheralSetCount = 0;

bool lp_openPeripheralGpio(LP_PERIPHERAL_GPIO* peripheral)
{
	if (peripheral == NULL || peripheral->pin < 0) { return false; }

	if (peripheral->opened) { return true; }

	if (peripheral->invertPin)
	{
		if (peripheral->initialState == GPIO_Value_High)
		{
			peripheral->initialState = GPIO_Value_Low;
		}
		else
		{
			peripheral->initialState = GPIO_Value_High;
		}
	}

	switch (peripheral->direction)
	{
	case LP_OUTPUT:
		peripheral->fd = GPIO_OpenAsOutput(peripheral->pin, GPIO_OutputMode_PushPull, peripheral->initialState);
		if (peripheral->fd < 0)
		{
			Log_Debug(
				"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
				strerror(errno), errno);
			return false;
		}
		break;
	case LP_INPUT:
		peripheral->fd = GPIO_OpenAsInput(peripheral->pin);
		if (peripheral->fd < 0)
		{
			Log_Debug(
				"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
				strerror(errno), errno);
			return false;
		}
		break;
	case LP_DIRECTION_UNKNOWN:
		Log_Debug("Unknown direction for peripheral %s", peripheral->name);
		return false;
		break;
	}

	peripheral->opened = true;
	return true;
}

void lp_openPeripheralGpioSet(LP_PERIPHERAL_GPIO** peripheralSet, size_t peripheralSetCount)
{
	_peripheralSet = peripheralSet;
	_peripheralSetCount = peripheralSetCount;

	for (int i = 0; i < _peripheralSetCount; i++)
	{
		_peripheralSet[i]->fd = -1;
		if (_peripheralSet[i]->initialise != NULL)
		{
			if (!_peripheralSet[i]->initialise(_peripheralSet[i]))
			{
				lp_terminate(ExitCode_Open_Peripheral);
				break;
			}
		}
	}
}

/// <summary>
///     Closes a file descriptor and prints an error on failure.
/// </summary>
/// <param name="fd">File descriptor to close</param>
/// <param name="fdName">File descriptor name to use in error message</param>
void lp_closePeripheralGpio(LP_PERIPHERAL_GPIO* peripheral)
{
	if (peripheral->opened && peripheral->fd >= 0)
	{
		int result = close(peripheral->fd);
		if (result != 0)
		{
			Log_Debug("ERROR: Could not close peripheral %s: %s (%d).\n", peripheral->name == NULL ? "No name" : peripheral->name, strerror(errno), errno);
		}
	}
	peripheral->fd = -1;
	peripheral->opened = false;
}

void lp_closePeripheralGpioSet(void)
{
	for (int i = 0; i < _peripheralSetCount; i++)
	{
		lp_closePeripheralGpio(_peripheralSet[i]);
	}
}

void lp_gpioOn(LP_PERIPHERAL_GPIO* peripheral)
{
	if (peripheral == NULL || peripheral->fd < 0 || peripheral->pin < 0 || !peripheral->opened) { return; }

	GPIO_SetValue(peripheral->fd, peripheral->invertPin ? GPIO_Value_Low : GPIO_Value_High);
}

void lp_gpioOff(LP_PERIPHERAL_GPIO* peripheral)
{
	if (peripheral == NULL || peripheral->fd < 0 || peripheral->pin < 0 || !peripheral->opened) { return; }

	GPIO_SetValue(peripheral->fd, peripheral->invertPin ? GPIO_Value_High : GPIO_Value_Low);
}

/// <summary>
/// Read Button LP_PERIPHERAL_GPIO returns state
/// </summary>
bool lp_gpioGetState(LP_PERIPHERAL_GPIO* peripheral, GPIO_Value_Type* oldState)
{
	bool isGpioOn = false;
	GPIO_Value_Type newState;

	if (peripheral->direction != LP_INPUT) { return false; }

	if (GPIO_GetValue(peripheral->fd, &newState) != 0)
	{
		lp_terminate(ExitCode_Gpio_Read);
	}
	else
	{
		// Button is pressed if it is low and different than last known state.
		isGpioOn = (newState != *oldState) && (newState == GPIO_Value_Low);
		*oldState = newState;
	}
	return isGpioOn;
}
