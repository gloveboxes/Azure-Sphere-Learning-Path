#include "peripheral_gpio.h"

bool lp_gpioOpen(LP_GPIO* peripheral)
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

void lp_gpioSetOpen(LP_GPIO** gpioSet, size_t gpioSetCount)
{
	for (int i = 0; i < gpioSetCount; i++)
	{
		if (!lp_gpioOpen(gpioSet[i]))
		{
			lp_terminate(ExitCode_Open_Peripheral);
			break;
		}
	}
}

/// <summary>
///     Closes a file descriptor and prints an error on failure.
/// </summary>
/// <param name="fd">File descriptor to close</param>
/// <param name="fdName">File descriptor name to use in error message</param>
void lp_gpioClose(LP_GPIO* peripheral)
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

void lp_gpioSetClose(LP_GPIO** gpioSet, size_t gpioSetCount)
{
	for (int i = 0; i < gpioSetCount; i++)
	{
		lp_gpioClose(gpioSet[i]);
	}
}

void lp_gpioOn(LP_GPIO* peripheral)
{
	if (peripheral == NULL || peripheral->fd < 0 || peripheral->pin < 0 || !peripheral->opened) { return; }

	GPIO_SetValue(peripheral->fd, peripheral->invertPin ? GPIO_Value_Low : GPIO_Value_High);
}

void lp_gpioOff(LP_GPIO* peripheral)
{
	if (peripheral == NULL || peripheral->fd < 0 || peripheral->pin < 0 || !peripheral->opened) { return; }

	GPIO_SetValue(peripheral->fd, peripheral->invertPin ? GPIO_Value_High : GPIO_Value_Low);
}

void lp_gpioStateSet(LP_GPIO* peripheral, bool state) {
	if (state) {
		lp_gpioOn(peripheral);
	}
	else {
		lp_gpioOff(peripheral);
	}
}

/// <summary>
/// Read Button LP_GPIO returns state
/// </summary>
bool lp_gpioStateGet(LP_GPIO* peripheral, GPIO_Value_Type* oldState)
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
