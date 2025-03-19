#ifndef STDINT_EXTENDED_H
#define	STDINT_EXTENDED_H
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : stdint_extended.h
 *
 *  DESCRIPTION   : Defines, typedefs, and unions to extend stdint.h
 *    like C18's GenericTypeDefs.h and Texas Instruments Code Composer Studio
 *
 *  WRITTEN BY    : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/08/16, Robert Kirby, NSWC H12
 *      Extend TI-like define BITx through base32hex triacontakaidecimal
 *    2016/11/02, Robert Kirby, NSWC H12
 *      Removed the unnecessary __pack directives that XC16 didn't like
 */
#include <xc.h>
#include <stdint.h>

// Define standard bits like TI does, extended to base32hex triacontakaidecimal
#define BIT0                   (0x0001)
#define BIT1                   (0x0002)
#define BIT2                   (0x0004)
#define BIT3                   (0x0008)
#define BIT4                   (0x0010)
#define BIT5                   (0x0020)
#define BIT6                   (0x0040)
#define BIT7                   (0x0080)
#define BIT8                   (0x0100)
#define BIT9                   (0x0200)
#define BITA                   (0x0400)
#define BITB                   (0x0800)
#define BITC                   (0x1000)
#define BITD                   (0x2000)
#define BITE                   (0x4000)
#define BITF                   (0x8000)
#define BITG               (0x00010000)
#define BITH               (0x00020000)
#define BITI               (0x00040000)
#define BITJ               (0x00080000)
#define BITK               (0x00100000)
#define BITL               (0x00200000)
#define BITM               (0x00400000)
#define BITN               (0x00800000)
#define BITO               (0x01000000)
#define BITP               (0x02000000)
#define BITQ               (0x04000000)
#define BITR               (0x08000000)
#define BITS               (0x10000000)
#define BITT               (0x20000000)
#define BITU               (0x40000000)
#define BITV               (0x80000000)

// typedef other commonly used integer type references
typedef unsigned char           BYTE;           // 8-bit unsigned
typedef unsigned short int      WORD;           // 16-bit unsigned
typedef unsigned long int       DWORD;          // 32-bit unsigned
typedef unsigned long long int  QWORD;          // 64-bit unsigned

// Emulate bitfield unions like Microchip C18 GenericTypeDefs.h defines
typedef enum _BIT   { CLEAR = 0, SET  } BIT;    // size of enum ??

typedef union
{
  uint8_t val;
  struct
  {
    uint8_t b0:1;
    uint8_t b1:1;
    uint8_t b2:1;
    uint8_t b3:1;
    uint8_t b4:1;
    uint8_t b5:1;
    uint8_t b6:1;
    uint8_t b7:1;
  };
} uint8val_t, uint8bits_t;

typedef union
{
  uint16_t val;
  uint8_t v[2];
  struct
  {
    uint8_t LB;
    uint8_t HB;
  };
  struct
  {
    uint8_t b0:1;
    uint8_t b1:1;
    uint8_t b2:1;
    uint8_t b3:1;
    uint8_t b4:1;
    uint8_t b5:1;
    uint8_t b6:1;
    uint8_t b7:1;
    uint8_t b8:1;
    uint8_t b9:1;
    uint8_t b10:1;
    uint8_t b11:1;
    uint8_t b12:1;
    uint8_t b13:1;
    uint8_t b14:1;
    uint8_t b15:1;
  };
} uint16val_t, uint16bits_t, uint16byte_t;

typedef union
{
  uint32_t val;
  uint16_t w[2];
  uint8_t  v[4];
  struct
  {
    uint16val_t low;
    uint16val_t high;
  };
  struct
  {
    uint16_t LW;  // Low word
    uint16_t HW;  // High word
  };
  struct
  {
    uint8_t LB;   // Least significant byte
    uint8_t HB;   // High byte
    uint8_t UB;   // Upper byte
    uint8_t MB;   // Most significant byte
  };
  struct
  {
    uint8_t b0:1;
    uint8_t b1:1;
    uint8_t b2:1;
    uint8_t b3:1;
    uint8_t b4:1;
    uint8_t b5:1;
    uint8_t b6:1;
    uint8_t b7:1;
    uint8_t b8:1;
    uint8_t b9:1;
    uint8_t b10:1;
    uint8_t b11:1;
    uint8_t b12:1;
    uint8_t b13:1;
    uint8_t b14:1;
    uint8_t b15:1;
    uint8_t b16:1;
    uint8_t b17:1;
    uint8_t b18:1;
    uint8_t b19:1;
    uint8_t b20:1;
    uint8_t b21:1;
    uint8_t b22:1;
    uint8_t b23:1;
    uint8_t b24:1;
    uint8_t b25:1;
    uint8_t b26:1;
    uint8_t b27:1;
    uint8_t b28:1;
    uint8_t b29:1;
    uint8_t b30:1;
    uint8_t b31:1;
  };
} uint32val_t, uint32bits_t, uint32byte_t;

typedef union
{
  uint64_t val;
  uint32_t d[2];
  uint16_t w[4];
  uint8_t  v[8];
  struct
  {
    uint32_t LD;
    uint32_t HD;
  };
  struct
  {
    uint16_t LW;
    uint16_t HW;
    uint16_t UW;
    uint16_t MW;
  };
  struct
  {
    uint8_t b0:1;
    uint8_t b1:1;
    uint8_t b2:1;
    uint8_t b3:1;
    uint8_t b4:1;
    uint8_t b5:1;
    uint8_t b6:1;
    uint8_t b7:1;
    uint8_t b8:1;
    uint8_t b9:1;
    uint8_t b10:1;
    uint8_t b11:1;
    uint8_t b12:1;
    uint8_t b13:1;
    uint8_t b14:1;
    uint8_t b15:1;
    uint8_t b16:1;
    uint8_t b17:1;
    uint8_t b18:1;
    uint8_t b19:1;
    uint8_t b20:1;
    uint8_t b21:1;
    uint8_t b22:1;
    uint8_t b23:1;
    uint8_t b24:1;
    uint8_t b25:1;
    uint8_t b26:1;
    uint8_t b27:1;
    uint8_t b28:1;
    uint8_t b29:1;
    uint8_t b30:1;
    uint8_t b31:1;
    uint8_t b32:1;
    uint8_t b33:1;
    uint8_t b34:1;
    uint8_t b35:1;
    uint8_t b36:1;
    uint8_t b37:1;
    uint8_t b38:1;
    uint8_t b39:1;
    uint8_t b40:1;
    uint8_t b41:1;
    uint8_t b42:1;
    uint8_t b43:1;
    uint8_t b44:1;
    uint8_t b45:1;
    uint8_t b46:1;
    uint8_t b47:1;
    uint8_t b48:1;
    uint8_t b49:1;
    uint8_t b50:1;
    uint8_t b51:1;
    uint8_t b52:1;
    uint8_t b53:1;
    uint8_t b54:1;
    uint8_t b55:1;
    uint8_t b56:1;
    uint8_t b57:1;
    uint8_t b58:1;
    uint8_t b59:1;
    uint8_t b60:1;
    uint8_t b61:1;
    uint8_t b62:1;
    uint8_t b63:1;
  };
} uint64val_t, uint64bits_t, uint64byte_t;


#endif	// STDINT_EXTENDED_H

