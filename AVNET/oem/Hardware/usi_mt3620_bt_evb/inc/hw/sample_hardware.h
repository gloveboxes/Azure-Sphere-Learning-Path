/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

// This file defines the mapping from the USI MT3620 Bluetooth (BT) Evaluation Board (EVB) to the
// 'sample hardware' abstraction used by the samples at https://github.com/Azure/azure-sphere-samples.
// Some peripherals are on-board on the USI MT3620 BT EVB, while other peripherals must be attached externally if needed.
// See https://aka.ms/AzureSphereHardwareDefinitions for more information on how to use hardware abstractions,
// to enable apps to work across multiple hardware variants.

// This file is autogenerated from ../../sample_hardware.json.  Do not edit it directly.

#pragma once
#include "usi_mt3620_bt_evb.h"

// MT3620 USI BT EVB: Connect external button using J32, pin 13.
#define SAMPLE_BUTTON_1 USI_MT3620_BT_EVB_J32_PIN13_GPIO12

// MT3620 USI BT EVB: Connect external button using J32, pin 14.
#define SAMPLE_BUTTON_2 USI_MT3620_BT_EVB_J32_PIN14_GPIO13

// MT3620 USI BT EVB: PWM LED controller
#define SAMPLE_LED_PWM_CONTROLLER USI_MT3620_BT_EVB_PWM_CONTROLLER2

// MT3620 USI BT EVB: Channel 2 for the PWM LED1 green. In the app manifest, it is only necessary to request the capability for the PWM Controller, SAMPLE_LED_PWM_CONTROLLER.
#define SAMPLE_LED_PWM_CHANNEL MT3620_PWM_CHANNEL2

// MT3620 USI BT EVB: Connect external LED using J32, pin 9.
#define SAMPLE_LED USI_MT3620_BT_EVB_J32_PIN9_GPIO8

// MT3620 USI BT EVB: ADC Potentiometer controller
#define SAMPLE_POTENTIOMETER_ADC_CONTROLLER USI_MT3620_BT_EVB_ADC_CONTROLLER0

// MT3620 USI BT EVB: Connect external potentiometer to ADC controller 0, channel 0 using J34 pin16. In the app manifest, it is only necessary to request the capability for the ADC Group Controller, SAMPLE_POTENTIOMETER_ADC_CONTROLLER.
#define SAMPLE_POTENTIOMETER_ADC_CHANNEL MT3620_ADC_CHANNEL0

// MT3620 USI BT EVB: Connect external red LED using J32, pin 10.
#define SAMPLE_RGBLED_RED USI_MT3620_BT_EVB_J32_PIN10_GPIO9

// MT3620 USI BT EVB: Connect external green LED using J32, pin 11.
#define SAMPLE_RGBLED_GREEN USI_MT3620_BT_EVB_J32_PIN11_GPIO10

// MT3620 USI BT EVB: Connect external blue LED using J32, pin 12.
#define SAMPLE_RGBLED_BLUE USI_MT3620_BT_EVB_J32_PIN12_GPIO11

// MT3620 USI BT EVB: Connect J32 pin 2 (RX) to J32 pin 3 (TX).
#define SAMPLE_UART_LOOPBACK USI_MT3620_BT_EVB_ISU0_UART

// MT3620 USI BT EVB: Connect external LSM6DS3 to I2C using J33, pin 15 (SDA) and pin 10 (SCL).
#define SAMPLE_LSM6DS3_I2C USI_MT3620_BT_EVB_ISU1_I2C

// MT3620 USI BT EVB: Connect external LSM6DS3 to SPI using J33, pin 7 (MISO), pin 5 (SCLK), pin 8 (CSA), pin 6 (MOSI).
#define SAMPLE_LSM6DS3_SPI USI_MT3620_BT_EVB_ISU2_SPI

// MT3620 SPI Chip Select (CS) value "A". This is not a peripheral identifier, and so has no meaning in an app manifest.
#define SAMPLE_LSM6DS3_SPI_CS MT3620_SPI_CS_A

// MT3620 USI BT EVB: BT_nRST (RESET) signal on nRF52810.
#define SAMPLE_NRF52_RESET USI_MT3620_BT_COMBO_NRF52_RESET

// MT3620 USI BT EVB: BT_FW_EN (DFU) signal on nRF52810.
#define SAMPLE_NRF52_DFU USI_MT3620_BT_COMBO_NRF52_DFU

// MT3620 USI BT EVB: UART on nRF52810.
#define SAMPLE_NRF52_UART USI_MT3620_BT_COMBO_NRF52_UART

// MT3620 USI BT EVB: Connect external LED using J32, pin 9.
#define SAMPLE_DEVICE_STATUS_LED USI_MT3620_BT_EVB_J32_PIN9_GPIO8

// MT3620 USI BT EVB: Connect external blue LED using J32, pin 16.
#define SAMPLE_PENDING_UPDATE_LED USI_MT3620_BT_EVB_J32_PIN16_GPIO15

