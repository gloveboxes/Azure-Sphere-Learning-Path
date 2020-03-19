#include "peripheral.h"

Peripheral** _peripherals = NULL;
size_t _peripheralCount = 0;

bool OpenPeripheral(Peripheral* peripheral) {
	if (peripheral == NULL || peripheral->pin < 0) { return false; }

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

	return true;
}

void OpenPeripheralSet(Peripheral** peripherals, size_t peripheralCount) {
	_peripherals = peripherals;
	_peripheralCount = peripheralCount;

	for (int i = 0; i < _peripheralCount; i++) {
		_peripherals[i]->fd = -1;
		if (_peripherals[i]->initialise != NULL) {
			if (!_peripherals[i]->initialise(_peripherals[i])) {
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
void CloseFdAndPrintError(int fd, const char* fdName) {
	if (fd >= 0) {
		int result = close(fd);
		if (result != 0) {
			Log_Debug("ERROR: Could not close peripheral %s: %s (%d).\n", fdName == NULL ? "No name" : fdName, strerror(errno), errno);
		}
	}
}

void ClosePeripheralSet(void) {
	for (int i = 0; i < _peripheralCount; i++) {
		CloseFdAndPrintError(_peripherals[i]->fd, _peripherals[i]->name);
	}
}

void Gpio_On(Peripheral* peripheral) {
	if (peripheral == NULL || peripheral->fd < 0 || peripheral->pin < 0) { return; }

	GPIO_SetValue(peripheral->fd, peripheral->invertPin ? GPIO_Value_Low : GPIO_Value_High);
}

void Gpio_Off(Peripheral* peripheral) {
	if (peripheral == NULL || peripheral->fd < 0 || peripheral->pin < 0) { return; }

	GPIO_SetValue(peripheral->fd, peripheral->invertPin ? GPIO_Value_High : GPIO_Value_Low);
}

