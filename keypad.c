////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : keypad.c
 *
 *  DESCRIPTION   : Define driver for hand-held 4-button keypad.
 *    Do to system diving requirement, button inputs accepted upon release.
 *    Driver has two types of operations: (1) periodic scan for user inputs
 *    and (2) interrupt on PWR button press to wake unit from PWR OFF state.
 *
 *  NOTE - this driver requires consumer enable internal pull-ups on R0 and R1
 *  NOTE - this driver uses Timer3 for periodic scanning of keypad for input
 *  NOTE - this driver uses INT2 for PWR button to wake PIC when unit 'OFF'
 *
 *      (1) void InitKeypadDriver(void)
 *      (2) void StartKeypadScanOps(void)
 *      (3) void StartKeypadWakeOps(void)
 *      (*) void BuildScanCode(void)
 *      (*) void __attribute__((interrupt, no_auto_psv)) _T3Interrupt( void )
 *      (*) void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt( void )
 *
 *  WRITTEN BY    : Robert Kirby, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/06/30, Robert Kirby, NSWC H12
 *      Fix routine BuildScanCode description and other comments
 *    2020/08/20, Robert Kirby, NSWC H12
 *      Make KEYPAD_SCANCODE_HDN a permanent feature (no conditional compile)
 *    2020/04/06, Robert Kirby, NSWC H12
 *      Add conditional compilation on ALLOW_HDN_INPUT ?defined? in keypad.h
 *      Refactor KEYPAD_SCANCODE_ADM to KEYPAD_SCANCODE_HDN (and related)
 *    2018/12/17, Robert Kirby, NSWC H12
 *      Modify _T3Interrupt() to accept 2-key input KEYPAD_SCANCODE_ADM
 *    2018/09/21, Robert Kirby, NSWC H12
 *      Found there was no need to include main.h when refactoring sysErrFlags
 *    2017/08/21, Robert Kirby, NSWC H12
 *      Mod _T3Interrupt to add logic for KEYPAD_SCANCODE_ZERO
 *      Define and use macros for releasing multi-button codes ZEROIZE and 911
 *    2017/04/17, Robert Kirby, NSWC H12
 *      Do not latch new keypad input if the last latched input was not NONE;
 *      all the better to handle diving [de]compression while turned on my dear.
 *    2017/04/07, Robert Kirby, NSWC H12
 *      Set upper time limit on keypad press down from 5 to 2 seconds
 *    2017/03/24, Robert Kirby, NSWC H12
 *      Initial development
 *
 *  REFERENCE DOCUMENTS
 *    1.
 */
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "keypad.h"
#include "micro_defs.h"     // for FCY, KP_C0 etc.
#include <libpic30.h>       // For delay functions MUST follow define of FCY


//----- DEFINES, ENUMS, STRUCTS, TYPEDEFS, ETC. --------------------------------
#define KEYPAD_C0_SET_ON      { KP_C0 = 1; }    // Keypad column 0 control
#define KEYPAD_C0_SET_OFF     { KP_C0 = 0; }
#define KEYPAD_C1_SET_ON      { KP_C1 = 1; }    // Keypad column 1 control
#define KEYPAD_C1_SET_OFF     { KP_C1 = 0; }
#define KEYPAD_R0_IS_ON        (KP_R0 == 1)     // Keypad row 0 determination
#define KEYPAD_R0_IS_OFF       (KP_R0 == 0)
#define KEYPAD_R1_IS_ON        (KP_R1 == 1)     // Keypad row 1 determination
#define KEYPAD_R1_IS_OFF       (KP_R1 == 0)

#if (10000000L == FCY)        // Ensure FCY is what these defines assume
  #define PR_128HZ      305   // (10MHz/256) / 305 ~= 128.1 Hz
  #define MIN_SCAN_CT     6   // 47 ms  ~= (  6 cycles / (128 cycles / 1 s))
  #define MAX_SCAN_CT   256   //  2 sec ~= (256 cycles / (128 cycles / 1 s))
#endif

//----- EXPOSED ATTRIBUTE DECLARATIONS -----------------------------------------
volatile bool         isKeypadInputReady;     // informs consumer when key ready
volatile uint8_t      acptKeypadInput;        // tell consumer the accepted key

//----- MODULE ATTRIBUTES ------------------------------------------------------
static volatile  uint8bits_t  scanCode;       // last scanned keypad code
static volatile  uint16_t     scanCodeRptCt;  // consecutive scans of same code


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
//  CALLS : NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void InitKeypadDriver(void)
{
  IEC1bits.INT2IE   = 0;        // Disable PWR button interrupt on INT2
  PMD1bits.T3MD     = 1;        // Temporarily de-clock/reset Timer-3 peripheral
  IEC0bits.T3IE     = 0;        // Ensure Timer-3's interrupt disabled

  KEYPAD_C0_SET_OFF;            // Set both keypad drive pins low
  KEYPAD_C1_SET_OFF;            // until need to actively scan keypad

  IFS1bits.INT2IF   = 0;        // Clear INT2 status flag
  INTCON2bits.INT2EP= 1;        // and setup for negative edge trigger
  IPC7bits.INT2IP   = INT2PRI;  // Set priority per micro_defs.h

  PMD1bits.T3MD     = 0;        // Now clock Timer-3 peripheral
  IPC2bits.T3IP     = T3PRI;    // Set priority per micro_defs.h
  T2CONbits.T32     = 0;        // run T2 and T3 as independent 16-bit timers
  T3CONbits.TCKPS0  = 1;        // Set Timer-3 prescaler to divide Fcy
  T3CONbits.TCKPS1  = 1;        // by 256
  PR3               = PR_128HZ; // and set the period for timer to expire.

  // Initialize module attributes
  scanCode.val        = KEYPAD_RESET_CODE;
  scanCodeRptCt       = 0;
  isKeypadInputReady  = false;
  acptKeypadInput     = KEYPAD_RESET_CODE;
} // end routine InitKeypadDriver


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void StartKeypadScanOps(void)
//  Starts the non-blocking process of periodically scanning for keypad inputs.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void StartKeypadScanOps(void)
{
  IEC1bits.INT2IE = 0;          // Disable power button interrupt

  TMR3            = 0x0000;     // Ensure full-length period for first time-out
  IFS0bits.T3IF   = 0;          // Clear the Timer-3 interrupt flag
  IEC0bits.T3IE   = 1;          // Enable Timer-3's interrupt
  T3CONbits.TON   = 1;          // Turn on Timer-3
} // end routine StartKeypadScanOps


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void StartKeypadWakeOps(void)
//  Makes the system ready for uC SLEEP from which the PWR button will wake it.
//  Note this does not put the uC in SLEEP, just sets wake-up trigger.
//
//  INPUT : NONE
//  OUTPUT: NONE
//  CALLS : NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void StartKeypadWakeOps(void)
{
  IEC0bits.T3IE   = 0;          // Disable Timer3 IRQ (periodic keypad scans)
  T3CONbits.TON   = 0;          // Turn off Timer-3 as we don't need it now

  IEC1bits.INT2IE = 1;          // Enable power button interrupt
} // end routine StartKeypadWakeOps


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void BuildScanCode(void)
//  Drive pins on keypad columns and read pins on keypad rows to build scanCode
//
//  INPUT : NONE
//  OUTPUT: NONE but modified attribute scanCode
//  CALLS : NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void BuildScanCode(void)
{                                             // Build keypad code using 2 scans
  KEYPAD_C0_SET_ON;                           // BITS - b7 b6 b5 b4 b3 b2 b1 b0
  KEYPAD_C1_SET_OFF;                          // PINS - c1 C0 R1 R0 C1 c0 R1 R0
  scanCode.val  = 0b01000000;                 // Set bit 6 as pin levels settle
  if (KEYPAD_R1_IS_ON)  { scanCode.b5 = 1; }  // Set bit 5 if R1 on 1st scan
  if (KEYPAD_R0_IS_ON)  { scanCode.b4 = 1; }  // Set bit 4 if R0 on 1st scan
  KEYPAD_C0_SET_OFF;                          // Swap which Cx pin is driving
  KEYPAD_C1_SET_ON;                           // the Rx pins for another scan
  scanCode.b3 = 1;                            // Set bit 3 as pin levels settle
  if (KEYPAD_R1_IS_ON)  { scanCode.b1 = 1; }  // Set bit 1 if R1 on 2nd scan
  if (KEYPAD_R0_IS_ON)  { scanCode.b0 = 1; }  // Set bit 0 if R0 on 2nd scan
  KEYPAD_C1_SET_OFF;                          // Done actively scanning buttons
} // end routine BuildScanCode


//++ INTERRUPT SERVICE ROUTINE +++++++++++++++++++++++++++++++++++++++++++++++++
//  _T3Interrupt
//  ~128 Hz timer interrupt for reading keypad status.  Because of need to
//  support diving, keypad input is accepted upon button release (provided the
//  button was not held too long).  Will also accept KEYPAD_SCANCODE_NONE but
//  only once between accepting other button presses.
//
//  CALLS : BuildScanCode
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt( void )
{
  // When was ZERO(UP+RT) allow either UP or RT just now as user releases button
  #define IS_ZERO_RELEASE   ((KEYPAD_SCANCODE_ZERO == lastKeypadScanCode) &&  \
                             ((KEYPAD_SCANCODE_UP  == scanCode.val) ||        \
                              (KEYPAD_SCANCODE_RT  == scanCode.val)))
  // When was 911(ENT+RT) allow either ENT or RT just now as user releases btn
  #define IS_911_RELEASE    ((KEYPAD_SCANCODE_911  == lastKeypadScanCode) &&  \
                             ((KEYPAD_SCANCODE_ENT == scanCode.val) ||        \
                              (KEYPAD_SCANCODE_RT  == scanCode.val)))
  // When was HDN(UP+ENT) allow either UP or ENT just now as user releases btn
  #define IS_HDN_RELEASE    ((KEYPAD_SCANCODE_HDN  == lastKeypadScanCode) &&  \
                             ((KEYPAD_SCANCODE_UP  == scanCode.val) ||        \
                              (KEYPAD_SCANCODE_ENT == scanCode.val)))

  uint8_t lastKeypadScanCode;
  bool    latchScanCode = false;

  IFS0bits.T3IF = 0;                    // Always clear T3 interrupt status flag
                                        // which brought us here.
  if (isKeypadInputReady)       {       // When last valid input not processed
    return;                     }       // nothing else to do but return

  lastKeypadScanCode = scanCode.val;    // Remember last keypad code before
  BuildScanCode();                      // building current keypad scan code

  if ( lastKeypadScanCode == scanCode.val )
  {                                     // When consecutive repeating scan code
    ++scanCodeRptCt;                    // increment consecutive same-key count
    if (0 == scanCodeRptCt)   {         // when increment causes uint rollover
      scanCodeRptCt--;        }         // set value back to uint max

    if ((MIN_SCAN_CT == scanCodeRptCt)          &&
        (KEYPAD_SCANCODE_NONE == scanCode.val)  &&
        (KEYPAD_SCANCODE_NONE != acptKeypadInput))
    { // exactly MIN_SCAN_CT reps of new KEYPAD_SCANCODE_NONE makes it valid
      latchScanCode = true;             // Latch in keypad returned to NONE here
    }                                   // but latch in others when key released
  }
  else // scancode change - button release or simultaneous button press scenario
  {
    if ((MIN_SCAN_CT <= scanCodeRptCt) &&               // Only when press time
        (MAX_SCAN_CT >= scanCodeRptCt) &&               // in valid range and
        (KEYPAD_SCANCODE_NONE == acptKeypadInput))      // previously NONE press
    {
      if (IS_ZERO_RELEASE)                              // Do special check for
      {                                                 // multi-btn ZERO code
        latchScanCode = true;                           // to latch release
      }
      else if (IS_911_RELEASE)                          // Do special check for
      {                                                 // multi-button 911 code
        latchScanCode = true;                           // to latch release
      }
      else if (IS_HDN_RELEASE)                          // Do special check for
      {                                                 // multi-button HDN code
        latchScanCode = true;                           // to latch release
      }
      else if (KEYPAD_SCANCODE_NONE  == scanCode.val)   // Regardless what was,
      {                                                 // when now NONE pressed
        latchScanCode = true;                           // whatever was pressed
      }                                                 // should be latched.
      // else invalid [combo] button pressed which we don't ever accept
    }
    else if (MAX_SCAN_CT < scanCodeRptCt)     // When experience excessive press
    {                                         // do NOT latch as input-ready but
      acptKeypadInput = lastKeypadScanCode;   // remember what it was as part of
    }                                         // not reacting to dive pressures

    scanCodeRptCt = 0;                        // scan code changed, so no repeat
  }

  if (latchScanCode)
  {                                           // When we decided to latch input
    acptKeypadInput    = lastKeypadScanCode;  // accept entry before btn release
    isKeypadInputReady = true;                // let consumer know input ready
    doNotSleep         = true;                // and run main program loop.
  }

} // end ISR _T3Interrupt


//++ INTERRUPT SERVICE ROUTINE +++++++++++++++++++++++++++++++++++++++++++++++++
//  _INT2Interrupt
//  INT2 interrupt for signal coming from keypad R0 on PWR button press while
//  in OFF state.  Get here when INT2 wakes the uC.  Debounce button to ensure
//  real press of power button -- but not too long, e.g. dive pressure.  Must
//  return to KEYPAD_SCANCODE_NONE within 2-seconds or we discount and go back
//  to sleep until the next time INT2 wakes the uC.
//
//  CALLS : BuildScanCode
//          __delay_ms
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt( void )
{
  #define MAX_MS_WAIT_REL   2000                // max millisec for btn release
  #define CHK_REL_MS_WAIT     10                // check period for btn release
  #define MAX_REL_WAITS    (MAX_MS_WAIT_REL/CHK_REL_MS_WAIT)

  uint16_t sanity = MAX_REL_WAITS;              // Have limited sanity waiting

  BuildScanCode();                              // Get current keypad scan code
  if (KEYPAD_SCANCODE_PWR == scanCode.val)
  {                                             // When only PWR button pressed
    __delay_ms(20);                             // de-bounce button input and
    BuildScanCode();                            // determine scan code again.
    if (KEYPAD_SCANCODE_PWR == scanCode.val)    // Successfully debounced PWR
    {                                           // only when PWR still pressed
      do
      {
        __delay_ms(CHK_REL_MS_WAIT);
        sanity--;
        BuildScanCode();
      } while (sanity && (KEYPAD_SCANCODE_NONE != scanCode.val));
      __delay_ms(20);                           // de-bounce button input and
      if (KEYPAD_SCANCODE_NONE == scanCode.val) // verify user released all btns
      {
        isKeypadInputReady = true;              // We are accepting keypad input
        acptKeypadInput = KEYPAD_SCANCODE_NONE; // must be NONE
        doNotSleep      = true;                 // when we wake from sleep.
      }
    }
  }
  IFS1bits.INT2IF = 0;                          // Clear INT2 interrupt flag
} // end ISR _INT2Interrupt
