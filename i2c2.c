////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : i2c2.c
 *
 *  DESCRIPTION   : definitions for using the PIC24FJ128GA202's I2C2 module in
 *    a mixed polled/interrupt mode (methods may block waiting for IF).
 *
 *  NOTE - this enables/disables WDT (Watch Dog Timer) for I2C time-out errors
 *
 *  NOTE - this requires micro_defs.h define I2C2_SCL_TRIS, I2C2_SCL_LAT,
 *    I2C2_SCL_PORT, I2C2_SDA_TRIS, I2C2_SDA_LAT, and I2C2_SDA_PORT
 *
 *      (1) void ResetBusI2c2()
 *      (2) bool WaitForIdleI2c2()
 *      (3) bool WaitForMirqI2c2()
 *      (4) bool SendStartI2c2()
 *      (5) bool SendRestartI2c2()
 *      (6) bool ReadI2c2(uint8_t * value)
 *      (7) bool WriteI2c2(uint8_t value)
 *      (8) bool SendNackI2c2()
 *      (9) bool SendAckI2c2()
 *     (10) bool SendStopI2c2()
 *      (*) void _SI2C2Interrupt(void)
 *      (*) void _MI2C2Interrupt(void)
 *
 *  WRITTEN BY    : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2018/09/21, Robert Kirby, NSWC H12
 *      Refactor sysErrFlags to esdErrFlags as it doesn't cover Ltng errors
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Drop using IRQ as not implemented correctly, needs a semaphore as
 *      any ol' IRQ would wake from Idle in WaitForMirqI2c2, just not worth it
 *      Move ISRs into this file from interrupts.c so I2C2 code not adrift
 *    2017/01/10, Megan Kozub, NSWC V14
 *      Modified for I2C2: Modified for I2C2: changed references from
 *      I2C1 to I2C2, updated procedure and variable names and comments
 *      to "I2C2", updated to I2C2 registers (IFS3 vice IFS1, IPC12 vice IPC4)
 *      and associated I2C2 bits within utilized registers
 *    2016/11/02, Robert Kirby, NSWC H12
 *      Initial development
 *
 *  REFERENCE DOCUMENTS
 *    1. PIC24FJ128GA204 Data Sheet (Microchip Technology Inc. DS30010038C)
 */
#include <xc.h>
#include <stdbool.h>
#include "i2c2.h"
#include "main.h"                 // to access esdErrFlags


static bool i2c2Err = false;      // Used to detect consecutive I2C bus hang-ups

//--PROCEDURE-------------------------------------------------------------------
//  void ResetBusI2c2()
//  Bit-Bang series of signals on I2C2 bus that should release any hung devices
//  and then return bus pins to control of the I2C peripheral.
//------------------------------------------------------------------------------
void ResetBusI2c2()
{
  I2C2_SCL_LAT  = 1;          // While pins hi-Z input
  I2C2_SDA_LAT  = 1;          // set latches to output HI
  I2C2_SCL_TRIS = 0;          // Make an output HI
  I2C2_SDA_TRIS = 0;          // Make an output HI          +C/+D
  __delay_us(50);             // Delay five bit periods
  I2C2_SDA_LAT  = 0;          // Set START condition to     +C/-D
  I2C_100KHZ_DELAY();         // stop any hung writes
  I2C2_SCL_LAT  = 0;          // and then activating clock  -C/-D
  I2C_100KHZ_DELAY();
  I2C2_SDA_LAT  = 1;          // Start output of HI-bits    -C/+D
  I2C_100KHZ_DELAY();
  int lc;
  for (lc = 9; lc; lc--)      // Clock out 9 total HI bits
  {                           // i.e. 8 data bits and NAK
    I2C2_SCL_LAT = 1;         //                            +C/+D
    I2C_100KHZ_DELAY();
    I2C2_SCL_LAT = 0;         //                            -C/+D
    I2C_100KHZ_DELAY();
  }                           // Exit loop on NAK bit set
  I2C2_SCL_LAT  = 1;          // and clock NAK condition    +C/+D
  I2C_100KHZ_DELAY();         // with std clk time
  I2C2_SDA_LAT  = 0;          // Set START condition        +C/-D
  I2C_100KHZ_DELAY();         // to guard against slave
  I2C2_SCL_LAT = 0;           // driving ACK on 1st START   -C/-D
  I2C_100KHZ_DELAY();         // Fully cycle clock during
  I2C2_SCL_LAT = 1;           // start condition as some    +C/-D
  I2C_100KHZ_DELAY();         // ICs need it before stop
  I2C2_SDA_LAT  = 1;          // Set STOP condition to      +C/+D
  I2C_100KHZ_DELAY();         // terminate bus activity
  I2C_100KHZ_DELAY();         // tad extra delay for luck
  I2C2_SCL_TRIS = 1;          // Make input for I2C use
  I2C2_SDA_TRIS = 1;          // Make input for I2C use
} // end routine ResetBusI2c2

//--PROCEDURE-------------------------------------------------------------------
//  bool WaitForIdleI2c2()
//  Wait for bus idle condition using the Watch-Dog Timer to prevent hang-up.
//  Will reset the bus if WDT timed out, i.e. the bus was hung.
//
//  Returns TRUE if bus idled properly, returns FALSE if bus required reset
//------------------------------------------------------------------------------
bool WaitForIdleI2c2()
{
  RCONbits.SWDTEN = 1;                  // enable WDT
  while ( ( ! RCONbits.WDTO)    &&      // then wait for timeout
          ( I2C2STATbits.TRSTAT ||      // or ALL of the activity
            I2C2CONLbits.SEN    ||      // bits to be cleared.
            I2C2CONLbits.PEN    ||
            I2C2CONLbits.RCEN   ||
            I2C2CONLbits.RSEN   ||
            I2C2CONLbits.ACKEN )  )
  { ; }
  RCONbits.SWDTEN = 0;                  // disable WDT
  if (RCONbits.WDTO)
  {                                     // When WDTO occurred something wrong.
    RCONbits.WDTO = 0;                  // Clear WDTO before
    ResetBusI2c2();                     // try to ensure bus cleared and note
    if (i2c2Err)            {           // If there have been consecutive I2C
      esdErrFlags.i2c = 1;  }           // hangs then declare system error
    else                    {           // If this is not a consecutive I2C
      i2c2Err = true;       }           // hang flag this occurrence for future
    return false;                       // desired condition not happening
  }
  else
  {                                     // When I2C op completed as expected
    i2c2Err = false;                    // clear consecutive error detector flag
  }

  return true;
} // end function WaitForIdleI2c2

//--PROCEDURE-------------------------------------------------------------------
//  bool WaitForMirqI2c2()
//  Wait for an I2C2 Master interrupt flag to be set using Watch-Dog Timer to
//  prevent hang-up. Will reset the bus if WDT timed out, i.e. the bus was hung.
//
//  Returns TRUE if bus IRQ timely, returns FALSE if reset required
//------------------------------------------------------------------------------
bool WaitForMirqI2c2()
{
  RCONbits.SWDTEN = 1;            // enable WDT
  while ( ! (IFS3bits.MI2C2IF || RCONbits.WDTO))
  { ; }
  RCONbits.SWDTEN = 0;            // disable WDT
  RCONbits.WDTO   = 0;            // clear WDTO and do final check on IF
  if( ! IFS3bits.MI2C2IF)
  {                               // When I2C IRQ flag didn't set
    ResetBusI2c2();               // try to ensure bus cleared and note
    if (i2c2Err)            {     // If there have been consecutive I2C
      esdErrFlags.i2c = 1;  }     // hangs then declare system error
    else                    {     // If this is not a consecutive I2C
      i2c2Err = true;       }     // hang flag this occurrence for future
    return false;                 // desired condition not happening
  }
  else
  {                               // When I2C op completed as expected
    i2c2Err = false;              // clear consecutive error detector flag
  }

  IFS3bits.MI2C2IF = 0;           // clear the I2C IRQ flag for next use
  return true;                    // and note desired condition did happen
} // end function WaitForMirqI2c2

//--PROCEDURE-------------------------------------------------------------------
//  bool SendStartI2c2()
//  Set start condition and wait for it to complete (resets bus if hang).
//
//  Returns: TRUE of bus event timely, returns FALSE if bus required reset
//------------------------------------------------------------------------------
bool SendStartI2c2()
{
  SEND_START_I2C2();
  return WaitForMirqI2c2();
} // end function SendStartI2c2

//--PROCEDURE-------------------------------------------------------------------
//  bool SendRestartI2c2()
//  Set re-start condition and wait for it to complete (resets bus if hang).
//
//  Returns: TRUE of bus event timely, returns FALSE if bus required reset
//------------------------------------------------------------------------------
bool SendRestartI2c2()
{
  SEND_RESTART_I2C2();
  return WaitForMirqI2c2();
} // end function SendRestartI2c2

//--PROCEDURE-------------------------------------------------------------------
//  bool ReadI2c2(uint8_t * value)
//  Set read condition and wait for byte to be clocked in (resets bus if hang).
//  If not hung, read byte out peripheral and into address provided by caller.
//
//  Returns: TRUE of bus event timely, returns FALSE if bus required reset
//------------------------------------------------------------------------------
bool ReadI2c2(uint8_t * value)
{
  BEGIN_READ_I2C2();
  if (WaitForMirqI2c2())
  {
    READ_I2C2(*value);
    return true;
  }
  else
  {
    return false;
  }
} // end function ReadI2c2

//--PROCEDURE-------------------------------------------------------------------
//  bool WriteI2c2(uint8_t value)
//  Place provided data byte into peripheral transmit register and wait for
//  byte to be clocked out (resets bus if hang).
//
//  Returns: TRUE of bus event timely, returns FALSE if bus required reset
//------------------------------------------------------------------------------
bool WriteI2c2(uint8_t value)
{
  WRITE_I2C2(value);
  return WaitForMirqI2c2();
} // end function WriteI2c2

//--PROCEDURE-------------------------------------------------------------------
//  bool SendNackI2c2()
//  Set NAK condition and wait for it to complete (resets bus if hang).
//
//  Returns: TRUE of bus event timely, returns FALSE if bus required reset
//------------------------------------------------------------------------------
bool SendNackI2c2()
{
  SEND_NACK_I2C2();
  return WaitForMirqI2c2();
} // end function SendNackI2c2

//--PROCEDURE-------------------------------------------------------------------
//  bool SendAckI2c2()
//  Set ACK condition and wait for it to complete (resets bus if hang).
//
//  Returns: TRUE of bus event timely, returns FALSE if bus required reset
//------------------------------------------------------------------------------
bool SendAckI2c2()
{
  SEND_ACK_I2C2();
  return WaitForMirqI2c2();
} // end function SendAckI2c2

//--PROCEDURE-------------------------------------------------------------------
//  bool SendStopI2c2()
//  Set stop condition and wait for it to complete (resets bus if hang).
//
//  Returns: TRUE of bus event timely, returns FALSE if bus required reset
//------------------------------------------------------------------------------
bool SendStopI2c2()
{
  SEND_STOP_I2C2();
  return WaitForMirqI2c2();
} // end function SendStopI2c2


//--INTERRUPT HANDLER-----------------------------------------------------------
// I2C2 interrupt for various I2C2 conditions in Slave mode
void __attribute__((interrupt, no_auto_psv)) _SI2C2Interrupt(void)
{
  // TODO:  Further refine actions required by Slave I2C2 ISR
  IFS3bits.SI2C2IF = 0;   // Clear status flag
  IFS5bits.I2C2BCIF = 0;  // Clear bus collision interrupt flag
}


//--INTERRUPT HANDLER-----------------------------------------------------------
// I2C2 interrupt for various I2C2 conditions in Master mode
void __attribute__((interrupt, no_auto_psv)) _MI2C2Interrupt(void)
{
  // TODO:  Further refine actions required by Master I2C2 ISR
  IFS3bits.MI2C2IF    = 0;    // Clear status flag
  IFS5bits.I2C2BCIF   = 0;    // Clear bus collision interrupt flag
  I2C2STATbits.BCL    = 0;    // Clear bus collision status flag
  I2C2STATbits.IWCOL  = 0;    // Clear write collision status flag
  I2C2STATbits.I2COV  = 0;    // Clear bus read overflow status flag
}
