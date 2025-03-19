////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : mc24aa512_i2c2.c
 *
 *  DESCRIPTION   : Define driver functions for Microchip EEPROM 24AA512. Write
 *    size is limited to size of EEPROM's Page Write Buffer (EEPROM_PAGE_SIZE).
 *
 *  NOTE: micro_defs.h must define MC24AA512_WRT_ADRS and MC24AA512_RD_ADRS
 *
 *      (1) bool ReadEepromToBfr(uint16_t address, uint8_t * bfr, uint16_t cnt)
 *      (2) bool WriteBfrToEeprom(uint16_t address, uint8_t * bfr, uint8_t cnt)
 *      (3) bool EraseEeprom(void)
 *
 *  WRITTEN BY    : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2017/08/21, Robert Kirby, NSWC H12
 *      Add EraseEeprom()
 *      use newly defined EEPROM_PAGE_SIZE and EEPROM_PAGE_CNT
 *    2017/01/10, Megan Kozub, NSWC V14
 *      Modified for I2C2: changed header reference, & calls to I2C2 (vice I2C1)
 *    2016/11/02, Robert Kirby, NSWC H12
 *      Initial development
 *
 *  REFERENCE DOCUMENTS
 *    1.  24AA512 512K I2C Serial EEPROM Datasheet (Microchip DS21754M)
 */
#include <xc.h>
#include <stddef.h>         // for NULL
#include <stdbool.h>
#include <stdint.h>
#include <string.h>         // for memset
#include "mc24aa512.h"      // for EEPROM_PAGE_SIZE
#include "i2c2.h"           // all the macros needed to run I2C in polled mode
#include "micro_defs.h"     // System funct/params, like osc/peripheral config
#include <libpic30.h>       // For delay functions MUST follow define of FCY


#define ISWA      (MC24AA512_WRT_ADRS)    // MC24AA512 I2C Slave Write Address
#define ISRA      (MC24AA512_RD_ADRS)     // MC24AA512 I2C Slave Read Address


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool ReadEepromToBfr(uint16_t address, uint8_t * bfr, uint16_t cnt)
//  Reads 'cnt' bytes of data into 'bfr' from EEPROM starting at 'address'
//
//  NOTE: if a read fails part-way through 'cnt', 'bfr' is partially modified
//
//  INPUT : uint16_t address - address within EEPROM from which to start read
//          uint8_t * bfr - pointer to destination of data bytes
//          int8_t cnt - number of bytes to read
//  OUTPUT: bool - true if read completed successfully, otherwise false,
//          uint8_t * bfr - updated with data
//  CALLS : a plethora of i2c2 functions declared in "i2c2.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool ReadEepromToBfr(uint16_t address, uint8_t * bfr, uint16_t cnt)
{
  uint8_t adrsHB = (uint8_t) ((address & 0xFF00) >> 8);
  uint8_t adrsLB = (uint8_t) (address & 0x00FF);
  uint8_t sanity;

  if (0 == cnt)             {return false;}
  if (NULL == bfr)          {return false;}
  if (!WaitForIdleI2c2())   {return false;}

  // NOTE: may need to wait for ongoing write, per spec completes in <=5ms,
  // allow 6.5ms (20 sanity loops with 325us delays --> 20 * 325us = 6.5ms)
  sanity  = 20;
  do
  {
    if (!SendStartI2c2())   {return false;}
    if (!WriteI2c2(ISWA))   {return false;} // EEPROM slave adrs, R/Wn as WRITE
    if (I2C2STATbits.ACKSTAT)       {       // allow time for EEPROM to finish
      __delay_us(325);              }       // any in-process WRITE operation
  } while (I2C2STATbits.ACKSTAT && sanity--);

  if (I2C2STATbits.ACKSTAT) {return false;} // Check EEPROM ACKed its address
  if (!WriteI2c2(adrsHB))   {return false;} // Tell EEPROM the internal address
  if (!WriteI2c2(adrsLB))   {return false;} // at which to start reading
  if (!SendRestartI2c2())   {return false;}
  if (!WriteI2c2(ISRA))     {return false;} // EEPROM slave adrs, R/Wn as READ
  if (!ReadI2c2(bfr))       {return false;} // Read first byte out
  for (cnt--; cnt; cnt--)                   // Decrement & check cnt each read
  {
    bfr++;
    if (!SendAckI2c2())     {return false;}
    if (!ReadI2c2(bfr))     {return false;}
  }

  if (!SendNackI2c2())      {return false;}
  if (!SendStopI2c2())      {return false;}

  return true;
} // end function ReadEepromToBfr


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool WriteBfrToEeprom(uint16_t address, uint8_t * bfr, uint8_t cnt)
//  Writes 'cnt' bytes of data from 'bfr' to EEPROM starting at 'address'.
//  Write size is limited to the size of EEPROM's 128-Byte Page Write Buffer.
//
//  NOTE - limited to writing bytes within a single physical page, regardless of
//    the number of bytes actually being written. Physical page boundaries start
//    at addresses that are integer multiples of the 128 bytes page size and end
//    at addresses that are 1 less than integer multiples of page size. Attempts
//    to write across a physical page boundary, the result is that the data
//    wraps around to the beginning of the current page (overwriting data
//    previously stored there), instead of being written to the next page as
//    might be expected. It is therefore necessary for the calling software to
//    prevent write operations that would attempt to cross a page boundary (or
//    use this behavior to its own advantage).
//
//  NOTE: Despite what the 24AA512 data sheet says, it seems the EEPROM does NOT
//    ACK each data byte that is sent as part of the block write.
//
//  INPUT : uint16_t address - address within EEPROM at which to start write
//          uint8_t * bfr - pointer to source of data bytes
//          uint8_t cnt - number of bytes to write, must not exceed 128
//  OUTPUT: bool - true if write completed successfully, otherwise false
//  CALLS : a plethora of i2c2 functions declared in "i2c2.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool WriteBfrToEeprom(uint16_t address, uint8_t * bfr, uint8_t cnt)
{
  #define CNT_INVALID ((0 == cnt) || (EEPROM_PAGE_SIZE < cnt))

  uint8_t adrsHB = (uint8_t) ((address & 0xFF00) >> 8);
  uint8_t adrsLB = (uint8_t) (address & 0x00FF);
  uint8_t sanity;

  if (NULL == bfr)              {return false;}
  if (CNT_INVALID)              {return false;}
  if (!WaitForIdleI2c2())       {return false;}

  // NOTE: may need to wait for ongoing write, per spec completes in <=5ms,
  // allow 6.5ms (20 sanity loops with 325us delays --> 20 * 325us = 6.5ms)
  sanity  = 20;
  do
  {
    if (!SendStartI2c2())       {return false;}
    if (!WriteI2c2(ISWA))       {return false;} // EEPROM adrs, R/Wn as WRITE
    if (I2C2STATbits.ACKSTAT)   {               // allow EEPROM time to finish
      __delay_us(325);          }               // any in-process operation
  } while (I2C2STATbits.ACKSTAT && sanity--);

  if (I2C2STATbits.ACKSTAT)     {return false;} // Check EEPROM ACKed its adrs
  if (!WriteI2c2(adrsHB))       {return false;} // Tell EEPROM the internal adrs
  if (!WriteI2c2(adrsLB))       {return false;} // at which to start writing
  for ( ; cnt; cnt--)                           // check/decrement cnt each wrt
  {
    if (!WriteI2c2(*bfr))       {return false;} // Write data byte to EEPROM
  //if (!I2C2STATbits.ACKSTAT)  {return false;} // Check that EEPROM accepted it
    bfr++;                                      // Update pointer for next use
  }

  if (!SendStopI2c2())          {return false;}

  return true;
} // end function WriteBfrToEeprom


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool EraseEeprom(void)
//  Writes 0xFF to all EEPROM addresses.
//
//  INPUT : NONE
//  OUTPUT: bool - true if erase completed successfully, otherwise false
//  CALLS : memset
//          WriteBfrToEeprom
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool EraseEeprom(void)
{
  uint8_t   bfr[EEPROM_PAGE_SIZE];
  uint16_t  i;
  uint16_t  adrs = 0;
  bool      isOk = true;

  memset((void*) bfr, 0xFF, EEPROM_PAGE_SIZE);
  for (i = EEPROM_PAGE_CNT; i; i--)
  {
    isOk &= WriteBfrToEeprom(adrs, bfr, EEPROM_PAGE_SIZE);
    adrs += EEPROM_PAGE_SIZE;
  }
  return isOk;
} // end function EraseEeprom


#undef ISWA
#undef ISRA
