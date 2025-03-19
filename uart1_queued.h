#ifndef UART1_QUEUED_H
#define UART1_QUEUED_H
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : uart1_queued.h
 *
 *  DESCRIPTION   : Declare interface to IRQ driven implementation of UART1
 *    as Asynchronous Receiver/Transmitter at specified baud rate and 8,N,1 that
 *    use queues to sink/source data.  Uart1QueuedRxIsr and Uart1QueuedTxIsr
 *    should be called from the appropriate interrupt vector.
 *    System global 'doNotSleep' is set when RX buffer full, RX or TX complete,
 *    RX Trigger (if enabled), and various UART error conditions.
 *    Code has conditionally compilation to trigger on receiving either a
 *    specified byte value or any byte (default is neither).  These triggers
 *    set system global 'doNotSleep' and increment UART1's uartstat_t 8-bit
 *    trigger counter 'uart1Stat.trigCnt' which is a union w/ 'uart1Stat.rxTrig'
 *    Recommend macro mInitUart1Queued() as part of [system] initialization.
 *
 *  NOTE: micro_defs.h __must__ define U1RXPRI, U1TXPRI, U1ERPRI, U1RX_RPINR,
 *    U1RX_RPX, U1TX_RPOR, and U1TX_OFN and declare system global 'doNotSleep'
 *
 *  NOTE: for special UART RX triggering micro_defs.h needs to do only one
 *    of either "#define UART1_RX_TRIG_ANY" or "#define UART1_RX_TRIG_BYTE val"
 *      Example: #define UART1_RX_TRIG_BYTE 0x0A   // RX trigger on <line feed>
 *
 *  NOTE: suggest mDecrementUart1TrigCnt() when each trigger is processed
 *
 *      (1) bool OpenUart1(void)  //(baud_t baudrate)
 *      (2) bool StartReadUart1(uint8queue* readQ, uint16_t count)
 *      (3) void StopReadUart1(void)
 *      (4) bool StartWriteUart1(uint8queue* writeQ)
 *      (5) void StopWriteUart1(void)
 *      (6) void CloseUart1(void)
 *      (7) uartstat_t GetUart1Status(void)
 *      (8) bool GetUart1IsWriteDone(void)
 *      (9) void ClearUart1StatusFlag(uartflag_t flag)
 *     (10) void _U1RXInterrupt(void)
 *     (11) void _U1TXInterrupt(void)
 *
 *  WRITTEN BY  : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2016/10/11, Robert Kirby, NSWC H12
 *      Comments on use updated and add macro mDecrementUart1TrigCnt()
 *    2016/07/21, Robert Kirby, NSWC H12
 *      Add conditional compilation for UARTx_RX_TRIG_ANY or UARTx_RX_TRIG_BYTE
 *    2015/09/23, Robert Kirby, NSWC Z17
 *      Initial development
 *
 *  REFERENCE DOCUMENTS
 *    1. PIC24FJ128GA204 Data Sheet (Microchip Technology Inc. DS30010038C)
 */
//--PROCEDURES------------------------------------------------------------------
//  void OpenUart1(void) // easily modified to(baud_t baudrate)
//  Clocks & configures UART1 for BAUD,8,N,1 asynchronous RX/TX operation if it
//  is not already in use.  Note that TX and RX are not enabled, just prep'ed.
//  Must call this before using StartReadUart1() or StartWriteUart1().
//  All UART1 IRQs are disabled by this initialization.
//
//  INPUT : NONE
//  OUTPUT: bool - returns 'true' on success and 'false' otherwise
//------------------------------------------------------------------------------
//  bool StartReadUart1(uint8queue* readQ, uint16_t count)
//  OpenUart1(...) must have been called prior to calling this.
//  If a valid queue is available, enable the RX interrupt so that the UART
//  read functionality can run behind the scenes using interrupts.  The UART
//  will continue to read data into queue until 'count' bytes are read, read is
//  explicitly stopped [StopReadUart1()], or the UART is closed [CloseUart1()].
//  This is non-blocking; it initiates an interrupt driven process.
//
//  INPUT : uint8queue* readQ - queue into which UART data will be read
//          uint16_t count - number of bytes to read, set to 0 for unlimited
//  OUTPUT: bool - 'true' if IRQ based UART Read enabled/started
//------------------------------------------------------------------------------
//  void StopReadUart1(void)
//  Disable RX ant its interrupt so that the UART read is stopped.
//  Simply call StartReadUart1(...) to start another read operation.
//
//  INPUT : NONE
//  OUTPUT: NONE
//------------------------------------------------------------------------------
//  bool StartWriteUart1(uint8queue* writeQ)
//  OpenUart1(...) must have been called prior to calling this.
//  If a valid queue containing data is available, enable the TX interrupt so
//  that UART write functionality can run behind the scenes using interrupts.
//  The UART will continue to write data from queue until it is empty, write is
//  explicitly stopped [StopWriteUart1()], or the UART is closed [CloseUart1()].
//  This is non-blocking; it initiates an interrupt driven process.
//
//  INPUT : uint8queue* writeQ - queue from which UART data will be written
//  OUTPUT: bool - 'true' if IRQ based UART Write enabled/started
//------------------------------------------------------------------------------
//  void StopWriteUart1(void)
//  Disable the TX interrupt so that the UART write is effectively stopped.
//  Simply call StartWriteUart1(...) to start another write operation.
//
//  INPUT : NONE
//  OUTPUT: NONE
//------------------------------------------------------------------------------
//  void CloseUart1(void)
//  Disables/declocks the UART and then sets the RX and TX pins as output low.
//  After calling this, must call OpenUart1() before using read/write again.
//
//  INPUT : NONE
//  OUTPUT: NONE
//------------------------------------------------------------------------------
//  uartstat_t GetUart1Status(void)
//  Accessor for module attribute uart1Stat, which is used in ISRs.
//
//  INPUT : NONE
//  OUTPUT: uartstat_t - UART1's status flags
//------------------------------------------------------------------------------
//  bool GetUart1IsWriteDone(void)
//  Checks module attribute and UART register values to determine if write done.
//
//  INPUT : NONE
//  OUTPUT: bool - true if last bit of write is done (stop bit), otherwise false
//------------------------------------------------------------------------------
//  void ClearUart1StatusFlag(uartflag_t flag)
//  Mutator for module attribute uart1Stat, which is used in ISRs.
//
//  INPUT : NONE
//  OUTPUT: NONE
//------------------------------------------------------------------------------
#include <stdbool.h>            // to get C99 bool types
#include "queue.h"              // for queues
#include "uart.h"


//----- EXPOSED ATRRIBUTES -----------------------------------------------------
// use GetUart1Status and ClearUart1ErrorFlag to get to uartstat_t uart1Stat


//----- EXPOSED PROCEDURES -----------------------------------------------------
bool OpenUart1(void);  //(baud_t baudrate);
bool StartReadUart1(uint8queue* readQ, uint16_t count);
void StopReadUart1(void);
bool StartWriteUart1(uint8queue* writeQ);
void StopWriteUart1(void);
void CloseUart1(void);
uartstat_t GetUart1Status(void);
bool GetUart1IsWriteDone();
void ClearUart1StatusFlag(uartflag_t flag);


//----- MACROS -----------------------------------------------------------------
#define mDecrementUart1TrigCnt()      (ClearUart1StatusFlag(TRIGCNT))
#define mUart1WriteIsDone()           (GetUart1IsWriteDone())
#define mUart1WriteIsBusy()           ( ! mUart1WriteIsDone())
#define mInitUart1Queued()            do {                                    \
                                        CloseUart1();                         \
                                        OpenUart1();                          \
                                      } while (0);


#endif // UART1_QUEUED_H
