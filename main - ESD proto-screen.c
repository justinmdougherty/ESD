////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : main.c
 *
 *  DESCRIPTION   : Define top level (main) attributes and methods required to
 *    prototype ESD screens in Pharos's LBHH.
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

  LCDWriteStringTerminal6X8  (0,  0, "abcdefghijklmnopq", false);
  LCDWriteStringTerminal6X8  (1,  0, "rstuvwxyz !\"#$%&'", false);
  LCDWriteStringTerminal6X8  (2,  0, "ABCDEFGHIJKLMNOPQ", false);
  LCDWriteStringTerminal6X8  (3,  0, "RSTUVWXYZ()*+,-.\\", false);
  LCDWriteStringTerminal6X8  (4,  0, "0123456789:;<=>?@", false);
  LCDWriteStringTerminal6X8  (5,  0, "[/]^_`{}|", false);
    Nop();    // breakpoint
    Nop();    // skid pad
    Nop();
  LCDClearScreen();
  LCDWriteStringTerminal12X16(0,  3, "abcdefgh", false);
  LCDWriteStringTerminal12X16(2,  3, "ABCDEFGH", false);
  LCDWriteStringTerminal12X16(4,  3, "rstuvwxy", false);
  LCDWriteStringTerminal12X16(6,  3, "RSTUVWXY", false);
    Nop();    // breakpoint
    Nop();    // skid pad
    Nop();
  LCDClearScreen();
  LCDWriteStringTerminal12X16(0,  3, "01234567", false);
  LCDWriteStringTerminal12X16(2,  3, "[/]^_`{}", false);
    Nop();    // breakpoint
    Nop();    // skid pad
    Nop();
  LCDClearScreen();
  LCDWriteStringTerminal12X16(0,  3, "jklmnopq", false);
  LCDWriteStringTerminal12X16(2,  3, "JKLMNOPQ", false);
  LCDWriteStringTerminal12X16(4,  3, " !\"#$%&'", false);
  LCDWriteStringTerminal12X16(6,  3, "()*+,-.\\", false);
    Nop();    // breakpoint
    Nop();    // skid pad
    Nop();
  LCDClearScreen();
  LCDWriteStringTerminal12X16(0,  3, "9:;<=>?@", false);
  LCDWriteStringTerminal12X16(2,  0, "iIzZ8|~", false);
    Nop();    // breakpoint
    Nop();    // skid pad
    Nop();
  LCDClearScreen();
  // LCD screen has pages (8 pixel rows) 0-7 and pixel columns 0-101
  // Pages are 8 pixels tall; Columns are 1 pixel wide
  // Font sizes are indicated COLxROW-pixels but write calls are (PAGE,COL,...)
  // Provides for 8 rows and 17 columns of 6X8 font characters
  // (17 char * 6 pixels = 102 char pixel columns when using 6X8 font)
  // or for 4 rows and 8.5 columns of 12X16 font characters
  do
  {
    uint8bits_t  scanCode;

    LCDWriteStringTerminal6X8  (0,  0, "I", false);
    LCDWriteStringTerminal6X8  (1,  0, "D", false);
    LCDWriteStringTerminal12X16(0,  6, "9357", false);
    LCDWriteStringTerminal6X8  (0, 60, "B", false);
    LCDWriteStringTerminal6X8  (1, 60, "C", false);
    LCDWriteStringTerminal12X16(0, 66, "241", false);

    LCDWriteStringTerminal12X16(2,  3, "06:32:19", false);

    LCDWriteStringTerminal12X16(4,  0, "!", true);
    LCDWriteStringTerminal12X16(4, 12, "820", false);
    LCDWriteStringTerminal6X8  (4, 48, "k", false);
    LCDWriteStringTerminal6X8  (5, 48, "m", false);
    LCDWriteStringTerminal12X16(4, 60, "315", false);
    LCDWriteStringTerminal6X8  (4, 96, "o", false);
    LCDWriteStringTerminal6X8  (5, 96, " ", false);

    LCDWriteStringTerminal6X8  (6,  0, " EXFIL", false);
    LCDWriteStringTerminal6X8  (7,  0, " MUTED", false);
    LCDWriteStringTerminal6X8  (7, 48, ENVELOPE_STR, false);
    LCDWriteStringTerminal6X8  (6, 66, "AoF 7m", false);
    LCDWriteStringTerminal6X8  (7, 66, "NO GPS", false);

    scanCode.val = KEYPAD_SCANCODE_NONE;
    do
    {
      __delay_ms(150);                    // some delay to see display
      {                                             // Build keypad code using 2 scans
        #define KEYPAD_C0_SET_ON      { KP_C0 = 1; }    // Keypad column 0 control
        #define KEYPAD_C0_SET_OFF     { KP_C0 = 0; }
        #define KEYPAD_C1_SET_ON      { KP_C1 = 1; }    // Keypad column 1 control
        #define KEYPAD_C1_SET_OFF     { KP_C1 = 0; }
        #define KEYPAD_R0_IS_ON        (KP_R0 == 1)     // Keypad row 0 determination
        #define KEYPAD_R0_IS_OFF       (KP_R0 == 0)
        #define KEYPAD_R1_IS_ON        (KP_R1 == 1)     // Keypad row 1 determination
        #define KEYPAD_R1_IS_OFF       (KP_R1 == 0)
        KEYPAD_C0_SET_ON;                           // BITS - b7 b6 b5 b4 b3 b2 b1 b0
        KEYPAD_C1_SET_OFF;                          // PINS - c1 C0 R1 R0 C1 c0 R1 R0
        scanCode.val  = 0b01000000;                 // Set bit 6 as pin levels settle
        if (KEYPAD_R1_IS_ON)  { scanCode.b5 = 1; }  // Set bit 5 if R1 on 1st scan
        if (KEYPAD_R0_IS_ON)  { scanCode.b4 = 1; }  // Set bit 4 if R0 on 1st scan
        KEYPAD_C0_SET_OFF;                          // Swap which Cx pin is driving
        KEYPAD_C1_SET_ON;                           // the Rx pins for another scan
        scanCode.b3 = 1;                            // Set bit 3 as pin levels settle
        if (KEYPAD_R1_IS_ON)  { scanCode.b1 = 1; }  // Set bit 1 if R1 on 2nd scan
        if (KEYPAD_R0_IS_ON)  { scanCode.b0 = 1; }  // Set bit 0 if R0 on 2nd scan
        KEYPAD_C1_SET_OFF;                          // Done actively scanning buttons
      } // end routine BuildScanCode
    } while (KEYPAD_SCANCODE_NONE == scanCode.val);
    LCDClearScreen();

    LCDWriteStringTerminal6X8  (0,  0, "I", false);
    LCDWriteStringTerminal6X8  (1,  0, "D", false);
    LCDWriteStringTerminal12X16(0,  6, "9357", false);
    LCDWriteStringTerminal6X8  (0, 60, "B", false);
    LCDWriteStringTerminal6X8  (1, 60, "C", false);
    LCDWriteStringTerminal12X16(0, 66, "241", false);

    LCDWriteStringTerminal12X16(3,  3, "06:32:19", false);

    LCDWriteStringTerminal12X16(6,  0, "*", true);
    LCDWriteStringTerminal12X16(6, 12, "820", false);
    LCDWriteStringTerminal6X8  (6, 48, "k", false);
    LCDWriteStringTerminal6X8  (7, 48, "m", false);
    LCDWriteStringTerminal12X16(6, 60, "315", false);
    LCDWriteStringTerminal6X8  (6, 96, "o", false);
    LCDWriteStringTerminal6X8  (7, 96, " ", false);

    scanCode.val = KEYPAD_SCANCODE_NONE;
    do
    {
      __delay_ms(150);                    // some delay to see display
      {                                             // Build keypad code using 2 scans
        #define KEYPAD_C0_SET_ON      { KP_C0 = 1; }    // Keypad column 0 control
        #define KEYPAD_C0_SET_OFF     { KP_C0 = 0; }
        #define KEYPAD_C1_SET_ON      { KP_C1 = 1; }    // Keypad column 1 control
        #define KEYPAD_C1_SET_OFF     { KP_C1 = 0; }
        #define KEYPAD_R0_IS_ON        (KP_R0 == 1)     // Keypad row 0 determination
        #define KEYPAD_R0_IS_OFF       (KP_R0 == 0)
        #define KEYPAD_R1_IS_ON        (KP_R1 == 1)     // Keypad row 1 determination
        #define KEYPAD_R1_IS_OFF       (KP_R1 == 0)
        KEYPAD_C0_SET_ON;                           // BITS - b7 b6 b5 b4 b3 b2 b1 b0
        KEYPAD_C1_SET_OFF;                          // PINS - c1 C0 R1 R0 C1 c0 R1 R0
        scanCode.val  = 0b01000000;                 // Set bit 6 as pin levels settle
        if (KEYPAD_R1_IS_ON)  { scanCode.b5 = 1; }  // Set bit 5 if R1 on 1st scan
        if (KEYPAD_R0_IS_ON)  { scanCode.b4 = 1; }  // Set bit 4 if R0 on 1st scan
        KEYPAD_C0_SET_OFF;                          // Swap which Cx pin is driving
        KEYPAD_C1_SET_ON;                           // the Rx pins for another scan
        scanCode.b3 = 1;                            // Set bit 3 as pin levels settle
        if (KEYPAD_R1_IS_ON)  { scanCode.b1 = 1; }  // Set bit 1 if R1 on 2nd scan
        if (KEYPAD_R0_IS_ON)  { scanCode.b0 = 1; }  // Set bit 0 if R0 on 2nd scan
        KEYPAD_C1_SET_OFF;                          // Done actively scanning buttons
      } // end routine BuildScanCode
    } while (KEYPAD_SCANCODE_NONE == scanCode.val);
    LCDClearScreen();
  } while (1);                            // loop continuously when done

} // end main routine
//-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

#undef RFSC
#undef RFPC
