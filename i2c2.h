#ifndef I2C2_H
#define	I2C2_H
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : i2c2.h
 *
 *  DESCRIPTION   : definitions for using the PIC24FJ128GA202's I2C2 module in
 *    a polled mode by invoking the functions or macros below.  The functions
 *    are safer as all the required waits are back-stopped by WDT and so will
 *    not hang forever like the WAIT_FOR_... macros may.
 *
 *  NOTE - this requires WDT (Watch Dog Timer) for I2C time-out errors
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
 *
 *  USE - invoke combination of macros and functions
 *    ResetBusI2c2();       // Output bus signals to free any hung device(s)
 *    INIT_I2C2();          // Clocks I2C, must INIT before OPEN (in sys init?)
 *    OPEN_I2C2();          // must open/enable before any operational use
 *    WaitForIdleI2c2();    // typically make sure bus not busy/hung
 *    SendStartI2c2();      // then start I2C operations
 *    ...                   // reading, writing, ACKing, NAKing, etc
 *    SendStopI2c2();       // and terminating individual transfers
 *    ...                   // Typically leave I2C open for future transfers.
 *    CLOSE_I2C2();         // This will disable I2C, can just OPEN to use again
 *    DISABLE_I2C2();       // This will de-clock I2C, must INIT to use again.
 *
 *  WRITTEN BY    : nicholus.sunshine, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Correct I2C2MD references, remove extraneous includes
 *      Drop using IRQ as needs a semaphore to work correctly and not worth it
 *    2017/01/31, Megan Kozub, NSWC V14
 *      Modified OPEN_I2C2() & CLOSE_I2C2() to enable/disable master I2C2
 *      interrupts respectively
 *      Removed erroneous macros
 *    2017/01/10, Megan Kozub, NSWC V14
 *      Modified for I2C2: Modified for I2C2: changed refs from I2C1 to I2C2,
 *      updated procedure and variable names and comments to "I2C2",
 *      updated to I2C2 registers (IFS3 vice IFS1, IPC12 vice IPC4) and
 *      associated I2C2 bits within utilized registers
 *    2015/10/05, Robert Kirby, NSWC Z17
 *      Refactored damn near every #define and made other style changes to match
 *      my thought style.
 *      Fixed WAIT_FOR_IDLE_I2C1 by changing all && to ||.
 *    2015/05/05, nicholus.sunshine, NSWC Z17
 *      Initial development
 *
 *  REFERENCE DOCUMENTS
 *    1. PIC24FJ128GA204 Data Sheet (Microchip Technology Inc. DS30010038C)
 */
#include <xc.h>
#include <stdbool.h>          // Includes true/false definitions (as 1 and 0)
#include <stdint.h>           // for C99 fixed size ints and such
#include "micro_defs.h"       // get FCY
#include <libpic30.h>         // For delay functions MUST follow define of FCY

#define I2C_100KHZ_DELAY()  __delay_us(5) // time of 1/2 clock cycle (HI or LO)

// I2CxBRG = ((FCY/2)/FSCL - 1)  or maybe -2, run the numbers & check on o-scope
#define I2Cx_BRG_100        ((FCY/2/100000) - 1)  // 49  Baud Rate Generator
#define I2Cx_BRG_400        ((FCY/2/400000) - 2)  // 12  Baud Rate Generator
#define I2C2_MY_ADRS        INVLD_I2C_ADRS


// Functions (return true if successful, else false)
void ResetBusI2c2();              // Bit-bang out conditions to clear bus
bool WaitForIdleI2c2();           // with built in ResetBusI2c2
bool WaitForMirqI2c2();           // with built in ResetBusI2c2
bool SendStartI2c2();             // with built in WaitForMirqI2c2
bool SendRestartI2c2();           // with built in WaitForMirqI2c2
bool ReadI2c2(uint8_t * value);   // with built in BEGIN_READ & WaitForMirqI2c2
bool WriteI2c2(uint8_t value);    // with built in WaitForMirqI2c2
bool SendNackI2c2();              // with built in WaitForMirqI2c2
bool SendAckI2c2();               // with built in WaitForMirqI2c2
bool SendStopI2c2();              // with built in WaitForMirqI2c2

// Macro Definitions
#define INIT_I2C2()             { PMD3bits.I2C2MD    = 0;                     \
                                  IPC12bits.SI2C2IP  = 2; /*Slv IRQ priority*/\
                                  IPC12bits.MI2C2IP  = 2; /*Mst IRQ priority*/\
                                  IPC21bits.I2C2BCIP = 2; /*BC IRQ priority */\
                                  I2C2BRG  = I2Cx_BRG_400;                    \
                                  I2C2ADD  = I2C2_MY_ADRS;                    \
                                  I2C2MSK  = 0x00;                            \
                                  I2C2CONL = 0x8800;                          \
                                }
#define OPEN_I2C2()             { I2C2CONLbits.I2CEN = 1; /*Enable I2Cx     */\
                                  IFS3bits.SI2C2IF   = 0; /*Clr Slv IRQ Flag*/\
                                  IFS3bits.MI2C2IF   = 0; /*Clr Mst IRQ Flag*/\
                                /*IEC3bits.SI2C2IE   = 1;   enable slv IRQ  */\
                                /*IEC3bits.MI2C2IE   = 1;   enable mst IRQ  */\
                                /*IEC5bits.I2C2BCIE  = 1;   enable b.c. IRQ */\
                                }
#define CLOSE_I2C2()            { I2C2CONLbits.I2CEN = 0;                     \
                                /*IEC3bits.MI2C2IE   = 0;   disable mst IRQ */\
                                }
#define DISABLE_I2C2()          { PMD3bits.I2C2MD = 1; }

#define SEND_START_I2C2()       { I2C2CONLbits.SEN = 1; }
#define SEND_RESTART_I2C2()     { I2C2CONLbits.RSEN = 1; }
#define SEND_STOP_I2C2()        { I2C2CONLbits.PEN = 1; }
#define BEGIN_READ_I2C2()       { I2C2CONLbits.RCEN = 1; }
#define READ_I2C2( value )      { value = I2C2RCV; }
#define WRITE_I2C2( value )     { I2C2TRN = value; }
#define SEND_NACK_I2C2()        { I2C2CONLbits.ACKDT = 1;                     \
                                  I2C2CONLbits.ACKEN = 1;                     \
                                }
#define SEND_ACK_I2C2()         { I2C2CONLbits.ACKDT = 0;                     \
                                  I2C2CONLbits.ACKEN = 1;                     \
                                }

#define CLEAR_MASTER_INT_I2C2() { IFS3bits.MI2C2IF = 0; }


#endif	// I2C2_H
