////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : config_memory.c
 *
 *  DESCRIPTION   : Define module that writes the configuration parameters to
 *    non-volatile memory. CRCs are used to protect against corrupt data blocks.
 *    Also provides methods to put/get gas gauge info to/from nvmem.
 *
 *      (*) static void ReadFromEepMem(void)
 *      (*) static void WriteToEepMem(void)
 *      (1) void ReadCfgFromNvMem(void)
 *      (2) void WriteCfgToNvMem(void)
 *      (3) uint16_t GetCfgMemErr(void)
 *      (4) bool ReadGasGaugeFromNvMem(uint16_t * acr, uint16_t * pct)
 *      (5) void WriteGasGaugeToNvMem(uint16_t acr, uint16_t pct)
 *
 *  NOTE - multi-byte data in the memory map is Little Endian (LSB,[...,]MSB)
 *
 *  WRITTEN BY    : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/07/15, Robert Kirby, NSWC H12
 *      Remove various vestigial includes
 *    2021/06/01, Robert Kirby, NSWC H12
 *      Increment mapVer and update CFGPGM as devcfg_t changes ripple through
 *    2020/08/06, Robert Kirby, NSWC H12
 *      Mod CFGPGM back-light default BS_HIGH to see back light upon uC flash
 *    2020/04/06, Robert Kirby, NSWC H12
 *      Use new typedef crc_t and define CRC_SIZE
 *    2019/08/28, Robert Kirby, NSWC H12
 *      Mod CFGPGM back to default waveform 1 since not all BOLTs are multi-wf
 *    2018/12/17, Robert Kirby, NSWC H12
 *      Remove FOG conditional compile but leave default cfg as waveform slot 2
 *    2018/11/13, Robert Kirby, NSWC H12
 *      Conditional compile to expediently support FOG in BOLT waveform slot 2
 *    2018/09/21, Robert Kirby, NSWC H12
 *      Refactor sysErrFlags to esdErrFlags as it doesn't cover Ltng errors
 *    2017/03/05, Robert Kirby, NSWC H12
 *      Initial development for ESD part of Lightning Bolt Hand-Held
 *
 *  REFERENCE DOCUMENTS
 *    1.  BOLT Software Interface (End User) ver 1.5.pdf
 */
#include <xc.h>
#include <string.h>             // for strcmp, memcpy, etc.
#include <stdbool.h>
#include <stdint.h>
#include "stdint_extended.h"    // for uint64byte_t, BIT0, etc.
#include "config_memory.h"
#include "crc.h"                // for crc_t
#include "main.h"               // for esdErrFlags, devcfg_t, FKLB, FKHB
#include "mc24aa512.h"          // to access EEPROM

// Address mapping below based on using 128-byte EEPROM pages
#define EEP_CFG_ADRS        (0x0000)      // Where config data is in EEPROM
#define EEP_CFG_END_ADRS    (EEP_CFG_ADRS + CFG_U8_SIZE - 1)

#define EEP_GAS_ADRS        (0x0080)      // Where gas gauge data is in EEPROM
#define GAS_EEP_U8_SIZE     (sizeof(uint64byte_t))  // 2 * 2-bytes w/XOR check
#define EEP_GAS_END_ADRS    (EEP_GAS_ADRS + GAS_EEP_U8_SIZE - 1)

#define EEP_RH_ADRS         (0x0100)      // Where humidity data is in EEPROM
#define RH_EEP_U8_SIZE      (sizeof(uint16byte_t))  // 1-byte w/XOR check
#define EEP_RH_END_ADRS     (EEP_RH_ADRS + RH_EEP_U8_SIZE - 1)

#define EEP_MSG_ADRS        (0x1000)      // Where infilled msgs are in EEPROM
#define MSG_EEP_U8_SIZE     (0x4000)      // 16384 = 128 msgs, 128 bytes each
#define EEP_MSG_END_ADRS    (EEP_MSG_ADRS + MSG_EEP_U8_SIZE - 1)

#define EEP_BRV_ADRS        (0x5000)      // where exfilled codes are in EEPROM
#define BRV_EEP_U8_SIZE     (0x1000)      // 4096 = 128 codes, 32 bytes each
#define EEP_BRV_END_ADRS    (EEP_BRV_ADRS + BRV_EEP_U8_SIZE - 1)


//----- MODULE ATTRIBUTES ------------------------------------------------------
static  cfgerr_t    cfgErr = {.val = 0xFFFF};
static  cfgblock_t  CFGPGM =
{ .u8 = {                           // 38 bytes of 128-byte page
  0x02,0x00,0x00,0x00,              // mapVer 0x02/write 0x000000
  FKLB,FKHB,                        // uint16_t    fwKey
  0x01,0x00,                        // opstat_t    opStat                  OS_ON
  0x30,0x30,0x31,0x00,              // char[4]     brevCode                  001
  0x00,0x00,                        // csysset_t   cSysSet                CS_DMS
  0x03,0x00,                        // bkltset_t   bkltSet               BS_HIGH
  0x30,0x00,                        // extcom_t    extCom          rsvd  EC_NONE
  0x33,0x00,                        // rxdtycy_t   rxDtyCy         rsvd  RA_NONE
  0x30,0x00,                        // rspsack_t   respAuto911Ack  rsvd  RDC_ON
  0x30,0x00,                        // rspsack_t   respMan911Ack   rsvd  RDC_ON
  0x31,0x00,                        // ws_t        selWfChar            WF_SLOT1
  0x01,0x00,                        // uint16_t    selWfIdx    (selWfChar - '0')
  0xFF,0xFF,                        // wgm_t       geoMuting              WGM_NA
  0x32,0x00,                        // txpwr_t     txPwr                 TP_HIGH
  0x31,0x00,                        // txdtycy_t   txDtyCy         rsvd TDC_NORM
  0x23,0x30,0x7B,0xED}              // uint32_t    crc             {LSB,...,MSB}
};
/*
{ .u8 = {                                   // 34 bytes of 128-byte page
  0x02,0x00,0x00,0x00,                      // mapVer 0x01/write 0x000000
  FKLB,FKHB,                                // uint16_t    fwKey
  0x01,0x00,                                // opstat_t    opStat          OS_ON
  0x30,0x30,0x31,0x00,                      // char[4]     brevCode          001
  0x00,0x00,                                // csysset_t   cSysSet        CS_DMS
  0x03,0x00,                                // bkltset_t   bkltSet       BS_HIGH
  0x30,0x00,                                // extcom_t    extCom           rsvd
  0x31,0x00,                                // waveform_t  waveform     WF_SLOT1
  0x32,0x00,                                // txpwr_t     txPwr         TP_HIGH
  0x31,0x00,                                // txdtycy_t   txDtyCy          rsvd
  0x33,0x00,                                // rxdtycy_t   rxDtyCy          rsvd
  0x30,0x00,                                // rspsack_t   respAuto911Ack   rsvd
  0x30,0x00,                                // rspsack_t   respMan911Ack    rsvd
  0x23,0x30,0x7B,0xED}                      // uint32_t    crc {LSB,...,MSB}
};
*/

//----- EXPOSED VARIABLE DECLARATIONS ------------------------------------------
cfgblock_t      cfgRam;
devcfg_t *const pDevCfg = (devcfg_t*)(&cfgRam.mbr.cfg);  // can't change ptr


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ReadFromEepMem(void)
//  void WriteToEepMem(void)
//  Reads & write device configuration parameters from & to EEPROM memory.
//  Write should never be done before read and read should never be done before
//  ReadFromPgmMem() has been called.
//
//  INPUT : NONE
//  OUTPUT: NONE (but updates cfgErr)
//  CALLS : ReadEepromToBfr / WriteBfrToEeprom
//          GetDataMemCrc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void ReadFromEepMem(void)
{
  if (!ReadEepromToBfr(EEP_CFG_ADRS, cfgRam.u8, CFG_U8_SIZE))
  {
    cfgErr.eepAcs = 1;
    return;
  }

  crc_t crc;
  // make sure the CRC matches before considering data valid
  crc = GetDataMemCrc(cfgRam.u16, CFG_CRC_OFFSET);
  if (crc != cfgRam.mbr.crc)                {
    cfgErr.eepCrc = 1;                      }
  else                                      {
    cfgErr.eepCrc = 0;                      }

  if (CFGPGM.mbr.mapVer.L !=
      cfgRam.mbr.mapVer.L)                  {
    cfgErr.eepMap = 1;                      }
  else                                      {
    cfgErr.eepMap = 0;                      }

  if ((0xFFFFFFFF == cfgRam.mbr.crc)  &&
      (0xFFFFFFFF == cfgRam.mbr.mapVer.val)){
    cfgErr.eepInit = 1;                     }
} // end routine ReadFromEepMem


static void WriteToEepMem(void)
{
  cfgRam.mbr.mapVer.w++;
  if (0x000000 == cfgRam.mbr.mapVer.w)      {
    cfgRam.mbr.mapVer.w--;                  }
  cfgRam.mbr.crc = GetDataMemCrc(cfgRam.u16, CFG_CRC_OFFSET);

  if (!WriteBfrToEeprom(EEP_CFG_ADRS, cfgRam.u8, CFG_U8_SIZE))
  {
    cfgErr.eepAcs = 1;
  }
} // end routine WriteToEepMem


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ReadCfgFromNvMem(void)
//  Reads device configuration from non-volatile memory into the globally
//  accessible 'devcfg_t' structure 'devCfg'.
//
//  It starts by clearing all configuration memory status errors accessed via
//  GetCfgMemErr().  If any non-fatal configuration memory errors are detected
//  it will initialize the set of affected parameters with default values.
//
//  INPUT : NONE
//  OUTPUT: NONE - but will set esdErrFlags.nvmem if any inconsistency detected
//  CALLS : ReadFromEepMem
//          memcpy
//          WriteToEepMem
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ReadCfgFromNvMem(void)
{
/*  TODO - stub out block of code
  { // Code to know how to set CRC in the const configuration variables
    volatile crc_t crc;
    crc = 0xffffffff;
    Nop();    // breakpoint
    Nop();    // skid pad
    Nop();
    crc = GetDataMemCrc(CFGPGM.u16, CFG_CRC_OFFSET);
    Nop();    // breakpoint
    Nop();    // skid pad
    Nop();
    if (CFGPGM.mbr.crc != crc)
    {
      esdErrFlags.fw = 1;
      esdErrFlags.uc = 1;
    }
  }
*/

  cfgErr.val = 0x0000;                  // Forgive all past config memory sins
  ReadFromEepMem();                     // Read config from EEPROM, flag errors

  if ( !(FATAL_CFG_ERR & cfgErr.val))
  {                                     // When no fatal error allow recovery...
    if (cfgErr.eepCrc || cfgErr.eepMap)
    {                                   // for non-fatal eep errors recover
      memcpy((void*)cfgRam.u8, (const void*)CFGPGM.u8, CFG_CRC_OFFSET);
      WriteToEepMem();                  // saving this back to EEPROM.
      if (cfgErr.eepInit)
      {                                 // When appears memory not initialized
        ReadFromEepMem();               // read it again to see if memory good &
      }                                 // hopefully clear errors (but not Init)
    }
  } // end if no fatal errors

  if (cfgErr.val)               {       // When had error(s) reading NVMEM
    esdErrFlags.nvmem = 1;      }       // note that in system error flag
} // end routine ReadCfgFromNvMem


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void WriteCfgToNvMem(void)
//  Writes device configuration values held in the globally accessable structure
//  cfgRam into non-volatile memory.  It takes care of erasing NVMEM if needed
//  to retain its freshness.
//
//  INPUT : NONE
//  OUTPUT: NONE - but will set esdErrFlags.nvmem if any inconsistency detected
//  CALLS : ReadEepromToBfr
//          WriteToEepMem
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void WriteCfgToNvMem(void)
{
  cfgblock_t svdCfg;
  uint16_t * svd;
  uint16_t * ram;
  uint16_t   i;
  uint16_t   diff;

  ReadEepromToBfr(EEP_CFG_ADRS, svdCfg.u8, CFG_U8_SIZE);
  svd  = svdCfg.u16;
  ram  = cfgRam.u16;
  diff = 0x0000;
  for (i = CFG_U16_CNT;  ((!diff) && i);  i--)
  {
    diff |= (*svd ^ *ram);
    svd++;  ram++;
  }
  if (diff)                 {
    WriteToEepMem();        }

  if (cfgErr.val)           {           // When had error(s) reading NVMEM
    esdErrFlags.nvmem = 1;  }           // note that in system error flag
} // end routine WriteCfgToNvMem


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  uint16_t GetCfgMemErr(void)
//  Returns the uint16_t representation of cfgerr_t configuration memory errors.
//  The return value is not valid before ReadCfgFromNvMem() has been called.
//
//  INPUT : NONE
//  OUTPUT: uint16_t cfgerr
//  CALLS : NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
uint16_t GetCfgMemErr(void)
{
  return cfgErr.val;
} // end routine GetCfgMemErr


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool ReadGasGaugeFromNvMem(uint16_t * acr, uint16_t * pct)
//  Reads battery gas gauge data from mapped NVMEM memory (with error detection)
//
//  INPUT : uint16_t * acr - ACR value for columb counter
//          uint16_t * pct - battery percent for display
//  OUTPUT: bool - true if successful read of previously saved data,
//                 otherwise false and sets the error flags
//                 esdErrFlags.gasGa [and esdErrFlags.nvmem] as appropriate
//  CALLS : ReadEepromToBfr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool ReadGasGaugeFromNvMem(uint16_t * acr, uint16_t * pct)
{
  uint64byte_t protData;
  bool isOk = false;

  if ( ! ReadEepromToBfr(EEP_GAS_ADRS, protData.v, GAS_EEP_U8_SIZE))
  {                                     // failure to read at all is 2 errors
    esdErrFlags.gasGa = esdErrFlags.nvmem = 1;
  }
  else if (0xFFFFFFFFFFFFFFFF == protData.val)
  {                                     // read but no prior data save so no err
    ;
  }
  else if ((protData.LW != ~protData.HW) || (protData.UW != ~protData.MW))
  {                                     // read but fails test, invalid gas data
    esdErrFlags.gasGa = 1;
  }
  else
  {                                     // Happy, happy, joy, joy!!! pass checks
    isOk = true;
    *acr = protData.LW;
    *pct = protData.UW;
  }
  return isOk;
} // end routine ReadGasGaugeFromNvMem


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void WriteGasGaugeToNvMem(uint16_t acr, uint16_t pct)
//  Write battery gas gauge data to mapped NVMEM memory (with error detection).
//
//  INPUT : uint16_t acr - the ACR value to save to EEPROM
//          uint16_t pct - the battery % value to save to EEPROM
//  OUTPUT: NONE (but may set esdErrFlags.gasGa and esdErrFlags.nvmem)
//  CALLS : WriteBfrToEeprom
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void WriteGasGaugeToNvMem(uint16_t acr, uint16_t pct)
{
  uint64byte_t protData;

  protData.LW  =  acr;
  protData.HW  = ~acr;
  protData.UW  =  pct;
  protData.MW  = ~pct;

  if (!WriteBfrToEeprom(EEP_GAS_ADRS, protData.v, GAS_EEP_U8_SIZE))
  {
    esdErrFlags.gasGa = esdErrFlags.nvmem = 1;
  }
} // end routine WriteGasGaugeToNvMem
