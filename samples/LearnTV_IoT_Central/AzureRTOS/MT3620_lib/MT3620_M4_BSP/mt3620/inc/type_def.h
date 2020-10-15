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

/**
 * @file type_def.h
 *
 *  TYPE definition.
 *
 */

#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__

#include <stdint.h>
#include <stdbool.h>
//#include "bit_op.h"

typedef unsigned int size_t;
#ifdef __CC_ARM
typedef long int ssize_t;
#endif

#if 1 //for HW header file
typedef char                INT8;
typedef short               INT16;
typedef int                 INT32;

typedef long long           INT64;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned short      UINT16LE;
typedef unsigned int        UINT32;
typedef unsigned int        UINT32LE;
typedef unsigned long long  UINT64;
typedef unsigned char       UCHAR;
typedef unsigned short      USHORT;
typedef unsigned int        UINT;
typedef unsigned long       ULONG;

typedef signed char         CHAR;
typedef signed short        SHORT;
typedef signed int          INT;
typedef signed long         LONG;
typedef signed long long    LONGLONG;

typedef unsigned long long  ULONGLONG;

/* Void Type */
typedef void                VOID;
typedef void                *PVOID;

typedef CHAR               	*PCHAR;
typedef UCHAR               *PUCHAR;
typedef unsigned char       *PUINT8;
typedef unsigned short      *PUINT16;
typedef unsigned int        *PUINT32;
typedef unsigned long long  *PUINT64;
typedef int                 *PINT32;
typedef long long           *PINT64;

typedef UINT8               BYTE;
typedef UINT16              WORD;
typedef UINT32            DWORD;
#endif

#ifndef NULL
#define NULL        (0)
#endif

#ifndef TRUE
#define TRUE        (1)
#endif

#ifndef FALSE
#define FALSE       (0)
#endif

#define TEXT_IN_RAM __attribute__ ((__section__(".ramTEXT")))
#define BSS_IN_TCM __attribute__ ((__section__(".tcmBSS")))


/*
 * MTK HW register definitions
 */

#define u_intHW             uint32_t
#define SWAP_ENDIAN(Value)  SWAP_ENDIAN32(Value)

#define ADDR(Field)         ((u_intHW volatile *) (Field##_ADDR  ))
#define ADDR32(Field)       ((uint32_t *) (Field##_ADDR  ))
#define MASK(Field)         (Field##_MASK)
#define SHFT(Field)         (Field##_SHFT)

#define ADDR16(Field)       ((u_intHW volatile *) \
                             (Field##_ADDR + device_addr + ((Field##_SHFT) < 16 ? (0) : (2)) ) )
#define MASK16(Field) \
                            ((Field##_SHFT) < 16 ? (Field##_MASK) : ((Field##_MASK) >> 16) )
#define SHFT16(Field) \
	((Field##_SHFT) < 16 ? (Field##_SHFT) : ((Field##_SHFT) - 16 ) )

#define mGetHWEntry(Field) \
	( ( *ADDR(Field) & (MASK(Field)) ) >> SHFT(Field) )

#define mSetHWEntry(Field, Value) \
	{ u_intHW volatile *addr = ADDR(Field); \
		*addr = ((((u_intHW)(Value) << SHFT(Field)) \
		& MASK(Field)) | (*addr & ~MASK(Field))) ; }

#define mGetHWEntry32(Field) ( *ADDR(Field) )

#define mSetHWEntry32(Field, Source) ( *ADDR(Field) = Source )

#define mSetSWEntry(p_Data, Field, Value) \
	{ /*u_intHW volatile *addr = ADDR(Field)*/; \
		*p_Data = ((((u_intHW)(Value) << SHFT(Field)) \
		& MASK(Field)) | (*p_Data & ~MASK(Field))) ; }


#define writel(value, addr)						\
	do {								\
		*(volatile uint32_t *) (addr) = (uint32_t) (value);	\
	} while (0);

#define readl(addr)	( *((volatile uint32_t *) (addr)) )

#define DRV_WriteReg16(addr,data)     ((*(volatile uint16_t *)(addr)) = (uint16_t)(data))
#define DRV_Reg16(addr)               (*(volatile uint16_t *)(addr))
#define DRV_WriteReg32(addr,data)     ((*(volatile uint32_t *)(addr)) = (uint32_t)(data))
#define DRV_Reg32(addr)               (*(volatile uint32_t *)(addr))
#define DRV_WriteReg8(addr,data)     ((*(volatile uint8_t *)(addr)) = (uint8_t)(data))
#define DRV_Reg8(addr)               (*(volatile uint8_t *)(addr))

#define HAL_REG_32(reg)         (*((volatile unsigned int *)(reg)))
#define REG32(x)		        (*(volatile unsigned int *)(x))
#define REG16(x)                (*(volatile unsigned short *)(x))
#define REG8(x)                 (*(volatile unsigned char *)(x))

#ifndef BIT
#define BIT(n)                  ((uint32_t) 1 << (n))

/*
 * bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

/* Endian byte swapping codes */
#define SWAP16(x) \
    ((uint16_t) (\
	       (((uint16_t) (x) & (uint16_t) 0x00ffU) << 8) | \
	       (((uint16_t) (x) & (uint16_t) 0xff00U) >> 8)))

#define SWAP32(x) \
    ((uint32_t) (\
	       (((uint32_t) (x) & (uint32_t) 0x000000ffUL) << 24) | \
	       (((uint32_t) (x) & (uint32_t) 0x0000ff00UL) << 8) | \
	       (((uint32_t) (x) & (uint32_t) 0x00ff0000UL) >> 8) | \
	       (((uint32_t) (x) & (uint32_t) 0xff000000UL) >> 24)))

#define SWAP64(x) \
    ((uint64_t)( \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x00000000000000ffULL) << 56) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x000000000000ff00ULL) << 40) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x0000000000ff0000ULL) << 24) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x00000000ff000000ULL) <<  8) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x000000ff00000000ULL) >>  8) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x0000ff0000000000ULL) >> 24) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x00ff000000000000ULL) >> 40) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0xff00000000000000ULL) >> 56) ))

#define cpu2be64(x) SWAP64((x))
#define be2cpu64(x) SWAP64((x))
#define cpu2be32(x) SWAP32((x))
#define be2cpu32(x) SWAP32((x))
#define cpu2be16(x) SWAP16((x))
#define be2cpu16(x) SWAP16((x))
#define cpu2le64(x) ((uint64_t)(x))
#define le2cpu64(x) ((uint64_t)(x))
#define cpu2le32(x) ((uint32_t)(x))
#define le2cpu32(x) ((uint32_t)(x))
#define cpu2le16(x) ((uint16_t)(x))
#define le2cpu16(x) ((uint16_t)(x))


#define HEADER 1
#ifdef HEADER
#define INREG32(addr) DRV_Reg32(addr)

/* reg scan macro */
#define REG_FLD(width, shift) \
    ((uint32_t)((((width) & 0xFF) << 16) | ((shift) & 0xFF)))

#define REG_FLD_WIDTH(field) \
    ((uint32_t)(((field) >> 16) & 0xFF))

#define REG_FLD_SHIFT(field) \
    ((uint32_t)((field) & 0xFF))

#define REG_FLD_MASK(field) \
    (((uint32_t)((uint64_t)(1) << REG_FLD_WIDTH(field)) - 1) << REG_FLD_SHIFT(field))

#define REG_FLD_GET(field, reg32) \
    (((reg32) & REG_FLD_MASK(field)) >> REG_FLD_SHIFT(field))

#define REG_FLD_VAL(field, val) \
    (((val) << REG_FLD_SHIFT(field)) & REG_FLD_MASK(field))

#define REG_FLD_SET(field, reg32, val)                  \
    do {                                                \
    (reg32) = (((reg32) & ~REG_FLD_MASK(field)) |   \
                   REG_FLD_VAL((field), (val)));        \
    } while (0)


/**
*  REG_SET macro : set 32 bits register at one time
*  @ reg32: macro which get the register definition
*  usage example: REG_SET(INT_STATUS, regval32);
*/

#define REG_SET(reg32, val)   \
    do {          \
      (reg32) = (val);  \
    } while (0)

enum {
  RO = 0,
  RW,
  W1C,
  WO,
};

#define SET_FLD_VAL(field, val) ((REG_FLD_VAL(field, val)) >> (REG_FLD_SHIFT(field)))

#endif



#endif

