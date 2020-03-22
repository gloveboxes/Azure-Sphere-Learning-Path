#include "../shared/azure_iot.h"
#include "../shared/globals.h"
#include "../oem/board.h"
#include "../shared/peripheral.h"
#include "../shared/terminate.h"
#include "../shared/timer.h"
#include "applibs_versions.h"
#include "exit_codes.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/powermanagement.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


// Forward signatures
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);
static void Led1BlinkHandler(EventLoopTimer* eventLoopTimer);
static void Led2OffHandler(EventLoopTimer* eventLoopTimer);
static void ButtonPressCheckHandler(EventLoopTimer* eventLoopTimer);
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer);

static const struct timespec led2BlinkPeriod = { 0, 300 * 1000 * 1000 };

static int led1BlinkIntervalIndex = 0;
static const struct timespec led1BlinkIntervals[] = { {0, 125000000}, {0, 250000000}, {0, 500000000}, {0, 750000000}, {1, 0} };
static const int led1BlinkIntervalsCount = NELEMS(led1BlinkIntervals);

// GPIO Peripherals
static Peripheral buttonA = { .fd = -1, .pin = BUTTON_A, .direction = INPUT, .initialise = OpenPeripheral, .name = "buttonA" };
static Peripheral buttonB = { .fd = -1, .pin = BUTTON_B, .direction = INPUT, .initialise = OpenPeripheral, .name = "buttonB" };
static Peripheral led1 = {
	.fd = -1, .pin = LED1, .direction = OUTPUT, .initialState = GPIO_Value_High, .invertPin = true,
	.initialise = OpenPeripheral, .name = "led1"
};
static Peripheral led2 = {
	.fd = -1, .pin = LED2, .direction = OUTPUT, .initialState = GPIO_Value_High, .invertPin = true,
	.initialise = OpenPeripheral, .name = "led2"
};
static Peripheral networkConnectedLed = {
	.fd = -1, .pin = NETWORK_CONNECTED_LED, .direction = OUTPUT, .initialState = GPIO_Value_High, .invertPin = true,
	.initialise = OpenPeripheral, .name = "networkConnectedLed"
};

// Timers
static Timer led1BlinkTimer = {
	.period = { 0, 125000000 },
	.name = "led1BlinkTimer", .timerEventHandler = Led1BlinkHandler
};
static Timer led2BlinkOffOneShotTimer = {
	.period = { 0, 0 },
	.name = "led2BlinkOffOneShotTimer", .timerEventHandler = Led2OffHandler
};
static Timer buttonPressCheckTimer = {
	.period = { 0, 1000000 },
	.name = "buttonPressCheckTimer", .timerEventHandler = ButtonPressCheckHandler
};
static Timer networkConnectionStatusTimer = {
	.period = { 5, 0 },
	.name = "networkConnectionStatusTimer", .timerEventHandler = NetworkConnectionStatusHandler
};

// Initialize peripheral, timer, device twin, and direct method sets
Peripheral* peripherals[] = { &buttonA, &buttonB, &led1, &led2, &networkConnectedLed };
Timer* timers[] = { &led1BlinkTimer, &led2BlinkOffOneShotTimer, &buttonPressCheckTimer, &networkConnectionStatusTimer };


int main(int argc, char* argv[]) {
	RegisterTerminationHandler();

	if (InitPeripheralsAndHandlers() != 0) {
		return ExitCode_Init_Failed;
	}

	// Main loop
	while (!IsTerminationRequired()) {
		int result = EventLoop_Run(GetTimerEventLoop(), -1, true);
		// Continue if interrupted by signal, e.g. due to breakpoint being set.
		if (result == -1 && errno != EINTR) {
			Terminate();
		}
	}

	ClosePeripheralsAndHandlers();

	Log_Debug("Application exiting.\n");
	return GetTerminationExitCode();
}

/// <summary>
/// Check status of connection to Azure IoT
/// </summary>
static void NetworkConnectionStatusHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (IsNetworkReady()) {
		Gpio_On(&networkConnectedLed);
	}
	else {
		Gpio_Off(&networkConnectedLed);
	}
}

/// <summary>
/// Turn on LED2 and set a one shot timer to turn LED2 off
/// </summary>
static void Led2On(void) {
	Gpio_On(&led2);
	SetOneShotTimer(&led2BlinkOffOneShotTimer, &led2BlinkPeriod);
}

/// <summary>
/// One shot timer to turn LED2 off
/// </summary>
static void Led2OffHandler(EventLoopTimer* eventLoopTimer) {
	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}
	Gpio_Off(&led2);
}

/// <summary>
/// Read Button Peripheral returns pressed state
/// </summary>
static bool IsButtonPressed(Peripheral button, GPIO_Value_Type* oldState) {
	bool isButtonPressed = false;
	GPIO_Value_Type newState;

	if (GPIO_GetValue(button.fd, &newState) != 0) {
		Terminate();
	}
	else {
		// Button is pressed if it is low and different than last known state.
		isButtonPressed = (newState != *oldState) && (newState == GPIO_Value_Low);
		*oldState = newState;
	}
	return isButtonPressed;
}

/// <summary>
/// Handler to check for Button Presses
/// </summary>
static void ButtonPressCheckHandler(EventLoopTimer* eventLoopTimer) {
	static GPIO_Value_Type buttonAState;
	static GPIO_Value_Type buttonBState;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	if (IsButtonPressed(buttonA, &buttonAState)) {

		led1BlinkIntervalIndex = (led1BlinkIntervalIndex + 1) % led1BlinkIntervalsCount;
		ChangeTimer(&led1BlinkTimer, &led1BlinkIntervals[led1BlinkIntervalIndex]);

		Led2On();
	}

	if (IsButtonPressed(buttonB, &buttonBState)) {
		Led2On();
	}
}

/// <summary>
/// Blink Led1 Handler
/// </summary>
static void Led1BlinkHandler(EventLoopTimer* eventLoopTimer) {
	static bool blinkingLedState = false;

	if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
		Terminate();
		return;
	}

	blinkingLedState = !blinkingLedState;

	if (blinkingLedState) { Gpio_Off(&led1); }
	else { Gpio_On(&led1); }
}

/// <summary>
///  Initialize peripherals, device twins, direct methods, timers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static int InitPeripheralsAndHandlers(void) {
	InitializeDevKit();

	OpenPeripheralSet(peripherals, NELEMS(peripherals));
	StartTimerSet(timers, NELEMS(timers));

	return 0;
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void) {
	Log_Debug("Closing file descriptors\n");

	StopTimerSet();
	ClosePeripheralSet();
	CloseDevKit();

	StopTimerEventLoop();
}