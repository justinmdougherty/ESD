////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : uc1701x.c
 *
 *  DESCRIPTION   : Defines interface to EA DOGS102W-6 UC1701 LCD display
 *    via the SPI1 module on the PIC24FJ128GA202. This display is 102x64 pixels.
 *    The 64 rows are controlled by 'pages' that are 8-pixels tall, resulting in
 *    8 'pages'.  So symbols are typically some multiple of 8 pixels tall.
 *    Any of the 102 columns can be specifically addressed.
 *
 *      (1) void InitUc1701x(void)
 *      (2) void UC1701XWrite(uint8_t value)
 *      (3) void UC1701XWriteData(uint8_t data)
 *      (4) bool UC1701XSetColumnAddress(uint8_t ca)
 *      (5) void UC1701XSetPowerControl(bool pc0, bool pc1, bool pc2)
 *      (6) bool UC1701XSetScrollLine(uint8_t sl)
 *      (7) bool UC1701XSetPageAddress(uint8_t pa)
 *      (8) bool UC1701XSetVLCDResistorRatio(uint8_t pc)
 *      (9) bool UC1701XSetElectronicVolume(uint8_t pm)
 *     (10) void UC1701XSetAllPixelsOn(bool dc1)
 *     (11) void UC1701XSetInverseDisplay(bool dc0)
 *     (12) void UC1701XSetDisplayEnable(bool dc2)
 *     (13) void UC1701XSetSEGDirection(bool lc0)
 *     (14) void UC1701XSetCOMDirection(bool lc1)
 *     (15) void UC1701XSystemReset()
 *     (16) void UC1701XSetAdvProgCntl0(bool tc, bool wc, bool wp)
 *     (17) void UC1701XSetLCDBiasRatio(bool br)
 *
 *  USE:
 *
 *  WRITTEN BY    : Nicholus Sunshine, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2018/09/21, Robert Kirby, NSWC H12
 *      Refactor sysErrFlags to esdErrFlags as it doesn't cover Ltng errors
 *    2017/04/10, Robert Kirby, NSWC H12
 *      Modify UC1701XWrite() so it can't hang forever
 *      If appropriate, set sysErrFlags.spi in UC1701XWrite
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Comments on what Nick developed for ESD
 *
 *  REFERENCE DOCUMENTS
 *    1.  DOGS Graphic Series 102x6 Dots Data Sheet 06.2013
 */
#include <xc.h>
#include "uc1701x.h"
#include "main.h"           // for esdErrFlags.spi
#include "micro_defs.h"     // System stuff like FCY
#include <libpic30.h>       // For delay functions MUST follow define of FCY

// UC1701 SPI Select line control output on B9 (Active Low)
#define UC1701_SELECT()     { LCD_CS0 = 0; }
#define UC1701_DESELECT()   { LCD_CS0 = 1; }

// UC1701 reset line control output on A1
#define UC1701_RESET()      { LCD_RST = 0; }
#define UC1701_ACTIVATE()   { LCD_RST = 1; }

// UC1701 data mode enable control output on A0
#define UC1701_DATA_MODE()  { LCD_CD = 1; }
#define UC1701_CMD_MODE()   { LCD_CD = 0; }

// Useful UC1701 macros
#define UC1701_POWER_UP()   {                                               \
                              UC1701_RESET();     __delay_ms(10);           \
                              UC1701_ACTIVATE();  __delay_ms(50);           \
                            }
#define UC1701_POWER_DOWN() {                                               \
                              UC1701_RESET();   __delay_ms(10);             \
                            }
#define UC1701_INIT() { /* Set scroll line to start line 0 */               \
                        UC1701XSetScrollLine(0);                            \
                        /* SEG reversed (6 o'clock orientation) */          \
                        UC1701XSetSEGDirection(true);                       \
                        /* Normal COM0-COM63 */                             \
                        UC1701XSetCOMDirection(false);                      \
                        /* Disable all pixels on (shows SRAM contents) */   \
                        UC1701XSetAllPixelsOn(false);                       \
                        /* Show normal SRAM contents */                     \
                        UC1701XSetInverseDisplay(false);                    \
                        /* Set Bias 1/9 (Duty 1/65) */                      \
                        UC1701XSetLCDBiasRatio(false);                      \
                        /* Set booster, regulator, and follower on */       \
                        UC1701XSetPowerControl(true, true, true);           \
                        /* Set Contrast, step 1 */                          \
                        UC1701XSetVLCDResistorRatio(3);                     \
                        /* Set Contrast, step 2 */                          \
                        UC1701XSetElectronicVolume(58);                     \
                        /* Set temp. compens. curve to -0.11% / degree C */ \
                        UC1701XSetAdvProgCntl0(true, false, false);         \
                        /* Display on */                                    \
                        UC1701XSetDisplayEnable(true);                      \
                      }

// Set the SRAM column address, CA = 0-131 (Default=0)
// CD D7 D6 D5 D4 D3  D2  D1  D0
// 0  0  0  0  0  CA3 CA2 CA1 CA0       Sets CA[3:0]
// 0  0  0  0  1  CA7 CA6 CA5 CA4       Sets CA[7:4]
/* Bitwise OR UC1701X_SPI_SET_SRAM_COL_ADDR_1 with least significant nibble
 * CA[3:0] and send first. */
/* Bitwise OR UC1701X_SPI_SET_SRAM_COL_ADDR_2 with most significat nibble
 * CA[7:4] and send second. */
#define UC1701X_SPI_SET_SRAM_COL_ADDR_1 (0b00000000)
#define UC1701X_SPI_SET_SRAM_COL_ADDR_2 (0b00010000)

// Set Power Control (Booster, Regulator, and Follower) (Default=0b000)
// CD D7 D6 D5 D4 D3 D2  D1  D0
// 0  0  0  1  0  1  PC2 PC1 PC0
// Bitwise OR UC1701X_SPI_SET_POWER_CONTROL with PC[2:0] before sending.
// PC0: 0=Booster OFF; 1=Booster ON
// PC1: 0=Regulator OFF; 1=Regulator ON
// PC2: 0=Follower OFF; 1 = Follower ON
#define UC1701X_SPI_SET_POWER_CONTROL (0b00101000)

// Set Scroll Line (display startline number SL=0-63) (Default=0)
// CD D7 D6 D5  D4  D3  D2  D1  D0
// 0  0  1  SL5 SL4 SL3 SL2 SL1 SL0
/* Bitwise OR UC1701X_SPI_SET_SCROLL_LINE with SL[5:0] holding value 0-63 for
 * scroll line number before sending. */
#define UC1701X_SPI_SET_SCROLL_LINE (0b01000000)

// Set Page Address (Set the SRAM page address PA=0-8) (Default=0)
// CD D7 D6 D5 D4 D3  D2  D1  D0
// 0  1  0  1  1  PA3 PA2 PA1 PA0
/* Bitwise OR UC1701X_SPI_SET_PAGE_ADDRESS with PA[3:0] holding value 0-8 for
 * page address number before sending. */
// Note page 8 corresponds to the icon output CIC
#define UC1701X_SPI_SET_PAGE_ADDRESS (0b10110000)

/* Set VLCD Resistor Ratio (Configure internal resistor ratio PC=0-7)
 * (Default=0b100) */
// CD D7 D6 D5 D4 D3 D2  D1  D0
// 0  0  0  1  0  0  PC5 PC4 PC3
/* Bitwise OR UC1701X_SPI_SET_VLCD_RESISTOR_RATIO with PC[5:3] holding value 0-7
 * for VLCD resistor ratio before sending */
#define UC1701X_SPI_SET_VLCD_RESISTOR_RATIO (0b00100000)

// Set Electronic Volume (Adjust contrast of LCD panel PM=0..63) (Default=32)
// CD D7 D6  D5  D4  D3  D2  D1  D0
// 0  1  0   0   0   0   0   0   1
// 0  0  0   PM5 PM4 PM3 PM2 PM1 PM0
// Send UC1701X_SPI_SET_ELECTRONIC_VOLUME_1 first
/* Bitwise OR UC1701X_SPI_SET_ELECTRONIC_VOLUME_2 with PM[5:0] holding value
 * 0-63 and send second */
#define UC1701X_SPI_SET_ELECTRONIC_VOLUME_1 (0b10000001)
#define UC1701X_SPI_SET_ELECTRONIC_VOLUME_2 (0b00000000)

// Set All Pixels On (Default=0)
// CD D7 D6 D5 D4 D3 D2 D1 D0
// 0  1  0  1  0  0  1  0  DC1
// DC1=0: show SRAM content; DC1=1: set all SEG drivers to ON.
// Bitwise OR UC1701X_SPI_SET_ALL_PIXELS_ON with DC1 before sending.
#define UC1701X_SPI_SET_ALL_PIXELS_ON (0b10100100)

// Set Inverse Display (Default=0)
// CD D7 D6 D5 D4 D3 D2 D1 D0
// 0  1  0  1  0  0  1  1  DC0
// DC0=0: show normal SRAM content; DC0=1: show inverse SRAM content.
// Bitwise OR UC1701X_SPI_SET_INVERSE_DISPLAY with DC0 before sending.
#define UC1701X_SPI_SET_INVERSE_DISPLAY (0b10100110)

// Set Display Enable (Default=0)
// CD D7 D6 D5 D4 D3 D2 D1 D0
// 0  1  0  1  0  1  1  1  DC2
// DC2=0: disable display (sleep); DC2=1: enable display (exit from sleep).
// Bitwise OR UC1701X_SPI_SET_DISPLAY_ENABLE with DC2 before sending.
#define UC1701X_SPI_SET_DISPLAY_ENABLE (0b10101110)

// Set SEG direction (Default=0)
// CD D7 D6 D5 D4 D3 D2 D1 D0
// 0  1  0  1  0  0  0  0  LC0
// LC0=0: normal SEG 0..131; LC0 = 1: mirror SEG 131-0.
// Bitwise OR UC1701X_SPI_SET SEG_DIRECTION with LC0 before sending.
// Note that LC0 is also referred to as MX (Mirror X SEG/Column)
#define UC1701X_SPI_SET_SEG_DIRECTION (0b10100000)

// Set COM direction (Default=0)
// CD D7 D6 D5 D4 D3  D2 D1 D0
// 0  1  1  0  0  LC1 0  0  0
// LC1=0: normal COM 0..63; LC1=1: mirror COM 63-0.
/* Bitwise OR UC1701X_SPI_SET_COM_DIRECTION with LC1 before sending.  Ensure LC1
 * is in digit 3. */
// Note that LC1 is also referred to as MY (Mirror Y COM/Row)
#define UC1701X_SPI_SET_COM_DIRECTION (0b11000000)

// System Reset
// CD D7 D6 D5 D4 D3 D2 D1 D0
// 0  1  1  1  0  0  0  1  0
// No Bitwise OR needed for this command
#define UC1701X_SPI_SYSTEM_RESET (0b11100010)

// Set LCD Bias Ratio (Default=0)
// CD D7 D6 D5 D4 D3 D2 D1 D0
// 0  1  0  1  0  0  0  1  BR
// BR=0: 1/9; BR=1: 1/7.
// Bitwise OR UC1701X_SPI_SET_LCD_BIAS_RATIO with BR before sending.
#define UC1701X_SPI_SET_LCD_BIAS_RATIO (0b10100010)

// Set Advanced Program Control (Default: TC=1, WC=0, WP=0)
// CD D7 D6 D5 D4 D3 D2 D1 D0
// 0  1  1  1  1  1  0  1  0
// 0  TC 0  0  1  0  0  WC WP
/* TC: Temperature compensation curve coefficient 0 = -0.05%/degree C;
 * 1 = -0.11%/degree C */
// WC: Column wrap around 0=OFF; 1=ON
// WP: Page wrap around 0=OFF; 1=ON
// Send UC1701X_SPI_SET_ADVANCED_PROGRAM_CONTROL_0_1 first.
/* Bitwise OR UC1701X_SPI_SET_ADVANCED_PROGRAM_CONTROL_0_2 with TC, WC, and WP,
 * paying mind to their bit positions, then send second. */
#define UC1701X_SPI_SET_ADVANCED_PROGRAM_CONTROL_0_1 (0b11111010)
#define UC1701X_SPI_SET_ADVANCED_PROGRAM_CONTROL_0_2 (0b00010000)



/* InitUc1701x
 * Configure SPI1 and OC1 peripherals used for LCD
 * and then go thru LCD's reset and configuration power-up sequence */
void InitUc1701x(void)
{
  // Communicate via SPI1, so initialize that peripheral
  // SPI1BRGL
  // UC1701 datasheet says minimum write cycle time is 30 ns --> 33.333333 MHz
  // FPB = Fcy = 10 MHz
  // Baud rate = FPB / (2 * (SPI1BRG + 1)) as per microcontroller datasheet
  // Setting BRG to 0 means baud rate will be 5 MHz => 200 ns write cycle time
  // Leaving SPI1 BRG as default of 0 for now
  // TODO try halting SPI1 when CPU idle as don't use it then anyway
  // TODO try SPI1 running faster using MCLK (Fosc/2 = 10Mhz instead of Fcy/2 = 5MHz)
  PMD1bits.SPI1MD = 1;              // Temporarily de-clock/reset SPI-1
  IEC0bits.SPI1IE = 0;              // Ensure SPI-1's interrupt disabled
  PMD1bits.SPI1MD = 0;              // Now clock SPI-1 peripheral
  SPI1BUFL;                         // Clear the SPI1 receive buffers
  SPI1BUFH;
  // 8-bit TX-only SPI master, Idle clk HI, falling edge TX, manual SSx
  SPI1CON1L = 0x0070;
/*
  // 8-bit TX-only SPI master, Idle clk HI, falling edge TX, manual SSx
  SPI1CON1L = 0x0070;
  // Halt in idle(), 8-bit TX-only SPI master, Idle clk HI, falling edge TX, manual SSx
  SPI1CON1L = 0x2070;
  // Use MCLK, Halt in idle(), 8-bit TX-only SPI master, Idle clk HI, falling edge TX, manual SSx
  SPI1CON1L = 0x2074;
*/
  SPI1CON1Lbits.SPIEN = 1;          // Enable SPI1 module

  // Use Output Compare 1 as PWM to drive backlight, so initialize peripheral
  PMD2bits.OC1MD    = 1;            // De-clock/reset OutputCompare-1
  IEC0bits.OC1IE    = 0;            // Ensure OutputCompare-1's IRQ disabled
  PMD2bits.OC1MD    = 0;            // Now clock OutputCompare-1 peripheral

  OC1CON1 = 0;                      // Best practice is initially clear off
  OC1CON2 = 0;                      // the control bits, ensures OC disabled
  OC1CON1bits.OCTSEL = 0x07;        // Peripheral clock (Fcy) ticks OC1TMR
  OC1R  = 0;                        // OC1 pin LO-HI edge when OC1TMR = x
  OC1RS = 0;                        // OC1 pin HI-LO edge when OC1TMR = y
  OC1CON2bits.SYNCSEL = 0b00000;    // Free-Running, no sync to other module
  OC1CON1bits.OCM = 0b000;          // Output Compare channel disabled, i.e. OFF
/*
  OC1CON1bits.OCM = 0b101;          // Double-Compare Continuous Pulse Mode
  SET_BKLT_OFF();                   // with OC1R and OC1RS such that no PWM
*/
                                    // Now ready to power-up the LCD display
  UC1701_DESELECT();                // Hold SPI CS inactive before LCD power up
  UC1701_POWER_UP();                // Pulse RESET signal with set-up delay
  UC1701_SELECT();                  // Hold SPI CS active and then
  UC1701_INIT();                    // send plethora of configuration commands
} // end routine InitUc1701x


/* UC1701XWrite
 * In: value: command or data byte to send to LCD */
void UC1701XWrite(uint8_t value)
{
//  while (0 == SPI1STATLbits.SPITBE); // wait while TX buff not empty
//  SPI1BUFL = value;
//  while (1 == SPI1STATLbits.SPIBUSY); // Wait for transmission to finish

  uint8_t sanity = 0xFF;                        // Start with full sanity

  while ((!SPI1STATLbits.SPITBE) && sanity) {   // Wait while TX buff not empty
    sanity--;                               }   // but don't wait insane time
  SPI1BUFL = value;                             // Place data to transmit
  while (SPI1STATLbits.SPIBUSY && sanity)   {   // Wait for transmit to finish
    sanity--;                               }   // but don't wait insane time
  if (!sanity)                              {   // When lost all sanity
    esdErrFlags.spi = 1;                    }   // note SPI bus issue

} // end routine UC1701XWrite


/* UC1701XWriteData
 * In: data: data byte to be written to LCD SRAM  */
void UC1701XWriteData(uint8_t data)
{
  UC1701_DATA_MODE();
  UC1701XWrite(data);
} // end routine UC1701XWriteData


/* UC1701XSetColumnAddress
 * In: ca: column address (valid input 0-101)
 * Out: Returns bool value with success/error code (false=failure, true=success)
 * Note: Will return false if ca is outside of valid range */
bool UC1701XSetColumnAddress(uint8_t ca)
{
  if (ca > 101)     {
    return false;   }

  UC1701_CMD_MODE();
  // Clear upper nibble of ca and bitwise OR it with first half of command before sending
  UC1701XWrite((ca&0x0F) | UC1701X_SPI_SET_SRAM_COL_ADDR_1);
  // Clear lower nibble of ca and shift right four times, then bitwise OR it with second half of command before sending
  UC1701XWrite(((ca&0xF0)>>4) | UC1701X_SPI_SET_SRAM_COL_ADDR_2);

  return true;
} // end function UC1701XSetColumnAddress


/* UC1701XSetPowerControl
 * In: pc0: voltage follower on (1) / off (0)
 *     pc1: voltage regulator on (1) / off (0)
 *     pc2: boost on (1) / off (0) */
void UC1701XSetPowerControl(bool pc0, bool pc1, bool pc2)
{
  UC1701_CMD_MODE();
  // Bitwise OR the command code with a bit for each bit position of PC[2:0] if set
  UC1701XWrite(UC1701X_SPI_SET_POWER_CONTROL | (pc0 ? 1 : 0) | (pc1 ? 2 : 0) | (pc2 ? 4 : 0));
} // end routine UC1701XSetPowerControl


/* UC1701XSetScrollLine
 * In: sl: scroll line (valid input 0-63)
 * Out: Returns bool value with success/error code (false=failure, true=success)
 * Note: Will return false if sl is outside of valid range */
bool UC1701XSetScrollLine(uint8_t sl)
{
  if (sl > 63)      {
    return false;   }

  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_SCROLL_LINE | sl);

  return true;
} // end function UC1701XSetScrollLine


/* UC1701XSetPageAddress
 * In: pa: page address (valid input 0-7)
 * Out: Returns bool value with success/error code (false=failure, true=success)
 * Note: Will return false if pa is outside of valid range */
bool UC1701XSetPageAddress(uint8_t pa)
{
  if (pa > 7)       {
    return false;   }

  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_PAGE_ADDRESS | pa);

  return true;
} // end function UC1701XSetPageAddress


/* UC1701XSetVLCDResistorRatio
 * In: pc: internal resistor ratio (valid input 0-7)
 * Out: Returns bool value with success/error code (false=failure, true=success)
 * Note:  Will return false if pc is outside of valid range  */
bool UC1701XSetVLCDResistorRatio(uint8_t pc)
{
  if (pc > 7)       {
    return false;   }

  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_VLCD_RESISTOR_RATIO | pc);

  return true;
} // end function UC1701XSetVLCDResistorRatio


/* UC1701XSetElectronicVolume
 * In: pm: electronic volume for VLCD voltage regulator to adjust contrast of LCD panel display (valid input 0-63)
 * Out: Returns int value with success/error code (false=failure, true=success)
 * Note:  Will return false if pm is outside of valid range  */
bool UC1701XSetElectronicVolume(uint8_t pm)
{
  if (pm > 63)      {
    return false;   }

  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_ELECTRONIC_VOLUME_1);
  UC1701XWrite(UC1701X_SPI_SET_ELECTRONIC_VOLUME_2 | pm);

  return true;
} // end function UC1701XSetElectronicVolume


/* UC1701XSetAllPixelsOn
 * In: dc1: false: show SRAM content; true: set all SEG drivers to ON.
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetAllPixelsOn(bool dc1)
{
  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_ALL_PIXELS_ON | (dc1 ? 1 : 0));
} // end routine UC1701XSetAllPixelsOn


/* UC1701XSetInverseDisplay
 * In: dc0: false: show normal SRAM content; true: show inverse SRAM content.
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetInverseDisplay(bool dc0)
{
  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_INVERSE_DISPLAY | (dc0 ? 1 : 0));
} // end routine UC1701XSetInverseDisplay


/* UC1701XSetDisplayEnable
 * In: dc2: false: disable display (sleep); true: enable display (exit from sleep)
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetDisplayEnable(bool dc2)
{
  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_DISPLAY_ENABLE | (dc2 ? 1 : 0));
} // end routine UC1701XSetDisplayEnable


/* UC1701XSetSEGDirection
 * In: lc0: false: normal SEG 0..131; true: mirror SEG 131-0.
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetSEGDirection(bool lc0)
{
  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_SEG_DIRECTION | (lc0 ? 1 : 0));
} // end routine UC1701XSetSEGDirection


/* UC1701XSetCOMDirection
 * In: lc1: false: normal COM 0..63; true: mirror COM 63-0
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetCOMDirection(bool lc1)
{
  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_COM_DIRECTION | (lc1 ? 1 : 0));
} // end routine UC1701XSetCOMDirection


/* UC1701XSystemReset
 * All control registers will be reset to their default values.
 * Data stored in RAM will not be affected. */
void UC1701XSystemReset()
{
  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SYSTEM_RESET);
} // end routine UC1701XSystemReset


/* UC1701XSetAdvProgCntl0
 * In: tc: Temperature compensation curve coefficient false = -0.05%/degree C; true = -0.11%/degree C
 *     wc: Column wrap around false=OFF; true=ON
 *     wp: Page wrap around false=OFF; true=ON */
void UC1701XSetAdvProgCntl0(bool tc, bool wc, bool wp)
{
  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_ADVANCED_PROGRAM_CONTROL_0_1);
  UC1701XWrite(UC1701X_SPI_SET_ADVANCED_PROGRAM_CONTROL_0_2 | (tc ? 128 : 0) | (wc ? 2 : 0) | (wp ? 1 : 0));
} // end routine UC1701XSetAdvProgCntl0


/* UC1701XSetLCDBiasRatio
 * In: br: Bias ratio, false = 1/9, true = 1/7 */
void UC1701XSetLCDBiasRatio(bool br)
{
  UC1701_CMD_MODE();
  UC1701XWrite(UC1701X_SPI_SET_LCD_BIAS_RATIO | (br ? 1 : 0));
} // end routine UC1701XSetLCDBiasRatio
