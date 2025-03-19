#ifndef QUEUE_H__
#define QUEUE_H__
/*------------------------------------------------------------------------------
 * Filename:    queue.h
 *
 * Description: Generic circular queue implemented either purely as macros
 *    (which can explode code size) or as a mix of macros and procedures
 *    (accessable via same macro names as used for pure macro implementation).
 *    The queues are safe when used in producer/consumer fashion but do not
 *    provide absolute protection from interrupts (e.g. when queue used with
 *    an I/O peripheral) because 'count' attribute could be corrupted (more
 *    of a worry on 8-bit than 16-bit micro-controllers).
 *    User must check queue full/empty conditions prior to enqueuing/
 *    dequeueing to ensure overflow/underflow does not occur.
 *
 *     (1) {void} QUEUE_INIT_EMPTY(macroq, q_items, q_size)
 *     (2) {void} QUEUE_INIT_PREPACKED(macroq, q_items, q_size)
 *     (3) {void} QUEUE_PUT(macroq, item)
 *     (4) {void} QUEUE_GET(macroq, item)
 *     (5) {void} QUEUE_UNGET(macroq, item)
 *     (6) {void} QUEUE_PURGE(macroq)
 *     (7) {void} QUEUE_PEEK(macroq, item)
 *     (8) {bool} QUEUE_OVERFLOWED(macroq)
 *     (9) {bool} QUEUE_UNDERFLOWED(macroq)
 *    (10) {bool} QUEUE_FULL(macroq)
 *    (11) {bool} QUEUE_NOT_FULL(macroq)
 *    (12) {int16_t} QUEUE_AVAIL_SPACE(macroq)
 *    (13) {bool} QUEUE_EMPTY(macroq)
 *    (14) {bool} QUEUE_NOT_EMPTY(macroq)
 *    (15) {int16_t} QUEUE_AVAIL_DATA(macroq)
 *    (16) uint16_t QueueDiscard(uint8queue* pQ, uint16_t cnt)
 *
 *    NOTE: macro QUEUE_INIT_EMPTY does _not_ initialize/clear queue's buffer.
 *
 * Example Usage
 *
 *   #include "queue.h"
 *
 *   #define MY_Q_SIZE 10
 *   uint8_t uint8Items[MY_Q_SIZE];
 *   uint8queue volatile q;          // make volatile for use in ISRs
 *   #define pQ ((uint8queue*)(&q))  // uint8queue* pQ = &q; not liked by TI CCS
 *
 *   QUEUE_INIT_EMPTY(pQ, uint8Items, MY_Q_SIZE);
 *
 *   if (!QUEUE_FULL(pQ))         {
 *      QUEUE_PUT(pQ, an_item);   }
 *
 *   if (!QUEUE_EMPTY(pQ))        {
 *      QUEUE_GET(pQ, an_item);   }
 *
 *  MODIFICATIONS (in reverse chronological order)
 *    2016/07/20, Robert Kirby, NSWC H12
 *      Add function QueueDiscard(...) but no MACRO equivalent at this time
 *    2015/09/24, Robert Kirby, NSWC Z17
 *      Switch back to using <stdint.h> now supported by XC16 compiler
 *      Updated comments
 *    2014/02/20, Robert Kirby, NSWC Z17
 *      Modify for use with PIC18 instead of MSP430 (no protection from IRQs)
 *    2012/08/17, Robert Kirby, NSWC Z17
 *      Have non-macro versions of UNGET & PURGE; update comments.
 *    2012/02/27, Robert Kirby, NSWC Z17
 *      Procedurize Queue (space savings v. pure macro)
 *      No volatile queues or data going into/out-of queue
 *      Comments...
 *    2011/12/14, Robert Kirby, NSWC Z17
 *      Initial development - FBI dot-on-map demo and Globalstar testing
 *----------------------------------------------------------------------------*/
/*--------------------------------- Includes ---------------------------------*/
#include <stdbool.h>
#include <stdint.h>

//#define PURE_MACRO_QUEUE

/*----------------------------- Defines & Types ------------------------------*/
/* ! These structures should be treated as Private - do not access directly ! */
typedef struct {                // Common to all regardless if types of items
  uint16_t front;               // get from here, unsigned so index can't be < 0
  uint16_t rear;                // put to here, unsigned so index can't be < 0
  int16_t count;                // items queued, signed to sense underflow
  int16_t size;                 // signed so has same value range as count
} queue_hdr_t;

// queue typedefs for queues holding different types of items (e.g. uint16_t)
typedef struct {
  queue_hdr_t hdr;              // must be named "hdr"
  int8_t* items;                // must be named "items",
} int8queue;

typedef struct {
  queue_hdr_t hdr;              // must be named "hdr"
  uint8_t* items;               // must be named "items",
} uint8queue;

typedef struct {
  queue_hdr_t hdr;              // must be named "hdr"
  int16_t* items;               // must be named "items",
} int16queue;

typedef struct {
  queue_hdr_t hdr;              // must be named "hdr"
  uint16_t* items;              // must be named "items",
} uint16queue;

typedef struct {
  queue_hdr_t hdr;              // must be named "hdr"
  int32_t* items;               // must be named "items",
} int32queue;

typedef struct {
  queue_hdr_t hdr;              // must be named "hdr"
  uint32_t* items;              // must be named "items",
} uint32queue;


/*--------------------------- Macros / Prototypes ----------------------------*/
/* more code-space intensive operations can either use macros or procedures   */
#ifdef PURE_MACRO_QUEUE
  #define QUEUE_INIT_EMPTY(macroq, q_items, q_size)                           \
      macroq->items = q_items;                                                \
      macroq->hdr.front = 0;                                                  \
      macroq->hdr.rear = 0;                                                   \
      macroq->hdr.count = 0;                                                  \
      macroq->hdr.size = (q_size > 0)                                         \
                       ? (q_size)                                             \
                       : 0;             // negative size not allowed

  #define QUEUE_INIT_PREPACKED(macroq, q_items, q_size)                       \
      macroq->items = q_items;                                                \
      macroq->hdr.front = 0;                                                  \
      macroq->hdr.rear = 0;                                                   \
      macroq->hdr.size = (q_size > 0)                                         \
                       ? (q_size)                                             \
                       : 0;                                                   \
      macroq->hdr.count = macroq->hdr.size;

  #define QUEUE_PUT(macroq, item)                                             \
      macroq->items[macroq->hdr.rear] = item;                                 \
      macroq->hdr.rear++;                                                     \
      if (macroq->hdr.rear >= macroq->hdr.size)   {                           \
        macroq->hdr.rear = 0;                     }                           \
      macroq->hdr.count++;

  #define QUEUE_GET(macroq, item)                                             \
      item = macroq->items[macroq->hdr.front];                                \
      macroq->hdr.front++;                                                    \
      if (macroq->hdr.front >= macroq->hdr.size)  {                           \
        macroq->hdr.front = 0;                    }                           \
      macroq->hdr.count--;

  #define QUEUE_UNGET(macroq, item) /*USE W/EXTREME CAUTION TO NOT CORRUPT Q*/\
      if (macroq->hdr.front == 0)                 {                           \
        macroq->hdr.front = macroq->hdr.size;     }                           \
      macroq->hdr.front--;                                                    \
      macroq->items[macroq->hdr.front] = item;                                \
      macroq->hdr.count++;

  #define QUEUE_PURGE(macroq)                                                 \
      macroq->hdr.count = 0;                                                  \
      macroq->hdr.front = 0;                                                  \
      macroq->hdr.rear  = 0;

#else
  void QueueInit(uint8queue* pQ, uint8_t* pBfr, int16_t size, bool isPrepacked);
  void QueuePut(uint8queue* pQ, uint8_t item);
  void QueueGet(uint8queue* pQ, uint8_t* pItem);
  void QueueUnget(uint8queue* pQ, uint8_t item);
  void QueuePurge(uint8queue* pQ);
  uint16_t QueueDiscard(uint8queue* pQ, uint16_t cnt);

  #define IS_PREPACKED    true
  #define IS_EMPTY        (!IS_PREPACKED)

  #define QUEUE_INIT_EMPTY(pQ, q_items, q_size)                               \
    QueueInit(pQ, q_items, q_size, IS_EMPTY)

  #define QUEUE_INIT_PREPACKED(pQ, q_items, q_size)                           \
    QueueInit(pQ, q_items, q_size, IS_PREPACKED)

  #define QUEUE_PUT(pQ, item)                                                 \
    QueuePut(pQ, item)

  #define QUEUE_GET(pQ, item)                                                 \
    QueueGet(pQ, &(item))

  #define QUEUE_UNGET(pQ, item) /* USE W/EXTREME CAUTION TO NOT CORRUPT Q */  \
    QueueUnget(pQ, item)

  #define QUEUE_PURGE(pQ)                                                     \
    QueuePurge(pQ)

#endif // PURE_MACRO_QUEUE else


/* for small operations always use macros                                     */
#define QUEUE_PEEK(pQ, item)  (item = pQ->items[pQ->hdr.front])

#define QUEUE_OVERFLOWED(pQ) (pQ->hdr.size < pQ->hdr.count)

#define QUEUE_UNDERFLOWED(pQ) (0 > pQ->hdr.count)

#define QUEUE_FULL(pQ)  (pQ->hdr.size <= pQ->hdr.count)

#define QUEUE_NOT_FULL(pQ)  (pQ->hdr.size > pQ->hdr.count)

#define QUEUE_AVAIL_SPACE(pQ) (pQ->hdr.size - pQ->hdr.count)

#define QUEUE_EMPTY(pQ) (0 >= pQ->hdr.count)

#define QUEUE_NOT_EMPTY(pQ) (0 < pQ->hdr.count)

#define QUEUE_AVAIL_DATA(pQ) (pQ->hdr.count)

#endif  // QUEUE_H__
