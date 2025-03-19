////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     :  interrupts.c
 *
 *  DESCRIPTION : Define various ISRs and _DefaultInterrupt
 *
 *    (1) _U1RXInterrupt( void )
 *    (2) _U1TXInterrupt( void )
 *    (3) _U1ErrInterrupt( void )
 *    (4) _T2Interrupt( void )
 *    (5) _T3Interrupt( void )
 *    (6) _T5Interrupt( void ) // for 32-bit TIMER4/5
 *    (7) _INT1Interrupt( void )
 *    (8) _INT2Interrupt( void )
 *    (9) _DefaultInterrupt(void)
 *
 *  MODIFICATIONS (in reverse chronological order)
 *    2018/09/21, Robert Kirby, NSWC H12
 *      Refactor sysErrFlags to esdErrFlags as it doesn't cover Ltng errors
 *    2017/02/03, Robert Kirby, NSWC H12
 *      Move _DefaultInterrupt here from traps.c; move all other IRQs to modules
 *    2017/01/31, Megan Kozub, NSWC V14
 *      Modified _MI2C2Interrupt to utilize CLEAR_MASTER_INT_I2C2 macro
 */
#include <xc.h>


/******************************************************************************/
/* Interrupt Vector Options                                                   */
/******************************************************************************/
/*
 * Refer to the C30 (MPLAB C Compiler for PIC24F MCUs and dsPIC33F DSCs) User
 * Guide for an up to date list of the available interrupt options.
 * Alternately these names can be pulled from the device linker scripts.
 *
 * PIC24F Primary Interrupt Vector Names:
 *
 * _INT0Interrupt      _IC4Interrupt
 * _IC1Interrupt       _IC5Interrupt
 * _OC1Interrupt       _IC6Interrupt
 * _T1Interrupt        _OC5Interrupt
 * _Interrupt4         _OC6Interrupt
 * _IC2Interrupt       _OC7Interrupt
 * _OC2Interrupt       _OC8Interrupt
 * _T2Interrupt        _PMPInterrupt
 * _T3Interrupt        _SI2C2Interrupt
 * _SPI1ErrInterrupt   _MI2C2Interrupt
 * _SPI1Interrupt      _INT3Interrupt
 * _U1RXInterrupt      _INT4Interrupt
 * _U1TXInterrupt      _RTCCInterrupt
 * _ADC1Interrupt      _U1ErrInterrupt
 * _SI2C1Interrupt     _U2ErrInterrupt
 * _MI2C1Interrupt     _CRCInterrupt
 * _CompInterrupt      _LVDInterrupt
 * _CNInterrupt        _CTMUInterrupt
 * _INT1Interrupt      _U3ErrInterrupt
 * _IC7Interrupt       _U3RXInterrupt
 * _IC8Interrupt       _U3TXInterrupt
 * _OC3Interrupt       _SI2C3Interrupt
 * _OC4Interrupt       _MI2C3Interrupt
 * _T4Interrupt        _U4ErrInterrupt
 * _T5Interrupt        _U4RXInterrupt
 * _INT2Interrupt      _U4TXInterrupt
 * _U2RXInterrupt      _SPI3ErrInterrupt
 * _U2TXInterrupt      _SPI3Interrupt
 * _SPI2ErrInterrupt   _OC9Interrupt
 * _SPI2Interrupt      _IC9Interrupt
 * _IC3Interrupt
 *
 * For alternate interrupt vector naming, simply add 'Alt' between the prim.
 * interrupt vector name '_' and the first character of the primary interrupt
 * vector name.  There are no Alternate or 'Alt' vectors for the 24E family.
 *
 * For example, the vector name _ADC2Interrupt becomes _AltADC2Interrupt in
 * the alternate vector table.
 *
 * Example Syntax:
 *
 * void __attribute__((interrupt,auto_psv)) <Vector Name>(void)
 * {
 *      <Clear Interrupt Flag>
 * }
 *
 * For more comprehensive interrupt examples refer to the C30 (MPLAB C
 * Compiler for PIC24 MCUs and dsPIC DSCs) User Guide in the
 * <compiler installation directory>/doc directory for the latest compiler
 * release.
 ******************************************************************************/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Default Interrupt Handler
//
// This executes when an interrupt occurs for an interrupt source with an
// improperly defined or undefined interrupt handling routine.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __attribute__((interrupt,no_auto_psv)) _DefaultInterrupt(void)
{
  //esdErrFlags.fw = 1;             // set flag so we know of problem
  //while(1);
  __asm__ volatile ("reset");

}
