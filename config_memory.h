#ifndef CONFIG_MEMORY_H__
#define CONFIG_MEMORY_H__
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : config_memory.h
 *
 *  DESCRIPTION   : Declares interface to the memory map of device configurable
 *    operational/algorithmic parameters stored in non-volatile memory.
 *    Also provides methods to put/get gas gauge info to/from nvmem.
 *    To use this, ReadCfgFromNvMem() into cfgRam (access values using devCfg).
 *    If applicable, make configuration change to cfgRam (devCfg) and save them
 *    to persistent memory using WriteCfgToNvMem().  These two functions take
 *    care of checking/updating CRC and other non-devcfg_t values.
 *
 *      (1) void ReadCfgFromNvMem(void)
 *      (2) void WriteCfgToNvMem(void)
 *      (3) uint16_t GetCfgMemErr(void)
 *      (4) bool ReadGasGaugeFromNvMem(uint16_t * acr, uint16_t * pct)
 *      (5) void WriteGasGaugeToNvMem(uint16_t acr, uint16_t pct)
 *
 *    EXAMPLE USE:
 *
 *  WRITTEN BY    : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2020/04/06, Robert Kirby, NSWC H12
 *      Use new typedef crc_t and define CRC_SIZE
 *    2018/09/21, Robert Kirby, NSWC H12
 *      Refactor sysErrFlags to esdErrFlags (only comments in this file)
 *    2017/03/24, Robert Kirby, NSWC H12
 *      Initial development for ESD part of Lightning Bolt Hand-Held
 *
 *  REFERENCE DOCUMENTS
 *  1. ... Interface Control Document (NSWCDD Special Systems Branch Code Z17)
 *  2. MMPLAB XC16 C COMPILER USER'S GUIDE (Microchip Technology Inc. DS______)
 *  *. MIL-STD-12D Military Standard Abbreviations ...
 */
//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ReadCfgFromNvMem(void)
//  Reads device configuration from non-volatile memory into the globally
//  accessible 'devcfg_t' structure 'devCfg'.
//
//  INPUT : NONE
//  OUTPUT: NONE - but will set esdErrFlags.nvmem if any inconsistency detected
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void WriteCfgToNvMem(void)
//  Writes device configuration values held in the globally accessable structure
//  tagCfg into non-volatile memory.  It takes care of erasing NVMEM if needed
//  to retain its freshness.
//
//  INPUT : NONE
//  OUTPUT: NONE - but will set esdErrFlags.nvmem if any inconsistency detected
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  uint16_t GetCfgMemErr(void)
//  Returns the uint16_t representation of cfgerr_t configuration memory errors.
//  The return value is not valid before ReadCfgFromNvMem() has been called.
//
//  INPUT : NONE
//  OUTPUT: NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool ReadGasGaugeFromNvMem(uint16_t * acr, uint16_t * pct)
//  Reads battery gas gauge data from mapped NVMEM memory (with error detection)
//
//  INPUT : uint16_t * acr - ACR value for columb counter
//          uint16_t * pct - battery percent for display
//  OUTPUT: bool - true if successful read of previously saved data,
//                 otherwise false and sets the error flags
//                 esdErrFlags.gasGa [and esdErrFlags.nvmem] as appropriate
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void WriteGasGaugeToNvMem(uint16_t acr, uint16_t pct)
//  Write battery gas gauge data to mapped NVMEM memory (with error detection).
//
//  INPUT : uint16_t acr - the ACR value to save
//          uint16_t pct - the battery % value to save
//  OUTPUT: NONE (but may set esdErrFlags.gasGa and esdErrFlags.nvmem)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "crc.h"                // for crc_t
#include "main.h"               // for devcfg_t

//----- DEFINES, ENUMS, STRUCTS, TYPEDEFS, ETC. --------------------------------
typedef union tagCONFIGURATION_MEMORY_ERRRORS
{
  struct
  {
    uint16_t  eepAcs  :1;       // BIT0 - EEPROM access
    uint16_t  eepCrc  :1;       // BIT1 - EEPROM CRC mismatch
    uint16_t  eepMap  :1;       // BIT2 - EEPROM map version mismatch
    uint16_t  eepInit :1;       // BIT3 - EEPROM never initialized
  };
  uint16_t val;
}cfgerr_t;
#define FATAL_CFG_ERR   (BIT0)  // eepAcs

typedef union tagMAP_VERSION
{ // map layout version and map data-write version (EEPROM endurance 1,000,000)
  struct
  {
    uint32_t  L   :8;   // map layout version
    uint32_t  w   :24;  // count of writes to data in configuration memory map
  };
  uint8_t   u8[4];
  uint32_t  val;
} mapver_t;
#define MAP_VER_SIZE        sizeof(mapver_t)    // 4

typedef struct tagCONFIGURATION_MEMORY
{
  mapver_t  mapVer;                     // Memory Map Version
  devcfg_t  cfg;                        // Full device configuration parameters
  crc_t     crc;                        // CRC used to validate stored data
} cfgmem_t;
#define CFG_U8_SIZE     (sizeof(cfgmem_t))
#define CFG_U16_CNT     ((CFG_U8_SIZE+1) / 2)
#define CFG_CRC_SIZE    (sizeof(uint32_t))
#define CFG_CRC_OFFSET  (CFG_U8_SIZE - CRC_SIZE)

typedef union tagCONFIGURATION_BLOCK
{
  cfgmem_t      mbr;
  uint8_t       u8[CFG_U8_SIZE];
  uint16_t      u16[CFG_U16_CNT];
} cfgblock_t;


//----- EXPOSED ATTRIBUTES -----------------------------------------------------
extern  devcfg_t *const pDevCfg;        // can't change pointer, can change data
extern  cfgblock_t cfgRam;              // would rather this be static but...
#define devCfg     cfgRam.mbr.cfg       // way to access only what should be


//----- EXPOSED PROCEDURES -----------------------------------------------------
void      ReadCfgFromNvMem(void);
void      WriteCfgToNvMem(void);
uint16_t  GetCfgMemErr(void);
bool      ReadGasGaugeFromNvMem(uint16_t * acr, uint16_t * pct);
void      WriteGasGaugeToNvMem(uint16_t acr, uint16_t pct);


//----- MACROS -----------------------------------------------------------------
#define mIsCfgErrFatal()  (GetCfgMemErr() & FATAL_CFG_ERR)

#endif // CONFIG_MEMORY_H__
