#ifndef TIMER_2_H__
#define TIMER_2_H__
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : tmr2.h
 *
 *  DESCRIPTION   : Declares 16-bit synchronous TIMER2 driver.  All service
 *    times are minimum (TIMER = request -0/+timer resolution).
 *
 *    (1) void  InitTmr2Driver(void)
 *    (2) bool  ReqTmr2Srvc(tmr2srvc_t period, pvfv_t callback)
 *    (3) bool  EndTmr2Srvc(void)
 *
 *  WRITTEN BY    : Robert Kirby, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2017-02-01, Robert Kirby, NSWC H12
 *      Initial development (based off tmr5 driver of 2016/11/02)
 *
 *  REFERENCE DOCUMENTS
 *    1.
 */
//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void InitTmr2Driver(void)
//  This should be called before making use of any Timer 2 services, typically
//  when initializing the system.  It resets the timer, but to save power does
//  not start it.  It also NULLs service callback function pointer.
//  The timer will be started/stopped as needed when services are [not] used.
//
//  INPUT : NONE
//  OUTPUT: NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool ReqTmr2Srvc(tmr2srvc_t period, pvfv_t callback)
//  Provides non-blocking timer services for predefined/enumerated periods. When
//  the requested period expires, the timer ISR will call the provided callback
//  function.  This will continue until EndTimerB0Service(period) is called.
//  If the timer service is already being used, the request will fail.
//
//  INPUT : tmr2srvc_t period - predefined period after which callback to occur
//          pvfv_t callback - callback function pointer "void (*)(void)"
//  OUTPUT: bool - TRUE if able to grant requested service, otherwise FALSE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void EndTmr2Srvc(void)
//  Halts periodic timer service and clears module callback pointer attribute.
//
//  INPUT : NONE
//  OUTPUT: NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <xc.h>
#include <stdbool.h>        // Includes true/false definition
#include "micro_defs.h"     // for FCY and pvfv_t typedef of pointer to function

#if (10000000L == FCY)
  // Prescale Fcy by 256 <1:0> = 11 --> max 1677.696ms with 25.6 us resolution
  #define T2S_PS1   1
  #define T2S_PS0   1
  typedef enum tagTIMER_2_SERVICE
  {                       // FCY = 10MHz
    T2S_100US =     4,    //     4 * (1/(10MHz/256)) =    0.1024 ms
    T2S_500US =    20,    //    20 * (1/(10MHz/256)) =    0.5120 ms
    T2S_10MS  =   391,    //   391 * (1/(10MHz/256)) =   10.0096 ms
    T2S_100MS =  3907,    //  3907 * (1/(10MHz/256)) =  100.0192 ms
    T2S_250MS =  9766,    //  9766 * (1/(10MHz/256)) =  250.0096 ms
    T2S_500MS = 19532,    // 19532 * (1/(10MHz/256)) =  500.0192 ms
    T2S_1SEC  = 39063,    // 39063 * (1/(10MHz/256)) = 1000.0128 ms
  } tmr2srvc_t;
//// Prescale Fcy by  64 <1:0> = 10 --> max  419.424ms with  6.4 us resolution
//// Prescale Fcy by   8 <1:0> = 01 --> max   52.428ms with  0.8 us resolution
//// Prescale Fcy by   1 <1:0> = 00 --> max    6.554ms with  0.1 us resolution
#else
  #error Unexpected FCY frequency for which Timer 2 driver not written
#endif // FCY is 10MHz


void InitTmr2Driver(void);
bool ReqTmr2Srvc(tmr2srvc_t period, pvfv_t callback);
void EndTmr2Srvc(void);


#endif // TIMER_2_H__
