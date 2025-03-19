/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__PIC24E__)
    	#include <p24Exxxx.h>
    #elif defined (__PIC24F__)||defined (__PIC24FK__)
	#include <p24Fxxxx.h>
    #elif defined(__PIC24H__)
	#include <p24Hxxxx.h>
    #endif
#endif

#include <stdint.h>        /* Includes uint16_t definition */
#include <stdbool.h>       /* Includes true/false definition */

/******************************************************************************/
/* Trap Function Prototypes                                                   */
/******************************************************************************/

/* <Other function prototypes for debugging trap code may be inserted here>   */

/* Use if INTCON2 ALTIVT=1 */
void __attribute__((interrupt,no_auto_psv)) _OscillatorFail( void );
void __attribute__((interrupt,no_auto_psv)) _AddressError( void );
void __attribute__((interrupt,no_auto_psv)) _StackError( void );
void __attribute__((interrupt,no_auto_psv)) _MathError( void );

#if defined(__PIC24F__)||defined(__PIC24H__)

/* Use if INTCON2 ALTIVT=0 */
void __attribute__((interrupt,no_auto_psv)) _AltOscillatorFail( void );
void __attribute__((interrupt,no_auto_psv)) _AltAddressError( void );
void __attribute__((interrupt,no_auto_psv)) _AltStackError( void );
void __attribute__((interrupt,no_auto_psv)) _AltMathError( void );

#endif

/* Default interrupt handler */
void __attribute__((interrupt,no_auto_psv)) _DefaultInterrupt( void );

/******************************************************************************/
/* Trap Handling                                                              */
/*                                                                            */
/* These trap routines simply ensure that the device continuously loops       */
/* within each routine.  Users who actually experience one of these traps     */
/* can add code to handle the error.  Some basic examples for trap code,      */
/* including assembly routines that process trap sources, are available at    */
/* www.microchip.com/codeexamples                                             */
/******************************************************************************/

/* Primary (non-alternate) address error trap function declarations */
void __attribute__((interrupt,no_auto_psv)) _OscillatorFail( void )
{
  INTCON1bits.OSCFAIL = 0;        /* Clear the trap flag */
  //while(1);
  __asm__ volatile ("reset"); 
}

void __attribute__((interrupt,no_auto_psv)) _AddressError( void )
{
  INTCON1bits.ADDRERR = 0;        /* Clear the trap flag */
  //while (1);
  __asm__ volatile ("reset"); 
}
void __attribute__((interrupt,no_auto_psv)) _StackError( void )
{
  INTCON1bits.STKERR = 0;         /* Clear the trap flag */
  //while (1);
  __asm__ volatile ("reset"); 
}

void __attribute__((interrupt,no_auto_psv)) _MathError( void )
{
  INTCON1bits.MATHERR = 0;        /* Clear the trap flag */
  //while (1);
  __asm__ volatile ("reset"); 
}

#if defined(__PIC24F__)||defined(__PIC24H__)

/* Alternate address error trap function declarations */
void __attribute__((interrupt,no_auto_psv)) _AltOscillatorFail( void )
{
  INTCON1bits.OSCFAIL = 0;        /* Clear the trap flag */
  //while (1);
  __asm__ volatile ("reset"); 
}

void __attribute__((interrupt,no_auto_psv)) _AltAddressError( void )
{
  INTCON1bits.ADDRERR = 0;        /* Clear the trap flag */
  //while (1);
  __asm__ volatile ("reset"); 
}

void __attribute__((interrupt,no_auto_psv)) _AltStackError( void )
{
  INTCON1bits.STKERR = 0;         /* Clear the trap flag */
  //while (1);
  __asm__ volatile ("reset"); 
}

void __attribute__((interrupt,no_auto_psv)) _AltMathError( void )
{
  INTCON1bits.MATHERR = 0;        /* Clear the trap flag */
  //while (1);
  __asm__ volatile ("reset"); 
}

#endif
