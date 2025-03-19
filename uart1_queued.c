////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : uart1_queued.c
 *
 *  DESCRIPTION   : Define interrupt driven implementation of UART1
 *    as Asynchronous Receiver/Transmitter at specified baudrate and 8,N,1 that
 *    use queues to sink/source data.  Uart1QueuedRxIsr and Uart1QueuedTxIsr
 *    should be called from the appropriate interrupt vector routine.
 *
 *  NOTE: micro_defs.h must define U1RXPRI, U1TXPRI, U1ERPRI, U1RX_RPINR,
 *    U1RX_RPX, U1TX_RPOR, and U1TX_OFN
 *
 *      (1) bool OpenUart1(void)    //(baud_t baudrate)
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
 *     (12) void _U1ErrInterrupt(void)
 *
 *  WRITTEN BY  : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2016/10/11, Robert Kirby, NSWC H12
 *      Make rxTrig/trigCnt increment on each trigger rather than assign 1.
 *    2016/07/21, Robert Kirby, NSWC H12
 *      Add conditional compilation for UARTx_RX_TRIG_ANY or UARTx_RX_TRIG_BYTE
 *    2015/09/23, Robert Kirby, NSWC Z17
 *      Initial development
 *
 *  REFERENCE DOCUMENTS
 *    1.
 */
#include <xc.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "micro_defs.h"         // to get U1RXPRI, etc.
#include "queue.h"              // for queues
#include "uart1_queued.h"


static uint8queue     *    rQUart1;             // UART1 Receive queue
static uint8queue     *    wQUart1;             // UART1 Write queue
static volatile uint16_t   uart1RxCtdn;         // countdown bytes to read
static volatile uartstat_t uart1Stat;           // bitfield of UART status


//--PROCEDURE-------------------------------------------------------------------
//  void OpenUart1(void) // easily modified for (baud_t baudrate)
//  Clocks & configures UART1 for BAUD,8,N,1 asynchronous RX/TX operation if it
//  is not already in use.  Note that TX and RX are not enabled, just prep'ed.
//  Must call this before using StartReadUart1() or StartWriteUart1().
//  All UART1 IRQs are disabled by this initialization.
//
//  INPUT : NONE // baud_t baudrate - enumerated desired baudrate for UART
//  OUTPUT: bool - returns 'true' on success and 'false' otherwise
//  CALLS : NONE
//------------------------------------------------------------------------------
bool OpenUart1(void)
{
  if (U1MODEbits.UARTEN)            {   // When UART is already busy we
    return false;                   }   // don't just brutally take it over

  uart1Stat.stat    = 0;                // Clear all status bits to start with
  uart1Stat.wrtLast = 1;                // Just opening so no write in progress
  PMD1bits.U1MD     = 0;                // Clock UART1 before writing registers!
//U1TXREG           = 0x0000;           // Same as default POR setting
//U1RXREG           = 0x0000;           // Same as default POR setting
//U1ADMD            = 0x0000;           // Same as default POR setting
//U1SCCON           = 0x0000;           // Same as default POR setting
//U1SCINT           = 0x0000;           // Same as default POR setting
//U1GTC             = 0x0000;           // Same as default POR setting
//U1WTCL            = 0x0000;           // Same as default POR setting
//U1WTCH            = 0x0000;           // Same as default POR setting
  U1MODEbits.BRGH   = 1;                // Use High Speed mode
  U1BRG             = UART19200_BRG;    // Set baud rate
  IEC0bits.U1RXIE   = 0;                // Disable RX interrupt
  IEC0bits.U1TXIE   = 0;                // Disable TX interrupt
  IEC4bits.U1ERIE   = 0;                // Disable Error interrupts
  IPC2bits.U1RXIP   = U1RXPRI;          // Set RX IRQ priority
  IPC3bits.U1TXIP   = U1TXPRI;          // Set TX IRQ priority
  IPC16bits.U1ERIP  = U1ERPRI;          // Set Error IRQ priority
//IFS0bits.U1RXIF   = 0;                // Clear RX flag <-- when start read
//IFS0bits.U1TXIF   = 0;                // Clear TX flag <-- when start write
//IFS4bits.U1ERIF   = 0;                // Clear Error flag <-- when start read
  U1STA             = 0x0000;           // UART RX/TX off, xfr one byte at time
  U1MODEbits.UARTEN = 1;                // Enable the UART

  return true;
} // end routine OpenUart1


//--PROCEDURE-------------------------------------------------------------------
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
//  CALLS : NONE
//------------------------------------------------------------------------------
bool StartReadUart1(uint8queue* readQ, uint16_t count)
{
  volatile uint8_t trash;

  if (U1STAbits.URXEN)              {   // When already busy reading something
    return false;                   }   // don't just brutally change queue

  if (NULL == readQ)
  {                                     // When not given a queue to use just
    uart1Stat.rqerr = 1;                // note the error condition and
    return false;                       // exit without starting to read.
  }
                                        // When provided adrs of a queue...
  uart1Stat.rqerr = 0;                  // Forgive all past queue sins
  rQUart1         = readQ;              // Initialize to where data is saved
  uart1RxCtdn     = count;              // and how many bytes to read in.
  U1STAbits.URXEN = 1;                  // Enable receiver function,
  U1STAbits.PERR  = 0;                  // ensure no parity error condition,
  U1STAbits.FERR  = 0;                  // ensure no framing error condition,
  U1STAbits.OERR  = 0;                  // ensure no overrun error condition,
  while(U1STAbits.URXDA == 1)       {   // then clear RX buffer by just
    trash = U1RXREG;                }   // trashing anything old.
/*
  IFS4bits.U1ERIF = 0;                  // Clear Error interrupt flag before
  IEC4bits.U1ERIE = 1;                  // enabling Error interrupts assoc w/RX.
*/
  IFS0bits.U1RXIF = 0;                  // Clear UART RX interrupt flag before
  IEC0bits.U1RXIE = 1;                  // enabling the UART RX interrupt.

  return true;                          // Happy to start reading the UART
} // end routine StartReadUart1


//--PROCEDURE-------------------------------------------------------------------
//  void StopReadUart1(void)
//  Disable RX and its interrupt so that the UART read is stopped.
//  Simply call StartReadUart1(...) to start another read operation.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
void StopReadUart1(void)
{
  U1STAbits.URXEN   = 0;                // Disable UART receive function
  IEC0bits.U1RXIE   = 0;                // Disable RX interrupt and
  IEC4bits.U1ERIE   = 0;                // Error interrupts assoc with RX.
  uart1RxCtdn       = 1;                // For safety - stop rogue RX asap.
} // end routine StopReadUart1


//--PROCEDURE-------------------------------------------------------------------
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
//  CALLS : NONE
//------------------------------------------------------------------------------
bool StartWriteUart1(uint8queue* writeQ)
{
  if ( ! uart1Stat.wrtLast)         {   // When already busy writing something
    return false;                   }   // don't just brutally change queue

  if (NULL == writeQ)
  {                                     // When not given a queue to use just
    uart1Stat.wqerr = 1;                // note the error condition and
    return false;                       // exit without starting to write.
  }
  else if (QUEUE_NOT_EMPTY(writeQ))     // When provided adrs of a queue...
  {                                     // Kick off write when queue has data
    uart1Stat.wqerr   = 0;              // Forgive all past queue sins
    wQUart1           = writeQ;         // Initialize from where we source data
    uart1Stat.wrtLast = 0;              // and note that write is NOT finished.
    U1STAbits.UTXEN   = 1;              // Transmit Enabled
    IEC0bits.U1TXIE   = 1;              // Enable the UART TX interrupt.
  }
  return true;                          // Happy to have started uart write
} // end routine StartWriteUart1


//--PROCEDURE-------------------------------------------------------------------
//  void StopWriteUart1(void)
//  Disable the TX interrupt so that the UART write is effectively stopped.
//  Simply call StartWriteUart1(...) to start another write operation.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
void StopWriteUart1(void)
{
  U1STAbits.UTXEN   = 0;                // Disabled UART transmitter and
  IEC0bits.U1TXIE   = 0;                // the UART1 TX interrupt.
  uart1Stat.wrtLast = 1;                // Note that write is finished.
} // end routine StopWriteUart1


//--PROCEDURE-------------------------------------------------------------------
//  void CloseUart1(void)
//  Disables/declocks the UART and then sets the RX and TX pins as output low.
//  After calling this, must call OpenUart1() before using read/write again.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
void CloseUart1(void)
{
/*
  This chunk of code accomplished by de-clocking the peripheral in line below
  IEC0bits.U1TXIE   = 0;                // Disable the UART1 TX interrupt,
  IEC0bits.U1RXIE   = 0;                // disable RX interrupt, and
  IEC4bits.U1ERIE   = 0;                // disable RX error interrupts.
  U1STAbits.UTXEN   = 0;                // Disable the UART transmitter.
  U1STAbits.URXEN   = 0;                // Disable the UART receiver.
*/
  U1MODEbits.UARTEN = 0;                // Disable/reset UART, it's available.
  PMD1bits.U1MD     = 1;                // De-clk UART1 to save max power.

/*
  This chunk of code accomplished by setting TRISx, CNPU1, and CNPU2 in InitApp
  UART1_TX_PIN_DIR  = 0;                // Set direction of TX port pin to
  UART1_TX_PIN_WR   = 0;                // save power -- set pin as output low.
  UART1_RX_PIN_DIR  = 1;                // RX port pin stays an input.
*/
  uart1Stat.wrtLast = 1;                // Note that write is finished.
  rQUart1           = NULL;             // Don't maintain connections to queues.
  wQUart1           = NULL;
} // end routine void CloseUart1


//--PROCEDURE-------------------------------------------------------------------
//  uartstat_t GetUart1Status(void)
//  Accessor for module attribute uart1Stat, which is used in ISRs.
//
//  INPUT : NONE
//  OUTPUT: uartstat_t - UART1's status flags
//  CALLS : NONE
//------------------------------------------------------------------------------
uartstat_t GetUart1Status(void)
{
  return uart1Stat;

/* Because this is an atomic operation, no need for these extraordinary measures
  uint16_t volatile answer;
  if (U1MODEbits.UARTEN)
  {
    int volatile disiCopy = DISICNT;
    __builtin_disi(0x3FFF);
    answer  = uart1Stat.stat;
    DISICNT = disiCopy;
  }
  else
  {
    answer  = uart1Stat;
  }
  return answer;
*/
} // end function GetUart1Status


//--PROCEDURE-------------------------------------------------------------------
//  bool GetUart1IsWriteDone(void)
//  Checks module attribute and UART register values to determine if write done.
//
//  INPUT : NONE
//  OUTPUT: bool - true if last bit of write is done (stop bit), otherwise false
//  CALLS : NONE
//------------------------------------------------------------------------------
bool GetUart1IsWriteDone(void)
{
  return (uart1Stat.wrtLast && U1STAbits.TRMT) ? true : false;
} // end function GetUart1IsWriteDone


//--PROCEDURE-------------------------------------------------------------------
//  void ClearUart1StatusFlag(uartflag_t flag)
//  Mutator for module attribute uart1Stat, which is used in ISRs.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
void ClearUart1StatusFlag(uartflag_t flag)
{
  int volatile disiCopy = DISICNT;

  if (U1MODEbits.UARTEN)            {
    __builtin_disi(0x3FFF);         }

  switch (flag)
  {
    case ALLERR   :   uart1Stat.stat   &= ~UARTSTAT_ERR_BITS;   break;
    case WQERR    :   uart1Stat.wqerr   = 0;                    break;
    case RQERR    :   uart1Stat.rqerr   = 0;                    break;
    case OERR     :   uart1Stat.oerr    = 0;                    break;
    case FERR     :   uart1Stat.ferr    = 0;                    break;
    case PERR     :   uart1Stat.perr    = 0;                    break;
    case RXTRIG   :   uart1Stat.trigCnt = 0;                    break;
    case TRIGCNT  :   uart1Stat.trigCnt--;                      break;
    default       :                                             break;
  }

  DISICNT = disiCopy;
} // end routine ClearUart1ErrorFlag


//--PROCEDURE-------------------------------------------------------------------
//  void _U1RXInterrupt(void)
//  Interrupt Service Routine that actually uses UART to read data and place it
//  within the read queue. It disables UART RX if specified amount of data read.
//
//  NOTE - UART RX interrupt flag auto-cleared when RXREG read
//
//  INPUT : NONE
//  OUTPUT: NONE but read queue (and possibly doNotSleep) updated
//  CALLS : NONE
//------------------------------------------------------------------------------
// UART 1 Receive Interrupt occurs when byte is received in U1RXREG buffer
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
  uint8_t data;
  uint8_t sanity = 5;                   // Our sanity is somewhat limited
  bool    isOk   = true;                // but we are optimistic data is good

  do
  {
    if (U1STAbits.FERR || U1STAbits.PERR)
    {                                   // Must check FERR || PERR _before_ read
      isOk        = false;              // Framing or Parity error is NOT okay,
      doNotSleep  = true;               // so exit sleep as comms have issues.
      if (U1STAbits.FERR)         {     // As appropriate, note that a
        uart1Stat.ferr  = 1;      }     // framing error occurred.
      if (U1STAbits.PERR)         {     // As appropriate, note that a
        uart1Stat.perr  = 1;      }     // parity error occurred.
    }

    data = U1RXREG;                     // Always read byte of data, clears ERRs
    #ifdef UART1_RX_TRIG_ANY
    doNotSleep  = true;                 // Run main statemachine to process data
    uart1Stat.trigCnt++;                // before 8-bit trigCnt can wrap to 0
    #endif

    if (uart1RxCtdn)                    // Countdown even if comms ERR...
    {                                   // When reading a number of bytes
      uart1RxCtdn--;                    // countdown remaining to receive.
      if (!uart1RxCtdn)
      {                                 // When just read the last byte
        U1STAbits.URXEN = 0;            // disable the UART receiver and
        doNotSleep      = true;         // exit SLEEP as comms finished.
      }
    }

    if (isOk)
    {                                   // When neither framing nor parity error
      #ifdef UART1_RX_TRIG_BYTE
      if (UART1_RX_TRIG_BYTE == data)
      {
        doNotSleep  = true;             // Run main statemachine to process data
        uart1Stat.trigCnt++;            // before 8-bit trigCnt can wrap to 0
      }
      #endif
      if ((NULL != rQUart1) && QUEUE_NOT_FULL(rQUart1))
      {                                 // Only if RX Q has space is
        QUEUE_PUT(rQUart1, data);       // data put into the queue and
        if (QUEUE_FULL(rQUart1))  {     // when Q just got full need to
          doNotSleep = true;      }     // exit SLEEP to process ASAP.
      }
      else
      {                                 // Yes, we can loose data!!
        uart1Stat.rqerr = 1;            // Set queue error flag (overflow)
      }
    }
    else
    {                                   // When either framing or parity error
      break;                            // break from do-while loop
    }                                   // without saving byte to queue

  } while (U1STAbits.URXDA && sanity--);// As we read out buffered RX data
                                        // decrement toward insanity
  if (U1STAbits.OERR)
  {                                     // When Overrun Error occurs
    U1STAbits.OERR  = 0;                // clearing bit resets UART's RX FIFO.
    uart1Stat.oerr  = 1;                // Set error condition flag and then
    doNotSleep      = true;             // exit sleep as comms have issues.
  }

  IFS0bits.U1RXIF = 0;                  // Lastly, clear UART IF
} // end ISR _U1RXInterrupt


//--PROCEDURE-------------------------------------------------------------------
//  void _U1TXInterrupt(void)
//  Interrupt Service Routine that actually uses UART to sends data from the
//  write queue.  It disables UART TX when there is no more data in the queue.
//
//  NOTE - UART TX interrupt flag auto-cleared when TXREG written
//
//  INPUT : NONE
//  OUTPUT: NONE but write queue (and possibly doNotSleep) updated
//  CALLS : NONE
//------------------------------------------------------------------------------
// UART 1 Transmit Interrupt occurs whenever there is space for another byte in U1TXREG buffer
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
  uint8_t data;

  if ((NULL != wQUart1) && QUEUE_NOT_EMPTY(wQUart1))
  {                                     // When data is queued
    QUEUE_GET(wQUart1, data);           // get the next byte and
    IFS0bits.U1TXIF = 0;                // clear UART IF before loading TXREG
    U1TXREG = data;                     // send it out the UART
  }
  else
  {                                     // When last data TXed (or bad queue)
    //U1STAbits.TXEN  = 0;              // don't disabled UART transmitter but
    IEC0bits.U1TXIE   = 0;              // disable the UART TX interrupt;
    uart1Stat.wrtLast = 1;              // note that write is finished and
    doNotSleep        = true;           // exit SLEEP when comms finished.
  }
} // end ISR _U1TXInterrupt

///* TODO don't use _U1ErrInterrupt
//--PROCEDURE-------------------------------------------------------------------
//  void _U1ErrInterrupt(void)
//  Interrupt Service Routine that actually uses UART to sends data from the
//  write queue.  It disables UART TX when there is no more data in the queue.
//
//  OUTPUT: NONE but UART FIFO and doNotSleep possibly updated
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
// For handling framing, parity, and/or buffer overrun errors on UART1
void __attribute__((interrupt, no_auto_psv)) _U1ErrInterrupt(void)
{
  // TODO:  Further refine Error ISR
  // PERR shouldn't occur with no parity bit in format
  // FERR should result in a retransmit request to sender
  // OERR should result in a retransmit request to sender

  uint8_t trash;

  if(U1STAbits.FERR || U1STAbits.PERR)  // When Framing and/or Parity error(s)
  {                                     // note auto-clearing error condition(s)
    if (U1STAbits.FERR)           {
      uart1Stat.ferr  = 1;        }
    if (U1STAbits.PERR)           {
      uart1Stat.perr  = 1;        }
    trash = U1RXREG;                    // then discard a single corrupted byte.
  }

  if (U1STAbits.OERR)
  {                                     // When Overrun Error occurs
    uart1Stat.oerr  = 1;                // set error condition flag and then
    U1STAbits.OERR  = 0;                // clearing bit resets UART's RX FIFO
  }

  doNotSleep      = true;               // Exit SLEEP as comms has a problem.
  IFS4bits.U1ERIF = 0;                  // Clear UART IF after clearing OERR
} // end ISR _U1ErrInterrupt
//*/
