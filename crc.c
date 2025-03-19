/*
 *  FILE NAME     : crc.c
 *    Modified/Simplified MPLAB® Code Configurator Generated CRC Driver File.
 *    It now does CRC on only 16-bit data space and 24-bit program space
 *    using 32-bit polynomial and returns 'raw' result ...but also has new
 *    turn-key CRC functions so you just have to make one function call.
 *
 *  DESCRIPTION   : Declare driver functions for PIC24FJ128GA204 CRC peripheral.
 *    To get the same answer for a fixed input buffer, every time need to:
 *    Initialize, SetSeed, Start, while !done PerformTask, GetResult ...OR...
 *    ...just call a Get____MemCrc function which does all of the above for you
 *
 *  NOTE - for data memory CRCs, an odd buffer address is an error !!
 *
 *      (1) void InitializeCrc(void)
 *      (2) void SetCrcSeed(uint32_t seed)
 *      (3) void StartCrcOnDataMemBfr(uint16_t *buffer, uint32_t sizeBytes)
 *      (4) void StartCrcOnPrgmMemBfr(uint32_t  startAddr, uint32_t sizeBytes)
 *      (5) void PerformCrcTask(void)
 *      (6) bool GetIsCrcCalcDone(void)
 *      (7) crc_t GetCrcCalcResult(void)
 *      (8) crc_t GetDataMemCrc(uint16_t *buffer, uint32_t sizeBytes)
 *      (9) crc_t GetPrgmMemCrc(uint32_t startAddr, uint32_t sizeBytes)
 *     (10) bool GetIsCrcInErr(void)
 *
 *  WRITTEN BY    : Microchip Technology Inc.
 *  MODIFICATIONS (in reverse chronological order)
 *    2020/04/06, Robert Kirby, NSWC H12
 *      Return CRC values as crc_t which is typedef of uint32_t.
 *    2017/09/08, Robert Kirby, NSWC H12
 *      Add protections against odd buffer address for data memory buffer
 *    2016/11/02, Robert Kirby, NSWC H12
 *      Initial re-development from Microchip's bloated Code Configurator
 *      Removed stuff I don't need, modify some stuff that doesn't fit my style
 */
/*
  Description:
    This source file provides implementations for driver APIs for CRC.
    Generation Information :
        Product Revision  :  MPLAB® Code Configurator - v2.25.2
        Device            :  PIC24FJ128GA204
        Driver Version    :  1.0.0
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.24
        MPLAB             :  MPLAB X v2.35 or v3.00

Copyright(c) 2013 - 2015 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 */
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "crc.h"


#define CRC_POLY_WIDTH    32      // This code only for 32-bit polynomial
#define CRCCON2_D16_P32   0x0F1F  // DWIDTH 0x0F (16 bits); PLEN 0x1F (32 bits)
#define CRCCON2_D32_P32   0x1F1F  // DWIDTH 0x1F (32 bits); PLEN 0x1F (32 bits)


//  CRC Calculation States
//    This enumeration defines the CRC calculation states.  The user
//    should not start a CRC calculation until the previous CRC calculation
//    has completed.
typedef enum
{                             // Relative order of enum values matters
  CRC_STATE_CALCULATE,
  CRC_STATE_FLUSH,
  CRC_STATE_CLEANUP,
  CRC_STATE_DONE,             // must be > all but CRC_STATE_ERR
  CRC_STATE_ERR,              // must be > CRC_STATE_DONE
} crcstate_t;

//  CRC Data Pointer
//    This union provides efficient way to access various types/sizes of data
typedef union
{
  uint16_t *data;
  uint32_t  program;
} crcdatptr_t;

//  CRC Calculation Object
//    This CRC calculation object contains the data needed to properly perform a
//    CRC calculation.
typedef struct
{
  crcdatptr_t ptr;
  uint32_t    remainingSize;
  crcstate_t  state;
  bool        program;
} crcsobj_t;

static crcsobj_t crcObj;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void InitializeCrc(void)
//    Initialize for 32-bit polynomial and 16-bit data starting with MSb
//
//    Polynomial:             0x04C11DB7
//    Data Shift:             Start with MSb
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void InitializeCrc(void)
{
  // Ensure peripheral is being clocked before writing its registers
  PMD3bits.CRCMD = 0;

  // Completely disable CRC so can start fresh if it was previously in use
  CRCCON2 = 0;
  CRCCON1 = 0;
  // CRCISEL disabled; CRCGO disabled; not LENDIAN Start with MSb;
  // VWORD 0; CSIDL disabled; CRCEN enabled;
  CRCCON1 = 0x8000;
  // DWIDTH for 16-bit data; PLEN for 32-bit polynomial
  CRCCON2  = CRCCON2_D16_P32;
  // Load 32-bit polynomial 0x04C11DB7
  CRCXORL = 0x1DB7;
  CRCXORH = 0x04C1;

  // Init crcObj as 32-bit polynomial and 16-bit data to match CRCCON2_D16_P32
  crcObj.remainingSize  = 0;
  crcObj.state          = CRC_STATE_DONE;
} // end InitializeCrc


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void SetCrcSeed(uint32_t seed)
//
//    Seed Shift:  Start with MSb (same as data)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void SetCrcSeed(uint32_t seed)
{
  uint16_t savedCRCCON2 = CRCCON2;

  // Run 32-bit seed through shift register using 32-bit polynomial
  CRCCON2 = CRCCON2_D32_P32;
  // Load 32-bit seed into the two 16-bit data registers (always CRCDATL first)
  CRCDATL = (uint16_t) seed;
  CRCDATH = (uint16_t) (seed >> 16);
  // Run the seed through the shift register
  IFS4bits.CRCIF = 0;
  CRCCON1bits.CRCGO = 1;
  while (IFS4bits.CRCIF == 0)   { ; }
  CRCCON1bits.CRCGO = 0;
  IFS4bits.CRCIF = 0;
  // restore CRCCON2
  CRCCON2  = savedCRCCON2;
} // end routine SetCrcSeed


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void StartCrcOnDataMemBfr(uint16_t *buffer, uint32_t sizeBytes)
//
//  NOTE - will set CRC_STATE_ERR if provided an odd buffer address
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void StartCrcOnDataMemBfr(uint16_t *buffer, uint32_t sizeBytes)
{
  if (0x0001 & (int)buffer)
  {                                     // Do NOT allow operations on odd adrs
    crcObj.state = CRC_STATE_ERR;       // it is an error that will completely
  }                                     // hang the peripheral and thus the code
  else
  {
    crcObj.remainingSize  = sizeBytes;
    crcObj.ptr.data       = buffer;
    crcObj.program        = false;
    crcObj.state          = CRC_STATE_CALCULATE;
    CRCCON2               = CRCCON2_D16_P32;
    CRCCON1bits.CRCGO     = 1;
  }
} // end routine StartCrcOnDataMemBfr


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void StartCrcOnPrgmMemBfr(uint32_t startAddr, uint32_t sizeBytes)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void StartCrcOnPrgmMemBfr(uint32_t startAddr, uint32_t sizeBytes)
{
  crcObj.remainingSize  = sizeBytes;
  crcObj.ptr.program    = startAddr;
  crcObj.program        = true;
  crcObj.state          = CRC_STATE_CALCULATE;
  CRCCON2               = CRCCON2_D32_P32;
  CRCCON1bits.CRCGO     = 1;
} // end routine StartCrcOnPrgmMemBfr


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  static bool CRC_16BitDataSpaceTask(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool CRC_16BitDataSpaceTask(void)
{
  if (CRCCON1bits.CRCFUL)           {
    return false;                   }

  CRCCON1bits.CRCGO = 0;
  IFS4bits.CRCIF = 0;

  while ((!CRCCON1bits.CRCFUL) && (crcObj.remainingSize))
  {
    CRCDATL = *crcObj.ptr.data++;
    crcObj.remainingSize -= 2;
  }

  CRCCON1bits.CRCGO = 1;

  return crcObj.remainingSize == 0;
} // end function CRC_16BitDataSpaceTask


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  static bool CRC_24BitPrgmSpaceTask(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool CRC_24BitPrgmSpaceTask(void)
{
  uint16_t size;
  uint16_t tempTbl;
  uint16_t lowWord;
  uint16_t highWord;

  if (CRCCON1bits.CRCFUL)           {
    return false;                   }

  size = 0xFFFF - (uint16_t) (crcObj.ptr.program & 0xFFFF);
  tempTbl = TBLPAG;

  CRCCON1bits.CRCGO = 0;
  IFS4bits.CRCIF = 0;

  TBLPAG = (uint16_t) (crcObj.ptr.program >> 16);

  while ((!CRCCON1bits.CRCFUL) && (crcObj.remainingSize) && (size))
  {
    lowWord = __builtin_tblrdl((uint16_t) (crcObj.ptr.program & 0xFFFF));
    crcObj.ptr.program++;
    highWord = __builtin_tblrdh((uint16_t) (crcObj.ptr.program & 0xFFFF));
    crcObj.ptr.program++;

    CRCDATL = lowWord;
    CRCDATH = highWord;

    crcObj.remainingSize -= 3;
    size -= 2;
  }

  TBLPAG = tempTbl;

  CRCCON1bits.CRCGO = 1;

  return crcObj.remainingSize == 0;
} // end function CRC_24BitPrgmSpaceTask


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  static bool CRC_FlushTask(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool CRC_FlushTask(void)
{
  if (IFS4bits.CRCIF)
  {
    CRCCON1bits.CRCGO = 0;
    IFS4bits.CRCIF = 0;
    CRCCON2 = CRCCON2_D32_P32;
    CRCDATL = 0;
    CRCDATH = 0;
    CRCCON1bits.CRCGO = 1;
    return true;
  }
  else
  {
    return false;
  }
} // end function CRC_FlushTask


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  static bool CRC_CleanUpTask(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool CRC_CleanUpTask(void)
{
  if (IFS4bits.CRCIF)
  {
    CRCCON2 = CRCCON2_D16_P32;
    CRCCON1bits.CRCGO = 0;
    IFS4bits.CRCIF = 0;
    return true;
  }
  else
  {
    return false;
  }
} // end function CRC_CleanUpTask


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void PerformCrcTask(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void PerformCrcTask(void)
{
  switch (crcObj.state)
  {
    case CRC_STATE_CALCULATE:
      if (crcObj.program)
      {
        if (CRC_24BitPrgmSpaceTask())       {
          crcObj.state = CRC_STATE_FLUSH;   }
      }
      else
      {
        if (CRC_16BitDataSpaceTask())       {
          crcObj.state = CRC_STATE_FLUSH;   }
      }
      break;

    case CRC_STATE_FLUSH:
      if (CRC_FlushTask())                  {
        crcObj.state = CRC_STATE_CLEANUP;   }
      break;

    case CRC_STATE_CLEANUP:
      if (CRC_CleanUpTask())                {
        crcObj.state = CRC_STATE_DONE;      }
      break;

    case CRC_STATE_DONE:
      break;

    case CRC_STATE_ERR:
    default:                            // disable CRC peripheral
      CRCCON1 = 0;
      CRCCON2 = 0;
      break;
  }
} // end routine PerformCrcTask


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool GetIsCrcCalcDone(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool GetIsCrcCalcDone(void)
{
  return (crcObj.state >= CRC_STATE_DONE);
} // end function GetIsCrcCalcDone


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  crc_t GetCrcCalcResult(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
crc_t GetCrcCalcResult(void)
{
  if (CRC_STATE_ERR == crcObj.state)  {
    return 0;                         }

  crc_t result;

  result   = (uint32_t) CRCWDATH;
  result <<= 16;
  result  |= (uint32_t) CRCWDATL;

  return result;
} // end function GetCrcCalcResult


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  crc_t GetDataMemCrc(uint16_t *buffer, uint32_t sizeBytes)
//  Performs a turn-key CRC calculation on buffer in data memory so you don't
//  have to implement all the individual function calls in your code
//  Not as flexible as the individual calls, e.g. no running CRC over multiple
//  buffers -- but hey, most times just want CRC for a particular input chunk.
//
//  INPUT:  uint16_t* buffer - address of source data on which to calculate CRC
//          uint32_t  sizeBytes - number of bytes over which to calculate CRC
//  OUTPUT: crc_t     CRC result
//  CALLS:  InitializeCrc
//          SetCrcSeed
//          StartCrcOnDataMemBfr
//          GetIsCrcCalcDone
//          PerformCrcTask
//          GetCrcCalcResult
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
crc_t GetDataMemCrc(uint16_t *buffer, uint32_t sizeBytes)
{
  InitializeCrc();                              // Initialize the CRC module
  SetCrcSeed(CRC_INITIALIZE_SEED);              // Set seed as 0x46AF6449
  StartCrcOnDataMemBfr(buffer, sizeBytes);      // Start the CRC
  while(GetIsCrcCalcDone() == false)        {   // While CRC calc not done
    PerformCrcTask();                       }   // perform the CRC task
  return GetCrcCalcResult();                    // Finally get the CRC value
} // end function GetDataMemCrc


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  crc_t GetPrgmMemCrc(uint32_t startAddr, uint32_t sizeBytes)
//  Performs a turn-key CRC calculation on buffer in program memory so you don't
//  have to implement all the individual function calls in your code
//  Not as flexible as the individual calls, e.g. no running CRC over multiple
//  buffers -- but hey, most times just want CRC for a particular input chunk.
//
//  INPUT:  uint32_t startAddr - address of source data on which to calc CRC
//          uint32_t sizeBytes - number of bytes over which to calculate CRC
//  OUTPUT: crc_t    CRC result
//  CALLS:  InitializeCrc
//          SetCrcSeed
//          StartCrcOnPrgmMemBfr
//          GetIsCrcCalcDone
//          PerformCrcTask
//          GetCrcCalcResult
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
crc_t GetPrgmMemCrc(uint32_t startAddr, uint32_t sizeBytes)
{
  InitializeCrc();                              // Initialize the CRC module
  SetCrcSeed(CRC_INITIALIZE_SEED);              // Set seed as 0x46AF6449
  StartCrcOnPrgmMemBfr(startAddr, sizeBytes);
  while(GetIsCrcCalcDone() == false)        {   // While CRC calc not done
    PerformCrcTask();                       }   // perform the CRC task
  return GetCrcCalcResult();                    // Finally get the CRC value
} // end function GetPrgmMemCrc


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool GetIsCrcInErr(void)
//
//  OUTPUT: returns 'true' if CRC peripheral in error and result not trustworthy
//          likely error is odd buffer address when call StartCrcOnDataMemBfr()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool GetIsCrcInErr(void)
{
  return (CRC_STATE_ERR == crcObj.state);
}
