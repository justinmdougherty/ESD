#ifndef MC24AA512_H
#define	MC24AA512_H
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : mc24aa512.h
 *
 *  DESCRIPTION   : Declare driver functions for Microchip EEPROM 24AA512
 *
 *  NOTE: micro_defs.h must define MC24AA512_WRT_ADRS and MC24AA512_RD_ADRS
 *
 *      (1) bool ReadEepromToBfr(uint16_t address, uint8_t * bfr, uint16_t cnt)
 *      (2) bool WriteBfrToEeprom(uint16_t address, uint8_t * bfr, uint8_t cnt)
 *      (3) bool EraseEeprom(void)
 *
 *  NOTE: When doing a write (be it 1 byte, a full 128 bytes page, or some
 *    number in between) the data in the rest of the page is refreshed along
 *    with the data bytes being written.  This will force the entire page to
 *    endure a write cycle, for this reason endurance is specified per page.
 *    This should be good reason to never do single byte writes when needing to
 *    write multiple contiguous addresses (if speed wasn't good enough reason).
 *
 *  NOTE: Page write operations are limited to writing bytes within a single
 *    physical page, regardless of the number of bytes actually being written.
 *    Physical page boundaries start at addresses that are integer multiples of
 *    the page buffer size (128-bytes) and end at addresses that are 1 less than
 *    integer multiples of page size. If a Page Write command attempts to write
 *    across a physical page boundary, the result is that the data wraps around
 *    to the beginning of the current page (overwriting data previously stored
 *    there), instead of being written to the next page as might be expected. It
 *    is therefore necessary for the application software to prevent page write
 *    operations that would attempt to cross a page boundary.
 *
 *  WRITTEN BY    : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2017/08/21, Robert Kirby, NSWC H12
 *      Add EraseEeprom()
 *      define EEPROM_BYTES, EEPROM_PAGE_SIZE, EEPROM_PAGE_CNT
 *    2015/10/05, Robert Kirby, NSWC Z17
 *      Initial development
 *
 *  REFERENCE DOCUMENTS
 *    1.  24AA512 512K I2C Serial EEPROM Datasheet (Microchip DS21754M)
 */
//++PROCEDURES******************************************************************
//  bool ReadEepromToBfr(uint16_t address, uint8_t * bfr, uint16_t cnt)
//  Reads 'cnt' bytes of data into 'bfr' from EEPROM starting at 'address'
//
//  NOTE: if a read fails part-way through 'cnt', 'bfr' is partially modified
//
//  INPUT : uint16_t address - address within EEPROM from which to start read
//          uint8_t * bfr - pointer to destination of data bytes
//          uint16_t cnt - number of bytes to read
//  OUTPUT: bool - true if read completed successfully, otherwise false,
//          uint8_t * bfr - updated with data
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
//  INPUT : uint16_t address - address within EEPROM at which to start write
//          uint8_t * bfr - pointer to source of data bytes
//          uint8_t cnt - number of bytes to write, must not exceed 128
//  OUTPUT: bool - true if write completed successfully, otherwise false
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool EraseEeprom(void)
//  Writes 0xFF to all EEPROM addresses.
//
//  INPUT : NONE
//  OUTPUT: bool - true if erase completed successfully, otherwise false
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#define EEPROM_BYTES      65536
#define EEPROM_PAGE_SIZE  128
#define EEPROM_PAGE_CNT   (EEPROM_BYTES / EEPROM_PAGE_SIZE)

bool ReadEepromToBfr(uint16_t address, uint8_t * bfr, uint16_t cnt);
bool WriteBfrToEeprom(uint16_t address, uint8_t * bfr, uint8_t cnt);
bool EraseEeprom(void);

#endif  // MC24AA512_H
