////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : queue.c
 *
 *  DESCRIPTION   : Define procedures to replace the macro based queue's most
 *    code-space expensive and typically most used processes.
 *
 *      (1) void QueueInit(pQ, pBfr, size, isPrepacked)
 *      (2) void QueuePut(int8queue* pQ, int8_t item)
 *      (3) void QueueGet(int8queue* pQ, int8_t pItem)
 *      (4) void QueueUnget(int8queue* pQ, int8_t item)
 *      (5) void QueuePurge(int8queue* pQ)
 *      (6) uint16_t QueueDiscard(uint8queue* pQ, uint16_t cnt)
 *
 *  WRITTEN BY    : Robert Kirby, NSWC Z17
 *  MODIFICATIONS (in reverse chronological order)
 *    2016/07/20, Robert Kirby, NSWC H12
 *      Add function uint16_t QueueDiscard(uint8queue* pQ, uint16_t cnt)
 *    2015/09/24, Robert Kirby, NSWC Z17
 *      Switch back to using <stdint.h> now supported by XC16 compiler
 *      Updated comments and removed stubbed out code
 *    2014/02/20, Robert Kirby, NSWC Z17
 *      Modify for use with PIC18 instead of MSP430 (no protection from IRQs)
 *      Pretty safe from IRQ corruption because 'count' attribute is only real
 *      concern and that is even more negligible with 16-bit micro-controllers)
 *    2012/08/17, Robert Kirby, NSWC Z17
 *      Make procedures (except INIT) temporarily disable IRQs to protect queue
 *      Have non-macro versions of UNGET & PURGE; update comments.
 *    2012/02/27, Robert Kirby, NSWC Z17
 *      Initial development
 *      Procedurize Queue (space savings v. pure macro)
 *      No volatile queues or data going into/out-of queue
 *
 *  REFERENCE DOCUMENTS
 *    1.
 */
#include <stdint.h>
#include "queue.h"
#include "micro_defs.h"          // to get identifier GIE in specific micro's .h

#ifndef PURE_MACRO_QUEUE

//--PROCEDURE-------------------------------------------------------------------
//  void QueueInit(uint8queue* pQ,uint8_t* pBfr,int16_t size,bool isPrepacked)
//  Initializes a circular queue which may be either prepacked/prefilled or not.
//  No error checks are made (other than ensuring non-negative size).
//
//  INPUT : int8queue* pQ - address of queue which is to be initialized
//          int8_t* pBfr - start address of buffer to use for circular queue
//          int16_t size - number of items that can be stored in buffer
//          bool isPrepacked - true if buffer already filled, otherwise false
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
void QueueInit(uint8queue* pQ, uint8_t* pBfr, int16_t size, bool isPrepacked)
{
  pQ->items     = pBfr;
  pQ->hdr.front = 0;
  pQ->hdr.rear  = 0;
  pQ->hdr.size  = (size > 0)
                ? size
                : 0;                    // Negative size not allowed
  pQ->hdr.count = (isPrepacked)         // When the queue is prepacked
                ? pQ->hdr.size          // it starts already filled,
                : 0;                    // otherwise it starts empty
} // end routine QueueInit


//--PROCEDURE-------------------------------------------------------------------
//  void QueuePut(uint8queue* pQ, uint8_t item)
//  Puts an item into the circular queue.  No error checks are made; may cause
//  overflow so consider using QUEUE_NOT_FULL or QUEUE_AVAIL_SPACE before call.
//
//  INPUT : int8queue* pQ - address of queue into which data item to be placed
//          int8_t item   - item to be placed into the queue
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
void QueuePut(uint8queue* pQ, uint8_t item)
{
  pQ->items[pQ->hdr.rear] = item;
  pQ->hdr.rear++;
  if (pQ->hdr.rear >= pQ->hdr.size)   { // check unsigned rear vs. signed size
    pQ->hdr.rear = 0;                 } // to be >= is invalid, so wrap rear
  pQ->hdr.count++;
} // end routine QueuePut


//--PROCEDURE-------------------------------------------------------------------
//  void QueueGet(uint8queue* pQ, uint8_t* pItem)
//  Gets an item from the circular queue.  No error checks are made;  may cause
//  underflow so consider using QUEUE_NOT_EMPTY or QUEUE_AVAIL_DATA before call.
//
//  INPUT : int8queue* pQ - address of queue from which to get data item
//          int8_t pItem  - address at which data item's value is to be copied
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
void QueueGet(uint8queue* pQ, uint8_t* pItem)
{
  *pItem = pQ->items[pQ->hdr.front];
  pQ->hdr.front++;
  if (pQ->hdr.front >= pQ->hdr.size)  { // check unsigned front vs. signed size
    pQ->hdr.front = 0;                } // to be => is invalid, so wrap front
  pQ->hdr.count--;
} // end routine QueueGet

//--PROCEDURE-------------------------------------------------------------------
//  void QueueUnget(uint8queue* pQ, uint8_t item)
//  Places item at front of circular queue.  No error checks are made; may cause
//  overflow so consider using QUEUE_NOT_FULL or QUEUE_AVAIL_SPACE before call.
//
//  INPUT : int8queue* pQ - address of queue to which data item is returned
//          int8_t item  - item to be placed (returned) back at front of queue
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
void QueueUnget(uint8queue* pQ, uint8_t item)
{
  if (pQ->hdr.front == 0)             { // Check if front was just wrapped and
    pQ->hdr.front = pQ->hdr.size;     } // when it was need to unwrap it.
  pQ->hdr.front--;                      // Update front in advance of
  pQ->items[pQ->hdr.front] = item;      // placing item back into the queue
  pQ->hdr.count++;                      // Yep, we have one more item now.
} // end routine QueueUnget


//--PROCEDURE-------------------------------------------------------------------
//  void QueuePurge(uint8queue* pQ)
//  Set queue's front, rear, and count attributes to make queue empty but does
//  not zeroize the data held in the queue's buffer.
//
//  INPUT : int8queue* pQ - address of queue to which data item is returned
//  OUTPUT: NONE
//  CALLS : NONE
//------------------------------------------------------------------------------
void QueuePurge(uint8queue* pQ)
{
  pQ->hdr.count = 0;
  pQ->hdr.front = 0;
  pQ->hdr.rear  = 0;
} // end routine QueuePurge


//--PROCEDURE-------------------------------------------------------------------
//  uint16_t QueueDiscard(uint8queue* pQ, uint16_t cnt)
//  Set queue's front, rear, and count attributes to make queue empty but does
//  not zeroize the data held in the queue's buffer.
//
//  INPUT : int8queue* pQ - address of queue to which data item is returned
//          uint16_t cnt - the requested # items to discard
//  OUTPUT: uint16_t - the number of items discarded from the queue
//  CALLS : NONE
//------------------------------------------------------------------------------
uint16_t QueueDiscard(uint8queue* pQ, uint16_t cnt)
{
  uint16_t flushed;

  if (cnt < pQ->hdr.count)
  {
    flushed        = cnt;
    pQ->hdr.count -= cnt;
    pQ->hdr.front += cnt;
    if(pQ->hdr.front >= pQ->hdr.size) { // check unsigned front vs. signed size
      pQ->hdr.front -= pQ->hdr.size;  } // to be => is invalid, so wrap front
  }
  else
  {
    flushed       = pQ->hdr.count;
    pQ->hdr.count = 0;
    pQ->hdr.front = 0;
    pQ->hdr.rear  = 0;
  }
  return flushed;
} // end routine QueueDiscard


#endif // ndef PURE_MACRO_QUEUE
