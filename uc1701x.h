#ifndef UC1701X_H__
#define	UC1701X_H__
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : uc1701x.h
 *
 *  DESCRIPTION   : Declares interface to EA DOGS102W-6 UC1701 LCD display
 *    via the SPI1 module on the PIC24FJ128GA202. This display is 102x64 pixels.
 *    The 64 rows are controlled by 'pages' that are 8-pixels tall, resulting in
 *    8 'pages'.  So symbols are typically some multiple of 8 pixels tall.
 *    Any of the 102 columns can be specifically addressed.
 *
 *      (1) void InitUc1701x(void)
 *      (2) void UC1701XWrite( uint8_t value )
 *      (3) void UC1701XWriteData( uint8_t data )
 *      (4) bool UC1701XSetColumnAddress( uint8_t ca )
 *      (5) void UC1701XSetPowerControl( bool pc0, bool pc1, bool pc2)
 *      (6) bool UC1701XSetScrollLine( uint8_t sl )
 *      (7) bool UC1701XSetPageAddress( uint8_t pa )
 *      (8) bool UC1701XSetVLCDResistorRatio( uint8_t pc )
 *      (9) bool UC1701XSetElectronicVolume( uint8_t pm )
 *     (10) void UC1701XSetAllPixelsOn( bool dc1 )
 *     (11) void UC1701XSetInverseDisplay( bool dc0 )
 *     (12) void UC1701XSetDisplayEnable( bool dc2 )
 *     (13) void UC1701XSetSEGDirection( bool lc0 )
 *     (14) void UC1701XSetCOMDirection( bool lc1 )
 *     (15) void UC1701XSystemReset()
 *     (16) void UC1701XSetAdvProgCntl0( bool tc, bool wc, bool wp )
 *     (17) void UC1701XSetLCDBiasRatio( bool br )
 *     (18) LCD_DISPLAY_MODE()
 *     (19) LCD_SLEEP_MODE()
 *     (20) SET_BKLT_OFF()
 *     (21) SET_BKLT_NVG()
 *     (22) SET_BKLT_LOW()
 *     (23) SET_BKLT_MED()
 *     (24) SET_BKLT_HIGH()
 *
 *  USE:
 *
 *  WRITTEN BY    : Nicholus Sunshine, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2019/07/22, Robert Kirby, NSWC H12
 *      Change NVG back light from 0.02% to 0.12% to make it readable w/o NVGs
 *      (but still NVG friendly) since focal length of NVGs is such that cannot
 *      read small font anyway
 *    2018/09/21, Robert Kirby, NSWC H12
 *      Add NVG and remove MED backlight setting (now have HIGH, LOW, NVG, OFF)
 *      after NavSpecWarCom N4 NVG office and NSW G3 LT Bishop evaluated four
 *      options and all picked the dimmest
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Comments on what Nick developed for ESD
 *
 *  REFERENCE DOCUMENTS
 *    1.  DOGS Graphic Series 102x6 Dots Data Sheet 06.2013
 */
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>


void InitUc1701x(void);


/* UC1701XWrite
 * In: value: command or data byte to send to LCD */
void UC1701XWrite( uint8_t value );

/* UC1701XWriteData
 * In: data: data byte to be written to LCD SRAM  */
void UC1701XWriteData( uint8_t data );

/* UC1701XSetColumnAddress
 * In: ca: column address (valid input 0-131)
 * Out: Returns bool value with success/error code (false=failure, true=success)
 * Note: Will return false if ca is outside of valid range */
bool UC1701XSetColumnAddress( uint8_t ca );

/* UC1701XSetPowerControl
 * In: pc0: voltage follower on (1) / off (0)
 *     pc1: voltage regulator on (1) / off (0)
 *     pc2: boost on (1) / off (0)
 * Out: Returns bool value with success/error code (false=failure,
 * true=success) */
void UC1701XSetPowerControl( bool pc0, bool pc1, bool pc2);

/* UC1701XSetScrollLine
 * In: sl: scroll line (valid input 0-63)
 * Out: Returns bool value with success/error code (false=failure, true=success)
 * Note: Will return false if sl is outside of valid range */
bool UC1701XSetScrollLine( uint8_t sl );

/* UC1701XSetPageAddress
 * In: pa: page address (valid input 0-8)
 * Out: Returns bool value with success/error code (false=failure, true=success)
 * Note: Will return false if pa is outside of valid range */
bool UC1701XSetPageAddress( uint8_t pa );

/* UC1701XSetVLCDResistorRatio
 * In: pc: internal resistor ratio (valid input 0-7)
 * Out: Returns bool value with success/error code (false=failure, true=success)
 * Note:  Will return false if pc is outside of valid range  */
bool UC1701XSetVLCDResistorRatio( uint8_t pc );

/* UC1701XSetElectronicVolume
 * In: pm: electronic volume for VLCD voltage regulator to adjust contrast of
 * LCD panel display (valid input 0-63)
 * Out: Returns int value with success/error code (false=failure, true=success)
 * Note:  Will return false if pm is outside of valid range  */
bool UC1701XSetElectronicVolume( uint8_t pm );

/* UC1701XSetAllPixelsOn
 * In: dc1: false: show SRAM content; true: set all SEG drivers to ON
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetAllPixelsOn( bool dc1 );

/* UC1701XSetInverseDisplay
 * In: dc0: false: show normal SRAM content; true: show inverse SRAM content
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetInverseDisplay( bool dc0 );

/* UC1701XSetDisplayEnable
 * In: dc2: false: disable display (sleep); true: enable display (exit from
 * sleep)
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetDisplayEnable( bool dc2 );

/* UC1701XSetSEGDirection
 * In: lc0: false: normal SEG 0..131; true: mirror SEG 131-0.
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetSEGDirection( bool lc0 );

/* UC1701XSetCOMDirection
 * In: lc1: false: normal COM 0..63; true: mirror COM 63-0
 * Out: Returns int value with success/error code (0=failure, 1=success) */
void UC1701XSetCOMDirection( bool lc1 );

/* UC1701XSystemReset
 * All control registers will be reset to their default values.
 * Data stored in RAM will not be affected. */
void UC1701XSystemReset();

/* UC1701XSetAdvProgCntl0
 * In: tc: Temperature compensation curve coefficient false = -0.05%/degree C;
 *     true = -0.11%/degree C
 *     wc: Column wrap around false=OFF; true=ON
 *     wp: Page wrap around false=OFF; true=ON */
void UC1701XSetAdvProgCntl0( bool tc, bool wc, bool wp );

/* UC1701XSetLCDBiasRatio
 * In: br: Bias ratio, false = 1/9, true = 1/7 */
void UC1701XSetLCDBiasRatio( bool br );


#define LCD_DISPLAY_MODE()  {                                   \
                              UC1701XSetDisplayEnable(true);    \
                              UC1701XSetAllPixelsOn(false);     \
                            }
#define LCD_SLEEP_MODE()    {                                   \
                              UC1701XSetDisplayEnable(false);   \
                              UC1701XSetAllPixelsOn(true);      \
                            }
// Back light control via PWM OCM & OC1RS manipulation
// Duty cycle of PWM driving backlight = (OC1RS / 65536) as PR1 == 0xFFFF
#define SET_BKLT_OFF()      { /* Output Compare channel disabled, i.e. off */ \
                              UC1701XSetInverseDisplay(false);                \
                              OC1CON1bits.OCM = 0b000;                        \
                            }
#define SET_BKLT_NVG()      { /* OC enabled, Duty cycle = 0.12% (nvg)      */ \
                              UC1701XSetInverseDisplay(true);                 \
                              OC1CON1bits.OCM = 0b101;                        \
                              OC1RS = 80;                                     \
                            }
#define SET_BKLT_LOW()      { /* OC enabled, Duty cycle = 1.28% (low)      */ \
                              UC1701XSetInverseDisplay(false);                \
                              OC1CON1bits.OCM = 0b101;                        \
                              OC1RS = 856;                                    \
                            }
#define SET_BKLT_HIGH()     { /* OC enabled, Duty cycle = 10.3% (high)     */ \
                              UC1701XSetInverseDisplay(false);                \
                              OC1CON1bits.OCM = 0b101;                        \
                              OC1RS = 6808;                                   \
                            }
#if (0)
#define SET_BKLT_MED()      { /* OC enabled, Duty cycle = 5.1% (med)       */ \
                              UC1701XSetInverseDisplay(false);                \
                              OC1CON1bits.OCM = 0b101;                        \
                              OC1RS = 3404;                                   \
                            }
#define SET_BKLT_NVG()      { /* OC enabled, Duty cycle = 0.24% (nvg)      */ \
                              UC1701XSetInverseDisplay(true);                 \
                              OC1CON1bits.OCM = 0b101;                        \
                              OC1RS = 160;                                    \
                            }
#define SET_BKLT_NVG()      { /* OC enabled, Duty cycle = 0.12% (nvg)      */ \
                              UC1701XSetInverseDisplay(true);                 \
                              OC1CON1bits.OCM = 0b101;                        \
                              OC1RS = 80;                                     \
                            }
#define SET_BKLT_NVG()      { /* OC enabled, Duty cycle = 0.06% (nvg)      */ \
                              UC1701XSetInverseDisplay(true);                 \
                              OC1CON1bits.OCM = 0b101;                        \
                              OC1RS = 40;                                     \
                            }
#define SET_BKLT_NVG()      { /* OC enabled, Duty cycle = 0.02% (nvg)      */ \
                              UC1701XSetInverseDisplay(true);                 \
                              OC1CON1bits.OCM = 0b101;                        \
                              OC1RS = 13;                                     \
                            }
#endif

#endif	/* UC1701X_H__ */
