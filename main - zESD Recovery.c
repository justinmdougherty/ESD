////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : main.c
 *
 *  DESCRIPTION   : Define top level (main) attributes and methods required to
 *    recover a zeroized ESD in Pharos's LBHH.  This is done by erasing EEPROM
 *    so there is no longer the zeroize finger print to keep ESD from running.
 */
#include <xc.h>
#include <stdbool.h>        // Includes true/false definition
#include <stdint.h>         // Includes uint16_t definition
#include <stdio.h>
#include <string.h>         // for strcmp, memcpy, etc.
#include <stdlib.h>
#include "stdint_extended.h"
#include "micro_defs.h"     // System stuff like FCY and macro BT_INIT_UART()
#include "main.h"
#include "esd_ver.h"        // for FW_VER_STR
#include "config_memory.h"
#include "coords.h"
#include "fonts.h"
#include "i2c2.h"           // functions (or macros) to run I2C in polled mode
#include "lcd.h"            // for LCD_MAX_COLS etc.
#include "ltc2943.h"        // for LTC2943_ZERO_PT
#include "mc24aa512.h"
#include "keypad.h"
#include "queue.h"
#include "tmr2.h"
#include "uc1701x.h"
#include "uart.h"
#include "uart1_queued.h"
#include <libpic30.h>      // For delay functions MUST follow define of FCY



//----- DEFINES, ENUMS, STRUCTS, TYPEDEFS, ETC. --------------------------------
#define RFSC    RTI_FSC       // shorten def for use in this file
#define RFPC    RTI_FPC       // shorten def for use in this file

typedef enum tagRESET_SOURCE
{
  NONE_FOUND = 0,
  POWER_ON,
  EXTERNAL,
  SOFTWARE,
  WATCHDOG,
  BROWN_OUT,
  CFG_WORD_MISMATCH,
  TRAP,
  ILLEGAL_INSTR,
} rstsrc_t;


//----- GLOBAL VARIABLE DECLARATIONS -------------------------------------------
volatile  bool          doNotSleep;         // Set in ISR and modules as needed
volatile  uint16bits_t  virtFwSig;          // Needed global for firmware signal
volatile  esd_t         esdErrFlags;        // Error flags, e.g. firmware error

//----- MODULE ATTRIBUTES ------------------------------------------------------


//----- MACROS -----------------------------------------------------------------


//----- PROCEDURES -------------------------------------------------------------


//-*-*- MAIN -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//  int16_t main(void)
//  The One to rule them all, One to find them, One to bring them all and in the
//  darkness bind them.
//
//  INPUT : NONE
//  OUTPUT: uint16_t
//  CALLS : Shutdown
//          InitSystem
//          FinishBit
//          OperateNormally
//          ProvisionBolt
//          ProvisionGdb
//          WipeMemories
//          ProcessErrors
//          Idle
//          Sleep
//-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
int16_t main(void)
{
  uint16_t i;
  bool     isOk;

  mGlobalIntDisable();                  // Disable all IRQs until run initialize
  RCONbits.SWDTEN = 0;                  // Disable Watchdog timer as well

  // Enable interrupt nesting (user priorities; hi-pri prempting lo-pri)
  INTCON1bits.NSTDIS = 0;     // Enable IRQ nesting - IPL bits can disable IRQs

  //======= Setup port functionality: ana/dig, direction, pull up/down =========
  // On reset: ANAx all analog, TRISx all inputs, LATx and PORTx are unknown,
  //           ODCx all open drain, CNPUx and CNPDx are disabled
  // Set unused I/O pins to digital output low to save power (tie I-only to GND)
  // Turn off as much as possible by default and turn on later as needed
  // I/O pins analog configuration as defined in micro_defs.h
  ANSA  = INIT_RA_ANS;
  ANSB  = INIT_RB_ANS;
  // I/O pins direction configuration as defined in micro_defs.h
  TRISA = INIT_RA_DIR;
  TRISB = INIT_RB_DIR;
  // Output pins level settings as defined in micro_defs.h
  LATA  = INIT_RA_LAT;
  LATB  = INIT_RB_LAT;
  // Output pins open drain configuration as defined in micro_defs.h
  ODCA  = INIT_RA_ODO;
  ODCB  = INIT_RB_ODO;
  // Input pins weak pull-up configuration as defined in micro_defs.h
  CNPU1 = CN1500_WPU;
  CNPU2 = CN3116_WPU;
  CNPU3 = CN4732_WPU;
  // Input pins weak pull-down configuration as defined in micro_defs.h
  CNPD1 = CN1500_WPD;
  CNPD2 = CN3116_WPD;
  CNPD3 = CN4732_WPD;

  // De-clock peripherals (Peripheral Module Disable) for max power savings
  PMD1 = 0xFFFF;
  PMD2 = 0xFFFF;
  PMD3 = 0xFFFF;
  PMD4 = 0xFFFF;
  PMD5 = 0xFFFF;
  PMD6 = 0xFFFF;
  PMD7 = 0xFFFF;
  PMD8 = 0xFFFF;

  // Set Peripheral Pin Select reconfig per micro_defs.h
  __builtin_write_OSCCONL(OSCCON&0xbf); // Unlock pin select registers
  INT1_RPINR  = INT1_RPX;               // Set gas gauge External IRQ-1 pin
  INT2_RPINR  = INT2_RPX;               // Set pwr btn External IRQ-2 pin
  OC1_RPOR    = OC1_OFN;                // Assign Output Compare pulse port pin
  SCK1_RPOR   = SCK1_OFN;               // Assign SPI Clock port pin
  SDO1_RPOR   = SDO1_OFN;               // Assign SPI MOSI port pin
  U1RX_RPINR  = U1RX_RPX;               // Set UART1 RX pin per micro_defs.h
  U1TX_RPOR   = U1TX_OFN;               // Set UART1 TX pin
  __builtin_write_OSCCONL(OSCCON|0x40); // Lock Registers

  ResetBusI2c2();                       // Try to ensure I2C bus not locked up
  INIT_I2C2();                          // then enable/init I2C2 peripheral
  OPEN_I2C2();                          // and open/start I2C2 for IC drivers.
  InitUc1701x();                        // Initialize SPI1, OC1, LCD display, &
  LCDClearScreen();                     // clear LCD's standard pwr-on funkiness
  SET_BKLT_HIGH();                      // Back-light makes it easy to read screen

  mGlobalIntEnable();

  LCDWriteStringTerminal6X8(0, 0, "Try zESD recovery", false);
  LCDWriteStringTerminal6X8(1, 0, "EEPROM Erase", false);
  for (i = 0,    isOk = false;
       i < 3 && !isOk;
       i++,      isOk = EraseEeprom())
  {
    LCDWriteStringTerminal6X8(2, 2*i*WIDTH_6X8, " .", false);
    Nop();    // breakpoint skidpad
    Nop();    // breakpoint skidpad
    Nop();    // breakpoint skidpad
    Nop();    // breakpoint skidpad
  }
  if (isOk)
  {
    LCDWriteStringTerminal12X16(3, 0, "worked", false);
    LTG_VLTG_EN = 1;    // Enable Lightning regulator to allow zLtng recovery
    LCDWriteStringTerminal6X8(6, 0, "Lightning pwr on", false);
    LCDWriteStringTerminal6X8(7, 0, "ESD reload ready", false);
  }
  else
  {
    LCDWriteStringTerminal12X16(4, 0, "failed", true);
    LCDWriteStringTerminal6X8(7, 0, "ESD not recovered", true);
  }

  while (1);                            // loop continuously when done

} // end main routine
//-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

#undef RFSC
#undef RFPC
