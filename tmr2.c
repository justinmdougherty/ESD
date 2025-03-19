////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : tmr2.c
 *
 *  DESCRIPTION   : Define 16-bit synchronous TIMER2 driver.  All service
 *    times are minimum (TIMER = request -0/+timer resolution).
 *
 *    (1) void  InitTmr2Driver(void)
 *    (2) bool  ReqTmr2Srvc(tmr2srvc_t period, pvfv_t callback)
 *    (3) void  EndTmr2Srvc(void)
 *    (*) void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
 *
 *  WRITTEN BY    : Robert Kirby, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2018/09/21, Robert Kirby, NSWC H12
 *      Refactor sysErrFlags to esdErrFlags as it doesn't cover Ltng errors
 *    2017-02-10, Robert Kirby, NSWC H12
 *      Initial development (based off tmr5 driver of 2017/02/28)
 *
 *  REFERENCE DOCUMENTS
 *    1.
 */
#include <xc.h>
#include <stdbool.h>            // Includes true/false definition
#include "tmr2.h"
#include "main.h"               // for esdErrFlags
#include "micro_defs.h"         // for T5PRI


static volatile pvfv_t tmr2cb;  // callback function for TIMER2 service


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void InitTmr2Driver(void)
//  This should be called before making use of any Timer 2 services, typically
//  when initializing the system.  It resets the timer, but to save power does
//  not start it.  It also NULLs service callback function pointer.
//  The timer will be started/stopped as needed when services are [not] used.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void InitTmr2Driver(void)
{
  PMD1bits.T2MD     = 1;        // Temporarily de-clock/reset Timer-2 peripheral
  IEC0bits.T2IE     = 0;        // Ensure Timer-2's interrupt disabled
  IPC1bits.T2IP     = T2PRI;    // Set IRQ priority per micro_defs.h
  tmr2cb            = NULL;     // No callback routine has been set

  PMD1bits.T2MD     = 0;        // Now clock Timer-2 peripheral
  T2CONbits.T32     = 0;        // run T2 and T3 as independent 16-bit timers
  T2CONbits.TCKPS0  = T2S_PS0;  // Set Timer-2 prescaler of Fcy as defined
  T2CONbits.TCKPS1  = T2S_PS1;  // in tmr2.h
} // end routine InitTmr2Driver


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool ReqTmr2Srvc(tmr2srvc_t period, pvfv_t callback)
//  Provides non-blocking timer services for predefined/enumerated periods. When
//  the requested period expires, the timer ISR will call the provided callback
//  function.  This will continue until EndTimerB0Service(period) is called.
//  If the timer service is already being used, the request will fail.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool ReqTmr2Srvc(tmr2srvc_t period, pvfv_t callback)
{
  if (NULL == tmr2cb)
  {                               // Only when not already using Timer-2
    tmr2cb          = callback;   // remember what to do when timer expires
    PR2             = period;     // and set the period for timer to expire.
    IFS0bits.T2IF   = 0;          // Clear the Timer-2 interrupt flag
    IEC0bits.T2IE   = 1;          // Enable Timer-2's interrupt
    TMR2            = 0x0000;     // Ensure full-length period first time-out
    T2CONbits.TON   = 1;          // Turn on Timer-2
    return true;                  // Return true to indicate service subscribed
  }
  else
  {
    esdErrFlags.fw  = 1;          // note firmware did something stupid
    return false;                 // Return false as timer already in use
  }
} // end function ReqTmr2Srvc


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void EndTmr2Srvc(void)
//  Halts periodic timer service and clears module callback pointer attribute.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void EndTmr2Srvc(void)
{
  IEC0bits.T2IE = 0;            // Disable Timer-2's interrupt
  T2CONbits.TON = 0;            // Turn off Timer-2
  tmr2cb        = NULL;         // Clear pointer to callback routine
} // end routine EndTmr2Srvc


//++ INTERRUPT SERVICE ROUTINE +++++++++++++++++++++++++++++++++++++++++++++++++
//  void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
//  Clears TIMER2's interrupt and calls provided callback function.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : callback function provided in to ReqTmr2Srvc(...)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
  // Note:  TMR2 register automatically resets to 0 upon interrupt
  IFS0bits.T2IF = 0;            // Clear T2 interrupt status flag
  if (NULL != tmr2cb)       {   // When a callback has been provided
    tmr2cb();               }   // call it to perform specific tasking
  else                      {   // otherwise, why are we even here -- so
    esdErrFlags.fw = 1;     }   // note firmware doing something stupid
} // end ISR
