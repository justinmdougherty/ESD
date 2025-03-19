#ifndef KEYPAD_H__
#define	KEYPAD_H__
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : keypad.h
 *
 *  DESCRIPTION   : Declare driver for hand-held 4-button keypad.
 *    Do to system diving requirement, button inputs accepted upon release.
 *    Driver has two types of operations: (1) periodic scan for user inputs
 *    and (2) interrupt on PWR button press to wake unit from PWR OFF state.
 *
 *    Will set system global doNotSleep when keypad input detected.
 *    When new input detected, sets exposed attributes isKeypadInputReady = true
 *    and acptKeypadInput to the scanned/accepted keypad input value.
 *
 *    Consumer __must__ set isKeypadInputReady = false after processing
 *    acptKeypadInput, after which new keypad inputs to be scanned/accepted.
 *
 *    micro_defs.h must define KP_C0, KP_C1, KP_R0, and KP_R1
 *
 *  NOTE - this driver requires internal pull-up resistors enabled on R0 and R1
 *  NOTE - Peripheral Pin Selection for INT2 must be made by higher level code
 *  NOTE - this driver uses Timer3 for periodic scanning of keypad for input
 *  NOTE - this driver uses INT2 for PWR button to wake PIC when unit 'OFF'
 *
 *      (1) void InitKeypadDriver(void)
 *      (2) void StartKeypadScanOps(void)
 *      (3) void StartKeypadWakeOps(void)
 *      (M) PAUSE_KEYPAD_OPS()
 *
 *  USE           :
 *    configure PIC pins and Peripheral-Pin-Select
 *    InitKeypadDriver();     // must call first
 *    StartKeypadScanOps();
 *    while ...
 *      if (isKeypadInputReady)
 *        Process(acptKeypadInput);
 *        isKeypadInputReady = false;   // set false only after processing input
 *      if need to sleep ...
 *        StartKeypadWakeOps();
 *        SLEEP();
 *
 *  WRITTEN BY    : Robert Kirby, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2020/08/20, Robert Kirby, NSWC H12
 *      Make KEYPAD_SCANCODE_HDN a permanent feature (no conditional compile)
 *    2020/04/06, Robert Kirby, NSWC H12
 *      Add conditional compilation on ALLOW_HDN_INPUT ?defined?
 *      Refactor KEYPAD_SCANCODE_ADM to KEYPAD_SCANCODE_HDN.  Add/mod comments.
 *    2018/12/17, Robert Kirby, NSWC H12
 *      Add KEYPAD_SCANCODE_ADM to get to temporary/expedient hidden screen
 *    2017/08/30, Robert Kirby, NSWC H12
 *      Add macro PAUSE_KEYPAD_OPS()
 *    2017/08/21, Robert Kirby, NSWC H12
 *      Add KEYPAD_SCANCODE_ZERO and update keypad scan comments
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Initial development
 */
//++PROCEDURES******************************************************************
//  void InitKeypadDriver(void)
//  Initializes driver module attributes and configures the needed interrupts
//  but does NOT enable the interrupts. Must call StartKeypad____Ops to kick-off
//  actual keypad scan or wake operations (note that one will cancel the other).
//
//  NOTE - higher level code must do Peripheral Pin Select for INT2 and I/O pin
//    configuration before this routine is called.
//
//  INPUT : NONE
//  OUTPUT: NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void StartKeypadScanOps(void)
//  Starts the non-blocking process of periodically scanning for keypad inputs.
//
//  INPUT : NONE
//  OUTPUT: NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void StartKeypadWakeOps(void)
//  Makes the system ready for uC SLEEP from which the PWR button will wake it.
//  Note this does not put the uC in SLEEP, just sets wake-up trigger.
//
//  INPUT : NONE
//  OUTPUT: NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

// When a keypad button is pressed, it grounds a pin and thus its BITx reads 0
// Definitions below assume internal pull-up resistors enabled on R0 & R1,
// keypad scans X & Y, and keypad scan code template of:
//    C1 C0 R1 R0 C1 C0 R1 R0   ==    BIT: 7   6   5   4     3   2   1   0
//    0  1  X  X  1  0  Y  Y    ==         0   1  '^' PWR    1   0  '>' ENT
#define KEYPAD_RESET_CODE     (0x00)  // Use to reset/invalidate scan code
#define KEYPAD_TEMPL_CODE     (0x48)  // Template on which all scan codes built
#define KEYPAD_SCANCODE_ALL   (0x48)  // All buttons simultaneously pressed
#define KEYPAD_SCANCODE_NONE  (0x7b)  // No button pressed

#define KEYPAD_SCANCODE_ENT   (0x7a)  // Enter ENT button pressed     BIT0 = 0
#define KEYPAD_SCANCODE_RT    (0x79)  // Right '>' button pressed     BIT1 = 0
#define KEYPAD_SCANCODE_PWR   (0x6b)  // Power PWR button pressed     BIT4 = 0
#define KEYPAD_SCANCODE_UP    (0x5b)  // Up '^' button pressed        BIT5 = 0

#define KEYPAD_SCANCODE_911   (0x78)  // 911 is ENT and '>' simultaneously
#define KEYPAD_SCANCODE_ZERO  (0x59)  // Zeroize is '^' and '>' simultaneously

// Do NOT mention to outsiders the existance of this hidden keypad entry option
// Use it sparingly and only in places where neither '^' nor ENT are valid input
#define KEYPAD_SCANCODE_HDN   (0x5a)  // HIDDEN is '^' and ENT simultaneously

//----- EXPOSED ATTRIBUTE DECLARATIONS -----------------------------------------
extern volatile bool     isKeypadInputReady;  // flags when new input accepted
extern volatile uint8_t  acptKeypadInput;     // accepted keypad input scan code


//----- EXPOSED METHODS --------------------------------------------------------
void InitKeypadDriver(void);
void StartKeypadScanOps(void);
void StartKeypadWakeOps(void);


//----- MACROS -----------------------------------------------------------------
#define PAUSE_KEYPAD_OPS()  InitKeypadDriver()


#endif  // KEYPAD_H__
