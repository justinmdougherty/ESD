#ifndef MICRO_DEFS_H
#define MICRO_DEFS_H
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : micro_defs.h
 *
 *  DESCRIPTION   : Declares microcontroller pin useage/definitions for Charade
 *    Red "Lightning" boards making use of the PIC24FJ128GA204 microcontroller
 *
 *  WRITTEN BY    : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2019/08/13, Robert Kirby, NSWC H12
 *      Stub out code for obsolete SI7021 Temperature & Humidity sensor IC
 *    2017/03/24, Robert Kirby, NSWC H12
 *      Expand as part of ESD re-write
 *    2017/01/10, Megan Kozub, NSWC V14
 *      Renamed I2C1 macros to I2C2 and updated registers to match I2C2
 *      Initial development for ESD (based on Lightning's 2016/11/30 version)
 *
 *  REFERENCE DOCUMENTS
 *    1.  PIC24FJ128GA202 Data Sheet (Microchip Technology Inc. DS30010038C)
 *    2.  Lightning Software Design Document (NSWCDD Code Z17)
 *    *.  Abbreviations and Acronyms... (ASME Y14.38-2007)
 *    *.  MIL-STD-12D Military Standard Abbreviations ...
 */
#include <xc.h>
#include <stddef.h>           // for NULL
#include <stdbool.h>          // Includes true/false definitions (as 1 and 0)
#include <stdint.h>           // for C99 fixed size ints and such
#include "stdint_extended.h"  // for BIT0, BYTE, uint8bits_t, etc.
#include "queue.h"            // so can define ISR-protected GET and PUT macros


extern volatile bool doNotSleep;    // bastard extern (allocated in module with
                                    // main() but here so it's always included)
// void (*funcPtr)(void);
// void funcFoo(void);
// void main (void) { funcPtr = funcFoo;   (*funcPtr)(); }
//typedef void (*rom pvfv_t)(void); // define function pointer to "void foo(void)"
typedef void (*pvfv_t)(void);       // define function pointer to "void foo(void)"

#define CTDN_OFF    0               // value for a countdown that is turned off


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// System Level #define Macros
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// this mGlobalIntEnable not really the safest thing to do esp. from w/in ISR
#define mGlobalIntEnable()    { SR &= ~0x00E0; }  // Clr SRbits.IPL2:0 to 0b000
#define mGlobalIntDisable()   { SR |=  0x00E0; }  // Set SRbits.IPL2:0 to 0b111

// Interrupt protected macros for when can use and need protection
#define PROT_Q_PUT(pQ,item)   do { uint16_t volatile oldipl = SRbits.IPL;     \
                                SRbits.IPL2=1; SRbits.IPL1=1; SRbits.IPL0=1;  \
                                QUEUE_PUT(pQ,item);                           \
                                SRbits.IPL = oldipl;                          \
                              } while (0);
#define PROT_Q_GET(pQ,item)   do { uint16_t volatile oldipl = SRbits.IPL;     \
                                SRbits.IPL2=1; SRbits.IPL1=1; SRbits.IPL0=1;  \
                                QUEUE_GET(pQ,item);                           \
                                SRbits.IPL = oldipl;                          \
                              } while (0);
#define PROT_Q_DISCARD(pQ,n)  do { uint16_t volatile oldipl = SRbits.IPL;     \
                                SRbits.IPL2=1; SRbits.IPL1=1; SRbits.IPL0=1;  \
                                QueueDiscard(pQ,(n));                         \
                                SRbits.IPL = oldipl;                          \
                              } while (0);

// define board's operating voltages (TODO - keep updated as design changes)
// LTC2943A Columb Counter has ability to report voltage if decide to use it
#define SYS_REG_XPWR     ((float)9.6)   // Voltage of regulated external power
#define SYS_MAX_VBAT     ((float)10.4)  // Max Vbat board can tolerate
#define SYS_MIN_VBAT     ((float)6.0)   // Min Vbat at which board can operate

// Define system operating frequency - Microcontroller MIPs (FCY)
#define SYS_FREQ        (20000000L)   // 20 MHz crystal at Y100
#define FCY             (SYS_FREQ/2)  // 2 clock cycles per instruction


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Interrupt Priorities used by system/drivers. 7 highest, 4 default, 0 disabled
#define U1RXPRI     4             // UART1 RX
#define U1TXPRI     4             // UART1 TX
#define U1ERPRI     4             // UART1 ERROR priority lower than TX & RX
//#define INT1PRI     4             // External INT1 (Gas Gauge)
#define INT2PRI     4             // External INT2 (PWR button)
#define T2PRI       4             // Timer2 (16-bit)
#define T3PRI       4             // Timer3 (16-bit)


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Peripheral Pin Selects required for UARTs, external INTs, etc.
//    ..._RPINR : register controlling PPS input source for given function
//    ..._RPX   : the 'x' in RPx of pin that provided function input
//    ..._RPOR  : register controlling PPS output assigned for given RPx pin
//    ..._OFN   : output function (number) to be assigned to RPx output pin
//------------------------------------------------------------------------------
// INT1 : used for Gas Gauge (which is active LO)
#define INT1_RPINR  RPINR0bits.INT1R    // PPS input register for given function
#define INT1_RPX    (4)                 // RPx pin that provided function input
// INT2 : used for PWR button
#define INT2_RPINR  RPINR1bits.INT2R
#define INT2_RPX    (6)
//  OC1 : Free-Running Output Compare Pulse
#define OC1_RPOR    RPOR4bits.RP8R
#define OC1_OFN     (13)
//  SPI1 - connect to DOGS102W LCD, which is a one-way SPI (MOSI-only) interface
#define SCK1_RPOR   RPOR6bits.RP12R
#define SCK1_OFN    (8)
#define SDO1_RPOR   RPOR2bits.RP5R
#define SDO1_OFN    (7)
//#define SS1_RPOR    RPOR4bits.RP9R    // SPI Slave Select done manually
//#define SS1_OFN     (9)
//  UART1 - connect to Lightning Interface (required by uart1_queued.c)
#define UART1_RX_TRIG_BYTE 0xB6   // RX trigger on <PILCROW> from Lightning
#define U1RX_RPINR  RPINR18bits.U1RXR
#define U1RX_RPX    (14)
#define U1TX_RPOR   RPOR7bits.RP15R     // ensure WPU matches
#define U1TX_OFN    (3)


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//  I2C2 - pin control registers (required by i2c2.c for manual bus reset)
#define I2C2_SCL_TRIS     TRISBbits.TRISB3 //SCL output
#define I2C2_SCL_LAT      LATBbits.LATB3   //SCL write
#define I2C2_SCL_PORT     PORTBbits.RB3    //SCL read
#define I2C2_SDA_TRIS     TRISBbits.TRISB2 //SDA output
#define I2C2_SDA_LAT      LATBbits.LATB2   //SDA write
#define I2C2_SDA_PORT     PORTBbits.RB2    //SDA read
//  I2C addresses used in Lightning
#define INVLD_I2C_ADRS        0x01  // address not used by any device on board
#define MC24AA512_WRT_ADRS    0xA0  // EEPROM writes
#define MC24AA512_RD_ADRS     0xA1  // EEPROM reads
//#define SI7021_I2C_WRT_ADRS   0x80  // Temperature & Humidity writes
//#define SI7021_I2C_RD_ADRS    0x81  // Temperature & Humidity reads
#define LTC2943_I2C_WRT_ADDR  0xC8  // Columb Counter writes
#define LTC2943_I2C_RD_ADDR   0xC9  // Columb Counter reads


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Power Source Indicator Signal from LTC4417 Power Path Controller
#define PWRSCEIND       PORTAbits.RA4     // 0 == ExtPwr and 1 == BatPwr
#define IS_ON_EXT_PWR   ( ! PWRSCEIND)
#define IS_ON_BAT_PWR   (PWRSCEIND)
#define PWRSCEIND_CNE   CNEN1bits.CN0IE   // TODO implement Interrupt-On-Change
#define PWRSCEIND_IF    IFS1bits.CNIF     // Notification for XPWRSELn signal
#define PWRSCEIND_IE    IEC1bits.CNIE     // even when asleep (PWR-OFF-ENT)
#define PWRSCEIND_IP    IPC4bits.CNIP     // to keep gas gauge more correct


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// LTC2943 Columb Counter INT on RB4 (could use falling edge INT1 per PPS above)
#define GASGA_ALCCn     PORBTbitts.RB4


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Lightning interface definitions
#define LTG_VLTG_EN               LATBbits.LATB7
#define LTG_INIT_UART()           OpenUart1()
#define LTG_GET_UART_STAT()       (GetUart1Status())
#define LTG_CLR_UART_ERR()        (ClearUart1StatusFlag(ALLERR))
#define LTG_CLOSE_UART()          CloseUart1()
#define LTG_UART_IS_CLOSED        ( ! U1MODEbits.UARTEN)
#define LTG_READ_NONBLOCKING(q)   StartReadUart1(q,0)
#define LTG_RX_ERR_SET            (GetUart1Status().errors)
#define LTG_RX_TRG_SET            (GetUart1Status().rxTrig)
#define LTG_DECR_RX_TRG()         (ClearUart1StatusFlag(TRIGCNT))
#define LTG_STOP_READ()           StopReadUart1()
#define LTG_WRITE_COMPLETE        (GetUart1IsWriteDone())
#define LTG_WRITE_NONBLOCKING(q)  StartWriteUart1(q)
#define LTG_WRITE_BLOCKING(q)     do {                                        \
                                     LTG_WRITE_NONBLOCKING(q);                \
                                     while (!LTG_WRITE_COMPLETE) { ; }        \
                                   } while (0);


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// UC1701 LCD Display (DOGS102W) with LED Backlight (LED39x41)
// UC1701 SPI Chip Select line manually controlled output on B9 (Active Low)
#define LCD_CS0           LATBbits.LATB9
//#define LCD_SELECT()      { LCD_CS0 = 0; }
//#define LCD_DESELECT()    { LCD_CS0 = 1; }
// UC1701 reset line control output on A1 (Active Low)
#define LCD_RST           LATAbits.LATA1
//#define LCD_RESET()       { LCD_RST = 0; }
//#define LCD_ACTIVATE()    { LCD_RST = 1; }
// UC1701 data mode enable control output on A0 (CMD = 0, DATA = 1)
#define LCD_CD            LATAbits.LATA0
//#define LCD_DATA_MODE()   { LCD_CD = 1; }
//#define LCD_CMD_MODE()    { LCD_CD = 0; }
// LCD's LED Backlight driven by OC1 free-running pulse train
#define LED_PWM_DRV       LATBbits.RB8  // done via OC1 on PPS set by OC1_RPOR
//#define SET_BACKLIGHT_OFF()   { STOP_OC1_PULSE(); }
//#define SET_BACKLIGHT_NVG()   { ConfigOc1Pulse(OCP_0PCT25); START_OC1_PULSE(); }
//#define SET_BACKLIGHT_LOW()   { ConfigOc1Pulse(OCP_1PCT31); START_OC1_PULSE(); }
//#define SET_BACKLIGHT_MED()   { ConfigOc1Pulse(OCP_5PCT19); START_OC1_PULSE(); }
//#define SET_BACKLIGHT_HIGH()  { ConfigOc1Pulse(OCP_10PCT4); START_OC1_PULSE(); }


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Keypad with 4 buttons (PWR, UP_ARROW, ENT, RT_ARROW)
#define KP_C0         LATBbits.LATB10   // Drive the two columns
#define KP_C1         LATBbits.LATB11
#define KP_R0         PORTBbits.RB6     // Read the two rows
#define KP_R1         PORTBbits.RB13


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ANS: Analog Select         - 0 = digital,  1 = ANALOG
// DIR: Digital I/O Direction - 0 = OUTPUT,   1 = INPUT
// LAT: I/O Pin Latch         - 0 = GND,      1 = Vcc (Vpull_up if open drain)
// ODO: Open Drain Output     - 0 = digital,  1 = Open Drain
// WPU: Input Weak Pull-Up    - 0 = disable,  1 = ENABLE, PORTx strewn over WPUy
// WPD: Input Weak Pull-Down  - 0 = disable,  1 = ENABLE, PORTx strewn over WPDy
//(BITF|BITE|BITD|BITC|BITB|BITA|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
//(                                     NULL                                      )
// PORTA (pull-up / pull-down controls scattered across CNPUx and CNPDx below)
//    RA0 LCD_CD;   RA1 LCD_RST;   RA2:3 POSC;   RA4 XPWRSELn;
#define INIT_RA_ANS   (NULL)
#define INIT_RA_DIR   (BIT4)
#define INIT_RA_LAT   (NULL)
#define INIT_RA_ODO   (NULL)
// PORTB (pull-up / pull-down controls scattered across CNPUx and CNPDx below)
//    RB0:1 ICSP;   RB2:3 I2C2;   RB4 CN1 GasGaugeINT;   RB5 SPI1 MOSI;  RB6 R0;
//    RB7 3V3_LTNG_EN;   RB8 OC1 Backlight;   RB9 LCD CS;   RB10 C0;   RB11 C1;
//    RB12 SPI1 CLK;   RB13 R1;   RB14:15 UART1/LTNG
#define INIT_RB_ANS   (NULL)
#define INIT_RB_DIR   (BITE|BITD|BITC|BIT6|BIT4|BIT3|BIT2)
#define INIT_RB_LAT   (NULL)
#define INIT_RB_ODO   (NULL)
// CN13/RB13 is keypad R1, CN24/RB6 is keypad R0 -- both need weak pull-ups
// no CN11/RB15 weak pull-up on UART TX pin when off as it back-powers LTNG uC
#define CN1500_WPU  (BITD/*|BITB*/)           // CN13/RB13 R1; no CN11/RB15 UART
#define CN3116_WPU  (BIT8)                    // CN24/RB6  R0
#define CN4732_WPU  (NULL)
// No pins need their weak pull-down enabled
#define CN1500_WPD  (NULL)
#define CN3116_WPD  (NULL)
#define CN4732_WPD  (NULL)


#endif // MICRO_DEFS_H
