/*
 * (C) 2005-2020 MediaTek Inc. All rights reserved.
 *
 * Copyright Statement:
 *
 * This MT3620 driver software/firmware and related documentation
 * ("MediaTek Software") are protected under relevant copyright laws.
 * The information contained herein is confidential and proprietary to
 * MediaTek Inc. ("MediaTek"). You may only use, reproduce, modify, or
 * distribute (as applicable) MediaTek Software if you have agreed to and been
 * bound by this Statement and the applicable license agreement with MediaTek
 * ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
 * PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS
 * ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO
 * LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED
 * HEREUNDER WILL BE ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
 * RECEIVER TO MEDIATEK DURING THE PRECEDING TWELVE (12) MONTHS FOR SUCH
 * MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __MT3620_HW_MEMMAP_H__
#define __MT3620_HW_MEMMAP_H__
#include "type_def.h"


/*************** For All CPU ***************/
#define CA7_FIREWALL_ROUTING_BASE	0x30000000
#define CA7_PINMUX_BASE				0x30010000
#define CA7_GPIO_BASE				0x30020000
#define CA7_TOPCFG_BASE            	0x30030000
#define CA7_DMA_TZ_BASE				0x30040000

#define CA7_WDT_BASE				0x30050000
#define CA7_SYSRAM_CTRL_BASE		0x30060000
#define CA7_SFCFG_BASE              0x30070000
#define CA7_INFRA_BUS_BASE			0x30080000
#define CA7_RTC_BASE				0x30090000

#define CA7_PMTR_BASE				0x300A0000

#define CA7_INTF_BASE				0x300C0000
#define CA7_MCUSYS_CFG_BASE			0x300D0000
#define CA7_EFUSE_BASE				0x300E0000

#define CA7_MBOX_HSP_SECURE_BASE	0x300F0000
#define CA7_SWD_BASE				0x30200000

#define CA7_MBOX_HSP_NORMAL_BASE	0x31000000
#define CA7_MBOX_IO0_BASE			0x31010000
#define CA7_MBOX_IO1_BASE			0x31020000
#define CA7_GPT_BASE                0x3103E000
#define CA7_UART0_BASE              0x31040000

#define CA7_DMA_BASE                0x31050000
#define CA7_IRQ_BASE				0x31060000



/*APB1 to APB14*/

#define CM4_ADC_BASE				0x38000000	/*APB1*/

#define Cm4_ADC_TOPCFGAON_CLK_RG			    0x30030208
#define CM4_PWM_0_BASE				0x38010000	/*APB2*/
#define CM4_PWM_1_BASE				0x38020000	/*APB3*/
#define CM4_PWM_2_BASE				0x38030000	/*APB4*/

#define CM4_GPIO_0_BASE				0x38010000	/*APB2*/
#define CM4_GPIO_1_BASE				0x38020000	/*APB3*/
#define CM4_GPIO_2_BASE				0x38030000	/*APB4*/

#define CM4_GPIO_3_BASE				0x38040000	/*APB5*/
#define CM4_GPIO_4_BASE				0x38050000	/*APB6*/
#define CM4_GPIO_5_BASE				0x38060000	/*APB7*/

#define CM4_ISU0_I2C_BASE			0x38070000	/*APB8*/
#define CM4_ISU0_SPIM_BASE			0x38070300
#define CM4_ISU0_SPIS_BASE			0x38070400
#define CM4_ISU0_UART_BASE			0x38070500

#define CM4_ISU1_I2C_BASE			0x38080000	/*APB9*/
#define CM4_ISU1_SPIM_BASE			0x38080300
#define CM4_ISU1_SPIS_BASE			0x38080400
#define CM4_ISU1_UART_BASE			0x38080500

#define CM4_ISU2_I2C_BASE			0x38090000	/*APB10*/
#define CM4_ISU2_SPIM_BASE			0x38090300
#define CM4_ISU2_SPIS_BASE			0x38090400
#define CM4_ISU2_UART_BASE			0x38090500

#define CM4_ISU3_I2C_BASE			0x380A0000	/*APB11*/
#define CM4_ISU3_SPIM_BASE			0x380A0300
#define CM4_ISU3_SPIS_BASE			0x380A0400
#define CM4_ISU3_UART_BASE			0x380A0500

#define CM4_ISU4_I2C_BASE			0x380B0000	/*APB12*/
#define CM4_ISU4_SPIM_BASE			0x380B0300
#define CM4_ISU4_SPIS_BASE			0x380B0400
#define CM4_ISU4_UART_BASE			0x380B0500

#define CM4_ISU5_I2C_BASE			0x380C0000	/*APB13*/
#define CM4_ISU5_SPIM_BASE			0x380C0300
#define CM4_ISU5_SPIS_BASE			0x380C0400
#define CM4_ISU5_UART_BASE			0x380C0500


#define CM4_I2S0_BASE				0x380D0000	/*APB14_0*/
#define CM4_I2S1_BASE				0x380E0000	/*APB14_1*/
#define CM4_I2S_0_data_BASE			0x380F0000
#define CM4_I2S_1_data_BASE			0x38100000

#define CM4_WIFI_HIF_BASE			0x60000000
#define CM4_WIFI_PAD_BASE			0x61000000
#define CM4_WFSOC_N9_SYSRAM_BASE	0x62000000
#define CM4_WFSOC_TOP_OFF_BASE		0x63000000
#define CM4_WFSOC_TOP_AON_BASE		0x64000000

#define CM4_WFSOC_HIF_BASE			0x65000000
#define CM4_WFSOC_WF_MAC_BASE		0x66000000
#define CM4_WFSOC_PSE_BASE			0x67000000
#define CM4_WFSOC_USER1_BASE		0x68000000
#define CM4_WFSOC_USER2_BASE		0x70000000

#define CA7_SYSRAM_BASE				0x80000000
#define CA7_SFLASH_BASE				0x90000000

#define CM4_EXEC_IN_PLACE_BASE			CA7_SFLASH_BASE
#define CM4_EXEC_IN_PLACE_END			0x9FFFFFFF

#define CA7_EXEC_IN_PLACE_BASE		0x90000000	/* XIP */
#define CA7_EXEC_IN_PLACE_END		  0x91000000	/* XIP End -- 16MB */


#define MCU_CFG_NVIC_BASE           0xE000E000
#define CM4_IRQ_BASE                0x21000000
#define CM4_IRQ_SENSE_BASE          0x21000200
#define MCU_CFG_NVIC_SENSE_BASE     CM4_IRQ_SENSE_BASE
#define CA7_HSP_CFG_DEBOUNCE_BASE   CA7_IRQ_BASE


#ifdef M4SC_ENABLE
/*************** For HSP CM4***************/
#define CM4_TCM_ROM_BASE			0x00000000
#define CM4_TCM_FIRST_BASE			0x00100000
#define CM4_TCM_SECOND_BASE			0x00110000
#define CM4_TCM_END					0x0011FFFF

#define CM4_CONFIG_BASE				0x20000000
#define CM4_IRQ_CFG_BASE			0x21000000

#define CM4_RGU_LP_BASE				0x21010000
#define CM4_WDT_BASE				0x21020000
#define CM4_GPT_BASE				0x21030000
//#define CM4_UART1_BASE				0x21040000
#define CM4_UART0_BASE				0x21040000

#define CM4_MBOX_NORMAL_BASE		0x21050000

#define CM4_MBOX_SECURE_BASE		0x21060000
#define CM4_INFRA_BUS_BASE			0x21070000
#define CM4_CHIP_MISC_CFG_BASE		0x21080000
#define CM4_CHIP_RESET_BASE			0x21090000
#define CM4_CHIP_RESOURCE_BASE		0x210A0000

#define CM4_SPI_SLAVE_BASE			0x210B0000
#define CM4_REGISTER_BASE			0x2F000000
#define CM4_SYSTEM_BASE				0xE000E000
#define CM4_SYSRAM_BASE				CM4_TCM_FIRST_BASE
#define CM4_SYSRAM_END				CM4_TCM_END

#define CM4_DMA_BASE
#define MCU_CFG_DEBOUNCE_BASE       CA7_HSP_CFG_DEBOUNCE_BASE

#else
/*************** For IO0/1 CM4***************/
#define CM4_1ST_JUMP_BASE			0x00000000
#define CM4_TCM_FIRST_BASE			0x00100000
#define CM4_TCM_SECOND_BASE			0x00110000
#define CM4_TCM_THIRD_BASE			0x00120000
#define CM4_ALIAS_TO_FLASH_BASE		0x10000000

#define CM4_CONFIG_BASE				0x20000000
#define CM4_CACHE_CTRL_BASE			0x20080000
#define CM4_CACHE_MPU_BASE			0x20090000
#define CM4_IRQ_CFG_BASE			0x21000000
#define CM4_RGU_LP_BASE				0x21010000
#define CM4_WDT_BASE				0x21020000
#define CM4_GPT_BASE				0x21030000

//#define CM4_UART1_BASE				0x21040000
#define CM4_UART0_BASE				0x21040000

#define CM4_MBOX_CA7_BASE			0x21050000
#define CM4_MBOX_IOCM4_BASE			0x21060000
#define CM4_INFRA_BUS_BASE			0x21070000
#define CM4_DMA_BASE				0x21080000

#define CM4_SYSRAM_BASE				0x22000000
#define CM4_SYSRAM_END				0x22FFFFFF

#define CM4_SYSTEM_BASE				0xE000E000
#define MCU_CFG_DEBOUNCE_BASE       CM4_IRQ_BASE

#endif


/**********************************************************/
/**********************************************************/
/**********************************************************/


/* AON */
//#define CA7_SYSRAM_BASE			0xB8000000
//#define CA7_SYSRAM_END			0xB8600000

/* CM4 AHB */

/* HSP */

/* HSP CM4 Local*/
#if 0//def M4SC_ENABLE
#define CM4_TCMROM_BASE						0x00000000
#define CM4_TCMRAM_BASE						0x00100000	/* TCM Start */
#define CM4_TCMRAM_END						0x0011FFFF	/* XIP End -- 128KB */
#define CM4_CONFIG_BASE						0x20000000
#define CM4_IRQ_BASE							0x21000000
#define CM4_TOPRGU_BASE						0x21010000
#define CM4_WDT_BASE							0x21020000
#define CM4_GPT_BASE							0x21030000
#define CM4_UART1_BASE						0x21040000
#define CM4_UART_DSN_BASE					CM4_UART1_BASE
#define CM4_MBOX_NORMAL_BASE			0x21050000
#define CM4_MBOX_SECURE_BASE			0x21060000
#define CM4_INFRA_BUS_BASE				0x21070000
#define CM4_DMA_BASE							0x21080000
#define CM4_SPISLAVE_BASE					0x210B0000
#define CM4_HSPI_BASE							0x2F000000
#define CM4_SYSRAM_BASE						CM4_TCMRAM_BASE
#define CM4_SYSRAM_END						CM4_TCMRAM_END
//#else
/* IO CM4 Local*/
#define CM4_TCMROM_BASE						0x00000000
#define CM4_TCMRAM_BASE						0x00100000	/* TCM Start */
#define CM4_TCMRAM_END						0x0012FFFF	/* TCM End */
#define CM4_CACHE_CTRL_BASE				0x20080000
#define CM4_CACHE_MPU_BASE				0x2008002C
#define CM4_EXEC_IN_PLACE_BASE		0x10000000	/* XIP */
#define CM4_EXEC_IN_PLACE_END			0x1FFFFFFF	/* XIP End -- 256MB */
#define CM4_CONFIG_BASE						0x20000000
#define CM4_IRQ_BASE							0x21000000
#define CM4_IRQ_SENSE_BASE				0x21000200
#define CM4_TOPRGU_BASE						0x21010000
#define CM4_WDT_BASE							0x21020000
#define CM4_GPT_BASE							0x21030000
#define CM4_UART1_BASE						0x21040000
#define CM4_UART_DSN_BASE					CM4_UART1_BASE
#define CM4_MBOX_NORMAL_BASE			0x21050000
#define CM4_MBOX_SECURE_BASE			0x21060000
#define CM4_INFRA_BUS_BASE				0x21070000
#define CM4_DMA_BASE							0x21080000
#define CM4_SYSRAM_BASE						0x22000000
#define CM4_SYSRAM_END						0x22FFFFFF
#endif

#ifdef M4SC_ENABLE
#define CM4_SPISLAVE_BASE					0x210B0000
#endif

/* Global for All CM4 and CA7 */
#define CM4_ADC_BASE							0x38000000

#define Cm4_ADC_TOPCFGAON_CLK_RG			    0x30030208
#define CM4_PWM_GRP0_BASE  				0x38010000
#define CM4_PWM_GRP1_BASE  				0x38020000
#define CM4_PWM_GRP2_BASE 				0x38030000

#define CM4_GPIO_GRP0_BASE  			0x38010000
#define CM4_GPIO_GRP1_BASE  			0x38020000
#define CM4_GPIO_GRP2_BASE 				0x38030000

#define CM4_GPIO_GRP3  						0x38040000
#define CM4_GPIO_GRP4  						0x38050000
#define CM4_GPIO_GRP5  						0x38060000

#define CM4_ISU0_I2C_BASE					0x38070000
#define CM4_ISU0_SPIM_BASE				0x38070300
#define CM4_ISU0_SPIS_BASE				0x38070400
#define CM4_ISU0_UART_BASE				0x38070500

#define CM4_ISU1_I2C_BASE					0x38080000
#define CM4_ISU1_SPIM_BASE				0x38080300
#define CM4_ISU1_SPIS_BASE				0x38080400
#define CM4_ISU1_UART_BASE				0x38080500

#define CM4_ISU2_I2C_BASE					0x38090000
#define CM4_ISU2_SPIM_BASE				0x38090300
#define CM4_ISU2_SPIS_BASE				0x38090400
#define CM4_ISU2_UART_BASE				0x38090500

#define CM4_ISU3_I2C_BASE					0x380A0000
#define CM4_ISU3_SPIM_BASE				0x380A0300
#define CM4_ISU3_SPIS_BASE				0x380A0400
#define CM4_ISU3_UART_BASE				0x380A0500

#define CM4_ISU4_I2C_BASE					0x380B0000
#define CM4_ISU4_SPIM_BASE				0x380B0300
#define CM4_ISU4_SPIS_BASE				0x380B0400
#define CM4_ISU4_UART_BASE				0x380B0500

#define CM4_ISU5_I2C_BASE					0x380C0000
#define CM4_ISU5_SPIM_BASE				0x380C0300
#define CM4_ISU5_SPIS_BASE				0x380C0400
#define CM4_ISU5_UART_BASE				0x380C0500

#define CM4_I2S0_BASE							0x380D0000
#define CM4_I2S1_BASE							0x380E0000

#define CM4_MTK_CRYPTO_BASE				0x830F0000	/* TODO: incorrect definition */

/* Global for HSP CM4 and CA7 */
#ifdef M4SC_ENABLE
#define CA7_WIFI_HIF_BASE					0x60000000
#define CA7_WIFI_PDA_BASE					0x60010000
#define CA7_WFSOC_N9_SYSRAM_BASE	0x60020000
#define CA7_WFSOC_TOP_OFF_BASE		0x60030000
#define CA7_WFSOC_TOP_AON_BASE		0x60040000
#define CA7_WFSOC_HIF_BASE				0x60050000
#define CA7_WFSOC_WF_MAC_BASE			0x60060000
#define CA7_WFSOC_PSE_BASE				0x60070000
#define CA7_WFSOC_USER1_BASE			0x60080000
#define CA7_WFSOC_USER2_BASE			0x70000000
#endif

//#define IOT_GPIO_PINMUX_AON_BASE	(0x81023000)
#define IOT_GPIO_PINMUX_AON_BASE	(0x30010000)
#define IOT_GPIO_PINMUX_OFF_BASE	(0x80025100)
#define IOT_GPIO_PINMUX_CA7_BASE	(0xBD00C000)


#define IOT_GPIO_OFF_BASE		(0x80025000) /* TODO: change to new GPIO GROUP setting */
#define IOT_GPIO_AON_BASE		(0x8300B000) /* TODO: change to new GPIO GROUP setting */


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

typedef enum _XTAL_FREQ_T {
    XTAL_FREQ_20M = 0,
    XTAL_FREQ_40M = 1,
    XTAL_FREQ_26M = 2,
    XTAL_FREQ_52M = 3,
} XTAL_FREQ_T, *P_XTAL_FREQ_T;


// IOT
typedef union _PWM_GLO_CTRL_T {
    struct {
        uint32_t    global_kick             : 1;    // All PWM modules with "pwm_global_kick_enable" would be kicked by this bit at the same time
        uint32_t    PWM_tick_clock_sel      : 2;    // 0: 32KHz 1: 2MHz 2: XTAL clock
        uint32_t    reserve3_31             : 29;   // Reserved
    } field;
    uint32_t word;
} PWM_GLO_CTRL_T, *P_PWM_GLO_CTRL_T;

typedef struct
{
  volatile uint32_t PWM_CTRL;
  volatile uint32_t PWM_PARAM_S0;
  volatile uint32_t PWM_PARAM_S1;
} IOT_PWM_TypeDef, *P_IOT_PWM_TypeDef;

typedef union _PWM_CTRL_T {
    struct {
        uint32_t    kick                    : 1;    // Module load PWM parameter setting and generate waveform
        uint32_t    replay_mode             : 1;    // replay_mode	Replay mode indication (Only available when S1 exists)
        uint32_t    polarity                : 1;    // PWM polarity setting, 0: active high 1:active low
        uint32_t    pwm_io_ctrl             : 1;    // 0: PIO (as output) 1: open drain (as output when active low)
        uint32_t    pwm_clock_gated         : 1;    // 1: Gating tick clock for PWM
        uint32_t    pwm_global_kick_enable  : 1;    // PWM would be kicked by global kick if this bit is set
        uint32_t    reserve6_7              : 2;    // Reserved
        uint32_t    S0_stay_cycle           : 12;   // The stay cycles of S0
        uint32_t    S1_stay_cycle           : 12;   // The stay cycles of S1 (If this field is 0, S1 does not exist)
    } field;
    uint32_t word;
} PWM_CTRL_T, *P_PWM_CTRL_T;

typedef union _PWM_PARAM_S_T {
    struct {
        uint32_t    S_pwm_on_time          : 16;   // State PWM_ON duration     (unit: tick clock period)
        uint32_t    S_pwm_off_time         : 16;   // State PWM_OFF duration     (unit: tick clock period)
    } field;
    uint32_t word;
} PWM_PARAM_S_T, *P_PWM_PARAM_S_T;



// IOT HAL common
typedef enum _ENUM_HAL_RET_T {
    HAL_RET_SUCCESS = 0,
    HAL_RET_FAIL
} __attribute__ ((__packed__)) ENUM_HAL_RET_T, *P_ENUM_HAL_RET_T;


// IOT RTC
typedef struct _IOT_RTC_CTRL_TypeDef
{
    volatile uint32_t RTC_PWRCHK1;        // 0x0004
    volatile uint32_t RTC_PWRCHK2;        // 0x0008
    volatile uint32_t RTC_KEY;            // 0x000C
    volatile uint32_t RTC_PROT1;          // 0x0010
    volatile uint32_t RTC_PROT2;          // 0x0014
    volatile uint32_t RTC_PROT3;          // 0x0018
    volatile uint32_t RTC_PROT4;          // 0x001C
    volatile uint32_t RTC_CTL;            // 0x0020
    volatile uint32_t RTC_LPD_CTL;        // 0x0024
    volatile uint32_t RTC_XOSC_CFG;       // 0x0028
    volatile uint32_t RTC_DEBNCE;         // 0x002C
    volatile uint32_t RTC_PMU_EN;         // 0x0030
    volatile uint32_t RTC_PAD_CTL;        // 0x0034
    volatile uint32_t RTC_RESV0;          // 0x0038
    volatile uint32_t RTC_WAVEOUT;        // 0x003C
    volatile uint32_t RTC_TC_YEA;         // 0x0040
    volatile uint32_t RTC_TC_MON;         // 0x0044
    volatile uint32_t RTC_TC_DOM;         // 0x0048
    volatile uint32_t RTC_TC_DOW;         // 0x004C
    volatile uint32_t RTC_TC_HOU;         // 0x0050
    volatile uint32_t RTC_TC_MIN;         // 0x0054
    volatile uint32_t RTC_TC_SEC;         // 0x0058
    volatile uint32_t RTC_RESV1;          // 0x005C
    volatile uint32_t RTC_AL_YEAR;        // 0x0060
    volatile uint32_t RTC_AL_MON;         // 0x0064
    volatile uint32_t RTC_AL_DOM;         // 0x0068
    volatile uint32_t RTC_AL_DOW;         // 0x006C
    volatile uint32_t RTC_AL_HOUR;        // 0x0070
    volatile uint32_t RTC_AL_MIN;         // 0x0074
    volatile uint32_t RTC_AL_SEC;         // 0x0078
    volatile uint32_t RTC_AL_CTL;         // 0x007C
    volatile uint32_t RTC_RIP_CTL;        // 0x0080
    volatile uint32_t RTC_RIP_CNTH;       // 0x0084
    volatile uint32_t RTC_RIP_CNTL;       // 0x0088
    volatile uint32_t RTC_RESV2;          // 0x008C
    volatile uint32_t RTC_TIMER_CTL;      // 0x0090
    volatile uint32_t RTC_TIMER_CNTH;     // 0x0094
    volatile uint32_t RTC_TIMER_CNTL;     // 0x0098
} IOT_RTC_CTRL_TypeDef, *P_IOT_RTC_CTRL_TypeDef;

typedef struct _IOT_RTC_SPARE_TypeDef
{
    volatile uint32_t RTC_SPARE0 ; //0x00C0
    volatile uint32_t RTC_SPARE1 ; //0x00C4
    volatile uint32_t RTC_SPARE2 ; //0x00C8
    volatile uint32_t RTC_SPARE3 ; //0x00CC
    volatile uint32_t RTC_SPARE4 ; //0x00D0
    volatile uint32_t RTC_SPARE5 ; //0x00D4
    volatile uint32_t RTC_SPARE6 ; //0x00D8
    volatile uint32_t RTC_SPARE7 ; //0x00DC
    volatile uint32_t RTC_SPARE8 ; //0x00E0
    volatile uint32_t RTC_SPARE9 ; //0x00E4
    volatile uint32_t RTC_SPARE10; //0x00E8
    volatile uint32_t RTC_SPARE11; //0x00EC
    volatile uint32_t RTC_SPARE12; //0x00F0
    volatile uint32_t RTC_SPARE13; //0x00F4
    volatile uint32_t RTC_SPARE14; //0x00F8
    volatile uint32_t RTC_SPARE15; //0x00FC
} IOT_RTC_SPARE_TypeDef, *P_IOT_RTC_SPARE_TypeDef;
#define RTC_SPARE0_OFFSET   (0xC0)

typedef struct _IOT_RTC_BACKUP_TypeDef
{
    volatile uint32_t RTC_BACKUP0 ; //0x0140
    volatile uint32_t RTC_BACKUP1 ; //0x0144
    volatile uint32_t RTC_BACKUP2 ; //0x0148
    volatile uint32_t RTC_BACKUP3 ; //0x014C
    volatile uint32_t RTC_BACKUP4 ; //0x0150
    volatile uint32_t RTC_BACKUP5 ; //0x0154
    volatile uint32_t RTC_BACKUP6 ; //0x0158
    volatile uint32_t RTC_BACKUP7 ; //0x015C
    volatile uint32_t RTC_BACKUP8 ; //0x0160
    volatile uint32_t RTC_BACKUP9 ; //0x0164
    volatile uint32_t RTC_BACKUP10; //0x0168
    volatile uint32_t RTC_BACKUP11; //0x016C
    volatile uint32_t RTC_BACKUP12; //0x0170
    volatile uint32_t RTC_BACKUP13; //0x0174
    volatile uint32_t RTC_BACKUP14; //0x0178
    volatile uint32_t RTC_BACKUP15; //0x017C
    volatile uint32_t RTC_BACKUP16; //0x0180
    volatile uint32_t RTC_BACKUP17; //0x0184
    volatile uint32_t RTC_BACKUP18; //0x0188
    volatile uint32_t RTC_BACKUP19; //0x018C
    volatile uint32_t RTC_BACKUP20; //0x0190
    volatile uint32_t RTC_BACKUP21; //0x0194
    volatile uint32_t RTC_BACKUP22; //0x0198
    volatile uint32_t RTC_BACKUP23; //0x019C
    volatile uint32_t RTC_BACKUP24; //0x01A0
    volatile uint32_t RTC_BACKUP25; //0x01A4
    volatile uint32_t RTC_BACKUP26; //0x01A8
    volatile uint32_t RTC_BACKUP27; //0x01AC
    volatile uint32_t RTC_BACKUP28; //0x01B0
    volatile uint32_t RTC_BACKUP29; //0x01B4
    volatile uint32_t RTC_BACKUP30; //0x01B8
    volatile uint32_t RTC_BACKUP31; //0x01BC
    volatile uint32_t RTC_BACKUP32; //0x01C0
    volatile uint32_t RTC_BACKUP33; //0x01C4
    volatile uint32_t RTC_BACKUP34; //0x01C8
    volatile uint32_t RTC_BACKUP35; //0x01CC
} IOT_RTC_BACKUP_TypeDef, *P_IOT_RTC_BACKUP_TypeDef;
#define RTC_BACKUP0_OFFSET   (0x140)


#define IOT_RTC_COREPDN_ADDR    (CA7_RTC_BASE + 0x0100)
#define RTC_COREPDN_G_ENABLE_OFFSET	        (1)
#define RTC_COREPDN_CORE_SHUTDOWN_OFFSET    (0)

#define IOT_RTC_RTC_PWRCHK1     (0xC6)
#define IOT_RTC_RTC_PWRCHK2     (0x9A)
#define IOT_RTC_RTC_KEY         (0x59)
#define IOT_RTC_RTC_PROT1       (0xA3)
#define IOT_RTC_RTC_PROT2       (0x57)
#define IOT_RTC_RTC_PROT3       (0x67)
#define IOT_RTC_RTC_PROT4       (0xD2)

// RTC_CTL 0x0020
#define RTC_CTL_DEBNCE_OK_OFFSET   (7)
#define RTC_CTL_INHIBIT_OFFSET     (6)
#define RTC_CTL_PROT_PASS_OFFSET   (4)
#define RTC_CTL_KEY_PASS_OFFSET    (3)
#define RTC_CTL_PWR_PASS_OFFSET    (2)
#define RTC_CTL_SIM_RTC_OFFSET     (1)
#define RTC_CTL_RC_STOP_OFFSET     (0)

// RTC_XOSC_CFG 0x0028
#define RTC_XOSC_CFG_OSCPDN_OFFSET      (7)
#define RTC_XOSC_CFG_AMPCTL_EN_OFFSET   (5)
#define RTC_XOSC_CFG_AMP_GSEL_OFFSET    (4)
#define RTC_XOSC_CFG_OSCCALI_MASK       (BITS(0, 3))
#define RTC_XOSC_CFG_OSCCALI_OFFSET     (0)

// RTC_DEBNCE 0x002C
#define RTC_DEBNCE_DEBOUNCE_OFFSET  (0)
#define RTC_DEBNCE_DEBOUNCE_MASK    BITS(0, 2)

// RTC_TIMER_CTL 0x0090
#define RTC_TIMER_CTL_TR_INTEN_OFFSET   (1)
#define RTC_TIMER_CTL_TR_EXTEN_OFFSET   (0)

// RTC_PMU_EN 0x0030
#define RTC_TIMER_STA_OFFSET        (5)
#define RTC_ALARM_STA_OFFSET        (4)
#define RTC_PMU_EN_STATE_OFFSET     (2)
#define RTC_PMU_EN_STATE_MASK       BITS(2, 3)
#define RTC_PMU_EN_EXT_OFFSET       (1)
#define RTC_PMU_EN_OFFSET           (0)

// Low Power related CR
#define CM4_HCLK_CK_CTRL			(CM4_RGU_LP_BASE + 0x0C)
#define CM4_HCLK_CK_SEL_OFFSET  (8)
#define CM4_HCLK_CK_SEL_MASK    BITS(8, 10)
#define TOP_AON_CM4_CKGEN0          (CM4_RGU_LP_BASE + 0x1B0)
#define CM4_CKGEN0_CM4_WBTAC_MCU_CK_SEL_OFFSET  (14)
#define CM4_CKGEN0_CM4_WBTAC_MCU_CK_SEL_MASK    BITS(14, 15)


#define TOP_AON_HCLK_2M_CKEGN       (CM4_RGU_LP_BASE + 0x1B4)
#define TOP_AON_CM4_PWRCTLCR        (CM4_RGU_LP_BASE + 0x1B8)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_OFFSET       (0)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_MASK         BITS(0, 10)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_20M_OFFSET   (4)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_40M_OFFSET   (9)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_26M_OFFSET   (6)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_52M_OFFSET   (10)
#define CM4_PWRCTLCR_CM4_MCU_PWR_STAT_OFFSET    (11)
#define CM4_PWRCTLCR_CM4_MCU_PWR_STAT_MASK      BITS(11, 12)
#define CM4_PWRCTLCR_CM4_XO_NO_OFF_OFFSET       (15)
#define CM4_PWRCTLCR_CM4_FORCE_MCU_STOP_OFFSET  (16)
#define CM4_PWRCTLCR_CM4_MCU_960_EN_OFFSET      (18)
#define CM4_PWRCTLCR_CM4_HCLK_IRQ_B_OFFSET      (19)
#define CM4_PWRCTLCR_CM4_HCLK_IRQ_EN_OFFSET     (20)
#define CM4_PWRCTLCR_CM4_DSLP_IRQ_B_OFFSET      (21)
#define CM4_PWRCTLCR_CM4_DSLP_IRQ_EN_OFFSET     (22)
#define CM4_PWRCTLCR_CM4_DSLP_IRQ_EN_MASK       BITS(22, 23)
#define CM4_PWRCTLCR_CM4_NEED_RESTORE_OFFSET    (24)
#define CM4_PWRCTLCR_CM4_DSLP_MODE_OFFSET       (25)
#define CM4_PWRCTLCR_WF_PLL_RDY_OFFSET          (26)
#define CM4_PWRCTLCR_BT_PLL_RDY_OFFSET          (27)
#define CM4_PWRCTLCR_CM4_MPLL_EN_OFFSET         (28)
#define CM4_PWRCTLCR_CM4_MPLL_EN_MASK           BITS(28, 29)
#define CM4_PWRCTLCR_CM4_MPLL_BT_EN_OFFSET      (29)
#define CM4_PWRCTLCR_CM4_HW_CONTROL_OFFSET      (30)
#define CM4_PWRCTLCR_CM4_HWCTL_PWR_STAT_OFFSET  (31)

#define TOP_AON_CM4_MISC                        (CA7_TOPCFGAON_BASE + 0x1BC)
//needto-check
#define TOP_AON_CM4_STRAP_STA		            (0x1C0)

#define CM4_STRAP_STA_XTAL_FREQ_OFFSET          (13)
#define CM4_STRAP_STA_XTAL_FREQ_MASK            BITS(13, 15)

#define STRAP_STA_XTAL_FREQ_OFFSET          (0)
#define STRAP_STA_XTAL_FREQ_MASK            BITS(0, 1)

typedef struct _IOT_CM4_RGU_SLP_CTRL_TypeDef
{
    volatile uint32_t CM4_SLP_CTRL;  //0x21010000
    volatile uint32_t CM4_SLP_AUX;           //0x21010004
    volatile uint32_t CM4_SLP_EVENT_RECORD;              //0x21010008
    volatile uint32_t CM4_HLCK_CK_CTRL;         //0x2101000C
} IOT_CM4_RGU_SLP_CTRL_TypeDef, *P_IOT_CM4_RGU_SLP_CTRL_TypeDef;
typedef struct _IOT_CM4_RGU_MEM_CTRL_TypeDef
{
    volatile uint32_t CM4_HW_MEM_PDN;  //0x21010100
    volatile uint32_t CM4_HW_MEM_SLP;           //0x21010104
    volatile uint32_t CM4_RESV0;              //0x21010108
    volatile uint32_t CM4_SW_MEM_PDN;         //0x2101010C
    volatile uint32_t CM4_SW_MEM_SLP;           //0x21010110
    volatile uint32_t CM4_SW_MEM_ISO;           //0x21010114
    volatile uint32_t CM4_HW_MEM_PWR_CTRL_OPTION;           //0x21010118
 #ifdef M4SC_ENABLE
    volatile uint32_t CM4_SW_ROM_RST_CTRL;           //0x2101011C
 #else
    volatile uint32_t CM4_RESV1;           //0x2101011C
 #endif
    volatile uint32_t CM4_DUMMY_REGISTER;           //0x21010120

} IOT_CM4_RGU_MEM_CTRL_TypeDef, *P_IOT_CM4_RGU_MEM_CTRL_TypeDef;
#define IOT_CM4_RGU_MEM_CTRL_OFFSET (0x17C)

#define CM4_CONFIG_AON_BASE               0x21000000


/* WIC */
#define CM4_WIC_SW_CON_ADDR         (CM4_CONFIG_AON_BASE + 0x340)

#define CM4_WIC_SW_CLR_ADDR         (CM4_CONFIG_AON_BASE + 0x344)
#define CM4_WIC_SW_CLR_MASK         BIT(0)
#define CM4_WIC_SW_CLR_SHFT         0

#define CM4_WIC_PEND_STA0           (CM4_CONFIG_AON_BASE + 0x364)
#define CM4_WIC_PEND_STA1           (CM4_CONFIG_AON_BASE + 0x368)
#define CM4_WIC_PEND_STA2           (CM4_CONFIG_AON_BASE + 0x36C)
#define CM4_WIC_PEND_STA3           (CM4_CONFIG_AON_BASE + 0x370)

#define CM4_WIC_SW_PEND_EN_ADDR     (CM4_CONFIG_AON_BASE + 0x340)
#define CM4_WIC_SW_PEND_EN_MASK     BIT(0)
#define CM4_WIC_SW_PEND_EN_SHIFT    0

#define CM4_WIC_SW_INT_EN_ADDR      (CM4_CONFIG_AON_BASE + 0x340)
#define CM4_WIC_SW_INT_EN_MASK      BIT(1)
#define CM4_WIC_SW_INT_EN_SHIFT     1

#define CM4_WIC_SW_CLR_EN_ADDR      (CM4_CONFIG_AON_BASE + 0x340)
#define CM4_WIC_SW_CLR_EN_MASK      BIT(2)
#define CM4_WIC_SW_CLR_EN_SHIFT     2

#define CM4_WIC_DIS_RESET_ADDR      (CM4_CONFIG_AON_BASE + 0x340)
#define CM4_WIC_DIS_RESET_MASK      BIT(3)
#define CM4_WIC_DIS_RESET_SHIFT     3
#endif

