#ifndef _UART_H_
#define _UART_H_
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : uart.h
 *
 *  DESCRIPTION   : Declare few things common to my queue based PIC UART drivers
 *
 *
 *  WRITTEN BY    : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2016/10/11, Robert Kirby, NSWC H12
 *      Rearrange uartstat_t bitfield to create an 8-bit counter, '.trigCnt'
 *      that maps to '.rxTrig' to create greater functionality. Similarly
 *      update uartflag_t
 *    2015/09/23, Robert Kirby, NSWC Z17
 *      Initial development
 *
 *  REFERENCE DOCUMENTS
 *    1. PIC24FJ128GA204 Data Sheet (Microchip Technology Inc. DS30010038C)
 */
#include <stdint.h>             // for C99 typedefs like uint16_t
#include "micro_defs.h"         // to get FCY


// Use UxBRGH = 1 if at all possible
// When UxBRGH = 1 divide FCY by BAUD_RATE * 4, otherwise BAUD_RATE * 16

//----- DEFINES, ENUMS, STRUCTS, TYPEDEFS, ETC. --------------------------------
typedef enum tagBAUDRATE
{
  BAUD9600 = 1,
  BAUD19200,
} baud_t;

//-- 9600-----------------------------------------------------------------------
// 9600 * 4 = 38400
#define UART9600_BRG  (FCY/38400.0 - 1)
//#if (UART9600_BRG < 2)
//  #error UART9600_BRG is too small
//#endif

//--19200-----------------------------------------------------------------------
// 19200 * 4 = 76800
#define UART19200_BRG (FCY/76800.0 - 1)
//#if (UART19200_BRG < 2)
//  #error UART19200_BRG is too small
//#endif

// define bitfield status structure type for the UART
typedef union tagUART_COMMS_STATUS
{
  uint16_t stat;
  struct
  {
    uint16_t rxTrig   :8;   // BIT0 - BIT7 - count of requested READ triggers
    uint16_t wqerr    :1;   // BIT8 - attempted WRITE w/NULL queue
    uint16_t rqerr    :1;   // BIT9 - attempted READ w/NULL queue or overflow
    uint16_t oerr     :1;   // BITA - UART RX data FIFO overflowed
    uint16_t ferr     :1;   // BITB - UART RX framing error detected
    uint16_t perr     :1;   // BITC - UART RX parity error detected
    uint16_t          :2;   // BITD-BITE are unused
    uint16_t wrtLast  :1;   // BITF - requested WRITE operation is [almost] done
//  uint16_t rxempty  :1;   not needed, get from queue supplied to UART driver
//  uint16_t rxfull   :1;   not needed, get from queue supplied to UART driver
//  uint16_t txempty  :1;   not needed, get from queue supplied to UART driver
//  uint16_t txfull   :1;   not needed, get from queue supplied to UART driver
  };
  struct
  {
    uint16_t trigCnt  :8;   // 8-bit counter of RX triggers, maps over .rxTrig
    uint16_t errors   :5;   // maps over .wqerr, .rqerr, .oerr, .ferr, and .perr
    uint16_t          :3;
  };
} uartstat_t;


// An enumeration of the type of flags in uartstat_t (defined above)
typedef enum tagUART_FLAGS
{
  ALLERR  = 0x1F00,     // used to request clearing all errors in uartstat_t
  WQERR   = 0x0100,     // Error with supplied Write Queue
  RQERR   = 0x0200,     // Error with supplied Read Queue or overflow
  OERR    = 0x0400,     // UART Overflow Error
  FERR    = 0x0800,     // UART Framing Error
  PERR    = 0x1000,     // UART Parity Error
  RXTRIG  = 0x00FF,     // UART RX Trigger occurred
  TRIGCNT = 0x0001,     // LSB to indicate decrement the 8-bit counter
} uartflag_t;


#define UARTSTAT_ERR_BITS   ALLERR


 #endif // _UART_H_
