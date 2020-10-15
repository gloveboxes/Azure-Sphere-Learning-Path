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

#ifndef __IRQ_H__
#define __IRQ_H__


/******************************************************************************/
/*                          CM4 peripheral IRQ number                         */
/******************************************************************************/
#ifdef M4SC_ENABLE

#define CM4_IRQ_WIC	0
#define CM4_IRQ_RTC	1
#define CM4_IRQ_SF	2
#define CM4_IRQ_GPT	3
#define CM4_IRQ_GPT3	4

#define CM4_IRQ_WDT_A7	5
#define CM4_IRQ_WDT_M4_IOA	6
#define CM4_IRQ_WDT_M4_IOB	7
#define CM4_IRQ_WDT_N9	8
#define CM4_IRQ_WDT_SW_A7	9
#define CM4_IRQ_WDT_SW_M4_IOA	10
#define CM4_IRQ_WDT_SW_M4_IOB	11
#define CM4_IRQ_WDT_SW_N9	12
#define CM4_IRQ_WDT_M4_HSP	13

#define CM4_IRQ_UART	14
#define CM4_IRQ_SPIS	15
#define CM4_IRQ_INFRA_BUS	16

#define CM4_IRQ_M4S2A7S_RD	17
#define CM4_IRQ_M4S2A7S_NF	18
#define CM4_IRQ_A7S2M4S_WR	19
#define CM4_IRQ_A7S2M4S_NE	20
#define CM4_IRQ_M4S_FIFO	21
#define CM4_IRQ_A7S2M4S_SW	22
#define CM4_IRQ_M4N2A7N_RD	23
#define CM4_IRQ_M4N2A7N_NF	24
#define CM4_IRQ_A7N2M4N_WR	25
#define CM4_IRQ_A7N2M4N_NE	26
#define CM4_IRQ_M4N_FIFO	27
#define CM4_IRQ_A7N2M4N_SW	28

#define CM4_IRQ_M4S_WAKE_A7S	29
#define CM4_IRQ_M4N_WAKE_A7N	30
#define CM4_IRQ_M4A_WAKE_A7N	31
#define CM4_IRQ_M4B_WAKE_A7N	32
#define CM4_IRQ_A7S_WAKE_M4S	33
#define CM4_IRQ_A7N_WAKE_M4N	34
#define CM4_IRQ_A7N_WAKE_M4A	35
#define CM4_IRQ_M4B_WAKE_M4A	36
#define CM4_IRQ_A7N_WAKE_M4B	37
#define CM4_IRQ_M4A_WAKE_M4B	38

#define CM4_IRQ_IOA2HSP_IDLE		39
#define CM4_IRQ_IOA2HSP_CORE_CG		40
#define CM4_IRQ_IOA2HSP_WAIT_SLP	41
#define CM4_IRQ_IOA2HSP_SLP			42
#define CM4_IRQ_IOA2HSP_WAIT_WAKEUP	43
#define CM4_IRQ_IOA2HSP_BUS_CK_RLS	44
#define CM4_IRQ_IOB2HSP_IDLE		45
#define CM4_IRQ_IOB2HSP_CORE_CG		46
#define CM4_IRQ_IOB2HSP_WAIT_SLP	47
#define CM4_IRQ_IOB2HSP_SLP			48
#define CM4_IRQ_IOB2HSP_WAIT_WAKEUP	49
#define CM4_IRQ_IOB2HSP_BUS_CK_RLS	50

#define CM4_IRQ_WFSOC2HSP_XTAL		51
#define CM4_IRQ_WFSOC2HSP_BUCK		52
#define CM4_IRQ_CA7_STANDBY_WFI		53
#define CM4_IRQ_CA7_STANDBY_WFI_L2	54
#define CM4_IRQ_CA7_STANDBY_WFE		55
#define CM4_IRQ_PMU_BROWN_OUT		56


#define CM4_IRQ_CA7_GPT			65
#define CM4_IRQ_CA7_GPT3		66
#define CM4_IRQ_CA7_INFRA_BUS_1	67
#define CM4_IRQ_CA7_INFRA_BUS_2	68
#define CM4_IRQ_CA7_INFRA_BUS_3	69
#define CM4_IRQ_CA7_INFRA_BUS_4	70


/*	rt_ca7_gpio_irq_src[0]~rt_ca7_gpio_irq_src[23] <-> int id : 71-94 */
#define CM4_IRQ_GPIO_G0_0	71
#define CM4_IRQ_GPIO_G0_1	72
#define CM4_IRQ_GPIO_G0_2	73
#define CM4_IRQ_GPIO_G0_3	74
#define CM4_IRQ_GPIO_G1_0	75
#define CM4_IRQ_GPIO_G1_1	76
#define CM4_IRQ_GPIO_G1_2	77
#define CM4_IRQ_GPIO_G1_3	78
#define CM4_IRQ_GPIO_G2_0	79
#define CM4_IRQ_GPIO_G2_1	80
#define CM4_IRQ_GPIO_G2_2	81
#define CM4_IRQ_GPIO_G2_3	82

#define CM4_IRQ_GPIO_G3_0	83
#define CM4_IRQ_GPIO_G3_1	84
#define CM4_IRQ_GPIO_G3_2	85
#define CM4_IRQ_GPIO_G3_3	86
#define CM4_IRQ_GPIO_G4_0	87
#define CM4_IRQ_GPIO_G4_1	88
#define CM4_IRQ_GPIO_G4_2	89
#define CM4_IRQ_GPIO_G4_3	90
#define CM4_IRQ_GPIO_G5_0	91
#define CM4_IRQ_GPIO_G5_1	92
#define CM4_IRQ_GPIO_G5_2	93
#define CM4_IRQ_GPIO_G5_3	94


/*    rt_ca7_irq_src[0]~rt_ca7_irq_src[32] <-> int id : 95-127   */
#define CM4_IRQ_ISU_G0_I2C	95
#define CM4_IRQ_ISU_G0_SPIM	96
#define CM4_IRQ_ISU_G0_SPIS	97
#define CM4_IRQ_ISU_G0_UART	98
#define CM4_IRQ_ISU_G1_I2C	99
#define CM4_IRQ_ISU_G1_SPIM	100
#define CM4_IRQ_ISU_G1_SPIS	101
#define CM4_IRQ_ISU_G1_UART	102
#define CM4_IRQ_ISU_G2_I2C	103
#define CM4_IRQ_ISU_G2_SPIM	104
#define CM4_IRQ_ISU_G2_SPIS	105
#define CM4_IRQ_ISU_G2_UART	106

#define CM4_IRQ_ISU_G3_I2C	107
#define CM4_IRQ_ISU_G3_SPIM	108
#define CM4_IRQ_ISU_G3_SPIS	109
#define CM4_IRQ_ISU_G3_UART	110
#define CM4_IRQ_ISU_G4_I2C	111
#define CM4_IRQ_ISU_G4_SPIM	112
#define CM4_IRQ_ISU_G4_SPIS	113
#define CM4_IRQ_ISU_G4_UART	114
#define CM4_IRQ_ISU_G5_I2C	115
#define CM4_IRQ_ISU_G5_SPIM	116
#define CM4_IRQ_ISU_G5_SPIS	117
#define CM4_IRQ_ISU_G5_UART	118

#define CM4_IRQ_I2S0	    119
#define CM4_IRQ_I2S1	    120
#define CM4_IRQ_ADC		    121

#define CM4_IRQ_GPIO_G0_CNT	122
#define CM4_IRQ_GPIO_G1_CNT	123
#define CM4_IRQ_GPIO_G2_CNT	124
#define CM4_IRQ_GPIO_G3_CNT	125
#define CM4_IRQ_GPIO_G4_CNT	126
#define CM4_IRQ_GPIO_G5_CNT	127

#define CM4_IRQ_A7DMA	    128
#define CM4_IRQ_TZDMA	    129
#define CM4_IRQ_TZ		    130
#define CM4_IRQ_WFSOC	    131
#define CM4_IRQ_WF_HIF	    132
#define HSPM4_CDBGPWRUPREQ  133
#define HSPM4_CDBGPWRUPACK  134
#define CA7_CDBGPWRUPREQ    135
#define CA7_CDBGPWRUPACK    136

#else

#define CM4_IRQ_WIC			0
#define CM4_IRQ_GPT			1
#define CM4_IRQ_GPT3		2

#define CM4_IRQ_WDT_M4_IO	3
#define CM4_IRQ_UART		4
#define CM4_IRQ_INFRA_BUS	5

#define CM4_IRQ_M42A7N_RD	6
#define CM4_IRQ_M42A7N_NF	7
#define CM4_IRQ_A7N2M4_WR	8
#define CM4_IRQ_A7N2M4_NE	9
#define CM4_IRQ_M42A7N_FIFO	10
#define CM4_IRQ_A7N2M4_SW	11

#define CM4_IRQ_M42M4_RD	12
#define CM4_IRQ_M42M4_NF	13
#define CM4_IRQ_M42M4_WR	14
#define CM4_IRQ_M42M4_NE	15
#define CM4_IRQ_M42M4_FIFO	16
#define CM4_IRQ_M42M4_SW	17

#define CM4_IRQ_A7N_WAKE_M4	18
#define CM4_IRQ_M4_WAKE_M4	19

/*	rt_iox_cm4_gpio_irq_src[0]~rt_iox_cm4_gpio_irq_src[23] <-> int id : 20-43 */
#define CM4_IRQ_GPIO_G0_0	20
#define CM4_IRQ_GPIO_G0_1	21
#define CM4_IRQ_GPIO_G0_2	22
#define CM4_IRQ_GPIO_G0_3	23
#define CM4_IRQ_GPIO_G1_0	24
#define CM4_IRQ_GPIO_G1_1	25
#define CM4_IRQ_GPIO_G1_2	26
#define CM4_IRQ_GPIO_G1_3	27
#define CM4_IRQ_GPIO_G2_0	28
#define CM4_IRQ_GPIO_G2_1	29
#define CM4_IRQ_GPIO_G2_2	30
#define CM4_IRQ_GPIO_G2_3	31

#define CM4_IRQ_GPIO_G3_0	32
#define CM4_IRQ_GPIO_G3_1	33
#define CM4_IRQ_GPIO_G3_2	34
#define CM4_IRQ_GPIO_G3_3	35
#define CM4_IRQ_GPIO_G4_0	36
#define CM4_IRQ_GPIO_G4_1	37
#define CM4_IRQ_GPIO_G4_2	38
#define CM4_IRQ_GPIO_G4_3	39
#define CM4_IRQ_GPIO_G5_0	40
#define CM4_IRQ_GPIO_G5_1	41
#define CM4_IRQ_GPIO_G5_2	42
#define CM4_IRQ_GPIO_G5_3	43

/*    rt_iox_cm4_irq_src[0]~rt_iox_cm4_irq_src[32] <-> int id : 44-76   */
#define CM4_IRQ_ISU_G0_I2C	44
#define CM4_IRQ_ISU_G0_SPIM	45
#define CM4_IRQ_ISU_G0_SPIS	46
#define CM4_IRQ_ISU_G0_UART	47
#define CM4_IRQ_ISU_G1_I2C	48
#define CM4_IRQ_ISU_G1_SPIM	49
#define CM4_IRQ_ISU_G1_SPIS	50
#define CM4_IRQ_ISU_G1_UART	51
#define CM4_IRQ_ISU_G2_I2C	52
#define CM4_IRQ_ISU_G2_SPIM	53
#define CM4_IRQ_ISU_G2_SPIS	54
#define CM4_IRQ_ISU_G2_UART	55

#define CM4_IRQ_ISU_G3_I2C	56
#define CM4_IRQ_ISU_G3_SPIM	57
#define CM4_IRQ_ISU_G3_SPIS	58
#define CM4_IRQ_ISU_G3_UART	59
#define CM4_IRQ_ISU_G4_I2C	60
#define CM4_IRQ_ISU_G4_SPIM	61
#define CM4_IRQ_ISU_G4_SPIS	62
#define CM4_IRQ_ISU_G4_UART	63
#define CM4_IRQ_ISU_G5_I2C	64
#define CM4_IRQ_ISU_G5_SPIM	65
#define CM4_IRQ_ISU_G5_SPIS	66
#define CM4_IRQ_ISU_G5_UART	67

#define CM4_IRQ_I2S0    	68
#define CM4_IRQ_I2S1	    69
#define CM4_IRQ_ADC		    70

#define CM4_IRQ_GPIO_G0_CNT	71
#define CM4_IRQ_GPIO_G1_CNT	72
#define CM4_IRQ_GPIO_G2_CNT	73
#define CM4_IRQ_GPIO_G3_CNT	74
#define CM4_IRQ_GPIO_G4_CNT	75
#define CM4_IRQ_GPIO_G5_CNT	76

#define CM4_IRQ_M4DMA		77

#define IOM4_CDBGPWRUPREQ   78
#define IOM4_CDBGPWRUPACK   79

#endif




#if 1
#define CM4_UART1_IRQ			0
#define CM4_DMA_IRQ			    1
#define CM4_HIF_IRQ			    2
//#define CM4_I2C1_IRQ			3
//#define CM4_I2C2_IRQ			4
#define CM4_UART2_IRQ			5
#define CM4_MTK_CRYPTO_IRQ		6
#define CM4_SF_IRQ			    7
#define CM4_EINT_IRQ			8
#define CM4_BTIF_IRQ			9
#define CM4_WDT_IRQ			    10
#define CM4_AHB_MON_IRQ			11
//#define CM4_SPI_SLAVE_0_IRQ		12
#define CM4_N9_WDT_IRQ			13
//#define CM4_ADC_IRQ			    14
#define CM4_IRDA_TX_IRQ			15
#define CM4_IRDA_RX_IRQ			16
#define CM4_USB_VBUS_ON_IRQ		17
#define CM4_USB_VBUS_OFF_IRQ	18
//#define CM4_SPIS_CS_WAKEUP_IRQ	19	/* for CM4 Wakeup */
#define CM4_GPT3_IRQ			20
#define CM4_IRQ21_IRQ			21	/* reserved */
#define CM4_AUDIO_IRQ			22
#define CM4_HIF_DMA_IRQ			23
#define CM4_GPT_IRQ			    24
#define CM4_ADC_COMP_IRQ		25
#define CM4_AUDIO_WAKE_IRQ		26
//#define CM4_SPIM_0_IRQ			27
#define CM4_USB_IRQ			    28
#define CM4_UDMA_IRQ			29
#define CM4_TRNG_IRQ            30

#define CM4_WIC_INT_WAKE_UP_IRQ 31
#define CM4_WIC_0_IRQ           32 //PAD_ANTSEL2
#define CM4_WIC_1_IRQ           33 //PAD_WAKE_N
#define CM4_WIC_2_IRQ           34 //PAD_SDIO_CLK
#define CM4_WIC_3_IRQ           35 //PAD_SDIO_DAT3
#define CM4_WIC_4_IRQ           36 //PAD_SDIO_DAT0
#define CM4_WIC_5_IRQ           37 //PAD_GPIO0
#define CM4_WIC_6_IRQ           38 //PAD_GPIO1
#define CM4_WIC_7_IRQ           39 //PAD_UART_RX
#define CM4_WIC_8_IRQ           40 //PAD_WF_RF_DIS_B
#define CM4_WIC_9_IRQ           41 //PAD_BT_RF_DIS_B
#define CM4_WIC_10_IRQ          42 //PAD_WF_LED_B
#define CM4_WIC_11_IRQ          43 //PAD_BT_LED_B
#define CM4_WIC_12_IRQ          44 //PAD_IN_ADC4
#define CM4_WIC_13_IRQ          45 //PAD_IN_ADC5
#define CM4_WIC_14_IRQ          46 //PAD_IN_ADC6
#define CM4_WIC_15_IRQ          47 //PAD_IN_ADC7
#define CM4_WIC_16_IRQ          48 //PAD_IN_ADC8
#define CM4_WIC_17_IRQ          49 //PAD_IN_ADC9
#define CM4_WIC_18_IRQ          50 //PAD_IN_ADC10
#define CM4_WIC_19_IRQ          51 //PAD_IN_ADC11
#define CM4_WIC_20_IRQ          52 //PAD_IN_ADC12
#define CM4_WIC_21_IRQ          53 //PAD_IN_ADC13
#define CM4_WIC_22_IRQ          54 //PAD_IN_ADC14
#define CM4_WIC_23_IRQ          55 //PAD_IN_ADC15

#define CM4_EINT_0_IRQ          CM4_IRQ_GPIO_G0_0
#define CM4_EINT_1_IRQ          CM4_IRQ_GPIO_G0_1
#define CM4_EINT_2_IRQ          CM4_IRQ_GPIO_G0_2
#define CM4_EINT_3_IRQ          CM4_IRQ_GPIO_G0_3
#define CM4_EINT_4_IRQ          CM4_IRQ_GPIO_G1_0
#define CM4_EINT_5_IRQ          CM4_IRQ_GPIO_G1_1
#define CM4_EINT_6_IRQ          CM4_IRQ_GPIO_G1_2
#define CM4_EINT_7_IRQ          CM4_IRQ_GPIO_G1_3
#define CM4_EINT_8_IRQ          CM4_IRQ_GPIO_G2_0
#define CM4_EINT_9_IRQ          CM4_IRQ_GPIO_G2_1
#define CM4_EINT_10_IRQ         CM4_IRQ_GPIO_G2_2
#define CM4_EINT_11_IRQ         CM4_IRQ_GPIO_G2_3
#define CM4_EINT_12_IRQ         CM4_IRQ_GPIO_G3_0
#define CM4_EINT_13_IRQ         CM4_IRQ_GPIO_G3_1
#define CM4_EINT_14_IRQ         CM4_IRQ_GPIO_G3_2
#define CM4_EINT_15_IRQ         CM4_IRQ_GPIO_G3_3
#define CM4_EINT_16_IRQ         CM4_IRQ_GPIO_G3_0
#define CM4_EINT_17_IRQ         CM4_IRQ_GPIO_G3_1
#define CM4_EINT_18_IRQ         CM4_IRQ_GPIO_G3_2
#define CM4_EINT_19_IRQ         CM4_IRQ_GPIO_G3_3
#define CM4_EINT_20_IRQ         CM4_IRQ_GPIO_G3_0
#define CM4_EINT_21_IRQ         CM4_IRQ_GPIO_G3_1
#define CM4_EINT_22_IRQ         CM4_IRQ_GPIO_G3_2
#define CM4_EINT_23_IRQ         CM4_IRQ_GPIO_G3_3



#define CM4_EINT_24_IRQ         80 //PAD_IN_GPIO45
#define CM4_EINT_25_IRQ         81 //PAD_IN_GPIO46
#define CM4_EINT_26_IRQ         82 //PAD_IN_GPIO47
#define CM4_EINT_27_IRQ         83 //PAD_IN_GPIO48
#define CM4_EINT_28_IRQ         84 //PAD_IN_GPIO49
#define CM4_EINT_29_IRQ         85 //PAD_IN_GPIO50
#define CM4_EINT_30_IRQ         86 //PAD_IN_GPIO51
#define CM4_EINT_31_IRQ         87 //PAD_IN_GPIO52
#define CM4_EINT_32_IRQ         88 //PAD_IN_GPIO53
#define CM4_EINT_33_IRQ         89 //PAD_IN_GPIO54
#define CM4_EINT_34_IRQ         90 //PAD_IN_PWM2
#define CM4_EINT_35_IRQ         91 //PAD_IN_PWM3
#define CM4_EINT_36_IRQ         92 //PAD_IN_PWM4
#define CM4_EINT_37_IRQ         93 //PAD_IN_PWM5
#define CM4_EINT_38_IRQ         94 //PAD_IN_PWM6
#define CM4_EINT_39_IRQ         95 //PAD_IN_PWM7
#endif


#endif
