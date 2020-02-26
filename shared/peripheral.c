#include "peripheral.h"

Peripheral** _peripherals = NULL;
size_t _peripheralCount = 0;

int OpenPeripheral(Peripheral* peripheral) {
	if (peripheral == NULL || peripheral->pin < 0) { return 0; }

	peripheral->fd = GPIO_OpenAsOutput(peripheral->pin, GPIO_OutputMode_PushPull, peripheral->initialState);
	if (peripheral->fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	return 0;
}

void RegisterPeripheralSet(Peripheral** peripherals, size_t peripheralCount) {
	_peripherals = peripherals;
	_peripheralCount = peripheralCount;

	for (int i = 0; i < _peripheralCount; i++) {
		if (_peripherals[i]->initialise != NULL) {
			_peripherals[i]->initialise(_peripherals[i]);
		} 
	}
}

/// <summary>
///     Closes a file descriptor and prints an error on failure.
/// </summary>
/// <param name="fd">File descriptor to close</param>
/// <param name="fdName">File descriptor name to use in error message</param>
void CloseFdAndPrintError(int fd, const char* fdName)
{
	if (fd >= 0) {
		int result = close(fd);
		if (result != 0) {
			Log_Debug("ERROR: Could not close fd %s: %s (%d).\n", fdName, strerror(errno), errno);
		}
	}
}

void ClosePeripheralSet(void) {
	for (int i = 0; i < _peripheralCount; i++) {
		CloseFdAndPrintError(_peripherals[i]->fd, _peripherals[i]->name);
	}
}



