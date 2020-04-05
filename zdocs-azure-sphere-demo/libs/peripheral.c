#include "peripheral.h"

Peripheral** _peripheralSet = NULL;
size_t _peripheralSetCount = 0;

bool OpenPeripheral(Peripheral* peripheral) {
	if (peripheral == NULL || peripheral->pin < 0 || peripheral->opened) { return false; }

	if (peripheral->invertPin) {
		if (peripheral->initialState == GPIO_Value_High) {
			peripheral->initialState = GPIO_Value_Low;
		}
		else {
			peripheral->initialState = GPIO_Value_High;
		}
	}

	switch (peripheral->direction) {
	case OUTPUT:
		peripheral->fd = GPIO_OpenAsOutput(peripheral->pin, GPIO_OutputMode_PushPull, peripheral->initialState);
		if (peripheral->fd < 0) {
			Log_Debug(
				"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
				strerror(errno), errno);
			return false;
		}
		break;
	case INPUT:
		peripheral->fd = GPIO_OpenAsInput(peripheral->pin);
		if (peripheral->fd < 0) {
			Log_Debug(
				"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
				strerror(errno), errno);
			return false;
		}
		break;
	case DIRECTION_UNKNOWN:
		Log_Debug("Unknown direction for peripheral %s", peripheral->name);
		return false;
		break;
	}

	peripheral->opened = true;
	return true;
}

void OpenPeripheralSet(Peripheral** peripheralSet, size_t peripheralSetCount) {
	_peripheralSet = peripheralSet;
	_peripheralSetCount = peripheralSetCount;

	for (int i = 0; i < _peripheralSetCount; i++) {
		_peripheralSet[i]->fd = -1;
		if (_peripheralSet[i]->initialise != NULL) {
			if (!_peripheralSet[i]->initialise(_peripheralSet[i])) {
				Terminate();
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
void ClosePeripheral(Peripheral* peripheral) {
	if (!peripheral->opened || peripheral->fd >= 0) {
		int result = close(peripheral->fd);
		if (result != 0) {
			Log_Debug("ERROR: Could not close peripheral %s: %s (%d).\n", peripheral->name == NULL ? "No name" : peripheral->name, strerror(errno), errno);
		}
	}
	peripheral->opened = false;
}

void ClosePeripheralSet(void) {
	for (int i = 0; i < _peripheralSetCount; i++) {
		ClosePeripheral(_peripheralSet[i]);
	}
}

void Gpio_On(Peripheral* peripheral) {
	if (peripheral == NULL || peripheral->fd < 0 || peripheral->pin < 0 || !peripheral->opened) { return; }

	GPIO_SetValue(peripheral->fd, peripheral->invertPin ? GPIO_Value_Low : GPIO_Value_High);
}

void Gpio_Off(Peripheral* peripheral) {
	if (peripheral == NULL || peripheral->fd < 0 || peripheral->pin < 0 || !peripheral->opened) { return; }

	GPIO_SetValue(peripheral->fd, peripheral->invertPin ? GPIO_Value_High : GPIO_Value_Low);
}

