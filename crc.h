#ifndef _CRC_H
#define _CRC_H
/*
 *  FILE NAME     : crc.h
 *    Modified MPLAB® Code Configurator Generated CRC Driver File.
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
 *      (4) void StartCrcOnPrgmMemBfr(uint32_t startAddr, uint32_t sizeBytes)
 *      (5) void PerformCrcTask(void)
 *      (6) bool GetIsCrcCalcDone(void)
 *      (7) crc_t GetCrcCalcResult(void)
 *      (8) crc_t GetDataMemCrc(uint16_t *buffer, uint32_t sizeBytes)
 *      (9) crc_t GetPrgmMemCrc(uint32_t startAddr, uint32_t sizeBytes)
 *     (10) bool GetIsCrcInErr(void)
 *
 *  Example 1
 *    uint16_t buffer[] = {0x0000,0x1111,0x3333,0x5555,0x7777,0xBBBB,0xDDDD};
 *    crc_t    bufferCRC;
 *
 *    bufferCrc = GetDataMemCrc(buffer, sizeof(buffer));
 *    if ((!bufferCrc) && GetIsCrcInErr()) { DoSomething();  }
 *
 *  Example 2 (will yield the same results as example 1)
 *    uint16_t buffer[] = {0x0000,0x1111,0x3333,0x5555,0x7777,0xBBBB,0xDDDD};
 *    crc_t    bufferCRC;
 *
 *    InitializeCrc();                              // Initialize the CRC module
 *    SetCrcSeed(CRC_INITIALIZE_SEED);              // Set seed as 0x46AF6449
 *    StartCrcOnDataMemBfr(buffer, sizeof(buffer)); // Start the CRC
 *    while(GetIsCrcCalcDone() == false)        {   // While CRC calc not done
 *        PerformCrcTask();                     }   // perform the CRC task
 *    bufferCRC = GetCrcCalcResult();               // Finally get the CRC value
 *    if ((!bufferCrc) && GetIsCrcInErr()) { DoSomething(); }
 *
 *  WRITTEN BY    : Microchip Technology Inc.
 *  MODIFICATIONS (in reverse chronological order)
 *    2020/04/06, Robert Kirby, NSWC H12
 *      Typedef crc_t and define CRC_SIZE for use by consumers.
 *    2017/09/08, Robert Kirby, NSWC H12
 *      Add protections against odd buffer address for data memory buffer
 *    2016/11/03, Robert Kirby, NSWC H12
 *      Initial re-development
 *      Removed stuff I don't need, modify some stuff that doesn't fit my style
 */
/*
Description:
    Generation Information :
        Product Revision  :  MPLAB® Code Configurator - v2.25.2
        Device            :  PIC24FJ128GA204
        Driver Version    :  1.0.0

--------------------------------------------------------------------------------
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

//--PROCEDURE-------------------------------------------------------------------
//  void InitializeCrc(void)
//    Initializes the CRC module
//    This routine sets the polynomial and data width; data and seed shift;
//    updates the polynomial and shifts the seed value.
//
//    After the routine is called, the CRC module is ready to calculate the
//    CRC of a data buffer.
//
//    Polynomial:             0x4C11DB7
//    Polynomial Length - 1:  0x1F
//    Data Length - 1:        0xF
//    Data Shift:             Start with MSb
//    Seed:                   0x46AF6449
//    Seed Shift:             Start with MSb
//
//  Returns
//    None.
//
//  Param
//    None.
//------------------------------------------------------------------------------
//  void SetCrcSeed(uint32_t seed)
//    Sets the CRC seed
//    This routine sets CRC seed with data direction of current seed direction.
//
//  Preconditions
//    The CRC module needs to be initialized with the desired settings.  Please
//    refer to the CRC initialization routines.
//
//  Returns
//    None.
//
//  Param
//    seed - the seed value of the CRC calculation.
//------------------------------------------------------------------------------
//  void StartCrcOnDataMemBfr(uint16_t *buffer, uint32_t sizeBytes)
//    Starts the CRC calculation
//    This routine starts the CRC calculation on a buffer in data space.
//
//  NOTE - will set CRC_STATE_ERR if provided an odd buffer address
//
//  Preconditions
//    The CRC module needs to be initialized with the desired settings.  Please
//    refer to the CRC initialization routines.
//
//  Returns
//    None.
//
//  Param
//    buffer - a pointer to desired data space buffre for the CRC calculation.
//    It is the caller's duty to ensure that the data starts on the proper
//    boundary.
//
//  Param
//    sizeBytes - the size, in bytes, of the buffer
//------------------------------------------------------------------------------
//  void StartCrcOnPrgmMemBfr(uint32_t startAddr, uint32_t sizeBytes)
//    Starts the CRC calculation
//    This routine starts the CRC calculation on a buffer in program space.
//
//  Preconditions
//    The CRC module needs to be initialized with the desired settings.  Please
//    refer to the CRC initialization routines.
//
//  Returns
//    None.
//
//  Param
//    startAddr - starting program space address.  Each program instruction is
//    3 bytes, the caller should account for that in the size.
//
//  Param
//    sizeBytes - size, in bytes, of the program space.  It is a multiple of 3.
//------------------------------------------------------------------------------
//  void PerformCrcTask(void)
//    performs the CRC calculation
//    This routine cycles through the CRC calculations.  This routine will load
//    the CRC module FIFO with the buffer data.
//
//  Preconditions
//    The CRC module needs to be initialized with the desired settings.  Please
//    refer to the CRC initialization routines.
//
//    The caller needs to call the routine to start the calculation.
//
//  Returns
//    None.
//
//  Param
//    None.
//------------------------------------------------------------------------------
//  bool GetIsCrcCalcDone(void)
//    Checks to see if the CRC calculation has completed.
//    Use this routine to determine if the CRC calculation has been completed.
//    Only after CRC calculation has been completed should the caller get the
//    CRC result.
//
//  Preconditions
//    The CRC module needs to be initialized with the desired settings.  Please
//    refer to the CRC initialization routines.
//
//  Returns
//    true if CRC calculation has completed and the result is ready to retrieve,
//    otherwise false.
//
//  Param
//    None.
//------------------------------------------------------------------------------
//  crc_t GetCrcCalcResult(void)
//    Gets the CRC result.
//    This routine returns the CRC result from the module.  If this routine is
//    called before the routine, GetIsCrcCalcDone, has returned true, the result
//    will be incorrect.  This routine will not preform any modification to the
//    result read from the CRC SFR.
//
//  Preconditions
//    The CRC module needs to be initialized with the desired settings.  Please
//    refer to the CRC initialization routines.
//
//    The routine GetIsCrcCalcDone has returned true.
//
//  Returns
//    If CRC module in error returns 0, otherwise the CRC result for the module.
//    Note that 0 is a valid result, so when 0 returned call GetIsCrcInErr()
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
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool GetIsCrcInErr(void)
//
//  OUTPUT: returns 'true' if CRC peripheral in error and result not trustworthy
//          likely error is odd buffer address when call StartCrcOnDataMemBfr()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>


//----- DEFINES, ENUMS, STRUCTS, TYPEDEFS, ETC. --------------------------------
typedef uint32_t crc_t;                           // CRC values are 32-bits
#define CRC_INITIALIZE_SEED    0x46AF6449         // 32-bit seed
#define CRC_SIZE              (sizeof(crc_t))     // CRC values are 32-bits


//----- EXPOSED PROCEDURES -----------------------------------------------------
void InitializeCrc(void);
void SetCrcSeed(uint32_t seed);
void StartCrcOnDataMemBfr(uint16_t *buffer, uint32_t sizeBytes);
void StartCrcOnPrgmMemBfr(uint32_t startAddr, uint32_t sizeBytes);
void PerformCrcTask(void);
bool GetIsCrcCalcDone(void);
crc_t GetCrcCalcResult(void);
crc_t GetDataMemCrc(uint16_t *buffer, uint32_t sizeBytes);
crc_t GetPrgmMemCrc(uint32_t startAddr, uint32_t sizeBytes);
bool GetIsCrcInErr(void);

//----- MACROS -----------------------------------------------------------------
#define CloseCrcPrphl()   {PMD3bits.CRCMD = 1;}


#endif  // _CRC_H