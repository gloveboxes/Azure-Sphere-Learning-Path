#include "peripheral.h"


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

void OpenDeviceTwin(DeviceTwinPeripheral* deviceTwinPeripheral) {

	// types JSON and String allocated dynamically when called in azure_iot.c
	switch (deviceTwinPeripheral->twinType)
	{
	case TYPE_INT:
		deviceTwinPeripheral->twinState = malloc(sizeof(int));
		break;
	case TYPE_FLOAT:
		deviceTwinPeripheral->twinState = malloc(sizeof(float));
		break;
	case TYPE_BOOL:
		deviceTwinPeripheral->twinState = malloc(sizeof(bool));
		break;
	default:
		break;
	}

	if (deviceTwinPeripheral->peripheral.initialise != NULL) {
		deviceTwinPeripheral->peripheral.initialise(&deviceTwinPeripheral->peripheral);
	}
}

void CloseDeviceTwin(DeviceTwinPeripheral* deviceTwinPeripheral) {
	if (deviceTwinPeripheral->twinState != NULL) {
		free(deviceTwinPeripheral->twinState);
		deviceTwinPeripheral->twinState = NULL;
	}

	CloseFdAndPrintError(deviceTwinPeripheral->peripheral.fd, deviceTwinPeripheral->peripheral.name);
}

