////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : main.c
 *
 *  DESCRIPTION   : Define top level (main) attributes and methods required to
 *    implement user interface (aka ESD) of NSWCDD's Lightning BOLT hand-held.
 *
 *    At application of power, firmware runs initialization and Built-In-Test.
 *    Then asks if batteries are new and if user wants to power-on (awake) or
 *    [time-out and] power-off (go to sleep).
 *    To user, sleep looks like OFF but really just super low power state.
 *    This paradigm results in full BIT every time power reapplied but during
 *    mission have fast "power-on/boot" from OFF (asleep) to ON (awake), which
 *    users really like.
 *
 *      (*) void Tmr2_1SecEventsCb(void)
 *      (*) uint8_t htou8(char * hex)
 *      (*) void SetWaveformTraits(void)
 *      (*) void PostLtngCmd(void)
 *      (*) bool QueueLtngCmd(cmdid_t cmd, char * param)
 *      (*) void ProcessLtngRpt(void)
 *      (*) void UpdateSysCheckDisplay(void)
 *      (*) void UpdateNewBtryDisplay(void)
 *      (*) void UpdatePwrOnOffDisplay(void)
 *      (*) void UpdateConfirmAdminDisplay(void)
 *      (*) void UpdateAdminWrnDisplay(void)
 *      (*) void UpdateExfilOptDisplay(void)
 *      (*) void UpdateGeoMuteOvrdDisplay(void)
 *      (*) void UpdateInfilOptDisplay(void)
 *      (*) void UpdateZeroizeDisplay(void)
 *      (*) void UpdateDisplayedCoord(void)
 *      (*) void UpdateDisplayedBitIcon(void)
 *      (*) void UpdateDisplayedPwrStat(void)
 *      (*) void UpdateDisplayedSysTime(void)
 *      (*) void UpdateDisplayedActvBrevCode(void)
 *      (*) void UpdateDisplayedMailIcon(void)
 *      (*) void UpdateDisplayedAof(void)
 *      (*) void UpdateDisplayedOpStatus(void)
 *      (*) void UpdateHomeDisplay(void)
 *      (*) void UpdateDisplayedGdbMsg(void)
 *      (*) void UpdateDisplayedGdbXY(void)
 *      (*) void UpdateGdbInfilDisplay(void)
 *      (*) void UpdateRngBrgDisplay(void)
 *      (*) void UpdateBitResultsDisplay(void)
 *      (*) void UpdateWfNameDisplay(void)
 *      (*) void UpdateKeyNamesDisplay(void)
 *      (*) void UpdateDisplayedBrevHundredsDgt(void)
 *      (*) void UpdateDisplayedBrevTensDgt(void)
 *      (*) void UpdateDisplayedBrevOnesDgt(void)
 *      (*) void UpdateBrevityCodeDisplay(void)
 *      (*) void UpdateSetCoordSysDisplay(void)
 *      (*) void UpdateSetBkltDisplay(void)
 *      (*) void UpdateTxDtyCyDisplay(void)
 *      (*) void UpdateTxPwrDisplay(void)
 *      (*) void UpdateLowPwrModeDisplay(void)
 *      (*) void UpdateDelInfilRbDisplay(void)
 *      (*) void ProcessTimeEvents(void)
 *      (*) void Invoke911(void)
 *      (*) void ProcessBrevCodeEnt(void)
 *      (*) void ProcessSysCheckUsrInp(void)
 *      (*) void ProcessNewBtryUsrInp(void)
 *      (*) void ProcessPwrOnOffUsrInp(void)
 *      (*) void ProcessHpwrOnOffUsrInp(void)
 *      (*) void ProcessZeroizeUsrInp(void)
 *      (*) void ProcessConfAdmUsrInp(void)
 *      (*) void ProcessAdmWarnUsrInp(void)
 *      (*) void ProcessExfilOptUsrInp(void)
 *      (*) void ProcessGeoMuteOvrdUsrInp(void)
 *      (*) void ProcessInfilOptUsrInp(void)
 *      (*) void ProcessHomeUsrInp(void)
 *      (*) void ProcessGdbUsrInp(void)
 *      (*) void ProcessRngBrgUsrInp(void)
 *      (*) void ProcessBitResultsUsrInp(void)
 *      (*) void ProcessWfNameUsrInp(void)
 *      (*) void ProcessKeyNamesUsrInp(void)
 *      (*) void ProcessBrevUsrInp(void)
 *      (*) void ProcessCoordSysUsrInp(void)
 *      (*) void ProcessBkltUsrInp(void)
 *      (*) void ProcessTxDtyCyUsrInp(void)
 *      (*) void ProcessTxPwrUsrInp(void)
 *      (*) void ProcessLowPwrModeUsrInp(void)
 *      (*) void ProcessConfDelInfilRbUsrInp(void)
 *      (*) void ProcessKeypadInput(void)
 *      (*) void ProcessLtngData(void)
 *      (*) void ProcessErrors(void)
 *      (*) void PrepForSleep(void)
 *      (*) void GetBackToWork(void)
 *      (*) void UpdateDisplay(void)
 *      (*) void InitSystem(void)
 *      (1) int16_t main(void)
 *
 *  WRITTEN BY    : Robert Kirby, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/10/26, Robert Kirby, NSWC H15
 *      More pre-defined waveforms, specifically for 2021/09/28 wf discussion, &
 *      anticipating future growth. Now (AME6|AME8|UWRL|SHLN * Band_1|2|3 * G)
 *      Handle D.GLL providing lat/lon as +DDD.ddddd rather than hDDD.MM.SS
 *      Add handling infil message 0C for display on new Range & Bearing screen
 *      Add attribute for index of newest msg unread wrt general GDB C2 infil
 *      Remove some stubbed out vestigial code; replace various coordinates
 *      buffers with single coords_t myLoc (also holds floating point coords);
 *      Routine code for Start Lbhh Zeroize since in more than one place;
 *      Routine code to FIFO general purpose GDB infil for readability;
 *      Refactor dsplfld_t .brev to .actvBrev for clarity;
 *      Move define of COORD_LEN from main.c to coords.h
 *      Add option to display GPS coordinates as decimal degrees (+DDD.ddddd)
 *      Add "Replace Batteries" notice to P-BIT's UpdateSysCheckDisplay()
 *      Automate "60-sec wait after waveform selection" and cycle Ltng power so
 *      no longer need to indicate an active brevity code.
 *      Change CH/UWRL duty cycle options to Mute, 1/10, 1/2, 1/1
 *      Fix Geo-Mute admin screen selection not getting saved to nvmem until
 *      something else changed (e.g. brevity).
 *      Fix not displaying 1st rcvd GDB if in GDB screen when 1st msg arrived
 *      Implement All C.TXP options not just Norm|Max (now Min|Low|Norm|Max)
 *    2021/07/15, Robert Kirby, NSWC H12
 *      Do not Invoke911() when geo-muted (i.e. WGM_ACTVD == selWfTrait.wgmOpt)
 *    2021/06/03, Robert Kirby, NSWC H12
 *      Support new/modified geo-mute & future waveform classes/families (wc_t)
 *      Support for geomute: add Admin OVERRIDE screen, set/use wgm_t in wfTrait
 *      and devCfg, check D.GLL data against geozone, mute transmits and disable
 *      BREV & DUTY CYCLE screens as appropriate
 *      Refactor devcfg_t .waveform to .selWfChar and use .selWfIdx
 *      Define and use selWfTrait for wfTrait[devCfg.selWfIdx] for readability
 *      Move TEMP_SCHED_CTDN to bt_waveform_traits.h (shared with Lightning)
 *      Update comments; delete stubbed out vestigial code
 *    2020/08/20, Robert Kirby, NSWC H12
 *      Have KEYPAD_SCANCODE_HDN in INFIL KEY NAMES screen send "C.ECP 1" & set
 *      focus to HOME (way to de-expose BOLT Debug port when units returned from
 *      CTC or NIWC and expose Lightning to COMx as is typical)
 *      Change CID_OECP/oEcp to CID_CECP/cEcp so save ECP config in Ltng nvmem
 *      Refactor ALLOW_HDN_INPUT to ALLOW_BITSCRN_HDN_INPUT & still not #defined
 *    2020/04/06, Robert Kirby, NSWC H12
 *      Implement new "waveform information" interface messages. This gets us
 *      away from wf load-out specific fw versions that depend on changing
 *      bt_waveform_load.h.
 *      Add conditional compilation on ALLOW_HDN_INPUT ?defined? in keypad.h
 *      Refactor KEYPAD_SCANCODE_ADM to KEYPAD_SCANCODE_HDN
 *      Modify EXFIL WAVEFORM screen to also display TX ID
 *      When invalid waveform selected modify screen flow & do NOT allow 911
 *    2019/12/09, Robert Kirby, NSWC H12
 *      From FP_XP_N/M go up to FP_ADM_N - unhide admin capabilities but don't
 *      remove the hidden path just yet (so two ways to access admin functions)
 *    2019/09/06, Robert Kirby, NSWC H12
 *      Stub out code for obsolete SI7021 Temperature & Humidity sensor IC
 *      Allow brevity 000 for UNDERWORLD waveforms, e.g. CH, as values limited
 *      Fix comments, max range of UnderWorld allowed user brevity entry is 014
 *    2019/06/28, Robert Kirby, NSWC H12
 *      Move define BOOT_MUTE to bt_waveform_load.h for use by both ESD & Ltng
 *      Modify UpdateTxDtyCyDisplay() & ProcessTxDtyCyUsrInp() to support four
 *      CH duty cycle options (Mute, 4/40, 4/8, 4/4) and brevity range comments
 *      Modify UpdateDisplayedOpStatus() to not countdown more than 30 sec
 *    2019/03/01, Robert Kirby, NSWC H12
 *      Use Lightning GKN messages to get names of loaded keys for admin screen
 *    2019/01/25, Robert Kirby, NSWC H12
 *      Move beyond use of LYNX_6, LYNX_8, & FOG defines; add SetWaveformTraits.
 *      Break ProcessKeypadInput into more manageable in-line routines and add
 *      significant waveform dependency capabilities to them.
 *      Add & use routines UpdateHomeDisplay, UpdateGdbInfilDisplay, and
 *      UpdateBrevityCodeDisplay to clean up switch statement in UpdateDisplay.
 *      Have QueueLtngCmd set esdErrFlags if not processing queue properly
 *      Some name refactoring and commenting.
 *    2018/12/17, Robert Kirby, NSWC H12
 *      Add hidden Admin screens & commands to select exfil and infil options
 *      Add screen that shows active exfil waveform
 *      Delete extra writes of Exfil & Infil...PASS in UpdateBitResultsDisplay
 *    2018/11/15, Robert Kirby, NSWC H12
 *      Add FOG waveform support via conditional compilation based on esd_ver.h,
 *      this impacts functionality associate w/brevity, duty cycle, and TX power
 *      user feedback is keep the menu flows the same (as much as practical)
 *      Modify ProcessLtngRpt to cause BIT icon & screen update when D.BIT rcvd
 *      Modify PrepForSleep to clear ltngBitRpt error flags
 *      Modify GetBackToWork to query active infil key names as part of PBIT
 *      Add define for BOOT_MUTE which is likely to become the standard but
 *      for now make it conditional until get broader concensus
 *      Delete vestigial define USE_LYNX8 as replaced by LYNX_x in esd_ver.h
 *    2018/09/25, Robert Kirby, NSWC H12
 *      Add NVG and remove MED backlight setting (now have HIGH, LOW, NVG, OFF)
 *      Refactor sysErrFlags to esdErrFlags as it doesn't cover Ltng errors
 *      UpdateBitResultsDisplay() add check of ltngBitRpt as well as esdErrFlags
 *      Adjust applicable display strings/offsets to rid leading <SP> in code
 *      Add comments in ProcessLtngRpt(), UpdateSysCheckDisplay(), etc.
 *    2017/12/18, Robert Kirby, NSWC H12
 *      Use defines LYNX_6 and LYNX_8 to support easy switches between 6 & 8
 *    2017/10/20, Robert Kirby, NSWC H12
 *      Modify PrepForSleep to clear most error flags so they don't carry-over
 *      Modify ProcessLtngRpt since esd_t no longer has field ltngBit
 *    2017/08/30, Robert Kirby, NSWC H12
 *      Don't accept keypad inputs while zeroize WORKING, i.e. PAUSE_KEYPAD_OPS
 *      Comments (esp wrt location of fields on HOME)
 *    2017/08/21, Robert Kirby, NSWC H12
 *      Change case of some screen texts
 *      Add support for zeroize (C.ZUM) and Lightning version info (D.LCM)
 *      Add support for Lynx-8 waveform (upgrading from Lynx-6)
 *      Add new screen to display up to top six BIT results + function htou8()
 *      Add new screen to display GDB Active Key Names
 *      Use uKey and inF in esd_t to flag/report unique GDB Key not loaded
 *      Use esd_ver.h to hold/provide firmware version string
 *      Add importance of relative values within esdfp_t to improve 911 handling
 *      Allow gas gauge roll-over but set sysErrFlag.gasGa when % increases
 *      Use ltng_cmpnt.h for saving/displaying reported Lightning BIT results
 *      Improve handling of KEYPAD_SCANCODE_PWR and KEYPAD_SCANCODE_911
 *      Handle no D.RST while in System Check screen; display DEPOT MAINT REQ
 *      Get rid of extra ' ' chars, use horizontal offset writing strings to LCD
 *      Re-order file location of procedures; update comments
 *    2017/07/27, Robert Kirby, NSWC H12
 *      Fix not going to Coordinates screen from Home when up-arrow in 911.
 *      Fix not displaying EXFIL MUTED when re-mute during CBC squawks in mute
 *      To prevent gas gauge roll-over, only check when btryPct not already 0
 *      Refactor UpdateSetCoordSys to UpdateSetCoordSysDisplay for consistency
 *      Refactor FP_TDC_A to FP_TDC_N, FP_XP_H to FP_XP_N, FP_XP_F to FP_XP_M
 *    2017/05/02, Robert Kirby, NSWC H12
 *      Modify ProcessLtngRpt() to strstr(rpt, "no GPS") before "GPS"
 *    2017/04/10, Robert Kirby, NSWC H12
 *      Set sysErrFlags.ltngBit based on received D.BIT report(s)
 *      Modify report for SYSHW errors based on updated esd_t
 *    2017/04/07, Robert Kirby, NSWC H12
 *      When AoF critical (CRIT_AOF), blink rather than clear GPS position info
 *      Rename gspValid aofValid as no longer stop displaying old fix location
 *      Add attribute gpsTime for decisions wrt time string and calculating NBE
 *      Set aof = 0 when get D.GLL instead of D.TGF (keep coord in sync w/ aof
 *      so coord cant get stuck in reverse video)
 *      When duty cycle is MUTE, display that instead of brevity code on HOME
 *      Rename TDC_AUTO to the more appropriate TDC_NORM
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Initial development as total revamp of the ESD written by N.Sunshine,
 *      which was itself based on A.Culpepper's Squish BT7GDB assembly code.
 *
 *  REFERENCE DOCUMENTS
 *    1.  PIC24FJ128GA202 Family Data Sheet (Microchip DS30010038C)
 *    2.  Lightning End User's Software ICD
 *    3.  ... System Requirements Document (NSWCDD Code H12)
 *    *.  Abbreviations and Acronyms... (ASME Y14.38-2007)
 *    *.  MIL-STD-12D Military Standard Abbreviations ...
 */
#include <xc.h>
#include <ctype.h>          // for isdigit()
#include <stdbool.h>        // Includes true/false definition
#include <stdint.h>         // Includes uint16_t definition
#include <stdio.h>
#include <string.h>         // for strcmp, memcpy, etc.
#include <stdlib.h>
#include "stdint_extended.h"
#include "main.h"
#include "micro_defs.h"     // System stuff like FCY and macro BT_INIT_UART()
#include "esd_ver.h"        // for FW_VER_STR
#include "config_memory.h"
#include "coords.h"
#include "fonts.h"
#include "geofence.h"
#include "i2c2.h"           // functions (or macros) to run I2C in polled mode
#include "lcd.h"            // for LCD_MAX_COLS etc.
#include "ltc2943.h"        // for LTC2943_ZERO_PT
#include "keypad.h"
#include "queue.h"
#include "tmr2.h"
#include "uc1701x.h"
#include "uart.h"
#include "uart1_queued.h"
#include <libpic30.h>       // For delay functions MUST follow define of FCY
#include "../Lightning/ltng_cmpnt.h"          // for ltng_t
#include "../Lightning/bt_waveform_traits.h"  // for BOOT_MUTE


//----- DEFINES, ENUMS, STRUCTS, TYPEDEFS, ETC. --------------------------------
#define BTRY_CRIT_LVL     5+1 // % below which battery level considered critical
//#define BTRY_CRIT_LVL   0   // 0% effectively disables critical battery level

// Useful definitions for displaying GDB C2 and R&B data from Lightning
#define TIMESTAMP_SIZE     9  // 8 char timestamp plus null terminator
#define GDB_MSG_Q_LEN     50  // Size of GDB messages queue (FIFO sorted by age)
#define PLI_DAT_Q_LEN     10  // Size of PLI data queue (FIFO sorted by ID)
// 123456789012345678901234567890123456789012345678901234567890123456789012
// sT.ABC 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEFp
#define LTG_RPT_MAX_LEN (1+6+16*4+1)  // D.GID are longest Lightning reports
// sD.GID 0F FREE TEXT LENGTH IS A MAXIMUM 43 CHARACTERS msg 1 of 9p
//        123456789012345678901234567890123456789012345678901234567
#define GDB_MSG_SIZE_MAX  61  // 57 char in msg plus four \0 for screen display

#define CRIT_AOF   (5*60 +15) // Per user rep 00:05:00, for BOLT timing 00:05:15
#define ZR2   {'0','0',0}     // two zero char str NULL terminated "00"
#define SP6   {' ',' ',' ',' ',' ',' ',0} // six SPACE char str
#define SP10  {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',0} // ten SPACE char str
// Useful definitions for screen timeouts and periodic system events
#define BIT_TEST_CTDN      8  // System Check should get D.RST in few seconds
#define BIT_PASS_CTDN      4  // System Check ENT time out when BIT passed
#define BIT_FAIL_CTDN     60  // System Check ENT time out when BIT failed
#define NEW_BTRY_CTDN     15  // New Battery Y/N time out in ~15 seconds
#define PWR_0_1_CTDN       3  // Power On/Off time out in ~2-3 seconds
#define DIR_N_Y_CTDN       3  // Delete INFIL and R&B timeout in ~2-3 seconds
#define ADMIN_CTDN         2  // Administrator Y/N time out in ~1-2 seconds
#define CSWF_CTDN         60  // Time for BOLT reset after swf, 60 <= ctdn < 100
#define ZUM_N_Y_CTDN       4  // Zeroize Unit Y/N time out in ~3-4 seconds
#define ZUM_ACT_CTDN      60  // Time for Zeroize functions to finish
#define GAS_CHK_CTDN      60  // Check gas gauge every 60 sec of battery use

typedef enum tagRESET_SOURCE
{
  NONE_FOUND = 0,
  POWER_ON,
  EXTERNAL,
  SOFTWARE,
  WATCHDOG,
  BROWN_OUT,
  CFG_WORD_MISMATCH,
  TRAP,
  ILLEGAL_INSTR,
} rstsrc_t;

typedef enum tagESD_FOCUS_POINT // !!! order of values matters !!!
{                   // Focus Point determines keypad input function
  FP_NA_SLP,        // No focus when just want to go to sleep
  FP_SYSCHK,        // System Check screen, waiting for Lightning RST & BIT
  FP_NEWBAT_N,      // New Battery screen - NO
  FP_NEWBAT_Y,      // New Battery screen - YES
  FP_PWR_0,         // Power screen (at reset or wake) - OFF
  FP_PWR_1,         // Power screen (at reset or wake) - ON
  FP_HPWR_0,        // Power screen (from HOME only) - OFF
  FP_HPWR_1,        // Power screen (from HOME only) - ON
  FP_ZERO_N,        // Zeroize screen - NO
  FP_ZERO_Y,        // Zeroize screen - YES
  FP_ZERO_A,        // Zeroize screen - ACTIVE/WORKING

  // Focus points for admin screens
  FP_EXF_CTDN,      // Countdown for power-cycle after sending C.SWF
  // For fp: (FP_EXF <= fp < FP_HOME) go to ADM WARNING screen on PWR press
  FP_EXF,           // Exfil option screen, no choice/change made yet
  FP_EXF_1,         // Send a C.SWF 1 - use waveform in BOLT slot 1
  FP_EXF_2,         // Send a C.SWF 2
  FP_EXF_3,         // Send a C.SWF 3
  FP_EXF_4,         // Send a C.SWF 4
  FP_OVRD_N,        // Overide Geo-Muting screen - NO
  FP_OVRD_Y,        // Overide Geo-Muting screen - YES
  FP_INF,           // Infil option screen, no choice/change made yet
  FP_INF_1,         // Send a C.GOK 1 - use infil key in group slot 1
  FP_INF_2,         // Send a C.GOK 2
  FP_INF_3,         // Send a C.GOK 3
  FP_INF_4,         // Send a C.GOK 4
  FP_INF_5,         // Send a C.GOK 5
  FP_INF_N,         // Send a C.GOK N - no active infil group key


  /**
   * This enumeration defines various FP_FSET values used in the system.
   * Each value represents a specific FP_FSET configuration.    
   * FP_FSET: Base value for FP_FSET.
   * FP_FSET_0 FP_FSET configuration 0 ETC...
   
   */
  FP_FSET,
  FP_FSET_0,
  FP_FSET_1,
  FP_FSET_2,
  FP_FSET_3,
  FP_FSET_4,
  FP_FSET_5,
  FP_FSET_6,
  FP_FSET_7,
  FP_FSET_8,
  FP_FSET_9,
  FP_FSET_10,
  FP_FSET_11,
  FP_FSET_12,
  FP_FSET_13,
  FP_FSET_14,
  FP_FSET_15,

  // Following focus points for normal operation after powering on...
  // For fp: (FP_HOME <= fp) allows Invoke911()
  FP_HOME,          // Home screen: PWR press goes to FP_HPWR_1, i.e. not cancel
  // For fp: (FP_HOME < fp) jump to HOME on PWR press (i.e. PWR acts as CANCEL)
  // For fp: (FP_HOME <= fp < FP_GDB) jump to FP_GDB when new GDB infil received
  FP_RNG_BRG,       // Range & Bearing to forwarded location
  FP_GDB,           // GDB screen
  // For fp: (FP_GDB < fp) do NOT jump to FP_GDB when new D.GID infil received
  FP_ADM_N,         // Entry to admin screens - NO, not administrator
  FP_ADM_Y,         // Entry to admin screens - yes, I'm administrator
  FP_ADM_WARN,      // Warning screen preceding & following admin action
  FP_BIT,           // Built-In-Test Results screen
  FP_WF_INFO,       // Active Exfil Waveform Info screen
  FP_KEYS,          // Active GDB Key Names screen
  FP_BREV,          // Brevity Code screen - nothing selected
  FP_BREV_H,        // Brevity Code screen - Hundreds digit
  FP_BREV_T,        // Brevity Code screen - Tens digit
  FP_BREV_O,        // Brevity Code screen - Ones digit
  FP_CS_DMS,        // Coordinate System - Degrees,Minutes,Seconds <HDDD.MM.SS>
  FP_CS_DEC,        // Coordinate System - Decimal degrees <+DDD.ddddd>
  FP_CS_MGRS,       // Coordinate System - MGRS <GZsi      ><1234 5678 >
  FP_BL_H,          // Backlight screen - HIGH
  FP_BL_L,          // Backlight screen - LOW
  FP_BL_N,          // Backlight screen - NVG
  FP_BL_O,          // Backlight screen - OFF
  FP_TDC_M,         // Exfil Transmit Duty Cycle screen - MUTE
  FP_TDC_S,         // Exfil Transmit Duty Cycle screen - SLOW
  FP_TDC_N,         // Exfil Transmit Duty Cycle screen - NORMAL
  FP_TDC_H,         // Exfil Transmit Duty Cycle screen - HIGH
  FP_XP_m,          // Exfil Transmit Power screen - MIN
  FP_XP_L,          // Exfil Transmit Power screen - LOW
  FP_XP_N,          // Exfil Transmit Power screen - NORM
  FP_XP_X,          // Exfil Transmit Power screen - MAX
  FP_LPM_N,         // Low Power Mode screen - NO
  FP_LPM_Y,         // Low Power Mode screen - Yes
  FP_DIR_N,         // Delete Infil/R&B screen - NO
  FP_DIR_Y,         // Delete Infil/R&B screen - YES
} esdfp_t;

typedef union tagDISPLAY_FIELD
{                           // UI display fields with dynamic data to be updated
  struct                    // Movement of already defined fields requires check
  {                         // for update HOME/GDB/BREV_DSPL_FLDS defines below!
    uint64_t coord    :1;   // BIT0 HOME: LAT/LON or two part MGRS info
    uint64_t bit      :1;   // BIT1 HOME: Built-In-Test indicator
    uint64_t pwr      :1;   // BIT2 HOME: ExtPwr or Battery %
    uint64_t time     :1;   // BIT3 HOME: system time
    uint64_t actvBrev :1;   // BIT4 HOME: active brevity code
    uint64_t mail     :1;   // BIT5 HOME or GDB: mail icon [and count]
    uint64_t aof      :1;   // BIT6 HOME: age of GPS fix
    uint64_t ops      :1;   // BIT7 HOME: system operating status (GPS,XMT,etc.)
    uint64_t brevH    :1;   // BIT8 BREVITY CODE: code hundreds digit
    uint64_t brevT    :1;   // BIT9 BREVITY CODE: code tens digit
    uint64_t brevO    :1;   // BITA BREVITY CODE: ones digit
    uint64_t gdb      :1;   // BITB GDB: time & message
    uint64_t gdbXY    :1;   // BITC GDB: message X/Y numbering
    uint64_t chgScr   :1;   // BITD change to entirely different screen
    uint64_t sysChk   :1;   // BITE various stages in System Check screen
    uint64_t newBat   :1;   // BITF NEW BATTERY screen Y/N options (w/ timeout)
    uint64_t pwr01    :1;   // BITG Power ON-OFF scrn ON/OFF options (w/timeout)
    uint64_t cSys     :1;   // BITH COORDINATE SYSTEM screen
    uint64_t bkLt     :1;   // BITI BACKLIGHT screen
    uint64_t txDtyCy  :1;   // BITJ TRANSMIT DUTY CYCLE screen
    uint64_t txPwr    :1;   // BITK TRANSMIT POWER screen
    uint64_t zeroize  :1;   // BITL ZEROIZE UNIT screen Y/N options
    uint64_t bitRslt  :1;   // BITM Built-In-Test Results screen
    uint64_t wfInfo   :1;   // BITN Selected Waveform Info
    uint64_t keyName  :1;   // BITO Active GDB Key Names
    uint64_t admin    :1;   // BITP ADMINISTRATOR screen Y/N options
    uint64_t admWrn   :1;   // BITQ Admin action warning screen
    uint64_t exfOpt   :1;   // BITR Admin EXFIL options screen
    uint64_t gmOvrd   :1;   // BITS Admin GEOMUTE OVERRIDE options screen
    uint64_t infOpt   :1;   // BITT Admin INFIL options screen
    uint64_t rngBrg   :1;   // BITU RANGE & BEARING screen
    uint64_t dirY_N   :1;   // BITV Delete INFIL and R&B Y/N option (w/ timeout)
    uint64_t lpmY_N   :1;   // BITW Use Low Power Mode Y/N option
  //uint64_t  :1; // BITa follows BITZ
  };              // Any movement of bits requires check/update of defines below
  uint64_t val;
} dsplfld_t;
//                    BIT VUTSRQPONMLKJIHGFEDCBA9876543210
#define HOME_DSPL_FLDS  0b00000000000000000000000011111111L
#define BREV_DSPL_FLDS  0b00000000000000000000011100000000L
#define GDB_DSPL_FLDS   0b00000000000000000001100000100000L

typedef union tagSYSTEM_STATUS_FLAGS
{
  struct                      // Statuses to be reset when power off
  {
    uint16_t cfgInit    :1;   // BIT0 ESD completed config of nvmem
    uint16_t gasInit    :1;   // BIT1 ESD completed gas gauage initialiation
    uint16_t rstDone    :1;   // BIT2 ESD considers rst done (got D.RST)
    uint16_t bitDone    :1;   // BIT3 ESD considers BIT done (got D.BIT)

    uint16_t ltngRdyCmd :1;   // BIT4 Lightning appears ready for commands
    uint16_t ltngAckReq :1;   // BIT5 Lightinig must ACK/NAK command sent to it

    uint16_t ltngRptRst :1;   // BIT6 Lightning has sent D.RST message
    uint16_t ltngRptWfi :1;   // BIT7 Lightning has sent D.WFI series
    uint16_t ltngRptGak :1;   // BIT8 Lightning has sent D.GAK message
    uint16_t ltngRptVer :1;   // BIT9 Lightning has sent D.LCM message
    uint16_t ltngRptBit :1;   // BITA Lightning has sent D.BIT message
    uint16_t ltngRptZed :1;   // BITB Lightning has sent D.OPS ZEROED message

    uint16_t aofValid   :1;   // BITC Lightning has sent D.GLL message
    uint16_t gpsTime    :1;   // BITD Lightning has sent D.TGF message
    uint16_t lowPwrMode :1;   // BITE Infil off, Exfil mute, GPS minimal
    uint16_t in911Mode  :1;   // BITF LBHH in 911 mode
  };
  uint16_t val;
} sysstat_t;

typedef enum tagDISPLAYED_OPERATIONAL_STATUS
{                 // Based on D.OPS and D.NBE messages from Lightning
  DS_NO_STAT,     // unknown or nothing to display
  DS_POR,         // Power-on reset
  DS_GPS,         // acquiring GPS
  DS_LPGPS,       // low-power GPS, Bolt processor turned off
  DS_NO_GPS,      // GPS fix fail
  DS_XMT,         // transmitting
  DS_GDB,         // infil msg
  DS_CTDN,        // in countdown until NBE (only next XMT supported)
  DS_A911,        // automated 911 ACK rcvd
  DS_M911,        // manual (command cell) 911 ACK rcvd
  DS_SLEEP,       // Bolt is in between modes
  DS_ZEROED,      // Bolt has been zeroized (won't work)
} dsplstat_t;

typedef enum tagLTNG_CMD_ID   // Ensure values work as uint8_t for queueing
{                             // IDs of Ltng commands that firmware uses!
  CID_NULL  =   0,            // Just a way to indicate not applicable
  CID_OCRAI,                  // Connection Request & Authentication initiation
  CID_OCRAA,                  // Answer to CRA question from Lightning
  CID_OCBC,                   // Change Brevity Code
  CID_OTXM,                   // Transmit Mode (set 911)
  CID_OLPM,                   // Low Power Mode
  CID_CSWF,                   // Configure Selected Waveform
  CID_CTXP,                   // Configure Selected Transmit Power
  CID_CBDC,                   // Configure Selected Beacon Duty Cycle
  CID_CGOK,                   // Configure GDB Optional/Group Key
  CID_CR9A,                   // Configure Response to 911 Automated ACK
  CID_CR9M,                   // Configure Response to 911 Man-in-loop ACK
  CID_CZUM,                   // Zeroize Unit Memories
  CID_CECP,                   // Configure Ltng's External COM Port connection
  CID_SWFI,                   // Subscribe/query all Waveform Info
  CID_SGKN,                   // Subscribe/query GDB loaded Key Names
  CID_SGAK,                   // Subscribe/query GDB Active Key names
//  CID_,               //
} cmdid_t;

typedef struct tagGDB_MESSAGE_INFO
{
  bool unread;
  char tStamp[TIMESTAMP_SIZE];
  char msg[GDB_MSG_SIZE_MAX];
} gdbmsg_t;


typedef struct tagPLI_DATA
{ // structure to hold data from D.
  uint16_t age;                 // seconds since PLI info received by ESD
  bool     xof;                 // device TXed old fix data
  double   lat;                 // 8-hex scaled latitude  converted to double
  double   lon;                 // 8-hex scaled longitude converted to double
  uint64_t uId;                 // ID as 64-bit unsigned int for easy compares
  char     cId[6+1];            // 6-char ID as native array for display
  char     brev[3+1];           // 3-digit exfil brevity code associate with PLI
  char     time[TIMESTAMP_SIZE];// time exfil received by ground station
} plidat_t;


//----- GLOBAL VARIABLE DECLARATIONS -------------------------------------------
volatile  bool      doNotSleep  = true;     // Set in ISR and modules as needed
volatile  esd_t     esdErrFlags = {.val=0}; // Error flags, e.g. firmware error

//----- MODULE ATTRIBUTES ------------------------------------------------------
static          char        fwVerStr[] = FW_VER_STR;// update w/ltng version
static          esd_t       prevErrFlags = {.val=0};// use to detect changes
static          ltng_t      ltngBitRpt = {.val=0};  // use for reported BIT
static          wet_t       wfTrait[N_WFI];     // Waveform drives user options
#define selWfTrait wfTrait[devCfg.selWfIdx]
static          char        uKeyName[7] = SP6;  // Unique GDB Key Name
static          char        gKeyName[7] = SP6;  // Active Group GDB Key Name
static          char        infKey1[7]  = SP6;  // Infil Group GDB Key 1 Name
static          char        infKey2[7]  = SP6;  // Infil Group GDB Key 2 Name
static          char        infKey3[7]  = SP6;  // Infil Group GDB Key 3 Name
static          char        infKey4[7]  = SP6;  // Infil Group GDB Key 4 Name
static          char        infKey5[7]  = SP6;  // Infil Group GDB Key 5 Name
static volatile uint16_t    ct1SecTick = 0;     // count of 1-sec IRQ time ticks
static          uint16_t    muteSquawkCtdn = 0; // Times to squawk in MUTE
static          esdfp_t     focusPoint = FP_SYSCHK; // User Interface focus
static          dsplfld_t   updtFld = {.val=0}; // display fields to be updated
static          uint16_t    scrnCtdn = CTDN_OFF;// Screen timeouts
static          sysstat_t   sysStat = {.val=0}; // gasInit, rcvdBit, etc.
static          char        exfilCode[3] = ZR2; // Ltng exfil err code
static          char        infilCode[3] = ZR2; // Ltng infil err code
static          char        ltngHwCode[3] = ZR2;// Ltng hardware err code
static          char        ltngFwCode[3] = ZR2;// Ltng firmware err code
static          bool        newBatSelMade=false;// user ENT at New Battery scrn
static          bool        wasOnBtry = false;  // on battery pwr at last check?
static          uint16_t    gasChkCtdn=CTDN_OFF;// Countdown to check gas gauge
static          uint16byte_t acr={.val=LTC2943_ZERO_PT}; // gas gauge Charge
static          uint16_t    btryPct = 0;        // remaining battery percent
static          dsplstat_t  dsplStatus = DS_POR;// enum of POR, ZEROED, etc.
static          coords_t    myLoc;
static volatile uint16_t    sysSec = 0;         // System/GPS Time's seconds
static          uint16_t    sysMin = 0;         // System/GPS Time's minutes
static          uint16_t    sysHr  = 0;         // System/GPS Time's hours
static          char        brevCodeH[2] = {'0',0}; // Hundreds digit of code
static          char        brevCodeT[2] = {'0',0}; // Tens digit of code
static          char        brevCodeO[2] = {'1',0}; // Ones digit of code
static          uint16_t    aof = 0;            // Age Of gps Fix
static          int16_t     nbeSec = 0;         // seconds to next BOLT event
static          int16_t     nbeMin = 0;         // minutes to next BOLT event
static          int16_t     nbeHr  = 0;         // hours to next BOLT event

static          gdbmsg_t    gdbMsgQ[GDB_MSG_Q_LEN]; // GDB msg queue buffer
static          uint16_t    hdGdbMsgQ      = 0; // head of GDB msg queue
static          uint16_t    tlGdbMsgQ      = 0; // tail of GDB msg queue
static          int16_t     nmuGdbIdx      = -1;// newest msg unread, <0 if read
static          uint16_t    numGdbMsgs     = 0; // number of GDB message
static          uint16_t    numUnrdGdbMsgs = 0; // number of unread GDB messages
static          uint16_t    dsplGdbMsg     = 0; // the index of msg to display

static          plidat_t    pliDatQ[PLI_DAT_Q_LEN]; // Head is always [0]
static          int16_t     nmuPliIdx  = -1;    // newest msg unread, <0 if read
static          int16_t     hiPliIdx   = -1;    // highest index containing data
static          uint16_t    dsplPliIdx =  0;    // index of PLI/R&B to display

#define LTNG_RPT_BFR_LEN    350         // largest single burst should be ~170
static  uint8_t    ltngRptBfr[LTNG_RPT_BFR_LEN];
static  uint8queue ltngRptQueue;        // Queue for msgs from Lightning
#define ltngRptQ ((uint8queue*)(&ltngRptQueue))

#define CID_Q_LEN          12           // Max # of commands that can be queued
static uint8_t    cidBfr[CID_Q_LEN];    // IDs of command still need to send
static uint8queue cidQueue;             // NOT the Lightning UART TX queue!!!
#define cidQ ((uint8queue*)(&cidQueue))
static          uint8queue ltngCmdQueue;        // Queue for msgs to Lightning
#define ltngCmdQ ((uint8queue*)(&ltngCmdQueue))

#define RTI_FSC     0xA1      // frames start w/ extended ASCII <FLIP-EXCLAME>
#define RTI_FPC     0xB6      // frames stop  w/ extended ASCII <PILCROW>
#define CRAI_LEN     7        // start Connection Request
#define CRAA_LEN    16        // Connection Authenticate
#define CBC_LEN     11        // Change Brevity Code
#define TXM_LEN      9        // TX Mode (911)
#define LPM_LEN      9        // Low Power Mode
#define BMR_LEN      9        // Beacon Mute Reports
#define ECP_LEN      9        // External COM Port
#define SWF_LEN      9        // Select Waveform
#define TXP_LEN      9        // TX Power
#define BDC_LEN      9        // TX/Beacon Duty Cycle
#define GOK_LEN      9        // GDB Optional Key
#define R9A_LEN      9        // Response to Auto 911 ACK
#define R9M_LEN      9        // Response to Man 911 ACK
#define ZUM_LEN      7        // Zerioze Unit Memories
#define WFI_LEN      7        // All Waveform Information
#define GKN_LEN      7        // GDB loaded Key Names
#define GAK_LEN      7        // GDB Active Key names
//                                  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
static char oCrai[CRAI_LEN] = {RTI_FSC,'O','.','C','R','A',RTI_FPC};
static char oCraa[CRAA_LEN] = {RTI_FSC,'O','.','C','R','A',' ','x','x','x','x','x','x','x','x',RTI_FPC};
static char oCbc[CBC_LEN]   = {RTI_FSC,'O','.','C','B','C',' ','d','d','d',RTI_FPC};
static char oTxm[TXM_LEN]   = {RTI_FSC,'O','.','T','X','M',' ','1',RTI_FPC};
static char oLpm[LPM_LEN]   = {RTI_FSC,'O','.','L','P','M',' ','d',RTI_FPC};
static char cSwf[SWF_LEN]   = {RTI_FSC,'C','.','S','W','F',' ','d',RTI_FPC};
static char cTxp[TXP_LEN]   = {RTI_FSC,'C','.','T','X','P',' ','d',RTI_FPC};
static char cBdc[BDC_LEN]   = {RTI_FSC,'C','.','B','D','C',' ','d',RTI_FPC};
static char cGok[GOK_LEN]   = {RTI_FSC,'C','.','G','O','K',' ','d',RTI_FPC};
static char cR9a[R9A_LEN]   = {RTI_FSC,'C','.','R','9','A',' ','d',RTI_FPC};
static char cR9m[R9M_LEN]   = {RTI_FSC,'C','.','R','9','M',' ','d',RTI_FPC};
static char cZum[ZUM_LEN]   = {RTI_FSC,'C','.','Z','U','M',RTI_FPC};
static char cEcp[ECP_LEN]   = {RTI_FSC,'C','.','E','C','P',' ','1',RTI_FPC};
static char sWfi[WFI_LEN]   = {RTI_FSC,'S','.','W','F','I',RTI_FPC};
static char sGkn[GKN_LEN]   = {RTI_FSC,'S','.','G','K','N',RTI_FPC};
static char sGak[GAK_LEN]   = {RTI_FSC,'S','.','G','A','K',RTI_FPC};

//----- MACROS -----------------------------------------------------------------


//----- PROCEDURES -------------------------------------------------------------
/*
//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  .
//
//  INPUT : TODO
//  OUTPUT: TODO
//  CALLS : TODO
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static
{

} // end
*/


//-- Callback function ---------------------------------------------------------
//  void Tmr2_1SecEventsCb(void)
//  Built-In-Test IRQ callback routine to clear error flag if get 1-sec tick.
//
//  INPUT : NONE
//  OUTPUT: NONE but increments volatile ct1SecTick (count 1-sec ticks)
//  CALLS : NONE
//------------------------------------------------------------------------------
void Tmr2_1SecEventsCb(void)
{
  ct1SecTick++;                         // used in ProcessTimeEvents()
  if ( ! ct1SecTick)
  {                                     // When count rolls over something wrong
    ct1SecTick--;                       // set count back to its max and
    esdErrFlags.fw = 1;                 // note that this should NOT happen
  }
  doNotSleep = true;                    // stay awake to update displayed time
} // end Tmr2_1SecEventsCb


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  uint8_t htou8(char * hex)
//  Converts two hextext characters to uint8_t value they represent.
//  NOTE - this does not verify either supplied character ishexdigit.
//
//  INPUT : char * hex - pointer to first of two hextext characters to convert
//  OUTPUT: uint8_t - converted value
//  CALLS : isdigit
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static uint8_t htou8(char * hex)
{
  uint8bits_t   msn;                    // most significant nibble
  uint8bits_t   lsn;                    // least significant nibble

  msn.val = *hex;     hex++;            // Copy two message hextext digits
  lsn.val = *hex;                       // for local nibble value processing.
  if (isdigit(msn.val))           {     // If msn character is in lower # range
    msn.val -= '0';               }     // of hexdigits keep its value, and
  else /* isxdigit */             {     // if character is a hex-only character
    msn.b5  = 0;                        // make sure that it is upper case A-F
    msn.val = msn.val +10 -'A';   }     // digit then keep its value
  msn.val <<= 4;                        // finally move value into the msn.
  if (isdigit(lsn.val))           {     // If lsn character is in lower # range
    lsn.val -= '0';               }     // of hexdigits keep its value, and
  else /* isxdigit */             {     // if character is a hex-only character
    lsn.b5  = 0;                        // make sure that it is upper case A-F
    lsn.val = lsn.val +10 -'A';   }     // digit then keep its value.

  return (uint8_t)msn.val + lsn.val;    // Place value in key destination array
} // end function htou8


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void SetWaveformTraits(wi_t * wfInfo, wet_t * wfTrait)
//  Determines & sets Waveform ESD-specific Traits based on waveform class
//
//  INPUT : wi_t * wfInfo   - info from a D.WFI message
//          wet_t * wfTrait - location where traits should be set
//  OUTPUT: NONE but may modify wfInfo and will set wfTrait
//  CALLS : memcpy
//          memset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void SetWaveformTraits(wi_t * wfInfo, wet_t * wfTrait)
{
  wc_t  family;         // waveform family to switch upon to set family traits
  bool  updateCfg;      // answers: ? update devCfg.geoMuting ?

  family    = (wc_t)wfInfo->family;
  updateCfg = ((ws_t)wfInfo->slot == devCfg.selWfChar); // Active waveform info?

  if ((WC_NA >= family) || (WC_GTMAX <= family))
  {
    wfn_t name = { '#','_','N','o','W','F' };
    *name = wfInfo->slot;
    memcpy((void*)wfInfo->name, (const void*)name, WF_NAME_LEN);
    memset((void*)wfInfo->txId, NVLD_TXID, WF_TXID_LEN);
  }
  memcpy((void*)wfTrait->name, (const void*)wfInfo->name, WF_NAME_LEN);
  memcpy((void*)wfTrait->txId, (const void*)wfInfo->txId, WF_TXID_LEN);
  wfTrait->name[WF_NAME_LEN] = '\0';    // ESD uses longer, NULL terminated
  wfTrait->txId[WF_TXID_LEN] = '\0';    // fields for LCDWriteString functions

  switch (family)
  {
    case WC_NA  :
      wfTrait->wbrOpt = WBR_NC;         // Brevity Range
      wfTrait->wdcOpt = WDC_4;          // Duty Cycle
      wfTrait->wtpOpt = WTP_N;          // Transmit Power
      wfTrait->wgmOpt = WGM_NA;         // Geo-Muting
      break;
    case WC_TEST:
      wfTrait->wbrOpt = WBR_NC;
      wfTrait->wdcOpt = WDC_4_8_32;
      wfTrait->wtpOpt = WTP_A;
      wfTrait->wgmOpt = WGM_NEVER;
      break;
    case WC_AME6_1:
    case WC_AME6_2:
    case WC_AME6_3:
      wfTrait->wbrOpt = WBR_1T253;
      wfTrait->wdcOpt = WDC_4_8_32;
      wfTrait->wtpOpt = WTP_A;
      wfTrait->wgmOpt = WGM_NEVER;
      break;
    case WC_AME8_1:
    case WC_AME8_2:
    case WC_AME8_3:
      wfTrait->wbrOpt = WBR_1T48_241T253;
      wfTrait->wdcOpt = WDC_4_8_32;
      wfTrait->wtpOpt = WTP_A;
      wfTrait->wgmOpt = WGM_NEVER;
      break;
    case WC_UWRL_1:
    case WC_UWRL_2:
    case WC_UWRL_3:
      wfTrait->wbrOpt = WBR_0T14;
      wfTrait->wdcOpt = WDC_4_8_40;
      wfTrait->wtpOpt = WTP_N;
      wfTrait->wgmOpt = WGM_NEVER;
      break;
    case WC_SHLN_1:
    case WC_SHLN_2:
    case WC_SHLN_3:
      wfTrait->wbrOpt = WBR_1T253;
      wfTrait->wdcOpt = WDC_4;
      wfTrait->wtpOpt = WTP_N;
      wfTrait->wgmOpt = WGM_NEVER;
      break;
    case WC_PWRL_1:
    case WC_PWRL_2:
    case WC_PWRL_3:
      wfTrait->wbrOpt = WBR_0T14;
      wfTrait->wdcOpt = WDC_4_8_40;
      wfTrait->wtpOpt = WTP_A;
      wfTrait->wgmOpt = WGM_NEVER;
      break;
    case WC_AME6_1G:
    case WC_AME6_2G:
    case WC_AME6_3G:
      wfTrait->wbrOpt = WBR_1T253;
      wfTrait->wdcOpt = WDC_4_8_32;
      wfTrait->wtpOpt = WTP_A;            // AME6_xG is a geo-muted waveform
      if (WGM_OVRDN > wfTrait->wgmOpt)  { // When not previously in a geo-muting
        wfTrait->wgmOpt = WGM_ACTVD;    } // mode -- make geo-muting ACTIVE now.
      if (WGM_NEVER < devCfg.geoMuting) { // When already configured to use
        updateCfg = false;              } // geo-muting -- do not change devCfg
      break;                              // as wgmOpt could be OVRND or OUTGZ
    case WC_AME8_1G:
    case WC_AME8_2G:
    case WC_AME8_3G:
      wfTrait->wbrOpt = WBR_1T48_241T253;
      wfTrait->wdcOpt = WDC_4_8_32;
      wfTrait->wtpOpt = WTP_A;            // AME8_xG is a geo-muted waveform
      if (WGM_OVRDN > wfTrait->wgmOpt)  { // When not previously in a geo-muting
        wfTrait->wgmOpt = WGM_ACTVD;    } // mode -- make geo-muting ACTIVE now.
      if (WGM_NEVER < devCfg.geoMuting) { // When already configured to use
        updateCfg = false;              } // geo-muting -- do not change devCfg
      break;                              // as wgmOpt could be OVRND or OUTGZ
    case WC_UWRL_1G:
    case WC_UWRL_2G:
    case WC_UWRL_3G:
      wfTrait->wbrOpt = WBR_0T14;
      wfTrait->wdcOpt = WDC_4_8_40;
      wfTrait->wtpOpt = WTP_N;            // UWRL_xG is a geo-muted waveform
      if (WGM_OVRDN > wfTrait->wgmOpt)  { // When not previously in a geo-muting
        wfTrait->wgmOpt = WGM_ACTVD;    } // mode -- make geo-muting ACTIVE now.
      if (WGM_NEVER < devCfg.geoMuting) { // When already configured to use
        updateCfg = false;              } // geo-muting -- do not change devCfg
      break;                              // as wgmOpt could be OVRND or OUTGZ
    case WC_SHLN_1G:
    case WC_SHLN_2G:
    case WC_SHLN_3G:
      wfTrait->wbrOpt = WBR_1T253;
      wfTrait->wdcOpt = WDC_4;
      wfTrait->wtpOpt = WTP_N;            // SHLN_xG is a geo-muted waveform
      if (WGM_OVRDN > wfTrait->wgmOpt)  { // When not previously in a geo-muting
        wfTrait->wgmOpt = WGM_ACTVD;    } // mode -- make geo-muting ACTIVE now.
      if (WGM_NEVER < devCfg.geoMuting) { // When already configured to use
        updateCfg = false;              } // geo-muting -- do not change devCfg
      break;                              // as wgmOpt could be OVRND or OUTGZ
      break;
    case WC_PWRL_1G:
    case WC_PWRL_2G:
    case WC_PWRL_3G:
      wfTrait->wbrOpt = WBR_0T14;
      wfTrait->wdcOpt = WDC_4_8_40;
      wfTrait->wtpOpt = WTP_A;            // PWRL_xG is a geo-muted waveform
      if (WGM_OVRDN > wfTrait->wgmOpt)  { // When not previously in a geo-muting
        wfTrait->wgmOpt = WGM_ACTVD;    } // mode -- make geo-muting ACTIVE now.
      if (WGM_NEVER < devCfg.geoMuting) { // When already configured to use
        updateCfg = false;              } // geo-muting -- do not change devCfg
      break;                              // as wgmOpt could be OVRND or OUTGZ
    default:
      esdErrFlags.fw = 1;
      wfTrait->wbrOpt = WBR_NC;
      wfTrait->wdcOpt = WDC_4;
      wfTrait->wtpOpt = WTP_N;
      wfTrait->wgmOpt = WGM_ACTVD;
      break;
  }
  if (updateCfg && (wfTrait->wgmOpt != devCfg.geoMuting))
  { // Update only active waveform's geoMute state only as required/appropriate
    // If wgmOpt == OVRDN||OUTGZ, devCfg is ACTVD (>NEVER), so updateCfg = false
    devCfg.geoMuting = wfTrait->wgmOpt; // Save NEVER or ACTVD (possibly NA) but
    WriteCfgToNvMem();                  // do not save OVRDN or OUTGZ to nvmem !
  }
} // end routine SetWaveformTraits


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void PostLtngCmd(void)
//  Attempts to send a previously queued message to Lightning interface.
//
//  INPUT : NONE
//  OUTPUT: NONE (but may result in emission of cmd to Lightning via UART)
//  CALLS : QUEUE_NOT_EMPTY
//          LTG_WRITE_COMPLETE
//          QUEUE_GET
//          QUEUE_INIT_PREPACKED
//          LTG_WRITE_NONBLOCKING
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void PostLtngCmd(void)
{
  uint8_t  cmd;                     // ID of command extracted from queue
  char    *pCmd;                    // pointer to command text
  int8_t   cmdLen;                  // length of command to be sent

  if ( (QUEUE_NOT_EMPTY(cidQ)) &&   // When a command is queued and
       (sysStat.ltngRdyCmd)    &&   // Lightning should be ready for cmds and
       ( ! sysStat.ltngAckReq) )    // not waiting for ACK of prior cmd
  {                                 // we can proceed to get/send queued cmd
    QUEUE_GET(cidQ, cmd);
    switch ((cmdid_t)cmd)
    {
      case CID_OCRAI:   pCmd = oCrai;   cmdLen = CRAI_LEN;    break;
      case CID_OCRAA:   pCmd = oCraa;   cmdLen = CRAA_LEN;    break;
      case CID_OCBC:    pCmd = oCbc;    cmdLen = CBC_LEN;     break;
      case CID_OTXM:    pCmd = oTxm;    cmdLen = TXM_LEN;     break;
      case CID_OLPM:    pCmd = oLpm;    cmdLen = LPM_LEN;     break;
      case CID_CSWF:    pCmd = cSwf;    cmdLen = SWF_LEN;     break;
      case CID_CTXP:    pCmd = cTxp;    cmdLen = TXP_LEN;     break;
      case CID_CBDC:    pCmd = cBdc;    cmdLen = BDC_LEN;     break;
      case CID_CGOK:    pCmd = cGok;    cmdLen = GOK_LEN;     break;
      case CID_CR9A:    pCmd = cR9a;    cmdLen = R9A_LEN;     break;
      case CID_CR9M:    pCmd = cR9m;    cmdLen = R9M_LEN;     break;
      case CID_CZUM:    pCmd = cZum;    cmdLen = ZUM_LEN;     break;
      case CID_CECP:    pCmd = cEcp;    cmdLen = ECP_LEN;     break;
      case CID_SGKN:    pCmd = sGkn;    cmdLen = GKN_LEN;     break;
      case CID_SWFI:    pCmd = sWfi;    cmdLen = WFI_LEN;     break;
      case CID_SGAK:    pCmd = sGak;    cmdLen = GAK_LEN;     break;
      default:          cmdLen = 0;     break;
    }

    if (cmdLen)                         // Valid commands have non-zero length
    {                                   // so when have command queue & write it
      QUEUE_INIT_PREPACKED(ltngCmdQ, (uint8_t*)pCmd, cmdLen);
      LTG_WRITE_NONBLOCKING(ltngCmdQ);
      sysStat.ltngAckReq = 1;
    }
  }
} // end routine PostLtngCmd


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool QueueLtngCmd(cmdid_t cmd, char* param)
//  Queues up messages needing to be sent to Lightning interface. Any waiting
//  command in the queue of the same type will be sent with latest param value.
//
//  INPUT : cmdid_t cmd - ID of command to be queued for output to Lightning
//          char* param - parameter char(s) to used when sending the command
//  OUTPUT: bool - true if command added to queue, otherwise false
//  CALLS : QUEUE_FULL
//          QUEUE_INIT_EMPTY
//          QUEUE_PUT
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool QueueLtngCmd(cmdid_t cmd, char * param)
{
  bool   isOk = true;

  if (QUEUE_FULL(cidQ))
  {
    isOk = false;                       // something is wrong as the
    esdErrFlags.sm = 1;                 // state machine not getting thru queue
    if (sysStat.ltngAckReq)
    {                                   // When appears we missed ACK/NAK from
      esdErrFlags.ltng   = 1;           // Ltng flag that interface as well
      sysStat.ltngAckReq = 0;           // and clear requirement for ACK
    }
  }
  // Even if command queue is full, update command parameter!!
  // We may be updating a command already in queue and always want newest param
  switch (cmd)
  {
    case CID_OCRAI:
      break;
    case CID_OCRAA:
      memcpy((void*)&oCraa[7], (const void*)param, 8);
      break;
    case CID_OCBC:
      memcpy((void*)&oCbc[7], (const void*)param, 3);
      break;
    case CID_OTXM:
      break;
    case CID_OLPM:
      oLpm[7] = *param;
      break;
    case CID_CSWF:
      cSwf[7] = *param;
      break;
    case CID_CTXP:
      cTxp[7] = *param;
      break;
    case CID_CBDC:
      cBdc[7] = *param;
      break;
    case CID_CGOK:
      cGok[7] = *param;
      break;
    case CID_CR9A:
      cR9a[7] = *param;
      break;
    case CID_CR9M:
      cR9m[7] = *param;
      break;
    case CID_CZUM:                                // When request is to zeroize
      QUEUE_INIT_EMPTY(cidQ, cidBfr, CID_Q_LEN);  // no other commands matter!!
      sysStat.ltngAckReq = 0;                     // Don't care about old ACKs.
      sysStat.ltngRdyCmd = sysStat.ltngRptRst;    // Don't wait for BIT report.
      break;
    case CID_CECP:
      cEcp[7] = *param;
      break;
    case CID_SWFI:
    case CID_SGKN:
    case CID_SGAK:
      break;
    default:
      isOk = false;
  }

  if (isOk)
  {                                     // When request was for valid command
    QUEUE_PUT(cidQ, (uint8_t)cmd);      // put updated command in the queue
    doNotSleep = true;                  // and keep main loop awake to see it.
  }

  return isOk;
} // end routine QueueLtngCmd


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void StartLbhhZeroize(void) - Start zeroize process due to infil command
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void StartLbhhZeroize()
{
  devCfg.opStat = OS_ZEROED;            // set status as ZEROED so that it's
  WriteCfgToNvMem();                    // in non-volatile memory for future;
  QueueLtngCmd(CID_CZUM, NULL);         // queue C.ZUM command to Lightning;
  PAUSE_KEYPAD_OPS();                   // no longer accept keypad inputs;
  focusPoint = FP_ZERO_A;               // no escape from this focus point
  updtFld.chgScr = 1;                   // and let user know it's happened.
} // end routine StartLbhhZeroize


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void AddGdbPliData(char * rpt) - Add PLI data to Q for Range & Bearing scrn
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void AddGdbPliData(char * rpt)
{
  plidat_t newPli;
  int16_t  i, j;                        // loop controls
  char * pS;                            // pointer into source
  char * pD;                            // pointer into destination

  newPli.age = 0;                       // This baby is 0 seconds old

  newPli.xof = ('0' == rpt[10]);        // '0' indicates device TXed old fix

  pS = &rpt[19];  pD = (char*)&newPli.uId;
  newPli.uId = 0;                       // Start with integer ID of zero then
  for (i = 6, newPli.uId = 0; i; i--) { // turn 6-alphanum ASCII 'unique ID'
    *pD++ = *pS--;                    } // into 64-bit value for <=> compares
  pS = &rpt[14];  pD = (char*)newPli.cId;
  for (i = 6; i; i--)                 { // Copy 6-char unique unit ID
    *pD++ = *pS++;                    } // in standard human readable format
  *pD = '\0';                           // then null terminate it for display

  pS = &rpt[21];  pD = newPli.brev;
  for (i = 3; i; i--)                 { // Copy 3-digit ASCII decimal brevity
    *pD++ = *pS++;                    } // in standard human readable format
  *pD = '\0';                           // then null terminate it for display

  pS = &rpt[25];  pD = newPli.time;
  for (i = TIMESTAMP_SIZE-1; i; i--)  { // Copy 8-char ASCII HH:mm:ss RX time
    *pD++ = *pS++;                    } // in standard human readable format
  *pD = '\0';                           // then null terminate it for display

  newPli.lat = HexDegToDblDeg(&rpt[34],1);// Covert 8-hexdigit scaled latitude &
  newPli.lon = HexDegToDblDeg(&rpt[43],0);// longitude for later computations

  if ((PLI_DAT_Q_LEN-1) <= hiPliIdx)
  {                                     // When the Q is already [over]full
    uint16_t maxAge = 0;                // need to find the oldest PLI data
    uint16_t nixIdx = (PLI_DAT_Q_LEN-1);// and remove it from pliDatQ
    for (i= PLI_DAT_Q_LEN-1; 0<=i; i--) // Must examine every PLI entry
    {                                   // because Q is sorted by id not age and
      if (pliDatQ[i].uId == newPli.uId) // because may need update not replace.
      {                                 // When already have PLI data for the ID
        nixIdx = PLI_DAT_Q_LEN;         // update it, don't nix any entry below!
        break;                          // It is time to break this 'for' loop
      }
      else if (maxAge < pliDatQ[i].age) // When haven't found to be updating...
      {                                 // track what and where oldest entry is
        nixIdx = i;                     // at whichever index proves to be the
        maxAge = pliDatQ[i].age;        // oldest requiring least data moves
      }
    }
    if (PLI_DAT_Q_LEN == nixIdx)        // When updating existing ID's PLI
    {                                   // there is no need to free space so
      hiPliIdx = (PLI_DAT_Q_LEN-1);     // just ensure validity as overkill.
    }
    else                                // When need space for new ID's PLI
    {                                   // remove oldest by copying data down
      for (i = nixIdx; (PLI_DAT_Q_LEN-1) > i; i++)  { // Doesn't loop if highest
        pliDatQ[i] = pliDatQ[i+1];                  } // index holds the oldest.
      if (nixIdx < dsplPliIdx)      {   // When copy-down included display' data
        dsplPliIdx--;               }   // decrement display indx so screen same
      hiPliIdx = (PLI_DAT_Q_LEN-2);     // The highest index now available.
    }
  }
  else if (-1 > hiPliIdx)
  {                                     // If corrupted, below value indicating
    hiPliIdx = -1;                      // nothing in Q, assign empty indicator
  }                                     // before use below setting inIdx
  //else-if (-1 <= hiPliIdx <= PLI_DAT_Q_LEN-2)
                                        // !! ALWAYS check match/update FIRST !!
  uint16_t inIdx = 0;                   // Initialize as suitable for empty Q
  for (i = hiPliIdx; 0 <= i; i--)       // Only when something in Q do we loop
  {                                     // to find where in Q to put newPli data
    if (pliDatQ[i].uId == newPli.uId)   // The Q is sorted by ID (high->low)
    {                                   // When IDs match we simply update the
      inIdx = i;                        // old data !! Check for update first !!
      break;                            // Exit for 0 <= i loop
    }                                   // Indexing in for j loop is safe since
    else if(pliDatQ[i].uId > newPli.uId)// hiPliIdx always < LEN-1 in this else
    {                                   // When new ID falls between existing
      inIdx = i+1;                      // entries put it above next higher ID
      for (j= hiPliIdx; inIdx <=j; j--){// by moving all lower ID entries up
        pliDatQ[j+1] = pliDatQ[j];     }// (also works if new ID less than all)
      if (inIdx < dsplPliIdx)       {   // When copy-up included display's data
        dsplPliIdx++;               }   // increment display indx so screen same
      hiPliIdx++;                       // and update highest valid Q data index
      break;                            // Exit for 0 <= i loop
    }                                   // Indexing in for j loop is safe since
    else if (0 == i)                    // hiPliIdx always < LEN-1 in this else
    {                                   // When new ID greater than all existing
      inIdx = 0;                        // entries put it under all those
      for (j = hiPliIdx; 0 <= j; j--)  {// by moving all lower ID entries up
        pliDatQ[j+1] = pliDatQ[j];     }// (to free index 0 for newPli) and
      if (inIdx < dsplPliIdx)       {   // when copy-up included display's data
        dsplPliIdx++;               }   // increment display indx so screen same
      hiPliIdx++;                       // update highest valid Q data index
    }                                   // Will naturally exit for 0 <= i loop
  }
  if (0 > hiPliIdx)                   { // In this case did not exec 'for' loop
    hiPliIdx = 0;                     } // so must update hiPliIdx now

  pliDatQ[inIdx] = newPli;              // FINALLY - put new data in its place,
  nmuPliIdx = inIdx;                    // note what's new & unread for priority
  updtFld.rngBrg  = 1;                  // Update Range & Bearing if being shown
  updtFld.mail    = 1;                  // should update the mail icon too
} // end routine AddGdbPliData


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void AddGdbGenMsg(char * rpt) - Add generic/C2 message to Q for INFIL screen
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void AddGdbGenMsg(char * rpt)
{
  char *pMsg;                     // add D.GID message data to INFIL scrn
  int  i,j;

  sprintf(gdbMsgQ[hdGdbMsgQ].tStamp, "%02u:%02u:%02u", sysHr, sysMin, sysSec);
  if ( ! sysStat.gpsTime)
  { // When no valid GPS time replace msg time-stamp's ':' with '-'
    gdbMsgQ[hdGdbMsgQ].tStamp[2] = gdbMsgQ[hdGdbMsgQ].tStamp[5] = '-';
  }

  if ((GDB_MSG_Q_LEN > numUnrdGdbMsgs) && ( ! gdbMsgQ[hdGdbMsgQ].unread))
  {                   // Only increment unread count if not already maxed and
    ++numUnrdGdbMsgs; // message being overwritten has been read
    gdbMsgQ[hdGdbMsgQ].unread = true; // Set new msg unread status to true
  }
  // Fill msg w/<SPACE> chars to clear all old data on display; NULL terminate
  memset((void*)(gdbMsgQ[hdGdbMsgQ].msg), ' ', GDB_MSG_SIZE_MAX-1);
  gdbMsgQ[hdGdbMsgQ].msg[GDB_MSG_SIZE_MAX-1] = '\0';
  for (i = 0, j = 0, pMsg = &rpt[7];  RTI_FPC != *pMsg;  ++i, ++j, ++pMsg)
  { // Copy actual message text from report, stopping on Framing Stop Char
    if (j && (0 == j % LCD_MAX_COLS))
    { // Add in NULL terminator after every 16th char for ease of display
      gdbMsgQ[hdGdbMsgQ].msg[i] = '\0';
      ++i;
    }
    gdbMsgQ[hdGdbMsgQ].msg[i] = *pMsg;  // move char to GDB msg structure
  }
  // Update count of messages in queue and tail position with wrap around
  if (GDB_MSG_Q_LEN > numGdbMsgs)
  {
    ++numGdbMsgs; // Cap count at msg queue size
  }
  else  // Tail increments only once msg queue's array is maxed out
  {
    tlGdbMsgQ++;
    if (GDB_MSG_Q_LEN <= tlGdbMsgQ)         {
      tlGdbMsgQ = 0;                        }
  }
  // Always set nmuGdbIdx then update head position, w/ wrap around if necessary
  nmuGdbIdx = hdGdbMsgQ;
  hdGdbMsgQ++;
  if (GDB_MSG_Q_LEN <= hdGdbMsgQ)           {
    hdGdbMsgQ = 0;                          }
  updtFld.gdb   = 1;                // Reverse video the Infil screen timestamp,
  updtFld.gdbXY = 1;                // the Y component just changed, and
  updtFld.mail  = 1;                // should update the mail icon too
} // end routine AddGdbGenMsg


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessLtngRpt(void)
//  Parses/removes a single message from the Lightning interface report queue.
//
//  INPUT : NONE (but accesses ltngRptQ and other module attributes)
//  OUTPUT: NONE (but updates a plethora of module attributes)
//  CALLS : LTG_RX_TRG_SET
//          QUEUE_NOT_EMPTY
//          PROT_Q_GET
//          LTG_DECR_RX_TRG
//          strstr
//          WriteCftToNvMem
//          atoi
//          QueueLtngCmd
//          SetWaveformTraits
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void ProcessLtngRpt(void)
{
  char rpt[LTG_RPT_MAX_LEN];            // Buffer to hold Lightning report
  int  i;                               // Loop indexer

  // If there is data to process, start processing it
  if (LTG_RX_TRG_SET)                   // When frame stop char received...
  {
    uint8_t c;                          // candidate character for report
    char * dest;                        // char's destination w/in rpt[]
    int extrStp = 2;                    // report extraction steps remaining
    i = LTG_RPT_MAX_LEN;
    while (extrStp && QUEUE_NOT_EMPTY(ltngRptQ) && i)
    {                                   // While report data need be extracted
      PROT_Q_GET(ltngRptQ, c)           // get a single report's data from queue
      if (RTI_FSC == c)
      {                                 // When found frame start character
        dest  = rpt;                    // [re]initialize destination to rpt[0]
        *dest = RTI_FSC;                // save the FSC
        dest++;                         // increment destination to rpt[1]
        i = LTG_RPT_MAX_LEN - 1;        // [re]initialize max char extraction
        extrStp = 1;                    // and now ready to extract report
      }
      else if (1 == extrStp)
      {                                 // When extracting a report
        *dest = (char)c;                // save current character in rpt[]
        dest++;                         // increment destination w/in rpt[]
        i--;                            // note one less space available
        if (RTI_FPC == c)
        {                               // When found frame stop character
          LTG_DECR_RX_TRG();            // decrement report trigger count
          extrStp = 0;                  // and end extraction steps
        }
      }
    }                                   // end while extracting Lightning report

    if (extrStp)
    {                                   // When still some extraction step
      esdErrFlags.ltng = 1;             // note extraction has failed
    }
    else if ('+' == rpt[1])
    {                                   // When Lightning ACK'ed a command
      sysStat.ltngAckReq = 0;           // no longer need ACK from Lightning
    }
    else if ('-' == rpt[1])
    {                                   // When Lightning NAK'ed a command
      sysStat.ltngAckReq = 0;           // no longer need ACK from Lightning
      esdErrFlags.ltng   = 1;           // but there is something wrong
    }
    else if (NULL != strstr(rpt, "D.OPS"))
    { // process various Operational Status messages
      if (sysStat.ltngRptZed)
      {                                 // When ZEROED operational status
        ;                               // already reported do not change
      }                                 // displayed status from ZEROED
      else if (NULL != strstr(rpt, "SLEEP"))
      {
        if ((TDC_MUTE == devCfg.txDtyCy) && ( ! muteSquawkCtdn))
        {                               // When sleep after squawk-set done in
          updtFld.actvBrev = 1;         // MUTE mode, set brevity on HOME screen
        }                               // to indicate exfil muted
        dsplStatus = DS_SLEEP;
      }
      else if (NULL != strstr(rpt, "no GPS"))
      {                                 // Must check "no GPS" before "GPS" !!!
        dsplStatus = DS_NO_GPS;
      }
      else if (NULL != strstr(rpt, "GPS"))
      {
        dsplStatus = DS_GPS;
      }
      else if (NULL != strstr(rpt, "XMT"))
      {
        if (muteSquawkCtdn)
        {                               // When squawking a set in mute mode
          muteSquawkCtdn--;             // countdown the squawks in the set so
        }                               // know when to update HOME upon sleep
        dsplStatus = DS_XMT;
      }
      else if (NULL != strstr(rpt, "GDB"))
      {
        dsplStatus = DS_GDB;
      }
      else if (NULL != strstr(rpt, "911-A"))
      {
        dsplStatus = DS_A911;
      }
      else if (NULL != strstr(rpt, "911-M"))
      {
        dsplStatus = DS_M911;
      }
      else if (NULL != strstr(rpt, "POR"))
      {
        dsplStatus = DS_POR;
      }
      else if (NULL != strstr(rpt, "ZEROED"))
      {
        dsplStatus = DS_ZEROED;
        sysStat.ltngRptZed = 1;
      }
      updtFld.ops = 1;
    }
    else if (NULL != strstr(rpt, "D.GLL"))
    { // process GPS Lat/Lon message -- now accepting only decimal degrees text
      SetCoordsFromDecLatLon(&myLoc, &rpt[7], &rpt[18]);
      if (COORD_ERR == myLoc.dblLat)
      {
        esdErrFlags.ltng = 1;
      }
      else if (WGM_OVRDN < devCfg.geoMuting)
      { // When waveform has geo-mute that isn't overridden, check if 'in zone'
        geopt16_t curPt;
        CoordsToGeopt16(&myLoc, &curPt);
        wgm_t wgm = WGM_OUTGZ;        // most of world is outside of geozone
        //llurzone16_t geoRect = USA_LLUR16;
        //if (TestInsideLlUrZone(curPt, geoRect))
        {
          static polyzone16_t usaPoly = {.n = USA_NUM_PTS, .v = USA_VERTICES};
          if (TestInsidePolygonZone(curPt, &usaPoly)) {
            wgm = WGM_ACTVD;                          }
        }
        if (wgm != selWfTrait.wgmOpt)
        { // Update wfTrait, various screen fields, and MUTE TX if appropriate
          selWfTrait.wgmOpt = wgm;
          updtFld.actvBrev  = 1;      // update HOME screen
          updtFld.brevO     = 1;      // update BREVITY CODE screen
          updtFld.txDtyCy   = 1;      // update DUTY CYCLE screen
          if ((WGM_ACTVD == wgm) && (TDC_MUTE != devCfg.txDtyCy))
          { // Need to actively geo-mute because Duty Cycle not already mute
            devCfg.txDtyCy = TDC_MUTE;
            QueueLtngCmd(CID_CBDC, (char*)(&devCfg.txDtyCy));
          } // intentionally _NO_ 'else' to jump out of mute
        }
      } // end check 'in geo-mute zone'

      aof               = 0;            // Note that fix is brand new
      sysStat.aofValid  = true;         // thus AoF is good.  Also update:
      updtFld.coord     = 1;            // a) coordinates on HOME screen
      updtFld.aof       = 1;            // b) AoF on HOME screen
      updtFld.rngBrg    = 1;            // c) RANGE & BEARING screen
    }
    else if (NULL != strstr(rpt, "D.TGF"))
    { // process Time of GPS Fix message
      sysHr  = ((rpt[7]  - '0') * 10) + (rpt[8]  - '0');
      sysMin = ((rpt[10] - '0') * 10) + (rpt[11] - '0');
      sysSec = ((rpt[13] - '0') * 10) + (rpt[14] - '0');
      sysStat.gpsTime = true;
      updtFld.time    = 1;
    }
    else if (NULL != strstr(rpt, "D.NBE"))
    { // process Next Beacon Event message
      // Only process NBE messages if a valid system time is available
      if (sysStat.gpsTime)
      { // Only when have valid GPS time can we know time to countdown to NBE
        // NBE day-of-week at rpt[7]        ex. 03
        // NBE hours       at rpt[10]-[11]  ex. 15
        // NBE minutes     at rpt[13]-[14]  ex. 45
        // NBE seconds     at rpt[16]-[17]  ex. 35
        // NBE code is     at rpt[19]       ex. 5
        //   000000000011111111112
        //   012345678901234567890
        //   !D.NBE 03 15:46:35 5P
        // TODO:  Process two-digit day of week if ever useful
        // TODO:  Process any other useful NBE codes
        if ('5' == rpt[19]) // Process NBE Transmit only for now
        {
          char * nbePointer;            // point to beginning of NBE time fields

          rpt[12] = '\0';               // Insert null chars at end of hours,
          rpt[15] = '\0';               // minutes, and seconds fields to
          rpt[18] = '\0';               // tokenize for use with atoi function

          nbePointer = &(rpt[10]);      // Point to first NBE hours digit
          nbeHr = atoi(nbePointer);     // convert hours text to integer
          nbePointer += 3;              // Point to first NBE minutes digit
          nbeMin = atoi(nbePointer);    // convert minutes text to integer
          nbePointer += 3;              // Point to first NBE seconds digit
          nbeSec = atoi(nbePointer);    // convert seconds text to integer

          if (nbeHr < sysHr)
          {                             // When NBE occurs tomorrow, must set
            nbeHr += 24;                // to artificial time past midnight
          }
          nbeSec -= sysSec;             // Calculate preliminary seconds delta
          nbeMin -= sysMin;             // Calculate preliminary minutes delta
          nbeHr  -= sysHr;              // Calculate preliminary hours delta
          while (0 > nbeSec)
          {                             // When needed, borrow seconds
            nbeSec += 60;
            --nbeMin;
          }
          while (0 > nbeMin)
          {                             // When needed, borrow minutes
            nbeMin += 60;
            --nbeHr;
          }
          if (0 <= nbeHr)
          {                             // When NBE time delta is valid
            dsplStatus = DS_CTDN;       // display NBE countdown on HOME screen
          }
        }
      }
      updtFld.ops = 1;
    }
    else if (NULL != strstr(rpt, "D.GID"))
    { // process GDB Infil Data message
      if ('1' == rpt[7] && '5' == rpt[8])
      {                                 // When got Zeroize GDB message...
        StartLbhhZeroize();             // kick off LBHH zeroization
      }
      else if ('0' == rpt[7] && 'C' == rpt[8])
      {                                 // When got Forwarded PLI GDB message...
        AddGdbPliData(rpt);             // add D.GID data for Range-Bearing scrn
      }
      else
      {                                 // When got generic/C2 GDB message...
        AddGdbGenMsg(rpt);              // add D.GID message data for INFIL scrn
      }
    }
    else if (NULL != strstr(rpt, "D.RST"))
    { // process Reset message
      sysStat.ltngRptRst = 1;
    }
    else if (NULL != strstr(rpt, "D.LCM"))
    { // Copy 7-char ltng ver from "!D.LCM M.m.f.tP" into "vW.x.y.z/vL_T_N_T"
      memcpy((void*)&fwVerStr[10], (const void*)&rpt[7], 7);
      sysStat.ltngRptVer = 1;           // Note Lightning version info available
      updtFld.pwr01      = 1;           // and set flag so version will display
    }
    else if (NULL != strstr(rpt, "D.BIT"))
    { // process Built-In Test message
      char * paramLoc;

      sysStat.ltngRptBit = 1;
      sysStat.ltngRdyCmd = 1;

      // Find "EXFIL" in "D.BIT" string, if found, exfil code at [6] and [7]
      paramLoc = strstr(rpt, "EXFIL");
      if (paramLoc)
      {
        memcpy((void*)exfilCode, (const void*)(paramLoc+6),  2);
        ltngBitRpt.exF = htou8(exfilCode);
      }
      // Find "INFIL" in "D.BIT" string, if found, infil code at [6] and [7]
      paramLoc = strstr(rpt, "INFIL");
      if (paramLoc)
      {
        memcpy((void*)infilCode, (const void*)(paramLoc+6),  2);
        ltngBitRpt.inF = htou8(infilCode);
      }
      // Find "LTGHW" in "D.BIT" string, if found, hardware code at [6] and [7]
      paramLoc = strstr(rpt, "LTGHW");
      if (paramLoc)
      {
        memcpy((void*)ltngHwCode, (const void*)(paramLoc+6),  2);
        ltngBitRpt.hwF = htou8(ltngHwCode);
      }
      // Find "LTGFW" in "D.BIT" string, if found, firmware code at [6] and [7]
      paramLoc = strstr(rpt, "LTGFW");
      if (paramLoc)
      {
        memcpy((void*)ltngFwCode, (const void*)(paramLoc+6),  2);
        ltngBitRpt.fwF = htou8(ltngFwCode);
      }

      updtFld.bitRslt = updtFld.bit = 1;    // update BIT icon and BIT screen
    }
    else if (NULL != strstr(rpt, "D.GKN"))
    { // save names of loaded group GDB keys, each key name exactly 6 characters
      memcpy((void*)infKey1, (const void*)&rpt[14], 6);
      memcpy((void*)infKey2, (const void*)&rpt[21], 6);
      memcpy((void*)infKey3, (const void*)&rpt[28], 6);
      memcpy((void*)infKey4, (const void*)&rpt[35], 6);
      memcpy((void*)infKey5, (const void*)&rpt[42], 6);
    }
    else if (NULL != strstr(rpt, "D.GAK"))
    { // save names of active GDB keys and set flag so key names will display
      memcpy((void*)uKeyName, (const void*)&rpt[7],  6);
      memcpy((void*)gKeyName, (const void*)&rpt[14], 6);
      sysStat.ltngRptGak = 1;
      esdErrFlags.uKey   = isdigit(uKeyName[0]);
      updtFld.bitRslt    = updtFld.bit = 1; // update BIT icon and BIT screen
      updtFld.keyName    = 1;
    }
    else if (NULL != strstr(rpt, "D.WFI"))
    { // save waveform info, when last reported set flag so P.BIT will continue
      uint8_t idx = (rpt[7] - '0');     // convert ascii slot# to numeric value
      wi_t *  pWi = (wi_t*)(&rpt[7]);

      if (MAX_WF == rpt[7])         {
        sysStat.ltngRptWfi = 1;     }
      SetWaveformTraits(pWi, &wfTrait[idx]);
    }
    else
    {
      // discard other messages
    }
  }
} // end routine ProcessLtngRpt


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateSysCheckDisplay(void)
//  Update P-BIT screen displayed when system power (bat/ext) initially applied.
//  Get config from nvmen, query Ltng for critical info, etc., & display results
//
//  INPUT : NONE  (access module attributes sysStat, updtFld, etc.)
//  OUTPUT: NONE  (updates module attributes)
//  CALLS : LCDWriteStringTerminal6X8
//          QueueLtngCmd
//          ReadCfgFromNvMem
//          SET_BKLT_OFF
//          LCDClearScreen
//          GetLtc2943Charge
//          ReadGasGaugeFromNvMem
////          Si7021ReadRelativeHumidityHoldMasterMode
////          Si7021ReadTemperatureHoldMasterMode
//          sprintf
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateSysCheckDisplay(void)
{
  if (updtFld.sysChk)
  {
    if (updtFld.chgScr)
    {
      LCDWriteStringTerminal6X8(0, 3+2*WIDTH_6X8, "SYSTEM CHECK", false);
      LCDWriteStringTerminal6X8(2, 0, "Waiting for... ", false);
      scrnCtdn = BIT_TEST_CTDN;         // Set countdown for timely Ltng D.RST
      QueueLtngCmd(CID_SWFI, NULL);     // query all waveform info
      QueueLtngCmd(CID_SGAK, NULL);     // and query names of active GDB keys
    }

    if ( ! sysStat.cfgInit)
    {
      LCDWriteStringTerminal6X8(3, 0, "NVMEM Read/Write ", false);
      ReadCfgFromNvMem();
      if (OS_ZEROED == devCfg.opStat)
      {                                 // Act immediately if opStat is zeroed
        do                              // TODO - wipe program memory
        {                               // When system has been zeroized
          LTG_VLTG_EN = 0;              // turn off Lightning
          SET_BKLT_OFF();               // turn off backlight
          LCDClearScreen();             // clear LCD of all text
          mGlobalIntDisable();          // disable all IRQs that could wake
          Sleep();                      // processor which want only to sleep
        } while (1);                    // and thus execute no more
      }
      switch (devCfg.bkltSet)
      {                                 // Set backlight per configuration
        case BS_HIGH:   SET_BKLT_HIGH();  break;
        case BS_LOW:    SET_BKLT_LOW();   break;
        case BS_NVG:    SET_BKLT_NVG();   break;
        default:        SET_BKLT_OFF();
      }
      if ((FW_KEY != devCfg.fwKey) ||   // When firmware & config mismatch
          (MAX_WF < devCfg.selWfChar)){ // or invalid wf selection in config
        esdErrFlags.nvmem = 1;        } // flag a config memory error.
      selWfTrait.wgmOpt = devCfg.geoMuting; // Set geo-muting mode
      #if (BOOT_MUTE == 1)              // Two user groups requested BOOT_MUTE
        devCfg.txDtyCy = TDC_MUTE;      // so always override any other setting
      #endif                            // when defined to function that way
      ReadGasGaugeFromNvMem(&acr.val, &btryPct);
      sysStat.cfgInit = 1;              // note device config initialized
    }
    else if (scrnCtdn >= BIT_TEST_CTDN-2)
    { // Users need help knowing when their batteries are not strong enough
      LCDWriteStringTerminal6X8(2, 0, "If keep jumping",   false);
      LCDWriteStringTerminal6X8(3, 0, "back here - then ",  false);
      LCDWriteStringTerminal6X8(4, 0, "replace batteries", false);
    }
    else if ( ! sysStat.gasInit)
    {
      uint16byte_t trashAcr;
      LCDWriteStringTerminal6X8(2, 0, "Waiting for... ", false);
      LCDWriteStringTerminal6X8(3, 0, "Gas Gauge Reading", false);
      LCDWriteStringTerminal6X8(4, 0, "                 ", false);
      if ( ! GetLtc2943Charge(&trashAcr.HB, &trashAcr.LB))
      {
        esdErrFlags.gasGa = 1;
      }

      if (IS_ON_BAT_PWR)
      {
        gasChkCtdn = GAS_CHK_CTDN;
        wasOnBtry  = true;
      }
      else // IS_ON_EXT_PWR
      {
        gasChkCtdn = CTDN_OFF;
        wasOnBtry  = false;
      }
      updtFld.pwr = 1;                  // note displayed power needs update
      sysStat.gasInit = 1;              // and that gas gauge initialized
    }
    else if ( ! sysStat.rstDone)
    {
      if (scrnCtdn)                                                   {
        LCDWriteStringTerminal6X8(3, 0, "Lightning Reset  ", false);  }
      else                                                            {
        LCDWriteStringTerminal6X8(3, 0, "DEPOT MAINT REQ !", false);  }
      if (sysStat.ltngRptRst)
      {
        sysStat.ltngRptRst = 0;         // Clear flag so can detect future event
        sysStat.rstDone    = 1;         // Clear this only when PrepForSleep()
      }
    }
    else if ( ! sysStat.bitDone)
    {
      LCDWriteStringTerminal6X8(3, 0, "Lightning BIT    ", false);
      if (sysStat.ltngRptBit)
      {
        sysStat.ltngRptBit = 0;         // Clear flag so can detect future event
        sysStat.bitDone    = 1;
      }
    }
    else if ( ! sysStat.ltngRptWfi)
    {
      LCDWriteStringTerminal6X8(3, 0, "Waveform Info    ", false);
    }
    else if ( ! sysStat.ltngRptGak)
    {
      LCDWriteStringTerminal6X8(3, 0, "GDB Key Names    ", false);
    }
    else
    {                                   // When all checks, done display results
      bool isBitPass = true;

      if (ltngBitRpt.exF)
      {
        LCDWriteStringTerminal6X8(2, 0, " EXFIL......0x", false);
        LCDWriteStringTerminal6X8(2, (14*WIDTH_6X8), exfilCode, false);
        isBitPass = false;
      }
      else
      {
        LCDWriteStringTerminal6X8(2, 0, " EXFIL......PASS", false);
      }
      if (ltngBitRpt.inF || esdErrFlags.inF)
      {
        if (esdErrFlags.inF)    {
          infilCode[0] = '8';   }
        LCDWriteStringTerminal6X8(3, 0, " INFIL......0x", false);
        LCDWriteStringTerminal6X8(3, (14*WIDTH_6X8), infilCode, false);
        isBitPass = false;
      }
      else
      {
        LCDWriteStringTerminal6X8(3, 0, " INFIL......PASS", false);
      }
      if (ltngBitRpt.hwF || esdErrFlags.hwF)
      {
        char esdHwCode[3];
        sprintf(esdHwCode, "%02X", esdErrFlags.hwF);
        LCDWriteStringTerminal6X8(4, WIDTH_6X8, "SYSHW....0x", false);
        LCDWriteStringTerminal6X8(4, (12*WIDTH_6X8), ltngHwCode,false);
        LCDWriteStringTerminal6X8(4, (14*WIDTH_6X8), esdHwCode, false);
        isBitPass = false;
      }
      else
      {
        LCDWriteStringTerminal6X8(4, WIDTH_6X8, "SYSHW......PASS", false);
      }
      if (ltngBitRpt.fwF || esdErrFlags.fwF)
      {
        char esdFwCode[3];
        sprintf(esdFwCode, "%02X", esdErrFlags.fwF);
        LCDWriteStringTerminal6X8(5, WIDTH_6X8, "SYSFW....0x", false);
        LCDWriteStringTerminal6X8(5, (12*WIDTH_6X8), ltngFwCode, false);
        LCDWriteStringTerminal6X8(5, (14*WIDTH_6X8), esdFwCode, false);
        isBitPass = false;
      }
      else
      {
        LCDWriteStringTerminal6X8(5, WIDTH_6X8, "SYSFW......PASS", false);
      }
      LCDWriteStringTerminal6X8(7, 3*WIDTH_6X8, " Press ENT ", true);

      if (isBitPass)              {     // When Built-In-Test passed set
        scrnCtdn = BIT_PASS_CTDN; }     // screen time out for ~4 seconds;
      else                        {     // otherwise give user more time,
        scrnCtdn = BIT_FAIL_CTDN; }     // 60 second, to digest results.

      updtFld.sysChk = 0;               // Field/screen updated so clear flag
    }
    __delay_ms(150);                    // some delay to see display but not
  }                                     // enough to overflow ltngRptQ
} // end routine UpdateSysCheckDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateNewBtryDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateNewBtryDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal12X16(0, 33, "New", false);
    LCDWriteStringTerminal12X16(3, 3, "Battery?", false);
    updtFld.newBat = 1;
  }
  if (updtFld.newBat)
  {
    bool invertYes;
    scrnCtdn = NEW_BTRY_CTDN;
    invertYes = (FP_NEWBAT_Y == focusPoint);
    LCDWriteStringTerminal12X16(6, 12, "Yes", invertYes);
    LCDWriteStringTerminal12X16(6, 66, "No", !invertYes);
    updtFld.newBat = 0;
  }
} // end routine UpdateNewBtryDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdatePwrOnOffDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdatePwrOnOffDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal12X16(1, 2*WIDTH_12X16, "Power", false);
    updtFld.pwr01 = 1;
  }
  if (updtFld.pwr01)
  {
    bool invertOff;
    scrnCtdn = PWR_0_1_CTDN;
    invertOff = (FP_PWR_0 == focusPoint || FP_HPWR_0 == focusPoint);
    LCDWriteStringTerminal12X16(4, 6, "Off", invertOff);
    LCDWriteStringTerminal12X16(4, 6*WIDTH_12X16, "On", !invertOff);
    if (sysStat.ltngRptVer)                             {
      LCDWriteStringTerminal6X8(7, 0, fwVerStr, false); }
    updtFld.pwr01 = 0;
  }
} // end routine UpdatePwrOnOffDisplay


//++ADMIN+PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateConfirmAdminDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateConfirmAdminDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(1, 9, "Are you a LBHH", false);
    LCDWriteStringTerminal6X8(2, 9, "administrator?", false);
    updtFld.admin = 1;
  }
  if (updtFld.admin)
  {
    bool invertYes = (FP_ADM_Y == focusPoint);
    scrnCtdn = ADMIN_CTDN;
    LCDWriteStringTerminal6X8(5, 12, "Yes", invertYes);
    LCDWriteStringTerminal12X16(4, 60, "NO", !invertYes);
    updtFld.admin = 0;
  }
} // end routine UpdateConfirmAdminDisplay


//++ADMIN+PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// void UpdateAdminWrnDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateAdminWrnDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal12X16(0, 15, "NOTICE", false);
//  updtFld.admWrn = 1;
//}
//if (updtFld.admWrn)
//{
    LCDWriteStringTerminal6X8(2, 0, "Exfil waveform &", false);
    LCDWriteStringTerminal6X8(3, 0, "infil group key", false);
    LCDWriteStringTerminal6X8(4, 0, "changes need to", false);
    LCDWriteStringTerminal6X8(5, 0, "be properly", false);
    LCDWriteStringTerminal6X8(6, 0, "coordinated for", false);
    LCDWriteStringTerminal6X8(7, 0, "data distribution", false);
//  updtFld.admWrn = 0;
  }
} // end routine UpdateAdminWrnDisplay

//++ADMIN+PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// void UpdateExfilOptDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateExfilOptDisplay(void)
{
  #define WF1_NAME (char*)wfTrait[1].name
  #define WF2_NAME (char*)wfTrait[2].name
  #define WF3_NAME (char*)wfTrait[3].name
  #define WF4_NAME (char*)wfTrait[4].name
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(0, 12, "Exfil Choices", false);
    switch (devCfg.selWfChar)
    {
      case WF_SLOT1:  focusPoint = FP_EXF_1;    break;
      case WF_SLOT2:  focusPoint = FP_EXF_2;    break;
      case WF_SLOT3:  focusPoint = FP_EXF_3;    break;
      case WF_SLOT4:  focusPoint = FP_EXF_4;    break;
      default:                                  break;
    }
    updtFld.exfOpt = 1;
  }
  if (updtFld.exfOpt)
  {
    LCDWriteStringTerminal6X8(2,  9, WF1_NAME, (FP_EXF_1 == focusPoint));
    LCDWriteStringTerminal6X8(2, 57, WF2_NAME, (FP_EXF_2 == focusPoint));
    LCDWriteStringTerminal6X8(4,  9, WF3_NAME, (FP_EXF_3 == focusPoint));
    LCDWriteStringTerminal6X8(4, 57, WF4_NAME, (FP_EXF_4 == focusPoint));
    updtFld.exfOpt = 0;
  }
} // end routine UpdateExfilOptDisplay

//++ADMIN+PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// void UpdateFsetDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateFsetDisplay(void)
{
  #define FSET_NAME(i) (char*)fsetTrait[i].name
  static uint8_t page = 0; // Page index to track which set of options to display

  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(0, 12, "Fset Choices", false);
    focusPoint = FP_FSET_0 + page * 6; // Set focus point to the first item on the current page
    updtFld.fsetOpt = 1;
  }
  if (updtFld.fsetOpt)
  {
    for (int i = 0; i < 6; i++)
    {
      uint8_t fsetIndex = page * 6 + i;
      if (fsetIndex < 15)
      {
        LCDWriteStringTerminal6X8(2 + (i / 2) * 2, (i % 2) * 48 + 9, FSET_NAME(fsetIndex), (focusPoint == FP_FSET_0 + fsetIndex));
      }
    }
    updtFld.fsetOpt = 0;
  }
} // end routine UpdateFsetDisplay


//++ADMIN+PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateExfilCtdnDisplay(void)
//  Behind the scene this take care of timing related to cycling Lightning power
//  All keypad inputs (except Zeroize) are locked out during life of screen
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateExfilCtdnDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(2, 18, "Please Wait", false);
    scrnCtdn = CSWF_CTDN;               // 60 <= ctdn < 100 to allow worst case
  }                                     // timing (wf change as BOLT TX ready!)
                                        // while not resulting in an overflow of
  char ctdnStr[3];                      // two digits plus NULL terminator to
  sprintf(ctdnStr, "%02d", scrnCtdn);   // display 60,59,...,01 countdown
  LCDWriteStringTerminal12X16(4, 39, ctdnStr, false);

  if (9 == scrnCtdn)                    // After BOLT had 45+ seconds to reset
  {                                     // as part of its changing waveform
    LTG_CLOSE_UART();                   // we need to turn off Lightning for
    LTG_VLTG_EN = 0;                    // a minimum of 3 seconds. Note the BOLT
    sysStat.gpsTime    = 0;             // will need to get GPS, Ltng needs to
    sysStat.ltngRptRst = 0;             // send D.RST before leave screen, and
    sysStat.ltngRdyCmd = 0;             // not cmd-ready until get D.BIT report.
    sysStat.lowPwrMode = 0;             // Ltng doesn't power-reset into LPM.
  }
  else if (4 == scrnCtdn)
  {                                     // Soon before timeout expires and after
    LTG_VLTG_EN = 1;                    // being turned off 3+ seconds (5 = 9-4)
    LTG_INIT_UART();                    // turn the Lightning back on so that it
    LTG_READ_NONBLOCKING(ltngRptQ);     // and BOLT get a fresh new start for
  }                                     // using recently selected waveform
  else if (( ! sysStat.ltngRptRst) && (2 >= scrnCtdn))
  {                                     // When haven't gotten the required
    scrnCtdn++;                         // D.RST from Ltng need to free-wheel
  }                                     // waiting for it (yes, this could hang)
} // end routine UpdateExfilCtdnDisplay


//++ADMIN+PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateGeoMuteOvrdDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateGeoMuteOvrdDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal12X16(0, 3, "Override", false);
    LCDWriteStringTerminal12X16(3, 3, "GeoMute?", false);
    focusPoint = (WGM_OVRDN == selWfTrait.wgmOpt)
               ? FP_OVRD_Y
               : FP_OVRD_N ;
    updtFld.gmOvrd = 1;
  }
  if (updtFld.gmOvrd)
  {
    bool invertYes;
    invertYes = (FP_OVRD_Y == focusPoint);
    LCDWriteStringTerminal6X8  (7, 12, "Yes", invertYes);
    LCDWriteStringTerminal12X16(6, 60, "NO", !invertYes);
    updtFld.gmOvrd = 0;
  }
} // end routine UpdateGeoMuteOvrdDisplay


//++ADMIN+PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// void UpdateInfilOptDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateInfilOptDisplay(void)
{
  #define IKEY_N   "No Grp"     // What to show to user when no group key active
  #define NO_KEY   "0GpKey"     // What Ltng returns, not what we want to show

  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(0, 12, "Infil Choices", false);
    if      ( ! strcmp(infKey1, gKeyName))  { focusPoint = FP_INF_1; }
    else if ( ! strcmp(infKey2, gKeyName))  { focusPoint = FP_INF_2; }
    else if ( ! strcmp(infKey3, gKeyName))  { focusPoint = FP_INF_3; }
    else if ( ! strcmp(infKey4, gKeyName))  { focusPoint = FP_INF_4; }
    else if ( ! strcmp(infKey5, gKeyName))  { focusPoint = FP_INF_5; }
    else if ( ! strcmp(NO_KEY,  gKeyName))  { focusPoint = FP_INF_N; }
    updtFld.infOpt = 1;
  }
  if (updtFld.infOpt)
  {
    LCDWriteStringTerminal6X8(2,  9, infKey1, (FP_INF_1 == focusPoint));
    LCDWriteStringTerminal6X8(2, 57, infKey2, (FP_INF_2 == focusPoint));
    LCDWriteStringTerminal6X8(4,  9, infKey3, (FP_INF_3 == focusPoint));
    LCDWriteStringTerminal6X8(4, 57, infKey4, (FP_INF_4 == focusPoint));
    LCDWriteStringTerminal6X8(6,  9, infKey5, (FP_INF_5 == focusPoint));
    LCDWriteStringTerminal6X8(6, 57, IKEY_N,  (FP_INF_N == focusPoint));
    updtFld.infOpt = 0;
  }
} // end routine UpdateInfilOptDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateZeroizeDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateZeroizeDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal12X16(0, 9, "Zeroize", false);
    LCDWriteStringTerminal12X16(3, 9+WIDTH_12X16, "Unit?", false);
    updtFld.zeroize = 1;
  }
  if (updtFld.zeroize)
  {
    if (FP_ZERO_A == focusPoint)
    {
      scrnCtdn = ZUM_ACT_CTDN;
      LCDWriteStringTerminal12X16(3, 3+WIDTH_12X16, " Unit ", false);
      LCDWriteStringTerminal12X16(6, 9, "WORKING", false);
    }
    else
    {
      bool invertYes;
      scrnCtdn = ZUM_N_Y_CTDN;
      invertYes = (FP_ZERO_Y == focusPoint);
      LCDWriteStringTerminal12X16(6, WIDTH_12X16, "Yes", invertYes);
      LCDWriteStringTerminal12X16(6, 6+5*WIDTH_12X16, "No", !invertYes);
    }
    updtFld.zeroize = 0;
  }
} // end routine UpdateZeroizeDisplay

//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedTXCount(void)
//  Places the TX count on the display at a specified location, need to detemine location.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedTXCount(void)
{
  if (updtFld.txCount)
  {
    char txCountStr[10];
    sprintf(txCountStr, "TX: %d", txCount);
    LCDWriteStringTerminal6X8(5, 66, txCountStr, false);
    updtFld.txCount = 0;                // Field updated so clear flag
  }
} // end routine UpdateDisplayedTXCount


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedCoord(void)
//  Places measured location coordinates on HOME screen rows 0 & 1 col 0-60.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedCoord(void)
{
  if (updtFld.coord)
  { // lat/lon from Lightning or MGRS should already be in their attributes
    bool invCoord;

    // Once age of fix is critical flash coordinates each second
    invCoord = ((CRIT_AOF <= aof) && (sysSec & 0x01));

    if (CS_DMS == devCfg.cSysSet)
    {
      LCDWriteStringTerminal6X8(0, 0, myLoc.dmsLat, invCoord);
      LCDWriteStringTerminal6X8(1, 0, myLoc.dmsLon, invCoord);
    }
    else if (CS_DEC == devCfg.cSysSet)
    {
      LCDWriteStringTerminal6X8(0, 0, myLoc.decLat, invCoord);
      LCDWriteStringTerminal6X8(1, 0, myLoc.decLon, invCoord);
    }
    else // CS_MGRS
    {
      LCDWriteStringTerminal6X8(0, 0, myLoc.mgrsGzd, invCoord);
      LCDWriteStringTerminal6X8(1, 0, myLoc.mgrs10m, invCoord);
    }
    updtFld.coord = 0;                  // Field updated so clear flag
  }
} // end routine UpdateDisplayedCoord


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedBitIcon(void)
//  Places the BIT failure icon on HOME screen rows 0-1 col 62-74.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedBitIcon(void)
{
  if (updtFld.bit)
  {
    if (esdErrFlags.val || ltngBitRpt.val)                {
      LCDWriteStringTerminal12X16(0, 62, "!", false);     }
    else                                                  {
      LCDWriteStringTerminal12X16(0, 62, " ", false);     }
    updtFld.bit = 0;                    // Field updated so clear flag
  }
} // end routine UpdateDisplayedBitIcon


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedPwrStat(void)
//  Places power status/battery icon on HOME screen rows 0-1 col 76-100.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedPwrStat(void)
{
  char btryPctStr[4];

  if (updtFld.pwr)
  {
    if (IS_ON_BAT_PWR)
    {
      // Lookup index to display battery graphic according to battery percentage
      //    100-96%: 11,   95-86%: 10,   ...,   15-6%: 2,   5-1%: 1,   0%: 0
      uint16_t        index;
      bool            invGa;            // Blink Gas Gauge when pwr critical
      const uint8_t * pFontChar;        // saves code space

      if (btryPct)                      {
        index = (btryPct + 14) / 10;    }
      else                              {
        index = 0;                      }
      invGa = ((BTRY_CRIT_LVL > btryPct) && (sysSec & 0x01));

      // NOTE - BATTERY_18X8_0 is reserved for when battery is at 0% or below
      //        and when power level is critical will flash inverse on & off
      switch (index)
      {
        case 11:  pFontChar = BATTERY_18X8_11;  break;
        case 10:  pFontChar = BATTERY_18X8_10;  break;
        case 9:   pFontChar = BATTERY_18X8_9;   break;
        case 8:   pFontChar = BATTERY_18X8_8;   break;
        case 7:   pFontChar = BATTERY_18X8_7;   break;
        case 6:   pFontChar = BATTERY_18X8_6;   break;
        case 5:   pFontChar = BATTERY_18X8_5;   break;
        case 4:   pFontChar = BATTERY_18X8_4;   break;
        case 3:   pFontChar = BATTERY_18X8_3;   break;
        case 2:   pFontChar = BATTERY_18X8_2;   break;
        case 1:   pFontChar = BATTERY_18X8_1;   break;
        default:  pFontChar = BATTERY_18X8_0;
      }
      LCDWriteFont(0,82,HEIGHT_18X8,WIDTH_18X8,pFontChar,invGa);
      sprintf(btryPctStr, "%3u%%", btryPct);
      LCDWriteStringTerminal6X8(1, 76, btryPctStr, invGa);

      if (esdErrFlags.gasGa)
      { // Show 'X' next to battery icon if had a gas gauge error
        LCDWriteStringTerminal6X8(0, 76, "X", invGa);
      }
      else
      { // otherwise overwrite any prior 'X' above
        LCDWriteStringTerminal6X8(0, 76, " ", invGa);
      }
    }
    else  // IS_ON_EXT_PWR
    {
      LCDWriteStringTerminal6X8(0, 76, " EXT", false);
      LCDWriteStringTerminal6X8(1, 76, " PWR", false);
    }

    updtFld.pwr = 0;                    // Field updated so clear flag
  }
} // end routine UpdateDisplayedPwrStat


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedSysTime(void)
//  Places current system time on HOME screen row 3-4 col 3-99.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedSysTime(void)
{
  if (updtFld.time)
  {
    char timeString[9];
    sprintf(timeString, "%02u:%02u:%02u", sysHr, sysMin, sysSec);
    if ( ! sysStat.gpsTime)
    { // when GPS not valid replace the two ':' in time with '-'
      timeString[2] = timeString[5] = '-';
    }
    LCDWriteStringTerminal12X16(3, 3, timeString, false);
    updtFld.time = 0;                   // Field updated so clear flag
  }
} // end routine UpdateDisplayedSysTime


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedActvBrevCode(void)
//  Places brevity code/exfil mute on HOME screen row 6-7 col 0-36.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedActvBrevCode(void)
{
  if (updtFld.actvBrev)
  {
    if (sysStat.lowPwrMode)
    {                                                   // When LBHH in special
      LCDWriteStringTerminal12X16(6, 0, "LPM", false);  // Low Power Mode need
    }                                                   // to indicate on HOME!!
    else if ((TDC_MUTE == devCfg.txDtyCy) && (WGM_ACTVD == selWfTrait.wgmOpt))
    {                                                   // When geo-muting
      LCDWriteStringTerminal6X8(6, 0, "  GEO ", false); // indicate that. Check
      LCDWriteStringTerminal6X8(7, 0, " MUTED", false); // TCD_MUTE lets 001 be
    }                                                   // shown when wf changed
    else if ((TDC_MUTE == devCfg.txDtyCy) && ( ! muteSquawkCtdn))
    {                                                   // When in mute and not
      LCDWriteStringTerminal6X8(6, 0, " EXFIL", false); // temporarily squawking
      LCDWriteStringTerminal6X8(7, 0, " MUTED", false); // due to a brev entry
    }                                                   // indicate MUTED
    else if (sysStat.in911Mode)
    {                                                   // When in 911 display
      LCDWriteStringTerminal12X16(6, 0, "911", false);  // that instead of the
    }                                                   // actual code for 911
    else
    {                                                   // otherwise show brev #
      LCDWriteStringTerminal12X16(6, 0, devCfg.brevCode, false);
    }
    updtFld.actvBrev = 0;               // Field updated so clear flag
  }
} // end routine UpdateDisplayedActvBrevCode


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedMailIcon(void)
//  Places envelop icon on HOME & GDB screens at row 7 col 48, and if on GDB
//  screen also places number of unread messages to its left, starting at col 33
//  Uses reverse video for envelope if the newest Range & Bearing data is unread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedMailIcon(void)
{
  #define ENV_COL   (8*WIDTH_6X8)       // Envelope column
  #define CNT_COL   (6*WIDTH_6X8 - 3)   // Msg count column
  if (updtFld.mail)
  {
    bool rvEnv     = (0 <= nmuPliIdx);  // Reverse video mail icon for R&B nmu?
    char cntStr[3] = "  ";              // Two char ( 1-49) plus NULL terminator
    if (numUnrdGdbMsgs)
    {
      LCDWriteStringTerminal6X8(7, ENV_COL, ENVELOPE_STR, rvEnv);
      if (FP_GDB == focusPoint)
      {
        sprintf(cntStr, "%2u", numUnrdGdbMsgs);
        LCDWriteStringTerminal6X8(7, CNT_COL, cntStr, false);
      }
    }
    else
    {
      if (rvEnv)
      { // no unread infil C2 messages but R&B newest message unread
        LCDWriteStringTerminal6X8(7, ENV_COL, ENVELOPE_STR, rvEnv);
      }
      else
      { // No unread C2 or R&B messages so clear the envelope space
        LCDWriteStringTerminal6X8(7, ENV_COL, "  ", false);
      }

      if (FP_GDB == focusPoint)
      {
        LCDWriteStringTerminal6X8(7, CNT_COL, cntStr, false);
      }
    }
    updtFld.mail = 0;                    // Field updated so clear flag
  }
} // end routine UpdateDisplayedMailIcon


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedAof(void)
//  Places the Age-of-Fix on HOME screen at row 6 col 66.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedAof(void)
{
  if (updtFld.aof)
  {
    char timeUnit;
    char aofString[7] = "      ";
    if (sysStat.aofValid)
    {
      uint16_t scaledAof;
      if (aof >= 3600)
      {
        timeUnit  = 'h';
        scaledAof = aof / 3600;
      }
      else if (aof >= 60)
      {
        timeUnit  = 'm';
        scaledAof = aof / 60;
      }
      else
      {
        timeUnit  = 's';
        scaledAof = aof;
      }
      sprintf(aofString, "aof%2u%c", scaledAof, timeUnit);
    }
    LCDWriteStringTerminal6X8(6, 66, aofString, false);
    updtFld.aof = 0;                    // Field updated so clear flag
  }
} // end routine UpdateDisplayedAof


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedOpStatus(void)
//  Places Operating Status on HOME screen at row 7 col 66-101.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedOpStatus(void)
{
  if (updtFld.ops)
  {
    char ctdnStr[8];
    switch (dsplStatus)
    {
      case DS_POR:
        LCDWriteStringTerminal6X8(7, 66, "   POR", false);
        break;
      case DS_GPS:
        LCDWriteStringTerminal6X8(7, 66, "   GPS", false);
        break;
      case DS_LPGPS:
        LCDWriteStringTerminal6X8(7, 66, " lpGPS", false);
        break;
      case DS_XMT:
        LCDWriteStringTerminal6X8(7, 66, "   XMT", false);
        break;
      case DS_CTDN:
        if (nbeHr == 0 && nbeMin == 0 && nbeSec < 31)         { // display only
          sprintf(ctdnStr, "X  %02us", nbeSec);               } // reasonable
        else                                                  { // countdown, is
          sprintf(ctdnStr, " SLEEP");                         } // truly asleep
        LCDWriteStringTerminal6X8(7, 66, ctdnStr, false);
        break;
      case DS_GDB:
        LCDWriteStringTerminal6X8(7, 66, "   GDB", false);
        break;
      case DS_NO_GPS:
        LCDWriteStringTerminal6X8(7, 66, "NO GPS", false);
        break;
      case DS_A911:
        LCDWriteStringTerminal6X8(7, 66, "911ACK", false);
        break;
      case DS_M911:
        LCDWriteStringTerminal6X8(7, 66, "911ACK", false);
        break;
      case DS_SLEEP:
        LCDWriteStringTerminal6X8(7, 66, " SLEEP", false);
        break;
      case DS_ZEROED:
        LCDWriteStringTerminal6X8(7, 66, "ZEROED", false);
        break;
      default:  // case DS_NO_STAT:
        LCDWriteStringTerminal6X8(7, 66, "      ", false);
        break;
    }
    updtFld.ops = 0;                    // Field updated so clear flag
  }
} // end routine UpdateDisplayedOpStatus


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateHomeDisplay(void)
//  Calls plethora of other routines to build & display the home screen
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void UpdateHomeDisplay(void)
{
  if (updtFld.chgScr)                 {   // If just coming to HOME screen
    updtFld.val |= HOME_DSPL_FLDS;    }   // update all home screen fields.
  UpdateDisplayedCoord();                 // rows 0-1,  columns  0-60
  UpdateDisplayedBitIcon();               // rows 0-1,  columns 62-74
  UpdateDisplayedPwrStat();               // rows 0-1,  columns 76-100
  UpdateDisplayedSysTime();               // rows 3-4,  columns  3-99
  UpdateDisplayedActvBrevCode();          // rows 6-7,  columns  0-36
  UpdateDisplayedMailIcon();              // rows   7,  columns 48-60
  UpdateDisplayedAof();                   // rows   6,  columns 66-101
  UpdateDisplayedOpStatus();              // rows   7,  columns 66-101
} // end routine UpdateHomeDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedGdbMsg(void)
//  Places timestamped GDB infil message on GDB screen rows 0-6 col 3-99.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedGdbMsg(void)
{
  #define LINE_0_IDX    (0 * (LCD_MAX_COLS + 1))  // Full line + NULL terminator
  #define LINE_1_IDX    (1 * (LCD_MAX_COLS + 1))
  #define LINE_2_IDX    (2 * (LCD_MAX_COLS + 1))
  #define LINE_3_IDX    (3 * (LCD_MAX_COLS + 1))

  if (updtFld.chgScr)
  { // numGdbMsgs, hdGdbMsgQ, tlGdbMsgQ, dsplGdbMsg initialized as 0 for empty Q
    if (numGdbMsgs)
    {                                   // Only when have msgs to display !!!...
      dsplGdbMsg  = (0 == hdGdbMsgQ)    // When enter scrn must index newest msg
                  ? (GDB_MSG_Q_LEN - 1) // If Q's head 0 then newest at max indx
                  : (hdGdbMsgQ - 1);    // otherwise newest at index before head
    } // When nothing to display leave dsplGdbMsg as initialized, i.e. 0, so
  }   // will display first msg if already on GDB screen when it arrives
  if (updtFld.gdb)
  {
    if (0 == numGdbMsgs)
    { //LCDClearScreen();
      LCDWriteStringTerminal6X8(0, 9, "INFIL MESSAGES", false);
      char * str = (sysStat.lowPwrMode)
                 ? "INFIL RCVR OFF  "
                 : "No C2 Messages";
      LCDWriteStringTerminal6X8(4, 6, str, false);
    }
    else
    {
      if (nmuGdbIdx == dsplGdbMsg)    { // When will display newest unread
        nmuGdbIdx = -1;               } // message, set this to indicate such
                                        // When newest message is/will be unread
      bool rvTime = (0 <= nmuGdbIdx);   // reverse video the 'time' field
      if (gdbMsgQ[dsplGdbMsg].unread)
      {                                 // When msg to display previously unread
        if (numUnrdGdbMsgs)           { // In theory this check unneeded before
          numUnrdGdbMsgs--;           } // decrement count of unread and note
        gdbMsgQ[dsplGdbMsg].unread = false;   // this particular msg now read
        updtFld.mail = 1;               // Update mail icon and # unread msgs
      }
      LCDWriteStringTerminal12X16(0,3,gdbMsgQ[dsplGdbMsg].tStamp,rvTime);
      LCDWriteStringTerminal6X8(3,3,&gdbMsgQ[dsplGdbMsg].msg[LINE_0_IDX],false);
      LCDWriteStringTerminal6X8(4,3,&gdbMsgQ[dsplGdbMsg].msg[LINE_1_IDX],false);
      LCDWriteStringTerminal6X8(5,3,&gdbMsgQ[dsplGdbMsg].msg[LINE_2_IDX],false);
      LCDWriteStringTerminal6X8(6,3,&gdbMsgQ[dsplGdbMsg].msg[LINE_3_IDX],false);
    }
    updtFld.gdb = 0;                    // Field updated so clear flag
  }
} // end routine UpdateDisplayedGdbMsg


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedGdbXY(void)
//  Places GDB infil message index info on GDB screen row 7 col 72-101.
//
//  NOTE - call this AFTER calling UpdateDisplayedGdbMsg() which sets dsplGdbMsg
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedGdbXY(void)
{
  if (updtFld.gdbXY)
  {
    char gdbXY[6] = "     ";
    if (0 < numGdbMsgs)
    {
      uint16_t curMsg;
      curMsg = (dsplGdbMsg >= tlGdbMsgQ)
             ? (dsplGdbMsg - tlGdbMsgQ + 1)
             : (GDB_MSG_Q_LEN - tlGdbMsgQ + dsplGdbMsg + 1);
      sprintf(gdbXY, "%02u/%02u", curMsg, numGdbMsgs);
    }
    LCDWriteStringTerminal6X8(7, (12*WIDTH_6X8), gdbXY , false);
    updtFld.gdbXY = 0;                  // Field updated so clear flag
  }
} // end routine UpdateDisplayedGdbXY


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateGdbInfilDisplay(void)
//  Calls plethora of other routines to build & display the GDB INFIL screen
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void UpdateGdbInfilDisplay(void)
{
  if (updtFld.chgScr)                 {   // If just coming to GDB INFIL screen
    updtFld.val |= GDB_DSPL_FLDS;     }   // update all screen fields
  UpdateDisplayedGdbMsg();                // rows 0-6,  columns  3-99
  UpdateDisplayedMailIcon();              // row    7,  columns 33-60
  UpdateDisplayedGdbXY();                 // row    7,  columns 72-101
} // end routine UpdateGdbInfilDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateRngBrgDisplay(void)
//  Calls plethora of other routines to build & display the GDB INFIL screen
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void UpdateRngBrgDisplay(void)
{
  if (updtFld.chgScr)
  {                                     // If just coming to RNG & BRG screen
    if (0 <= nmuPliIdx)         {       // and when newest message unread
      dsplPliIdx = nmuPliIdx;   }       // be sure to display that message
    LCDWriteStringTerminal6X8(0, 6, "RANGE & BEARING", false);
    updtFld.rngBrg = 1;
  }
  if (updtFld.rngBrg)
  {
    if (0 > hiPliIdx)                   // Negative indicates no R&B data rcvd
    {
      //LCDClearScreen();
      char * str = (sysStat.lowPwrMode)
                 ? "INFIL RCVR OFF  "
                 : "No R&B data rcvd";
      LCDWriteStringTerminal6X8(3, 3, str, false);
    }
    else if ( ! sysStat.gpsTime)        // No GPS time indicates no GPS fix
    {                                   // and need a fix to calculate R&B
      //LCDClearScreen();
      LCDWriteStringTerminal6X8(3, 0, "R&B data received", false);
      LCDWriteStringTerminal6X8(4, 9, "but LBHH needs", false);
      LCDWriteStringTerminal6X8(5, 3, "to get a GPS fix", false);
    }
    else
    {
      if (nmuPliIdx == dsplPliIdx)    { // When will display newest unread
        nmuPliIdx = -1;               } // message, set this to indicate such
                                        // When newest message is/will be unread
      bool rvTime = (0 <= nmuPliIdx);   // reverse video the 'time' field.
      char rng[4] = {0,0,0,0};          // 3-char range + NULL terminator
      char brg[4] = {0,0,0,0};          // 3-char bearing + NULL terminator
      char tmp[2] = {0,0};              // 1-char temporary + NULL terminator
      bool kmRng  = CalcRngBrg(myLoc.dblLat, myLoc.dblLon,
                               pliDatQ[dsplPliIdx].lat, pliDatQ[dsplPliIdx].lon,
                               rng, brg);

      LCDClearScreen();
      *tmp = pliDatQ[dsplPliIdx].cId[0];
      LCDWriteStringTerminal6X8  (0, 0,tmp,false);
      *tmp = pliDatQ[dsplPliIdx].cId[1];
      LCDWriteStringTerminal6X8  (1, 0,tmp,false);
      LCDWriteStringTerminal12X16(0, 6,&pliDatQ[dsplPliIdx].cId[2],false);
      LCDWriteStringTerminal6X8  (0,60,"B",false);
      LCDWriteStringTerminal6X8  (1,60,"C",false);
      LCDWriteStringTerminal12X16(0,66,pliDatQ[dsplPliIdx].brev,false);
      LCDWriteStringTerminal12X16(3, 3,pliDatQ[dsplPliIdx].time,rvTime);
      #define GOS '*' // Grain Of Salt
      *tmp = ((CRIT_AOF <= pliDatQ[dsplPliIdx].age) ||  // When PLI is aged or
              (pliDatQ[dsplPliIdx].xof)             ||  // PLI was questionable,
              (CRIT_AOF <= aof))                        // or our fix aged: take
           ? GOS                                        // R&B w/ grain of salt.
           : ' ';                                       // Otherwise no flag
      LCDWriteStringTerminal12X16(6, 0,tmp,(GOS == *tmp));
      LCDWriteStringTerminal12X16(6,12,rng,false);
      *tmp = (kmRng) ? 'k' : ' ';
      LCDWriteStringTerminal6X8  (6,48,tmp,false);
      *tmp = 'm';
      LCDWriteStringTerminal6X8  (7,48,tmp,false);
      LCDWriteStringTerminal12X16(6,60,brg,false);
      LCDWriteStringTerminal6X8  (6,96,"o",false);
    }
    updtFld.rngBrg = 0;                 // Field updated so clear flag
  }
} // end routine UpdateRngBrgDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateBitResultsDisplay(void)
//  Display short strings that indicate Built-In-Test results (any errors in
//  prioritized order, but max of six).
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateBitResultsDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(0, 3+2*WIDTH_6X8, "SYSTEM CHECK", false);
    updtFld.bitRslt = 1;
  }
  if (updtFld.bitRslt)
  {
    uint16_t row;

    for (row = 1; row <= LCD_MAX_PAGES; row++)
    {
      LCDWriteStringTerminal6X8(row, 0, "                 ", false);
    }

    if (esdErrFlags.val || ltngBitRpt.val)
    {
      uint16_t i;
      LCDWriteStringTerminal6X8(1, WIDTH_6X8, "Failures Found:", false);
      for (i = 29, row = 2; i && row <= LCD_MAX_PAGES; i--)
      {
        switch (i)
        {
          case 29:
            if (ltngBitRpt.rtcc)
            {
              LCDWriteStringTerminal6X8(row, 0, "DEPOT MAINT REQ !", false); //This should maybe have a better explaination of why the maint is required. ie.e Keepercell battery low, etc.
              row++;
            }
            break;
          case 28:
            if (esdErrFlags.uKey)
            {
              LCDWriteStringTerminal6X8(row, 0, "INFIL NOT KEYED !", false);
              row++;
            }
            break;
          case 27:
            if (ltngBitRpt.vbat)
            {
              LCDWriteStringTerminal6X8(row, 0, "battery volts low", false);
              row++;
            }
            break;
          case 26:
            if (ltngBitRpt.vmain)
            {
              LCDWriteStringTerminal6X8(row, 0, "main regulator   ", false);
              row++;
            }
            break;
          case 25:
            if (ltngBitRpt.vboost)
            {
              LCDWriteStringTerminal6X8(row, 0, "boost regulator  ", false);
              row++;
            }
            break;
          case 24:
            if (ltngBitRpt.uc)
            {
              LCDWriteStringTerminal6X8(row, 0, "in/exfil ctrlr   ", false);
              row++;
            }
            break;
          case 23:
            if (esdErrFlags.uc)
            {
              LCDWriteStringTerminal6X8(row, 0, "usr intfc ctrlr  ", false);
              row++;
            }
            break;
          case 22:
            if (esdErrFlags.tmr3)
            {
              LCDWriteStringTerminal6X8(row, 0, "keypad-scan timer", false);
              row++;
            }
            break;
          case 21:
            if (esdErrFlags.tmr2)
            {
              LCDWriteStringTerminal6X8(row, 0, "1-sec sys timer  ", false);
              row++;
            }
            break;
          case 20:
            if (ltngBitRpt.bolt)
            {
              LCDWriteStringTerminal6X8(row, 0, "exfil transmitter", false);
              row++;
            }
            break;
          case 19:
            if (ltngBitRpt.gdb)
            {
              LCDWriteStringTerminal6X8(row, 0, "infil receiver   ", false);
              row++;
            }
            break;
          case 18:
            if (ltngBitRpt.ltsnsr)
            {
              LCDWriteStringTerminal6X8(row, 0, "tamper sensor    ", false);
              row++;
            }
            break;
          case 17:
            if (ltngBitRpt.fw)
            {
              LCDWriteStringTerminal6X8(row, 0, "in/exfil firmware", false);
              row++;
            }
            break;
          case 16:
            if (esdErrFlags.excp)
            {
              LCDWriteStringTerminal6X8(row, 0, "usr intfc trap  ", false);
              row++;
            }
            break;
          case 15:
            if (esdErrFlags.sm)
            {
              LCDWriteStringTerminal6X8(row, 0, "usr state machine", false);
              row++;
            }
            break;
          case 14:
            if (ltngBitRpt.esd)
            {
              LCDWriteStringTerminal6X8(row, 0, "usr intfc data   ", false);
              row++;
            }
            break;
          case 13:
            if (ltngBitRpt.comx)
            {
              LCDWriteStringTerminal6X8(row, 0, "ext com port data", false);
              row++;
            }
            break;
          case 12:
            if (ltngBitRpt.i2c)
            {
              LCDWriteStringTerminal6X8(row, 0, "in/exfil I2C bus ", false);
              row++;
            }
            break;
          case 11:
            if (esdErrFlags.i2c)
            {
              LCDWriteStringTerminal6X8(row, 0, "usr intfc I2C bus", false);
              row++;
            }
            break;
          case 10:
            if (esdErrFlags.nvmem)
            {
              LCDWriteStringTerminal6X8(row, 0, "usr intfc cfg mem", false);
              row++;
            }
            break;
          case 9:
            if (ltngBitRpt.nvmem)
            {
              LCDWriteStringTerminal6X8(row, 0, "in/exfil cfg mem ", false);
              row++;
            }
            break;
          case 8:
            if (esdErrFlags.gasGa)
            {
              LCDWriteStringTerminal6X8(row, 0, "gas gauge        ", false);
              row++;
            }
            break;
          case 7:
            if (ltngBitRpt.adc)
            {
              LCDWriteStringTerminal6X8(row, 0, "voltage check ADC", false);
              row++;
            }
            break;
          case 6:
            if (esdErrFlags.ths)
            {
              LCDWriteStringTerminal6X8(row, 0, "humidity sensor  ", false);
              row++;
            }
            break;
          case 5:
            if (ltngBitRpt.brd)
            {
              LCDWriteStringTerminal6X8(row, 0, "exfil module data", false);
              row++;
            }
            break;
          case 4:
            if (ltngBitRpt.ota)
            {
              LCDWriteStringTerminal6X8(row, 0, "infil module data", false);
              row++;
            }
            break;
          case 3:
            if (esdErrFlags.spi)
            {
              LCDWriteStringTerminal6X8(row, 0, "usr display bus  ", false);
              row++;
            }
            break;
          case 2:
            if (esdErrFlags.ltng)
            {
              LCDWriteStringTerminal6X8(row, 0, "in/exfil ctrl dat", false);
              row++;
            }
            break;
          case 1:
            if (esdErrFlags.fw)
            {
              LCDWriteStringTerminal6X8(row, 0, "usr intfc code   ", false);
              row++;
            }
            break;
          default: break;
        } // end switch (i)
      } // end for (i && row)
    }
    else
    {
      LCDWriteStringTerminal6X8(2, WIDTH_6X8, "EXFIL......PASS", false);
      LCDWriteStringTerminal6X8(3, WIDTH_6X8, "INFIL......PASS", false);
      LCDWriteStringTerminal6X8(4, WIDTH_6X8, "SYSHW......PASS", false);
      LCDWriteStringTerminal6X8(5, WIDTH_6X8, "SYSFW......PASS", false);
    }
    updtFld.bitRslt = 0;
  }
} // end routine UpdateBitResultsDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateWfNameDisplay(void)
//  Display exfil waveform name and wf unique transmitter ID for this device.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateWfNameDisplay(void)
{
  #define SEL_NAME (char*)selWfTrait.name
  #define SEL_TXID (char*)selWfTrait.txId
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(1, 9, "EXFIL WAVEFORM", false);
    updtFld.wfInfo = 1;
  }
  if (updtFld.wfInfo)
  {
    LCDWriteStringTerminal6X8(3, 5*WIDTH_6X8, SEL_NAME, false);
    LCDWriteStringTerminal6X8(5, 5*WIDTH_6X8, SEL_TXID, false);

    // Display the Fset value under the TX ID
    char fsetStr[10];
    sprintf(fsetStr, "Fset: %d", devCfg.fset); // Assuming devCfg.fset holds the Fset value
    LCDWriteStringTerminal6X8(7, 5*WIDTH_6X8, fsetStr, false);

    updtFld.wfInfo = 0;
  }

  // Check if the waveform is Lynx 6 or Lynx 8 and display Fset if true
  if (strcmp(SEL_NAME, "Lynx-6") == 0 || strcmp(SEL_NAME, "Lynx-8") == 0)
  {
    UpdateFsetDisplay();
  }
} // end routine UpdateWfNameDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateKeyNamesDisplay(void)
//  Display active infil key names; reverse video name if key not activated.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateKeyNamesDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(1, WIDTH_6X8, "INFIL KEY NAMES", false);
    updtFld.keyName = 1;
  }
  if (updtFld.keyName)
  {
    bool notKeyed;

    notKeyed = isdigit(uKeyName[0]);
    LCDWriteStringTerminal6X8(3, WIDTH_6X8, "UNIQUE - ", notKeyed);
    if (notKeyed)                                                     {
      LCDWriteStringTerminal6X8(3, 10*WIDTH_6X8, "No Key", true);     }
    else                                                              {
      LCDWriteStringTerminal6X8(3, 10*WIDTH_6X8, uKeyName, false);    }

    notKeyed = isdigit(gKeyName[0]);
    LCDWriteStringTerminal6X8(4, WIDTH_6X8, "GROUP  - ", notKeyed);
    if (notKeyed)                                                     {
      LCDWriteStringTerminal6X8(4, 10*WIDTH_6X8, "No Key", true);     }
    else                                                              {
      LCDWriteStringTerminal6X8(4, 10*WIDTH_6X8, gKeyName, false);    }

    updtFld.keyName = 0;
  }
} // end routine UpdateKeyNamesDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedBrevHundredsDgt(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedBrevHundredsDgt(void)
{
  if (updtFld.chgScr)
  {
    *brevCodeH = devCfg.brevCode[0];  // isolate hundreds place
  }
  if (updtFld.brevH)
  {
    LCDWriteStringVerdana34X56(1, 0, brevCodeH, (FP_BREV_H == focusPoint));
    updtFld.brevH = 0;                  // Field updated so clear flag
  }
} // end routine UpdateDisplayedBrevHundredsDgt


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedBrevTensDgt(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedBrevTensDgt(void)
{
  if (updtFld.chgScr)
  {
    *brevCodeT = devCfg.brevCode[1];  // isolate tens place
  }
  if (updtFld.brevT)
  {
    LCDWriteStringVerdana34X56(1, 34, brevCodeT, (FP_BREV_T == focusPoint));
    updtFld.brevT = 0;                  // Field updated so clear flag
  }
} // end routine UpdateDisplayedBrevTensDgt


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplayedBrevOnesDgt(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplayedBrevOnesDgt(void)
{
  if (updtFld.chgScr)
  {
    *brevCodeO = devCfg.brevCode[2];  // isolate ones place
  }
  if (updtFld.brevO)
  {
    LCDWriteStringVerdana34X56(1, 68, brevCodeO, (FP_BREV_O == focusPoint));
    updtFld.brevO = 0;                  // Field updated so clear flag
  }
} // end routine UpdateDisplayedBrevOnesDgt


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateBrevityCodeDisplay(void)
//  Updates entire screen by calling updater for each individual digit
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateBrevityCodeDisplay(void)
{
  if (updtFld.chgScr)
  {
    updtFld.val |= BREV_DSPL_FLDS;
    LCDWriteStringTerminal6X8(0, 3+2*WIDTH_6X8, "Brevity Code", false);
  }
  if ((WGM_ACTVD == selWfTrait.wgmOpt) &&
      (updtFld.brevO || updtFld.brevT || updtFld.brevH))
  { // Clear any vestigial brevity code then tell user that brevity disabled
    LCDWriteStringVerdana34X56 (1, 0, "   ", false);
    LCDWriteStringTerminal12X16(2, 3, "DISABLED", false);
    LCDWriteStringTerminal6X8  (4,30, "because", false);
    LCDWriteStringTerminal6X8  (5, 9, "Exfil Waveform", false);
    LCDWriteStringTerminal6X8  (6,24, "Geo-Muted", false);
    updtFld.val &= ~BREV_DSPL_FLDS;
  }
  else
  { // Displaying brevity will clear any vestigial message
    UpdateDisplayedBrevHundredsDgt();
    UpdateDisplayedBrevTensDgt();
    UpdateDisplayedBrevOnesDgt();
  }
} // end routine UpdateBrevityCodeDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateSetCoordSysDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateSetCoordSysDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(0, 0, "Coordinate System", false);
    if (CS_MGRS == devCfg.cSysSet)      {
      focusPoint = FP_CS_MGRS;          }
    else if (CS_DEC == devCfg.cSysSet)  {
      focusPoint = FP_CS_DEC;           }
    else                                {
      focusPoint = FP_CS_DMS;           }
    updtFld.cSys = 1;
  }
  if (updtFld.cSys)
  {
    LCDWriteStringTerminal12X16(2, 9, "DMS", (FP_CS_DMS  == focusPoint));
    LCDWriteStringTerminal12X16(2,57, "DEC", (FP_CS_DEC  == focusPoint));
    LCDWriteStringTerminal12X16(5,27, "MGRS",(FP_CS_MGRS == focusPoint));
    updtFld.cSys = 0;
  }
} // end routine UpdateSetCoordSysDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateSetBkltDisplay(void)
//  NOTE - do not change back-light setting until ENT key is pressed and go HOME
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateSetBkltDisplay(void)
{
  #define BL_LF_COL   (3)
  #define BL_RT_COL   (3+5*WIDTH_12X16)
  #define BL_H_COL    BL_LF_COL
  #define BL_L_COL    BL_RT_COL
  #define BL_N_COL    BL_LF_COL
  #define BL_O_COL    BL_RT_COL
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(0, 4*WIDTH_6X8, "Backlight", false);
    switch (devCfg.bkltSet)
    {
      case BS_HIGH:   focusPoint = FP_BL_H;  break;
      case BS_LOW :   focusPoint = FP_BL_L;  break;
      case BS_NVG :   focusPoint = FP_BL_N;  break;
      default:        focusPoint = FP_BL_O;
    }
    updtFld.bkLt = 1;
  }
  if (updtFld.bkLt)
  {
    LCDWriteStringTerminal12X16(2, BL_H_COL, "High", (FP_BL_H == focusPoint));
    LCDWriteStringTerminal12X16(2, BL_L_COL, "Low",  (FP_BL_L == focusPoint));
    LCDWriteStringTerminal12X16(5, BL_N_COL, "NVG",  (FP_BL_N == focusPoint));
    LCDWriteStringTerminal12X16(5, BL_O_COL, "Off",  (FP_BL_O == focusPoint));
    updtFld.bkLt = 0;
  }
} // end routine UpdateSetBkltDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateTxDtyCyDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateTxDtyCyDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(0, WIDTH_6X8, "# XMT / Minutes ", false);
    switch (selWfTrait.wdcOpt)
    {
      case WDC_4_8_32:
      case WDC_4_8_40:
        switch (devCfg.txDtyCy)
        {
          case TDC_SLOW:  focusPoint = FP_TDC_S;  break;
          case TDC_NORM:  focusPoint = FP_TDC_N;  break;
          case TDC_HIGH:  focusPoint = FP_TDC_H;  break;
          default:        focusPoint = FP_TDC_M;
        }
        break;
      case WDC_4:
        switch (devCfg.txDtyCy)
        {
          case TDC_HIGH:  focusPoint = FP_TDC_H;  break;
          default:        focusPoint = FP_TDC_M;
        }
        break;
      default:
        break;
    }
    updtFld.txDtyCy = 1;
  }
  if (updtFld.txDtyCy)
  {
    if (WGM_ACTVD == selWfTrait.wgmOpt)
    { // Clear any vestigial options then tell user duty cycle disabled
      LCDWriteStringVerdana34X56(1, 0, "   ", false);
      LCDWriteStringTerminal12X16(2, 3, "DISABLED", false);
      LCDWriteStringTerminal6X8(4,30, "because", false);
      LCDWriteStringTerminal6X8(5, 9, "Exfil Waveform", false);
      LCDWriteStringTerminal6X8(6,24, "Geo-Muted", false);
      focusPoint = FP_TDC_M;          // protect sysStat.lowPowerMode & geoMute
    }
    else
    { // Clear any vestigial message then populate duty cycle options
      LCDWriteStringVerdana34X56(1, 0, "   ", false);
      switch (selWfTrait.wdcOpt)
      {
        case WDC_4_8_32:
          LCDWriteStringTerminal12X16(2, 0,  "Mute", (FP_TDC_M == focusPoint));
          LCDWriteStringTerminal12X16(2, 54, "4/32", (FP_TDC_S == focusPoint));
          LCDWriteStringTerminal12X16(5, 6,  "4/8",  (FP_TDC_N == focusPoint));
          LCDWriteStringTerminal12X16(5, 60, "4/4",  (FP_TDC_H == focusPoint));
          break;
        case WDC_4_8_40:
          LCDWriteStringTerminal12X16(2, 0,  "Mute", (FP_TDC_M == focusPoint));
          LCDWriteStringTerminal12X16(2, 54, "1/10", (FP_TDC_S == focusPoint));
          LCDWriteStringTerminal12X16(5, 6,  "1/2",  (FP_TDC_N == focusPoint));
          LCDWriteStringTerminal12X16(5, 60, "1/1",  (FP_TDC_H == focusPoint));
          break;
        case WDC_4:
          LCDWriteStringTerminal12X16(2, 0,  "Mute", (FP_TDC_M == focusPoint));
          LCDWriteStringTerminal12X16(2, 60, "4/4",  (FP_TDC_H == focusPoint));
          break;
        default:
          esdErrFlags.fw = 1;
          break;
      }
    }
    updtFld.txDtyCy = 0;
  }
} // end routine UpdateTxDtyCyDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateTxPwrDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateTxPwrDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(0, 4*WIDTH_6X8, "XMT Power", false);
    switch (selWfTrait.wtpOpt)
    {
      case WTP_A:
        switch (devCfg.txPwr)
        {
          case TP_LOW :   focusPoint = FP_XP_m;  break;   // MIN
          case TP_MED :   focusPoint = FP_XP_L;  break;   // LOW
          case TP_FULL:   focusPoint = FP_XP_X;  break;   // MAX
          case TP_HIGH:   // our default, do NOT break;   // NORM
          default:        focusPoint = FP_XP_N;
        }
        break;
      case WTP_N:
        focusPoint = FP_XP_N;
        break;
      default:
        break;
    }
    updtFld.txPwr = 1;
  }
  if (updtFld.txPwr)
  {
    switch (selWfTrait.wtpOpt)
    {
      case WTP_A:
        LCDWriteStringTerminal12X16(2, 10, "Min",  (FP_XP_m == focusPoint));
        LCDWriteStringTerminal12X16(2, 62, "Low",  (FP_XP_L == focusPoint));
        LCDWriteStringTerminal12X16(5,  4, "Norm", (FP_XP_N == focusPoint));
        LCDWriteStringTerminal12X16(5, 62, "Max",  (FP_XP_X == focusPoint));
        break;
      case WTP_N:
        LCDWriteStringTerminal12X16(3, 24, "Norm", (FP_XP_N == focusPoint));
        break;
      default:
        esdErrFlags.fw = 1;
        break;
    }
    updtFld.txPwr = 0;
  }
} // end routine UpdateTxPwrDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateLowPwrModeDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateLowPwrModeDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal12X16(0, 0, "L",    false);
    LCDWriteStringTerminal6X8  (1,12, "OW",   false);
    LCDWriteStringTerminal12X16(0,30, "P",    false);
    LCDWriteStringTerminal6X8  (1,42, "OWER", false);
    LCDWriteStringTerminal12X16(0,72, "M",    false);
    LCDWriteStringTerminal6X8  (1,84, "ODE",  false);
    focusPoint = (sysStat.lowPwrMode) ? FP_LPM_Y : FP_LPM_N;
    updtFld.lpmY_N = 1;
  }
  if (updtFld.lpmY_N)
  {
    bool rvYes = (FP_LPM_Y == focusPoint);
    LCDWriteStringTerminal6X8  (3, 24, "Yes", rvYes);
    LCDWriteStringTerminal12X16(3, 66, "NO", !rvYes);
    if (rvYes)
    {
      LCDWriteStringTerminal6X8(5, 6, "INFIL RCVR OFF  ", false);
      LCDWriteStringTerminal6X8(7, 6, "GPS MINIMIZED",    false);
      LCDWriteStringTerminal6X8(6, 6, "EXFIL MUTED",      false);
    }
    else
    {
      LCDWriteStringTerminal6X8(5, 0, "                 ", false);
      LCDWriteStringTerminal6X8(6, 0, "                 ", false);
      LCDWriteStringTerminal6X8(7, 0, "                 ", false);
    }
    updtFld.lpmY_N = 0;
  }
} // end routine UpdateLowPwrModeDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDelInfilRbDisplay(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDelInfilRbDisplay(void)
{
  if (updtFld.chgScr)
  {
    LCDWriteStringTerminal6X8(2,21, "Delete all", false);
    LCDWriteStringTerminal6X8(3, 9, "INFIL and R&B?", false);
    updtFld.dirY_N = 1;
  }
  if (updtFld.dirY_N)
  {
    bool invertYes;
    scrnCtdn = DIR_N_Y_CTDN;
    invertYes = (FP_DIR_Y == focusPoint);
    LCDWriteStringTerminal6X8(5, 24, "Yes", invertYes);
    LCDWriteStringTerminal6X8(5, 66, "NO", !invertYes);
    updtFld.dirY_N = 0;
  }
} // end routine UpdateDelInfilRbDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessTimeEvents(void)
//  Process timed events when there has been a 1-sec time time
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void ProcessTimeEvents(void)
{
  if ( ! ct1SecTick)            {       // Nothing to do if no time ticks
     return;                    }       // so just return immediately
                                        // Otherwise start 1-sec periodic tasks
  mGlobalIntDisable();                  // protect ct1SecTick from IRQ while
  sysSec += ct1SecTick;                 // update system seconds by count
  ct1SecTick = 0;                       // before clearing count ASAP
  mGlobalIntEnable();                   // Let 1-sec tick count update per need

  {                                     // Complete clock update started by IRQ
    while (sysSec >= 60)                // unlikely but IRQ may multi-ticked
    {                                   // When seconds are super-max
      ++sysMin;                         // increment minutes and
      sysSec -= 60;                     // rollover seconds then
    }
    while (sysMin >= 60)
    {                                   // when minutes super-max
      ++sysHr;                          // increment hours and
      sysMin -= 60;                     // rollover minutes
    }
    while (sysHr >= 24)
    {                                   // when hours super-max
      sysHr -= 24;                      // rollover to next day
    }
    updtFld.time = 1;                   // Note HOME screen clock needs update
  } // end system time update

  {                                     // Just single tick other cumulative age
    int i = PLI_DAT_Q_LEN;
    plidat_t * p = pliDatQ;
    do
    {                                   // For RANGE & BEARING info
      i--;                              // each second need to
      p->age++;                         // increment age of PLI data
      if ( ! p->age)  {                 // but don't let it roll-over
        p->age--;     }                 // to zero and restart count.
      p++;                              // When certain age is reached
    } while (0 < i);                    // the R&B screen warns 'old'

    if (sysStat.aofValid)
    {                                   // When AoF valid update GPS Age Of Fix
      ++aof;                            // by incrementing it
      if (0 == aof)
      {                                 // When increment wrapped uint to zero
        aof--;                          // return it to its max uint value
        sysStat.aofValid = false;       // and mark AoF as no-longer valid
      }
      updtFld.aof = 1;                  // Note HOME screen AoF needs update
    }
  } // end things with less critical cumulative age

  if (DS_CTDN == dsplStatus)
  {                                   // When counting down to GPS, XMT, etc.
    if (0 != nbeSec || 0 != nbeMin || 0 != nbeHr)
    {                                 // when have some NBE time to decrement
      if (0 != nbeSec)
      {                               // decrement seconds to NBE
        --nbeSec;
      }
      else
      {                               // when no seconds to decrement
        if (0 != nbeMin)              // try borrow from minutes place
        {
          --nbeMin;
          nbeSec = 59;
        }
        else                          // If here, we know (nbeHr != 0)
        {                             // When no minutes to borrow
          --nbeHr;                    // borrow from hours place
          nbeMin = 59;
          nbeSec = 59;
        }
      }
      updtFld.ops = 1;
    }
  } // end countdown to next beacon event in operational status field

  if (CTDN_OFF != scrnCtdn)
  {                                     // When screen countdown active
    scrnCtdn--;                         // decrement the countdown
    if (0 == scrnCtdn)
    {                                   // When countdown timed-out always
      updtFld.chgScr = 1;               // change screen and take action
      switch (focusPoint)               // based on user interface focus point
      {
        case FP_SYSCHK:                 // System Check testing or ENT timeout
          if (updtFld.sysChk)         { // When SYSTEM CHECK testing hung
            updtFld.chgScr = 0;       } // don't change screen, let it hang
          else if (IS_ON_BAT_PWR)     { // When check done and on battery power
            focusPoint = FP_NEWBAT_N; } // ask user if new battery (default No)
          else                        { // otherwise on external power so
            focusPoint = FP_PWR_1;    } // go to Power On/Off (default On)
          break;

        case FP_NEWBAT_N:               // New Battery query timeout waiting
        case FP_NEWBAT_Y:               // for ENT to Y or N selection
          newBatSelMade = false;        // Note no answer entered
          focusPoint    = FP_NA_SLP;    // and go to SLEEP
          break;

        case FP_PWR_0:                  // Power just applied, timeout waiting
        case FP_PWR_1:                  // for ENT to power ON/OFF query
          focusPoint  = FP_NA_SLP;      // so go to SLEEP
          break;

        case FP_EXF_CTDN:               // Finished delay for BOLT to reset
          if (WGM_NEVER < devCfg.geoMuting) { // When geo-muting applies to wf
            focusPoint = FP_OVRD_N;         } // go to OVERRIDE GEOMUTE screen
          else                              { // but when geo-muting n/a to wf
            focusPoint = FP_ADM_WARN;       } // display ADMIN WARNING
          break;

        case FP_HPWR_0:                 // Home screen PWR ON/OFF ENT timed-out
        case FP_HPWR_1:                 // waiting for ENT to ON/OFF query
        case FP_ADM_N:                  // -or- Verify Administrator timed-out
        case FP_ADM_Y:                  // waiting for ENT to Y or N selection
        case FP_DIR_N:                  // -or- Delete INFIL and R&B timed-out
        case FP_DIR_Y:                  // waiting for ENT to Y or N selection
        case FP_ZERO_N:                 // -or- Verify Zeroize Unit timed-out
        case FP_ZERO_Y:                 // waiting for ENT to Y or N selection
          focusPoint  = FP_HOME;        // ...so go to HOME screen
          break;

        case FP_ZERO_A:                 // Zeroize work should be finished
          __asm__ volatile ("reset");   // perform software reset of PIC uC
          break;

        default:
          esdErrFlags.fw = 1;
      }
    }
  } // end screen countdown

  if (IS_ON_BAT_PWR != wasOnBtry)
  { // every second check if power source has been switched
    uint16_t sanity = 3;

    if (wasOnBtry)
    { // when switch to external power from battery, get & remember ACR value
      gasChkCtdn = CTDN_OFF;
      while (sanity && !GetLtc2943Charge(&acr.HB,&acr.LB))  { sanity--; }
      WriteGasGaugeToNvMem(acr.val, btryPct);
    }
    else  // when switch to battery power from external power, must correct ACR
    {     // because ACR ticks even when on external power (poor HW design)
      while (sanity && !SetLtc2943Charge(acr.HB, acr.LB))   { sanity--; }
      gasChkCtdn = GAS_CHK_CTDN;
    }
    wasOnBtry   = !wasOnBtry;           // now must set 'was' to its other state
    updtFld.pwr = 1;                    // and note displayed power needs update
  }
  // NOTE - it is possible for ACR to roll over from ZERO_PT to FULL_PT, but
  // this is not a practical event running on actual batteries.
  if (IS_ON_BAT_PWR)
  {
    gasChkCtdn--;
    if ( ! gasChkCtdn)
    { // When on battery power and it is time to check gas gauge, do so
      uint16_t oldBtryPct;
      uint16_t sanity = 3;

      gasChkCtdn = GAS_CHK_CTDN;
      oldBtryPct = btryPct;

      while (sanity && !GetLtc2943Charge(&acr.HB,&acr.LB))  { sanity--; }

      // Calculate battery percentage

      // Commented out section below represents code that used to work prior to
      // the LTC2943 sense leads being reversed on latest production hardware.
      // This code should be restored when the sense leads are corrected.
      //if (acr.val <= LTC2943_ZERO_PT)
      //{
      //  btryPct = 0;
      //}
      //else
      //{
      //  uint32_t pctCalc = acr.val;
      //  pctCalc -= (uint32_t) LTC2943_ZERO_PT;
      //  pctCalc *= 100UL;
      //  pctCalc /= (uint32_t) LTC2943_BATTERY_RANGE;
      //  btryPct = (uint16_t) pctCalc;
      //}

      if (LTC2943_ZERO_PT <= acr.val)
      {
        btryPct = 0;
      }
      else
      {
        uint32_t pctCalc = (uint32_t)LTC2943_ZERO_PT;
        pctCalc -= acr.val;
        pctCalc *= 100UL;
        pctCalc /= (uint32_t)LTC2943_BATTERY_RANGE;
        btryPct  = (uint16_t)pctCalc;
      }

      if (sanity && oldBtryPct != btryPct)
      { // When % has changed save values and update display field
        if (oldBtryPct < btryPct)  { esdErrFlags.gasGa = 1; }
        WriteGasGaugeToNvMem(acr.val, btryPct);
        updtFld.pwr = 1;
      }
    }
  } // end of countdown to check gas gauge

  if (CRIT_AOF <= aof)
  { // Once age of fix is critical flash coordinates each second
    updtFld.coord = 1;
  }

  if (BTRY_CRIT_LVL > btryPct)
  { // Once btryPct is critical flash battery icon each second
    updtFld.pwr = 1;
  }

} // end routine ProcessTimeEvents


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void Invoke911(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void Invoke911(void)
{
  if (NVLD_TXID == *selWfTrait.txId)
  {                                       // When invalid waveform selected
    focusPoint  = FP_ADM_N;               // just go up to ADMIN screens
  }
  else
  {                                       // When valid waveform selected
    if (sysStat.lowPwrMode)
    {
      char mode = TDC_HIGH;               // Let invoking 911
      sysStat.lowPwrMode = 0;             // pop us out of Low Power Mode; queue
      QueueLtngCmd(CID_OLPM, &mode);      // exit Low Power Mode ops command.
    }
    QueueLtngCmd(CID_OTXM, NULL);         // queue Lightning 911 cmd
    sysStat.in911Mode = 1;                // Set flag for no other brev changes
    if (TDC_MUTE == devCfg.txDtyCy)     { // When in MUTE mode should indicate
      muteSquawkCtdn = TEMP_SCHED_CTDN; } // brevity code only while squawking
    focusPoint  = FP_HOME;                // and go to HOME (if not there)
  }
  updtFld.chgScr  = 1;                    // Update entire [new] screen
} // end routine Invoke911


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessBrevCodeEnt(void)
//  Check entered brevity.  When valid, queue CBC command to Lightning using new
//  brevity code.  When invalid, change code to valid entry and make user hit
//  ENT again.
//
//  NOTE - because this highlights __all__ digits when entry out of range, all
//    brevity code UP and RT keypad inputs must update all digit fields
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void ProcessBrevCodeEnt(void)
{
  uint16_t  codeVal  = ((*brevCodeH - '0') * 100)
                     + ((*brevCodeT - '0') * 10)
                     +  (*brevCodeO - '0');
  bool      autoCorr = false;

  if ((1 > codeVal) && (WBR_0T14 < selWfTrait.wbrOpt))
  {                                       // When user entry absolutely too low
    autoCorr  = true;                     // auto-correct digits, update fields
    *brevCodeH = '0';   updtFld.brevH = 1;// Note WBR_0T14 allows brevity code 0
    *brevCodeT = '0';   updtFld.brevT = 1;// This code makes correction for
    *brevCodeO = '1';   updtFld.brevO = 1;// waveforms that do NOT allow 000
  }
  else                                    // Need to check to values too large
  {
    switch (selWfTrait.wbrOpt)
    {
    //case WBR_NC:
    //    autoCorr  = true;               // auto-correct digits, update fields
    //    *brevCodeH = '0';   updtFld.brevH = 1;
    //    *brevCodeT = '0';   updtFld.brevT = 1;
    //    *brevCodeO = '0';   updtFld.brevO = 1;
    //  break;
      case WBR_0T14:
        if (14 < codeVal)
        {                                 // When user entry absolutely too high
          autoCorr  = true;               // auto-correct digits, update fields
          *brevCodeH = '0';   updtFld.brevH = 1;
          *brevCodeT = '1';   updtFld.brevT = 1;
          *brevCodeO = '4';   updtFld.brevO = 1;
        }
        break;
      case WBR_1T48_241T253:
        if (253 < codeVal)
        {                                 // When user entry absolutely too high
          autoCorr  = true;               // auto-correct digits, update fields
          *brevCodeH = '2';   updtFld.brevH = 1;
          *brevCodeT = '5';   updtFld.brevT = 1;
          *brevCodeO = '3';   updtFld.brevO = 1;
        }
        else if (241 > codeVal && 64 <= codeVal)
        {                                 // When user entry below upper range
          autoCorr  = true;               // auto-correct digits, update fields
          *brevCodeH = '2';   updtFld.brevH = 1;
          *brevCodeT = '4';   updtFld.brevT = 1;
          *brevCodeO = '1';   updtFld.brevO = 1;
        }
        else if (64 > codeVal && 48 < codeVal)
        {                                 // When user entry above lower range
          autoCorr  = true;               // auto-correct digits, update fields
          *brevCodeH = '0';   updtFld.brevH = 1;
          *brevCodeT = '4';   updtFld.brevT = 1;
          *brevCodeO = '8';   updtFld.brevO = 1;
        }
        break;
      case WBR_1T253:
        if (253 < codeVal)
        {                                 // When user entry absolutely too high
          autoCorr  = true;               // auto-correct digits, update fields
          *brevCodeH = '2';   updtFld.brevH = 1;
          *brevCodeT = '5';   updtFld.brevT = 1;
          *brevCodeO = '3';   updtFld.brevO = 1;
        }
        break;
      default:
        esdErrFlags.fw = 1;
        break;
    }
  }

  if (autoCorr)
  {                                       // When auto-corrected bad brevity
    esdfp_t  realFp   = focusPoint;       // remember current/real focus point
    focusPoint  = FP_BREV_H;              // before temporarily set focusPoint
    UpdateDisplayedBrevHundredsDgt();     // so every digit gets highlighted
    focusPoint  = FP_BREV_T;
    UpdateDisplayedBrevTensDgt();
    focusPoint  = FP_BREV_O;
    UpdateDisplayedBrevOnesDgt();
    focusPoint  = realFp;                 // then set focus back where it was
  }
  else
  {                                       // When user entered code is valid
    devCfg.brevCode[0] = *brevCodeH;      // place new value in config memory
    devCfg.brevCode[1] = *brevCodeT;
    devCfg.brevCode[2] = *brevCodeO;
    WriteCfgToNvMem();                    // and save to non-volatile memory
    if (sysStat.lowPwrMode)
    {
      char mode = TDC_HIGH;               // Let entering a brevity code
      sysStat.lowPwrMode = 0;             // pop us out of Low Power Mode; queue
      QueueLtngCmd(CID_OLPM, &mode);      // exit Low Power Mode ops command.
    }
    QueueLtngCmd(CID_OCBC,devCfg.brevCode); // queue Lightning brevity code cmd
    if (TDC_MUTE == devCfg.txDtyCy)     { // When in MUTE mode should indicate
      muteSquawkCtdn = TEMP_SCHED_CTDN; } // brevity code only while squawking
    updtFld.chgScr  = 1;                  // Start w/ entirely new screen
    focusPoint  = FP_HOME;                // as must return to HOME
  }
} // end routine ProcessBrevCodeEnt


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessSysCheckUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessSysCheckUsrInp(void)
{
  if ( ! updtFld.sysChk)
  {                                     // When done with P-BIT system checks
    if (KEYPAD_SCANCODE_ENT == acptKeypadInput)
    {                                   // ENT is only acceptable input
      scrnCtdn = CTDN_OFF;              // Got it so stop screen countdown
      updtFld.chgScr  = 1;              // Start w/ entirely new screen
      focusPoint  = (IS_ON_BAT_PWR)
                  ? FP_NEWBAT_N
                  : FP_PWR_1;
    }
  }                                     // Do nothing when not done w/ sys check
} // end routine ProcessSysCheckUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessNewBtryUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessNewBtryUsrInp(void)
{
  if (KEYPAD_SCANCODE_ENT == acptKeypadInput)
  {
    uint16_t sanity = 3;

    scrnCtdn = CTDN_OFF;                // ENT stops screen countdown asap
    newBatSelMade = true;               // Note that user made a selection
    if (FP_NEWBAT_Y == focusPoint)
    {                                   // When user says have a new battery
      btryPct = 100;                    // set value for display to 100%,
      acr.val = LTC2943_FULL_PT;        // set columb counter for FULL, and
      WriteGasGaugeToNvMem(LTC2943_FULL_PT, 100);  // remember it
    }                                   // else sysChk already read from NVMEM
    while (sanity && !SetLtc2943Charge(acr.HB, acr.LB))   { sanity--; }
    updtFld.chgScr  = 1;                // Start w/ entirely new POWER screen
    focusPoint      = FP_PWR_1;         // and default selection to ON
  }
  else if (KEYPAD_SCANCODE_RT == acptKeypadInput)
  {
    updtFld.newBat  = 1;
    focusPoint      = (FP_NEWBAT_N == focusPoint)
                    ? FP_NEWBAT_Y
                    : FP_NEWBAT_N;
  }
} // end routine ProcessNewBtryUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessPwrOnOffUsrInp(void)
//  Power ON-OFF screen -- at Power-On-Reset or GetBackToWork from OFF state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessPwrOnOffUsrInp(void)
{
  if (KEYPAD_SCANCODE_ENT == acptKeypadInput)
  {
    scrnCtdn    = CTDN_OFF;             // ENT stops screen countdown
    updtFld.chgScr  = 1;                // Go to either SLEEP or HOME screen
    if (FP_PWR_0 == focusPoint)
    {
      focusPoint = FP_NA_SLP;
    }
    else
    { // queue commands to Lightning since it was just turned on
      if ( ! sysStat.ltngRptWfi)                            { // 1st, if not POR
        QueueLtngCmd(CID_SWFI, NULL);                       } // get all wf info
      QueueLtngCmd(CID_CTXP, (char*)(&devCfg.txPwr));
      if (TDC_MUTE != devCfg.txDtyCy)                       { // Don't let
        QueueLtngCmd(CID_CBDC, (char*)(&devCfg.txDtyCy));   } // DutyCycle
      QueueLtngCmd(CID_OCBC, devCfg.brevCode);                // missquawk
      if (TDC_MUTE == devCfg.txDtyCy)                       { // unless
        QueueLtngCmd(CID_CBDC, (char*)(&devCfg.txDtyCy));   } // muting TX
      focusPoint = FP_HOME;
    }
  }
  else if (KEYPAD_SCANCODE_RT == acptKeypadInput)
  {
    updtFld.pwr01 = 1;
    focusPoint = (FP_PWR_0 == focusPoint)
               ? FP_PWR_1
               : FP_PWR_0;
  }
  else if (KEYPAD_SCANCODE_NONE != acptKeypadInput)
  {                                     // Invalid key input...
    scrnCtdn    = CTDN_OFF;             // stop screen countdown
    focusPoint  = FP_NA_SLP;            // and go to sleep
    updtFld.chgScr  = 1;                // w/ entirely blank screen
  }
} // end routine ProcessPwrOnOffUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessHpwrOnOffUsrInp(void)
//  Power ON-OFF screen -- from HOME only, i.e. when was unit already fully ON
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessHpwrOnOffUsrInp(void)
{
  if (KEYPAD_SCANCODE_ENT == acptKeypadInput)
  {
    scrnCtdn = CTDN_OFF;                // ENT stops screen countdown
    updtFld.chgScr  = 1;                // Go to either SLEEP or HOME screen
    if (FP_HPWR_0 == focusPoint)
    {
      focusPoint = FP_NA_SLP;
    }
    else
    { // do NOT queue commands to Lightning, it was never turned off
      focusPoint = FP_HOME;
    }
  }
  else if (KEYPAD_SCANCODE_RT == acptKeypadInput)
  {
    updtFld.pwr01 = 1;
    focusPoint = (FP_HPWR_0 == focusPoint)
               ? FP_HPWR_1
               : FP_HPWR_0;
  }
  else if (KEYPAD_SCANCODE_NONE != acptKeypadInput)
  {                                     // Invalid key input...
    scrnCtdn    = CTDN_OFF;             // stop screen countdown
    updtFld.chgScr  = 1;                // Update entire [new] screen
    focusPoint  = FP_HOME;              // as cancel and go to HOME
  }
} // end routine ProcessHpwrOnOffUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessZeroizeUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessZeroizeUsrInp(void)
{
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      scrnCtdn    = CTDN_OFF;           // ENT stops screen countdown
      updtFld.chgScr  = 1;              // Go to either ACTIVATED or HOME scrn
      if (FP_ZERO_Y == focusPoint)
      {                                 // When user confirmed ZEROIZE
        StartLbhhZeroize();             // immediately start that process
      }                                 // which stops further keypad inputs
      else
      {
        focusPoint = FP_HOME;           // to HOME screen
      }
      break;
    case KEYPAD_SCANCODE_RT:
      updtFld.zeroize = 1;
      focusPoint = (FP_ZERO_N == focusPoint)
                 ? FP_ZERO_Y
                 : FP_ZERO_N;
      break;
    case KEYPAD_SCANCODE_PWR:           // The PWR key input cancel zeroize
      scrnCtdn    = CTDN_OFF;           // stop screen countdown
      updtFld.chgScr  = 1;              // Update entire [new] screen
      focusPoint  = FP_HOME;            // as cancel and go to HOME
      break;
    default:
      break;
  }
} // end routine ProcessZeroizeUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessConfAdmUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessConfAdmUsrInp(void)
{
  if (KEYPAD_SCANCODE_ENT == acptKeypadInput)
  {
    scrnCtdn    = CTDN_OFF;             // ENT stops screen countdown
    updtFld.chgScr  = 1;                // Update entire [new] screen
    if (FP_ADM_Y == focusPoint)
    {                                   // When claimed to be admin
      QueueLtngCmd(CID_SWFI, NULL);     // query all waveform info and then
      focusPoint  = FP_ADM_WARN;        // go to Admin warning screen
    }
    else
    {                                   // When not claiming admin
      focusPoint  = FP_HOME;            // just return HOME
    }
  }
  else if (KEYPAD_SCANCODE_RT == acptKeypadInput)
  {
    updtFld.admin = 1;
    focusPoint  = (FP_ADM_Y == focusPoint)
                ? FP_ADM_N
                : FP_ADM_Y;
  }
  else if (KEYPAD_SCANCODE_NONE != acptKeypadInput)
  {                                     // Invalid key input...
    scrnCtdn    = CTDN_OFF;             // stop screen countdown
    updtFld.chgScr  = 1;                // Update entire [new] screen
    focusPoint  = FP_HOME;              // as cancel and go to HOME
  }
} // end routine ProcessConfAdmUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessAdmWarnUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessAdmWarnUsrInp(void)
{
  if (KEYPAD_SCANCODE_UP == acptKeypadInput)
  {
    QueueLtngCmd(CID_SGKN, NULL);       // Query names of loaded GDB keys
    updtFld.chgScr = 1;                 // Update entire [new] screen
    focusPoint = FP_EXF;                // as must go to EXFIL setting screen
  }
  else if (KEYPAD_SCANCODE_NONE != acptKeypadInput)
  {                                     // Invalid key input...
    scrnCtdn    = CTDN_OFF;             // stop screen countdown
    updtFld.chgScr  = 1;                // Update entire [new] screen
    focusPoint  = FP_HOME;              // as cancel and go to HOME
  }
} // end routine ProcessAdmWarnUsrInp

//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessFsetOptUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessFsetOptUsrInp(void)
{
  static uint8_t page = 0; // Page index to track which set of options to display
  #define MAX_FSET 15        // Maximum number of FSET options available

  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      // Save the selected Fset option to non-volatile memory
      devCfg.selFset = focusPoint - FP_FSET_0 + page * 6;
      WriteCfgToNvMem();
      updtFld.chgScr = 1;                // Start with an entirely new screen
      focusPoint = FP_HOME;              // Return to HOME screen
      break;
    case KEYPAD_SCANCODE_RT:
      focusPoint++;
      if (focusPoint >= FP_FSET_0 + 6)   // Wrap around within the current page
      {
        focusPoint = FP_FSET_0;
      }
      updtFld.fsetOpt = 1;
      break;
    case KEYPAD_SCANCODE_UP:
      page = (page + 1) % (MAX_FSET / 6); // Move to the next page of options
      updtFld.chgScr = 1;                // Update the entire screen
      break;
    default:
      break;
  }
} // end routine ProcessFsetOptUsrInp

//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessExfilOptUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessExfilOptUsrInp(void)
{
  uint8_t lc, idx;
  wgm_t   wgm;          // Waveform Geo-Mute setting to save in nvmem & wfTrait
                        // Any geo-mute 'used' wf trait will result in WGM_ACTVD
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      switch (focusPoint)
      {
        case FP_EXF_1:
          devCfg.selWfChar = WF_SLOT1;
          devCfg.selWfIdx  = 1;
          wgm = (WGM_NEVER < wfTrait[1].wgmOpt) ? WGM_ACTVD : wfTrait[1].wgmOpt;
          wfTrait[1].wgmOpt = wgm;
          break;
        case FP_EXF_2:
          devCfg.selWfChar = WF_SLOT2;
          devCfg.selWfIdx  = 2;
          wgm = (WGM_NEVER < wfTrait[2].wgmOpt) ? WGM_ACTVD : wfTrait[2].wgmOpt;
          wfTrait[2].wgmOpt = wgm;
          break;
        case FP_EXF_3:
          devCfg.selWfChar = WF_SLOT3;
          devCfg.selWfIdx  = 3;
          wgm = (WGM_NEVER < wfTrait[3].wgmOpt) ? WGM_ACTVD : wfTrait[3].wgmOpt;
          wfTrait[3].wgmOpt = wgm;
          break;
        case FP_EXF_4:
          devCfg.selWfChar = WF_SLOT4;
          devCfg.selWfIdx  = 4;
          wgm = (WGM_NEVER < wfTrait[4].wgmOpt) ? WGM_ACTVD : wfTrait[4].wgmOpt;
          wfTrait[4].wgmOpt = wgm;
          break;
        default:
          wgm = (WGM_NEVER < devCfg.geoMuting) ? WGM_ACTVD : devCfg.geoMuting;
          break;
      }
      devCfg.geoMuting   = wgm;               // NEVER or ACTVD (possibly NA)
      devCfg.txDtyCy     = TDC_MUTE;          // Lightning will mute the BOLT
      devCfg.brevCode[0] = *brevCodeH = '0';  // When change waveform reset
      devCfg.brevCode[1] = *brevCodeT = '0';  // brevity code to universal code
      devCfg.brevCode[2] = *brevCodeO = '1';  // for use now & in configuration
      WriteCfgToNvMem();                  // Save selection to non-volatile mem
      QueueLtngCmd(CID_CSWF, (char*)(&devCfg.selWfChar)); // Cmd waveform change
      updtFld.actvBrev  = 1;              // so indicate that on HOME screen
      updtFld.chgScr    = 1;              // Start w/ entirely new screen
      focusPoint        = FP_EXF_CTDN;    // immediately display countdown scrn
      break;                              // which will auto power-cycle Ltng
    case KEYPAD_SCANCODE_RT:
      for (lc = MAX_WF - '0'; lc; lc--)
      {                                         // Increment fp looking for next
        focusPoint++;                           // waveform with a valid TX ID
        if (FP_EXF_4 < focusPoint)            { // When passed end of ExfilOpt
          focusPoint = FP_EXF_1;              } // wrap back around to the first
        idx = focusPoint - FP_EXF;              // Index wfTrait based off fp
        if (NVLD_TXID != *wfTrait[idx].txId)  { // and when find wf with TX ID
          break;                              } // stop looping, found our fp.
      }
      updtFld.exfOpt = 1;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr  = 1;                // Start w/ entirely new screen
      if (WGM_NEVER < devCfg.geoMuting) { // When geo-muting applies to waveform
        focusPoint = FP_OVRD_N;         } // go to OVERRIDE GEOMUTE screen
      else                              { // but when geo-muting n/a to waveform
        focusPoint = FP_INF;            } // go to INFIL setting screen
      break;
    default: break;
  }
} // end routine ProcessExfilOptUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessGeoMuteOvrdUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessGeoMuteOvrdUsrInp(void)
{
#define NV_GEO_OVRD 1 // 0: forget override when turn off; 1: save to nvmem
                      //***************************************************************** This should probably be set to 0 so that the override is forgotten when the unit is turned off to prevent accidental TX on a GEOMUTED WF *****************************************************************
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      #if (0 == NV_GEO_OVRD)
        selWfTrait.wgmOpt = (FP_OVRD_Y == focusPoint) ? WGM_OVRDN : WGM_ACTVD;
      #elif (1 == NV_GEO_OVRD)
        devCfg.geoMuting = selWfTrait.wgmOpt
                         = (FP_OVRD_Y == focusPoint)
                         ? WGM_OVRDN
                         : WGM_ACTVD;
        WriteCfgToNvMem();
      #else
        #error invalid NV_GEO_OVRD value
      #endif
      devCfg.txDtyCy = TDC_MUTE;        // Regardless of choice just MUTE now
      muteSquawkCtdn = 0;               // and ensure no longer expect to squawk
      QueueLtngCmd(CID_CBDC, (char*)(&devCfg.txDtyCy));
      updtFld.chgScr   = 1;             // Start w/ entirely new screen
      focusPoint       = FP_ADM_WARN;   // as going to ADMIN WARNING screen
      updtFld.actvBrev = 1;             // Update HOME screen BREV when go there
      break;
    case KEYPAD_SCANCODE_RT:
      updtFld.gmOvrd = 1;
      focusPoint = (FP_OVRD_Y == focusPoint) ? FP_OVRD_N : FP_OVRD_Y;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr  = 1;              // Start w/ entirely new screen
      focusPoint      = FP_INF;         // as going to INFIL OPTIONS screen
      break;
    default: break;
  }
} // end routine ProcessGeoMuteOvrdUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessInfilOptUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessInfilOptUsrInp(void)
{
  char gok;

  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      switch (focusPoint)
      {
        case FP_INF_1:  gok = '1';  QueueLtngCmd(CID_CGOK, &gok);   break;
        case FP_INF_2:  gok = '2';  QueueLtngCmd(CID_CGOK, &gok);   break;
        case FP_INF_3:  gok = '3';  QueueLtngCmd(CID_CGOK, &gok);   break;
        case FP_INF_4:  gok = '4';  QueueLtngCmd(CID_CGOK, &gok);   break;
        case FP_INF_5:  gok = '5';  QueueLtngCmd(CID_CGOK, &gok);   break;
        case FP_INF_N:  gok = 'N';  QueueLtngCmd(CID_CGOK, &gok);   break;
        default: break;
      }
      QueueLtngCmd(CID_SGAK, NULL);     // Query new names of active GDB keys
      updtFld.chgScr  = 1;              // Start w/ entirely new screen
      focusPoint  = FP_ADM_WARN;        // as must display warning message
      break;
    case KEYPAD_SCANCODE_RT:
      focusPoint++;
      if (FP_INF_N < focusPoint)  {
        focusPoint = FP_INF_1;    }
      updtFld.infOpt = 1;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr  = 1;              // Update entire [new] screen
      focusPoint = FP_ADM_WARN;         // and go to warning message screen
      break;
    default: break;
  }
} // end routine ProcessInfilOptUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessHomeUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessHomeUsrInp(void)
{
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_PWR:
      updtFld.chgScr = 1;
      focusPoint = FP_HPWR_1;
      break;
    case KEYPAD_SCANCODE_RT:
      updtFld.chgScr = 1;
      focusPoint = FP_GDB;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr = 1;
      if (NVLD_TXID == *selWfTrait.txId)
      {                                 // When invalid waveform selected
          focusPoint  = FP_ADM_N;       // just go up to ADMIN screens
      }                                 // so user can select a valid wf
      else if (sysStat.in911Mode)
      {                                 // When in 911 mode
        focusPoint = FP_CS_DMS;         // skip over BREVITY screen as not
      }                                 // allowed to change brevity code
      else
      {                                 // When no special circumstances
        focusPoint = FP_BREV;           // move user to BREVITY screen, which
      }                                 // is the normal screen flow
      break;
    default: break;
  }
} // end routine ProcessHomeUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessGdbUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessGdbUsrInp(void)
{
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_UP:
      if (numGdbMsgs)                   // Display {next_older | newest} msg
      {                                     // When there are msgs and
        if (dsplGdbMsg)                 {   // not displaying index 0
          dsplGdbMsg--;                 }   // decrement msg index but
        else                            {   // when displaying index 0
          dsplGdbMsg = numGdbMsgs - 1;  }   // wrap around in msg queue
        updtFld.gdb   = 1;              // Update displayed msg text/time and
        updtFld.gdbXY = 1;              // which msg of msgs is displayed
      }
      break;
    case KEYPAD_SCANCODE_RT:
      updtFld.chgScr  = 1;              // Update entire [new] screen
      focusPoint  = FP_RNG_BRG;         // as must go to Range & Bearing
      break;
    default: break;
  }
} // end routine ProcessGdbUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessRngBrgUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessRngBrgUsrInp(void)
{
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_UP:
      if (0 < hiPliIdx)                 // When multi PLI msgs: user wants to
      {                                 // display {next_higher | lowest} data
        if (dsplPliIdx)             {   // When not displaying index 0 decrement
          dsplPliIdx--;             }   // data index to show higher ID but
        else                        {   // when displaying index 0 (highest ID)
          dsplPliIdx = hiPliIdx;    }   // wrap around in PLI data to lowest ID
        updtFld.rngBrg  = 1;            // Update displayed data
      }
      break;
    case KEYPAD_SCANCODE_RT:
      updtFld.chgScr  = 1;              // Update entire [new] screen
      focusPoint  = FP_BIT;             // as must go to BIT Results
      break;
    default: break;
  }
} // end routine ProcessRngBrgUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessBitResultsUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessBitResultsUsrInp(void)
{
  if (KEYPAD_SCANCODE_RT  == acptKeypadInput)
  {
    QueueLtngCmd(CID_SGAK, NULL);       // Query Lightning for GDB Key Names
    updtFld.chgScr = 1;                 // Update entire [new] screen
    focusPoint     = FP_WF_INFO;        // as must go to Waveform Name
  }
#ifdef ALLOW_BITSCRN_HDN_INPUT
  // TODO - Put similar snippet of code in whatever UsrInp() routine you need
  //        Set the focus point to whatever you want hidden screen/action to be
  else if (KEYPAD_SCANCODE_HDN == acptKeypadInput)
  {                                     // When HIDDEN button press accepted
    updtFld.chgScr  = 1;                // update entire [new] screen and go to
    focusPoint  = FP_ADM_N;             // Administrator Y/N screen
  }
#endif
} // end routine ProcessBitResultsUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessWfNameUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessWfNameUsrInp(void)
{
  if (KEYPAD_SCANCODE_RT  == acptKeypadInput)
  {
    updtFld.chgScr  = 1;                // Update entire [new] screen
    focusPoint  = FP_KEYS;              // as must go to Infil Key Names
  }
} // end routine ProcessWfNameUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessKeyNamesUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessKeyNamesUsrInp(void)
{
  if (KEYPAD_SCANCODE_RT  == acptKeypadInput)
  {
    updtFld.chgScr  = 1;                // Update entire [new] screen
    focusPoint  = FP_HOME;              // as must go to HOME
  }                                     // (or in future other screen)
  else if (KEYPAD_SCANCODE_HDN == acptKeypadInput)
  {                                     // When HIDDEN button pressed here
    char ltngHwi = '1';                 // it is to config Ltng's COMx to
    QueueLtngCmd(CID_CECP, &ltngHwi);   // connect to Ltng HWI (e.g. Ltng PIC),
    updtFld.chgScr  = 1;                // update entire new screen, and
    focusPoint  = FP_HOME;              // go to HOME as action indicator
  }
} // end routine ProcessKeyNamesUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessBrevUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessBrevUsrInp(void)
{
  if ((WGM_ACTVD == selWfTrait.wgmOpt)  &&
      ((KEYPAD_SCANCODE_ENT == acptKeypadInput) ||
       (KEYPAD_SCANCODE_RT  == acptKeypadInput)))
  {                                       // When waveform is actively geo-muted
    return;                               // do not allow ENT or RT key actions
  }

  switch (focusPoint)
  {
    case FP_BREV:
      switch (acptKeypadInput)
      {
        case KEYPAD_SCANCODE_ENT:
          ProcessBrevCodeEnt();
          break;
        case KEYPAD_SCANCODE_UP:
          updtFld.chgScr  = 1;            // Update entire [new] screen
          focusPoint  = FP_CS_DMS;        // as must go to COORDINATE SYSTEM
          break;
        case KEYPAD_SCANCODE_RT:
          focusPoint  = (WBR_0T14 != selWfTrait.wbrOpt)
                      ? FP_BREV_H         // all but UWRL move to HUNDREDS
                      : FP_BREV_T;        // UWRL has no HUNDREDS, go to TENS
          updtFld.val |= BREV_DSPL_FLDS;  // and highlight proper digit
          break;
        default:
          break;
      }
      break;
    case FP_BREV_H:
      switch (acptKeypadInput)
      {
        case KEYPAD_SCANCODE_ENT:
          ProcessBrevCodeEnt();
          break;
        case KEYPAD_SCANCODE_UP:
          brevCodeH[0]++;
          switch (selWfTrait.wbrOpt)
          {
            case WBR_1T253:               // 001-253
              if ('2' < *brevCodeH)   {
                *brevCodeH = '0';     }
              break;
            case WBR_1T48_241T253:        // 001-048, 241-253
              if ('2' < *brevCodeH)   {
                *brevCodeH = '0';     }
              else                    {
                *brevCodeH = '2';     }
            default:
              break;
          }
          updtFld.brevH = 1;              // Highlight updated digit
          break;
        case KEYPAD_SCANCODE_RT:
          switch (selWfTrait.wbrOpt)
          {
            case WBR_1T253:               // 001-253
              if ('2' == *brevCodeH && '5' < *brevCodeT)        {
                *brevCodeT = '5';                               }
              break;
            case WBR_1T48_241T253:        // 001-048, 241-253
              if ('0' == *brevCodeH && '4' < *brevCodeT)        {
                *brevCodeT = '4';                               }
              else if ('2' == *brevCodeH && '4' > *brevCodeT)   {
                *brevCodeT = '4';                               }
              break;
            default:
              break;
          }
          focusPoint   = FP_BREV_T;       // Move to TENS digit
          updtFld.val |= BREV_DSPL_FLDS;  // Highlight proper digit
          break;
        default:
          break;
      }
      break;
    case FP_BREV_T:
      switch (acptKeypadInput)
      {
        case KEYPAD_SCANCODE_ENT:
          ProcessBrevCodeEnt();
          break;
        case KEYPAD_SCANCODE_UP:
          brevCodeT[0]++;
          switch (selWfTrait.wbrOpt)
          {
            case WBR_1T253:               // 001-253
              if ('2' == *brevCodeH && '5' < *brevCodeT)        {
                *brevCodeT = '0';                               }
              else if ('9' < *brevCodeT)                        {
                *brevCodeT = '0';                               }
              break;
            case WBR_1T48_241T253:        // 001-048, 241-253
              if ('0' == *brevCodeH && '4' < *brevCodeT)        {
                *brevCodeT = '0';                               }
              else if ('2' == *brevCodeH && '5' < *brevCodeT)   {
                *brevCodeT = '4';                               }
              break;
            case WBR_0T14:                // 000-014
              if ('1' < *brevCodeT)                             {
                *brevCodeT = '0';                               }
            default:
              break;
          }
          updtFld.brevT = 1;              // Highlight updated digit
          break;
        case KEYPAD_SCANCODE_RT:
          switch (selWfTrait.wbrOpt)
          {
            case WBR_1T253:               // 001-253
              if ('2'==*brevCodeH && '5'==*brevCodeT && '3'<*brevCodeO)     {
                *brevCodeO = '3';                                           }
              else if ('0'==*brevCodeH && '0'==*brevCodeT && '1'>*brevCodeO){
                *brevCodeO = '1';                                           }
              break;
            case WBR_1T48_241T253:        // 001-048, 241-253
              if ('2'==*brevCodeH && '5'==*brevCodeT && '3'<*brevCodeO)     {
                *brevCodeO = '3';                                           }
              else if ('2'==*brevCodeH && '4'==*brevCodeT && '1'>*brevCodeO){
                *brevCodeO = '1';                                           }
              else if ('0'==*brevCodeH && '4'==*brevCodeT && '8'<*brevCodeO){
                *brevCodeO = '8';                                           }
              else if ('0'==*brevCodeH && '0'==*brevCodeT && '1'>*brevCodeO){
                *brevCodeO = '1';                                           }
              break;
            case WBR_0T14:                // 000-014
              if ('1'==*brevCodeT && '4'<*brevCodeO)                        {
                *brevCodeO = '4';                                           }
              break;
            default:
              break;
          }
          focusPoint   = FP_BREV_O;       // Move to ONES digit
          updtFld.val |= BREV_DSPL_FLDS;  // Highlight proper digit
          break;
        default:
          break;
      }
      break;
    case FP_BREV_O:
      switch (acptKeypadInput)
      {
        case KEYPAD_SCANCODE_ENT:
          ProcessBrevCodeEnt();
          break;
        case KEYPAD_SCANCODE_UP:
          brevCodeO[0]++;
          switch (selWfTrait.wbrOpt)
          {
            case WBR_1T253:               // 001-253
              if ('2'==*brevCodeH && '5'==*brevCodeT && '3'<*brevCodeO)     {
                *brevCodeO = '0';                                           }
              else if ('0'==*brevCodeH && '0'==*brevCodeT && '9'<*brevCodeO){
                *brevCodeO = '1';                                           }
              else if ('9'<*brevCodeO)                                      {
                *brevCodeO = '0';                                           }
              break;
            case WBR_1T48_241T253:        // 001-048, 241-253
              if ('2'==*brevCodeH && '5'==*brevCodeT && '3'<*brevCodeO)     {
                *brevCodeO = '0';                                           }
              else if ('2'==*brevCodeH && '4'==*brevCodeT && '9'<*brevCodeO){
                *brevCodeO = '1';                                           }
              else if ('0'==*brevCodeH && '4'==*brevCodeT && '8'<*brevCodeO){
                *brevCodeO = '0';                                           }
              else if ('0'==*brevCodeH && '0'==*brevCodeT && '9'<*brevCodeO){
                *brevCodeO = '1';                                           }
              else if ('9'<*brevCodeO)                                      {
                *brevCodeO = '0';                                           }
              break;
            case WBR_0T14:                // 000-014
              if ('1'==*brevCodeT && '4'<*brevCodeO)                        {
                *brevCodeO = '0';                                           }
              else if ('9'<*brevCodeO)                                      {
                *brevCodeO = '0';                                           }
              break;
            default:
              break;
          }
          updtFld.brevO = 1;              // Highlight updated digit
          break;
        case KEYPAD_SCANCODE_RT:
          focusPoint  = (WBR_0T14 != selWfTrait.wbrOpt)
                      ? FP_BREV_H         // all but UWRL move to HUNDREDS digit
                      : FP_BREV_T;        // UWRL has no HUNDREDS, move to TENS
          updtFld.val |= BREV_DSPL_FLDS;  // Highlight proper digit
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
} // end routine ProcessBrevUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessCoordSysUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessCoordSysUsrInp(void)
{
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      switch (focusPoint)
      {
        case FP_CS_DMS:   devCfg.cSysSet = CS_DMS;    break;
        case FP_CS_DEC:   devCfg.cSysSet = CS_DEC;    break;
        case FP_CS_MGRS:  devCfg.cSysSet = CS_MGRS;   break;
        default: break;
      }
      WriteCfgToNvMem();
      updtFld.chgScr  = 1;              // Start w/ entirely new screen
      focusPoint  = FP_HOME;            // as must return to HOME
      break;
    case KEYPAD_SCANCODE_RT:
      focusPoint++;
      if (FP_CS_MGRS < focusPoint)  {
        focusPoint = FP_CS_DMS;     }
      updtFld.cSys = 1;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr  = 1;              // Update entire [new] screen
      focusPoint  = FP_BL_H;            // as must go to BACKLIGHT
      break;
    default:
      break;
  }
} // end routine ProcessCoordSysUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessBkltUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessBkltUsrInp(void)
{
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      switch (focusPoint)
      {
        case FP_BL_H:  devCfg.bkltSet = BS_HIGH;  SET_BKLT_HIGH();  break;
        case FP_BL_L:  devCfg.bkltSet = BS_LOW;   SET_BKLT_LOW();   break;
        case FP_BL_N:  devCfg.bkltSet = BS_NVG;   SET_BKLT_NVG();   break;
        case FP_BL_O:  devCfg.bkltSet = BS_OFF;   SET_BKLT_OFF();   break;
        default: break;
      }
      WriteCfgToNvMem();
      updtFld.chgScr  = 1;              // Start w/ entirely new screen
      focusPoint  = FP_HOME;            // as must return to HOME
      break;
    case KEYPAD_SCANCODE_RT:
      focusPoint++;
      if (FP_BL_O < focusPoint) {
        focusPoint = FP_BL_H;   }
      updtFld.bkLt = 1;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr  = 1;              // Update entire [new] screen
      focusPoint = FP_TDC_M;            // and go to XMT DUTY CYCLE screen
      break;
    default:
      break;
  }
} // end routine ProcessBkltUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessTxDtyCyUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessTxDtyCyUsrInp(void)
{
  if ((WGM_ACTVD == selWfTrait.wgmOpt)  &&
      ((KEYPAD_SCANCODE_ENT == acptKeypadInput) ||
       (KEYPAD_SCANCODE_RT  == acptKeypadInput)))
  {                                       // When waveform is actively geo-muted
    return;                               // do not allow ENT or RT key actions
  }

  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      switch (focusPoint)
      {
        case FP_TDC_M:
          muteSquawkCtdn = 0;           // Ensure no longer expect to squawk
          devCfg.txDtyCy = TDC_MUTE;
          break;
        case FP_TDC_S:
          devCfg.txDtyCy = TDC_SLOW;
          break;
        case FP_TDC_N:
          devCfg.txDtyCy = TDC_NORM;
          break;
        case FP_TDC_H:
          devCfg.txDtyCy = TDC_HIGH;
          break;
        default:
          break;
      }
      #if (BOOT_MUTE != 1)              // Two user groups requested BOOT_MUTE
        WriteCfgToNvMem();              // so never save any other setting
      #endif                            // when defined to function that way
      if (sysStat.lowPwrMode && (FP_TDC_M != focusPoint))
      {
        char mode = TDC_HIGH;           // Let choosing non-mute duty cycle
        sysStat.lowPwrMode = 0;         // pop us out of Low Power Mode; queue
        QueueLtngCmd(CID_OLPM, &mode);  // exit Low Power Mode ops command.
      }
      QueueLtngCmd(CID_CBDC, (char*)(&devCfg.txDtyCy));   // duty cycle cmd
      updtFld.chgScr  = 1;              // Start w/ entirely new screen
      focusPoint  = FP_HOME;            // as must return to HOME
      break;
    case KEYPAD_SCANCODE_RT:
      switch (selWfTrait.wdcOpt)
      {
        case WDC_4_8_32:
        case WDC_4_8_40:
          focusPoint++;
          if (FP_TDC_H < focusPoint)  {
            focusPoint = FP_TDC_M;    }
          break;
        case WDC_4:
          focusPoint  = (FP_TDC_M == focusPoint)
                      ? FP_TDC_H
                      : FP_TDC_M;
          break;
        default:
          break;
      }
      updtFld.txDtyCy = 1;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr  = 1;              // Update entire [new] screen
      focusPoint  = FP_XP_N;            // and go to XMT POWER screen
      break;
    default:
      break;
  }
} // end routine ProcessTxDtyCyUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessTxPwrUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessTxPwrUsrInp(void)
{
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      switch (focusPoint)
      {
        case FP_XP_m:   devCfg.txPwr = TP_LOW;    break;   // MIN
        case FP_XP_L:   devCfg.txPwr = TP_MED;    break;   // LOW
        case FP_XP_X:   devCfg.txPwr = TP_FULL;   break;   // MAX
        case FP_XP_N:   // our default, so do NOT break;   // NORM
        default:        devCfg.txPwr = TP_HIGH;
      }
      WriteCfgToNvMem();
      QueueLtngCmd(CID_CTXP, (char*)(&devCfg.txPwr));  // TX power command
      updtFld.chgScr  = 1;              // Start w/ entirely new screen
      focusPoint  = FP_HOME;            // as must return to HOME
      break;
    case KEYPAD_SCANCODE_RT:
      switch (selWfTrait.wtpOpt)
      {
        case WTP_A:
          focusPoint++;
          if (FP_XP_X < focusPoint)   {
            focusPoint = FP_XP_m;     }
          break;
      //case WTP_N: there is no alternative to normal
        default:
          break;
      }
      updtFld.txPwr = 1;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr  = 1;              // Start w/ entirely new screen as must
      focusPoint  = FP_LPM_N;           // go to Low Power Mode N/y screen
      break;
    default:
      break;
  }
} // end routine ProcessTxPwrUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessLowPwrModeUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessLowPwrModeUsrInp(void)
{
  uint16_t old;
  char mode;
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      old = sysStat.val;
      if (FP_LPM_Y == focusPoint)
      {
        sysStat.lowPwrMode = 1;
        mode = TDC_SLOW;
      }
      else
      {
        sysStat.lowPwrMode = 0;
        mode = TDC_HIGH;
      }
      if (old != sysStat.val)
      {                                 // Only when a change is made do we
        QueueLtngCmd(CID_OLPM, &mode);  // command selected Low Power Mode
        devCfg.txDtyCy = TDC_MUTE;      // Entering or exiting LPM we will MUTE
        muteSquawkCtdn = 0;             // so ensure no longer expect to squawk.
      }
      updtFld.chgScr = 1;               // Always start w/ entirely new screen
      focusPoint     = FP_HOME;         // as must return to HOME
      break;
    case KEYPAD_SCANCODE_RT:
      updtFld.lpmY_N = 1;
      focusPoint = (FP_LPM_N == focusPoint) ? FP_LPM_Y : FP_LPM_N;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr  = 1;              // Start w/ entirely new screen as must
      focusPoint  = FP_DIR_N;           // go to DELETE INFIL and R&B N/y
      break;
    default:
      break;
  }
} // end routine ProcessLowPwrModeUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessConfDelInfilRbUsrInp(void)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline void ProcessConfDelInfilRbUsrInp(void)
{
  switch (acptKeypadInput)
  {
    case KEYPAD_SCANCODE_ENT:
      scrnCtdn    = CTDN_OFF;           // ENT stops screen countdown
      if (FP_DIR_Y == focusPoint)
      {                                 // When user entered YES
        hdGdbMsgQ      =  0;            // reset INFIL and R&B queue controls
        tlGdbMsgQ      =  0;            // as means of 'deleting' their data
        nmuGdbIdx      = -1;
        numGdbMsgs     =  0;
        numUnrdGdbMsgs =  0;
        dsplGdbMsg     =  0;
        memset((void*)&gdbMsgQ, 0, sizeof(gdbMsgQ));
        nmuPliIdx      = -1;
        hiPliIdx       = -1;
        dsplPliIdx     =  0;
        memset((void*)&pliDatQ, 0, sizeof(pliDatQ));
      }
      updtFld.chgScr  = 1;              // Always start w/ entirely new screen
      focusPoint  = FP_HOME;            // as must return to HOME
      break;
    case KEYPAD_SCANCODE_RT:
      updtFld.dirY_N = 1;
      focusPoint = (FP_DIR_N == focusPoint)
                 ? FP_DIR_Y
                 : FP_DIR_N;
      break;
    case KEYPAD_SCANCODE_UP:
      updtFld.chgScr  = 1;              // Start w/ entirely new screen
      focusPoint  = FP_ADM_N;           // as must go to ADMIN N/y
      break;
    default:
      break;
  }
} // end routine ProcessConfDelInfilRbUsrInp


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessKeypadInput(void)
//  Ring Master of the user interface circus, it calls a plethora of subroutines
//
//  Most keypad inputs handled depending on the interface focus point, but ...
//  ZEROIZE input accepted from any screen at any time unit is awake;
//  911 input accepted from any screen once power-on fully completed;
//  PWR input will cancel & return HOME from normal 'setting' or 'info' screens;
//  PWR input in 'admin setting' screen will cancel & return to ADMIN WARNING.
//
//  Most subroutines should be tolerant of invalid user inputs, but not all.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void ProcessKeypadInput(void)
{
#define _911_OVRD_GEO 0   // 0: 911 does not override geo-mute; 1: 911 will ovrd
  if (isKeypadInputReady)
  {
    if (KEYPAD_SCANCODE_ZERO == acptKeypadInput)
    {                                   // Always accept ZEROIZE as top priority
      updtFld.chgScr  = 1;              // Update entire [new] screen and
      focusPoint  = FP_ZERO_N;          // go to ZEROIZE screen with N selected
    }                                   // which will set scrnCtdn
#if (0 == _911_OVRD_GEO)
    else if (KEYPAD_SCANCODE_911 == acptKeypadInput &&
             (FP_HOME <= focusPoint && WGM_ACTVD != selWfTrait.wgmOpt))
    {                                   // When 911 button press accepted
      Invoke911();                      // queue Lightning command & overhead
                                        // but only for HOME/info/settings scrns
    }                                   // and only when not geo-muted
#elif (1 == _911_OVRD_GEO)
    else if (KEYPAD_SCANCODE_911 == acptKeypadInput && FP_HOME <= focusPoint)
    {                                   // When 911 button press accepted
      if(WGM_OVRDN < selWfTrait.wgmOpt){// but geo-mute being checked/enforced
        selWfTrait.wgmOpt = WGM_OVRDN; }// go ahead and override for emergency
      Invoke911();                      // THEN queue Lightning cmd & overhead
    }                                   // but only for HOME/info/settings scrns
#else
    #error invalid _911_OVRD_GEO value
#endif
    else if (KEYPAD_SCANCODE_PWR == acptKeypadInput && FP_HOME < focusPoint)
    {                                   // When PWR button press accepted
      updtFld.chgScr  = 1;              // update entire [new] screen
      focusPoint  = FP_HOME;            // as 'cancel' and go to HOME but
    }                                   // only for 'info' or 'settings' screens
    else if (KEYPAD_SCANCODE_PWR == acptKeypadInput &&
             (FP_EXF <= focusPoint && FP_HOME > focusPoint))
    {                                   // When PWR button press accepted in an
      updtFld.chgScr  = 1;              // admin scrn update entire [new] screen
      focusPoint  = FP_ADM_WARN;        // as 'cancel' and go to ADMIN WARNING
    }
    else switch (focusPoint)            // else scancode is focusPoint dependent
    {
      case FP_SYSCHK:                   // Get here when sys power first applied
        ProcessSysCheckUsrInp();
        break;

      case FP_NEWBAT_N:                 // Get here only when system power
      case FP_NEWBAT_Y:                 // first applied and it is battery power
        ProcessNewBtryUsrInp();
        break;

      // BEGIN key inputs in power on/off screens
      // these screen must be intolerant of invalid inputs and time limited
      case FP_PWR_1:                    // Get here when 1st apply system power
      case FP_PWR_0:                    // or when wake from PWR-OFF-ENT
        ProcessPwrOnOffUsrInp();
        break;

      case FP_HPWR_1:                   // Get here when press PWR
      case FP_HPWR_0:                   // from the HOME screen
        ProcessHpwrOnOffUsrInp();
        break;
      // END key inputs in power on/off screens

      // ZEROIZE _must_ tolerate stressed operator, but time limit an accident
      case FP_ZERO_N:                   // Get here on user initiated zeroize,
      case FP_ZERO_Y:                   // for which we want to confirm intent
        ProcessZeroizeUsrInp();
        break;
      case FP_ZERO_A:                   // Accept no button while zeroize active
        break;
      // END key inputs in zeroize screen

      // BEGIN key inputs in admin screens
      // these screen should be intolerant of invalid (untrained?) inputs
      case FP_ADM_Y:                    // Admin screens entrance, confirm admin
      case FP_ADM_N:
        ProcessConfAdmUsrInp();
        break;

      case FP_ADM_WARN:                 // Admin legal disclaimer screen
        ProcessAdmWarnUsrInp();
        break;

      case FP_EXF_CTDN:                 // Accept no button during SWF countdown
        break;

      case FP_EXF:                      // Admin change active waveform screen
      case FP_EXF_1:
      case FP_EXF_2:
      case FP_EXF_3:
      case FP_EXF_4:
        ProcessExfilOptUsrInp();
        break;

      case FP_OVRD_N:                   // Admin change geo-mute override screen
      case FP_OVRD_Y:
        ProcessGeoMuteOvrdUsrInp();
        break;

      case FP_INF:                      // Admin change infil group key screen
      case FP_INF_1:
      case FP_INF_2:
      case FP_INF_3:
      case FP_INF_4:
      case FP_INF_5:
      case FP_INF_N:
        ProcessInfilOptUsrInp();
        break;
      // END key inputs in admin screens

      case FP_HOME:                     // Center point of screen flows
        ProcessHomeUsrInp();
        break;

      case FP_GDB:                      // Infil message screen (03,04,0F infil)
        ProcessGdbUsrInp();
        break;

      case FP_RNG_BRG:                  // Range & Bearing screen (0C PLI infil)
        ProcessRngBrgUsrInp();
        break;

      case FP_BIT:                      // BIT results screen
        ProcessBitResultsUsrInp();
        break;

      case FP_WF_INFO:                  // Active waveform info screen
        ProcessWfNameUsrInp();
        break;

      case FP_KEYS:                     // Active infil key names screen
        ProcessKeyNamesUsrInp();
        break;

      case FP_BREV:                     // Brevity code screen
      case FP_BREV_H:
      case FP_BREV_T:
      case FP_BREV_O:
        ProcessBrevUsrInp();
        break;

      case FP_CS_DMS:                   // Coordinate system screen
      case FP_CS_DEC:
      case FP_CS_MGRS:
        ProcessCoordSysUsrInp();
        break;

      case FP_BL_H:                     // Backlight options screen
      case FP_BL_L:
      case FP_BL_N:
      case FP_BL_O:
        ProcessBkltUsrInp();
        break;

      case FP_TDC_M:                    // Transmit duty cycle screen
      case FP_TDC_S:
      case FP_TDC_N:
      case FP_TDC_H:
        ProcessTxDtyCyUsrInp();
        break;

      case FP_XP_m:                     // Transmit power screen
      case FP_XP_L:
      case FP_XP_N:
      case FP_XP_X:
        ProcessTxPwrUsrInp();
        break;

      case FP_LPM_N:                    // Enter Low Power Mode N/y screen
      case FP_LPM_Y:
        ProcessLowPwrModeUsrInp();
        break;

      case FP_DIR_N:                    // Delete Infil and Range & Bearing scrn
      case FP_DIR_Y:
        ProcessConfDelInfilRbUsrInp();
        break;

      default:
        break;
    } // end if else switch(focusPoint)

    isKeypadInputReady = false;         // last,clear flag so may accept new key
  } // end if isKeypadInputReady
} // end routine ProcessKeypadInput


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessLtngData(void)
//  Read reports from Lightning then send queries & commands to it.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void ProcessLtngData(void)
{
  ProcessLtngRpt();
  PostLtngCmd();
  if (LTG_RX_TRG_SET)             {   // While there is full rpt data queued
    doNotSleep = true;            }   // don't sleep in main() state machine
} // end routine ProcessLtngData


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ProcessErrors(void)
//  Give user indicator of error(s).  TODO - damage control
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void ProcessErrors(void)
{
  esd_t newErrState;  // will indicate if/how error state changed

  newErrState.val = (prevErrFlags.val ^ esdErrFlags.val);

  if (newErrState.val)
  {
    if (newErrState.val & esdErrFlags.val)
    { // things are going downhill, do damage control
    }
    else // (newErrState.val & prevErrFlags.val)
    { // things are improving, restore capabilities
    }
    prevErrFlags.val = esdErrFlags.val;
    updtFld.bit = 1;
    updtFld.bitRslt = 1;
  } // end in change of error state

} // end routine ProcessErrors


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void PrepForSleep(void)
//  Wind down operation in prep to Sleep() after PWR-OFF-ENT. Reset most errors.
//  Enforces a 3-second OFF period to ensure BOLT given time to fully power off.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void PrepForSleep(void)
{
  LTG_CLOSE_UART();
  LTG_VLTG_EN = 0;
  LCDClearScreen();
  SET_BKLT_OFF();
  LCD_SLEEP_MODE();
  EndTmr2Srvc();
  if (sysStat.in911Mode)
  {                                     // When going to sleeping from 911
    devCfg.brevCode[0] = '0';           // set brevity code back to
    devCfg.brevCode[1] = '0';           // our default of "001" for
    devCfg.brevCode[2] = '1';           // use when wake up sleep
    WriteCfgToNvMem();                  // Save this in non-volatile memory
  }                                     // before close I2C bus used by NvMem
  CLOSE_I2C2();

  selWfTrait.wgmOpt = devCfg.geoMuting; // Reset geo-muting mode

  ct1SecTick      = 0;                  // count of 1-sec IRQ time ticks
  muteSquawkCtdn  = 0;                  // If in MUTE, wakeup w/o TX expectation
  dsplStatus      = DS_POR;             // enum of POR, ZEROED, etc.
  ClearCoords(&myLoc);                  // Clear out myLoc GPS fix related data
  sysSec          = 0;                  // System/GPS Time's seconds
  sysMin          = 0;                  // System/GPS Time's minues
  sysHr           = 0;                  // System/GPS Time's hours
  aof             = 0;                  // Age Of gps Fix
  nbeSec          = 0;                  // seconds to next BOLT event
  nbeMin          = 0;                  // minutes to next BOLT event
  nbeHr           = 0;                  // hours to next BOLT event
  QUEUE_INIT_EMPTY(cidQ, cidBfr, CID_Q_LEN);
  QUEUE_INIT_EMPTY(ltngRptQ, ltngRptBfr, LTNG_RPT_BFR_LEN);

  sysStat.val       = 0;                // reset status to be starting over
  ltngBitRpt.val    = 0;                // clear out old Lightning BIT report
  esdErrFlags.ltng  = 0;                // clear errors that power cycle may clr
  esdErrFlags.fw    = 0;
  esdErrFlags.sm    = 0;
  esdErrFlags.excp  = 0;
  esdErrFlags.spi   = 0;
  esdErrFlags.ths   = 0;
//esdErrFlags.gasGa = 0;
//esdErrFlags.nvmem = 0;
  esdErrFlags.i2c   = 0;
  esdErrFlags.tmr2  = 0;
  esdErrFlags.tmr3  = 0;
//esdErrFlags.uc    = 0;
  esdErrFlags.uKey  = 0;
  prevErrFlags.val  = esdErrFlags.val;

  __delay_ms(3000);                     // Lock out user as Lightning discharges
} // end routine PrepForSleep


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void GetBackToWork(void)
//  Prepare for normal functions after waking from PWR-OFF-ENT Sleep().
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void GetBackToWork(void)
{
  #if (BOOT_MUTE == 1)              // Two user groups requested BOOT_MUTE
    devCfg.txDtyCy = TDC_MUTE;      // so always turn back on that way
  #endif                            // when defined to function that way
  LTG_VLTG_EN = 1;
  LTG_INIT_UART();
  LTG_READ_NONBLOCKING(ltngRptQ);
  OPEN_I2C2();
  ReqTmr2Srvc(T2S_1SEC, Tmr2_1SecEventsCb);
  LCD_DISPLAY_MODE();
  switch (devCfg.bkltSet)
  {
    case BS_HIGH:   SET_BKLT_HIGH();  break;
    case BS_LOW:    SET_BKLT_LOW();   break;
    case BS_NVG:    SET_BKLT_NVG();   break;
    default:        SET_BKLT_OFF();
  }

  if (IS_ON_BAT_PWR && !newBatSelMade)
  {                                 // When on battery but don't know if new
    updtFld.newBat = 1;             // need to display New Battery screen
    focusPoint     = FP_NEWBAT_N;   // Set user interface point of focus
  }
  else
  {                                 // otherwise just
    updtFld.pwr01 = 1;              // display Power ON-OFF screen.
    focusPoint    = FP_PWR_0;       // Set user interface point of focus
  }
  QueueLtngCmd(CID_SGAK, NULL);     // query names of active GDB keys as BIT
} // end routine GetBackToWork


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void UpdateDisplay(void)
//  Front Man of the user interface circus, it calls a plethora of subroutines
//  based on the interface focus point, controlling what is being shown.
//
//  Special handling to immediately go from HOME to INFIL if newest msg unread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UpdateDisplay(void)
{
  if (0 <= nmuGdbIdx && (FP_HOME <= focusPoint && FP_GDB > focusPoint))
  { // When newest GDB message has not been read and on lower priority screen
    focusPoint = FP_GDB;                // change focus to GDB screen
    updtFld.chgScr = 1;                 // (noting we are changing screen) since
  }                                     // newest infil is highest priority
  if (updtFld.chgScr)
  {                                     // When changing screens immediately
    scrnCtdn = CTDN_OFF;                // turn off any active screen countdown
    LCDClearScreen();                   // and clear off the old screen
  }

  switch (focusPoint)
  {
    case FP_NA_SLP:                     // No screen -- turn OFF to sleep
      mGlobalIntDisable();              // Disable IRQs so we can safely
      doNotSleep = false;               // clear doNotSleep (to Sleep() below)
      PrepForSleep();                   // and now prepare to sleep deeply.
      StartKeypadWakeOps();             // Future keypad wake ops ISR sets flag
      while ( ! doNotSleep)             // that we check to break sleep cycle.
      {                                 // Loop here while no PWR button input
        mGlobalIntEnable();             // each time enabling IRQ to wake uC
        Sleep();                        // Put uC to SLEEP for max power savings
        mGlobalIntDisable();            // When wake immediately disable IRQ so
      }                                 // loop can safely check need to wake.
      mGlobalIntEnable();               // Re-enable IRQs when loop broken
      GetBackToWork();                  // and get back to work by powering up
      StartKeypadScanOps();             // Keypad drive & timeouts now have ctrl
      updtFld.chgScr = 1;               // Start back up w/ entirely new screen
      return;                           // Leave routine before code chgScr = 0;

    case FP_SYSCHK:                     // Screen when running P-BIT
      UpdateSysCheckDisplay();
      break;

    case FP_NEWBAT_N:                   // Screen to get input regarding battery
    case FP_NEWBAT_Y:
      UpdateNewBtryDisplay();
      break;

    case FP_PWR_0:                      // Screen for selecting unit ON/OFF
    case FP_PWR_1:                      // Inputs handled differently depending
    case FP_HPWR_0:                     // on if ON/OFF started from HOME screen
    case FP_HPWR_1:                     // or if starting from sleep/unpowered.
      UpdatePwrOnOffDisplay();
      break;

    // BEGIN admin screens
    case FP_ADM_Y:                      // Screen to get admin role confirmation
    case FP_ADM_N:
      UpdateConfirmAdminDisplay();
      break;

    case FP_ADM_WARN:                   // Warning regarding admin actions
      UpdateAdminWrnDisplay();
      break;

      case FP_FSET:                     // Admin screen to set active Fset
      case FP_FSET_0:                     
      case FP_FSET_1:
      case FP_FSET_2:
      case FP_FSET_3:
      case FP_FSET_4:
      case FP_FSET_5:
      case FP_FSET_6:
      case FP_FSET_7:
      case FP_FSET_8:
      case FP_FSET_9:
      case FP_FSET_10:
      case FP_FSET_11:
      case FP_FSET_12:
      case FP_FSET_13:
      case FP_FSET_14:
      case FP_FSET_15:
        UpdateFsetDisplay();
        break;

    case FP_EXF:                        // Admin screen to set active waveform
    case FP_EXF_1:
    case FP_EXF_2:
    case FP_EXF_3:
    case FP_EXF_4:
      UpdateExfilOptDisplay();
      break;

    case FP_OVRD_N:                     // Admin change geo-mute override screen
    case FP_OVRD_Y:
      UpdateGeoMuteOvrdDisplay();
      break;

    case FP_EXF_CTDN:                   // Admin countdown after EXF selection
      UpdateExfilCtdnDisplay();
      break;

    case FP_INF:                        // Admin screen to set active group key
    case FP_INF_1:
    case FP_INF_2:
    case FP_INF_3:
    case FP_INF_4:
    case FP_INF_5:
    case FP_INF_N:
      UpdateInfilOptDisplay();
      break;
    // END admin screens

    case FP_ZERO_N:                     // Screens associated w/ zeroizing unit
    case FP_ZERO_Y:
    case FP_ZERO_A:   // Zeroize active
      UpdateZeroizeDisplay();
      break;

    case FP_HOME:                       // Grand Central Station w/ status info
      UpdateHomeDisplay();
      break;

    case FP_GDB:                        // Already in INFIL or drop-in from HOME
      UpdateGdbInfilDisplay();
      break;

    case FP_RNG_BRG:                    // Range & Bearing display
      UpdateRngBrgDisplay();
      break;

    case FP_BIT:                        // More friendly Built-In-Test results
      UpdateBitResultsDisplay();
      break;

    case FP_WF_INFO:                    // Screen w/ name of active waveform
      UpdateWfNameDisplay();
      break;

    case FP_KEYS:                       // Screen w/ active infil group key name
      UpdateKeyNamesDisplay();
      break;

    case FP_BREV:     // No digit sel   // Screen to set exfil brevity code
    case FP_BREV_H:   // Hundreds digit
    case FP_BREV_T:   // Tens digit
    case FP_BREV_O:   // Ones digit
      UpdateBrevityCodeDisplay();
      break;

    case FP_CS_DMS:   // HDDD.MM.SS     // Screen to select coordinate system
    case FP_CS_DEC:   // +DDD.ddddd
    case FP_CS_MGRS:  // MGRS
      UpdateSetCoordSysDisplay();
      break;

    case FP_BL_H:     // High           // Screen to set display's backlighting
    case FP_BL_L:     // Low
    case FP_BL_N:     // NVG
    case FP_BL_O:     // Off
      UpdateSetBkltDisplay();
      break;

    case FP_TDC_M:    // Mute           // Screen to set exfil duty cycle
    case FP_TDC_S:    // Slow
    case FP_TDC_N:    // Normal
    case FP_TDC_H:    // High
      UpdateTxDtyCyDisplay();
      break;

    case FP_XP_m:   // Min    (BT Low)  // Screen to set exfil TX Power
    case FP_XP_L:   // Low    (BT Med)
    case FP_XP_N:   // Normal (BT High)
    case FP_XP_X:   // Max    (BT Full)
      UpdateTxPwrDisplay();
      break;

    case FP_LPM_N:                      // Screen to disable INFIL & TX, min GPS
    case FP_LPM_Y:
      UpdateLowPwrModeDisplay();
      break;

    case FP_DIR_N:                      // Screen to set Delete Infil/R&B
    case FP_DIR_Y:
      UpdateDelInfilRbDisplay();
      break;

    default: break;
  }

  updtFld.chgScr = 0;
} // end routine UpdateDisplay


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void InitSystem(void)
//  Setup the uC pins & peripheral; initialize software's values & services.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void InitSystem(void)
{
  mGlobalIntDisable();                  // Disable all IRQs until run initialize
  RCONbits.SWDTEN = 0;                  // Disable Watchdog timer as well

  // Enable interrupt nesting (user priorities; hi-pri prempting lo-pri)
  INTCON1bits.NSTDIS = 0;     // Enable IRQ nesting - IPL bits can disable IRQs

  //======= Setup port functionality: ana/dig, direction, pull up/down =========
  // On reset: ANAx all analog, TRISx all inputs, LATx and PORTx are unknown,
  //           ODCx all open drain, CNPUx and CNPDx are disabled
  // Set unused I/O pins to digital output low to save power (tie I-only to GND)
  // Turn off as much as possible by default and turn on later as needed
  // I/O pins analog configuration as defined in micro_defs.h
  ANSA  = INIT_RA_ANS;
  ANSB  = INIT_RB_ANS;
  // I/O pins direction configuration as defined in micro_defs.h
  TRISA = INIT_RA_DIR;
  TRISB = INIT_RB_DIR;
  // Output pins level settings as defined in micro_defs.h
  LATA  = INIT_RA_LAT;
  LATB  = INIT_RB_LAT;
  // Output pins open drain configuration as defined in micro_defs.h
  ODCA  = INIT_RA_ODO;
  ODCB  = INIT_RB_ODO;
  // Input pins weak pull-up configuration as defined in micro_defs.h
  CNPU1 = CN1500_WPU;
  CNPU2 = CN3116_WPU;
  CNPU3 = CN4732_WPU;
  // Input pins weak pull-down configuration as defined in micro_defs.h
  CNPD1 = CN1500_WPD;
  CNPD2 = CN3116_WPD;
  CNPD3 = CN4732_WPD;

  // De-clock peripherals (Peripheral Module Disable) for max power savings
  PMD1 = 0xFFFF;
  PMD2 = 0xFFFF;
  PMD3 = 0xFFFF;
  PMD4 = 0xFFFF;
  PMD5 = 0xFFFF;
  PMD6 = 0xFFFF;
  PMD7 = 0xFFFF;
  PMD8 = 0xFFFF;

  // Set Peripheral Pin Select reconfig per micro_defs.h
  __builtin_write_OSCCONL(OSCCON&0xbf); // Unlock pin select registers
  INT1_RPINR  = INT1_RPX;               // Set gas gauge External IRQ-1 pin
  INT2_RPINR  = INT2_RPX;               // Set pwr btn External IRQ-2 pin
  OC1_RPOR    = OC1_OFN;                // Assign Output Compare pulse port pin
  SCK1_RPOR   = SCK1_OFN;               // Assign SPI Clock port pin
  SDO1_RPOR   = SDO1_OFN;               // Assign SPI MOSI port pin
  U1RX_RPINR  = U1RX_RPX;               // Set UART1 RX pin per micro_defs.h
  U1TX_RPOR   = U1TX_OFN;               // Set UART1 TX pin
  __builtin_write_OSCCONL(OSCCON|0x40); // Lock Registers
/*
  switch (IdentifyResetSource())
  { // set parameter in D.RST 'reset' report to RTI(s)
    case POWER_ON:            *pRstCode = 'P';    break;
    case BROWN_OUT:           *pRstCode = 'B';    break;
    case EXTERNAL:            *pRstCode = 'A';    break;
  //case SOFTWARE:            *pRstCode = 'S';    break;
  //case WATCHDOG:            *pRstCode = 'S';    break;
  //case ILLEGAL_INSTR:       *pRstCode = 'S';    break;
  //case TRAP:                *pRstCode = 'S';    break;
  //case CFG_WORD_MISMATCH:   *pRstCode = 'S';    break;
  //case NONE_FOUND:          *pRstCode = 'S';    break;
    default:                  *pRstCode = 'S';    break;
  }
*/

  // Almost all system attributes initialized at declaration/definition
  memset((void*)wfTrait, NVLD_TXID, sizeof(wfTrait));
  memset((void*)&gdbMsgQ, 0, sizeof(gdbMsgQ));
  memset((void*)&pliDatQ, 0, sizeof(pliDatQ));
  ClearCoords(&myLoc);
  QUEUE_INIT_EMPTY(cidQ, cidBfr, CID_Q_LEN);
  QUEUE_INIT_EMPTY(ltngRptQ, ltngRptBfr, LTNG_RPT_BFR_LEN);

  // init peripherals and start services
  LTG_VLTG_EN = 1;                      // Enable Lightning regulator
  LTG_INIT_UART();                      // Enable UART to Lightning and
  LTG_READ_NONBLOCKING(ltngRptQ);       // start endless read immediately
  ResetBusI2c2();                       // Try to ensure I2C bus not locked up
  INIT_I2C2();                          // then enable/init I2C2 peripheral
  OPEN_I2C2();                          // and open/start I2C2 for IC drivers.
  InitLtc2943();                        // Configure Gas Gauge IC on I2C
  InitUc1701x();                        // Initialize SPI1, OC1, LCD display, &
  LCDClearScreen();                     // clear LCD's standard pwr-on funkiness
  InitTmr2Driver();                     // Initialize timer design uses for
  ReqTmr2Srvc(T2S_1SEC, Tmr2_1SecEventsCb); // 1-sec system timing functions.
  InitKeypadDriver();                   // Initialize C0, C1, Timer-3, and INT2;
  StartKeypadScanOps();                 // then start periodic looking at keypad
  // SET_BACKLIGHT____(); called when FP_SYSCHK
  mGlobalIntEnable();

  __delay_us(52);                       // Timers 2 & 3 have 25.6 us resolution
  if (OSCCONbits.CF)                {   // When Clock Fail Detect bit still set
    esdErrFlags.uc = 1;             }   // note primary oscillator's failure
  if ( ! TMR2)                      {   // When Timer 2 has failed to tick
    esdErrFlags.tmr2 = 1;           }   // set our error flag for it.
  if ( ! TMR3)                      {   // When Timer 3 has failed to tick
    esdErrFlags.tmr3 = 1;           }   // set our error flag for it

  updtFld.chgScr  = 1;                  // Initialize with a screen change
  updtFld.sysChk  = 1;                  // so that display System Check
  focusPoint      = FP_SYSCHK;          // Set user interface point of focus
                                        // so system runs/displays system check
} // end routine InitSystem


//-*-*- MAIN -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//  int16_t main(void)
//  The One to rule them all, One to find them, One to bring them all and in the
//  darkness bind them.  The order of processes does matter and is done in such
//  a way as to manage interdependencies, e.g. update display last before
//  potentially sleeping to reflect all changes induced by other processes.
//
//  INPUT : NONE
//  OUTPUT: uint16_t - esdErrFlags.val
//  CALLS : InitSystem
//          ProcessTimeEvents
//          ProcessKeypadInput
//          ProcessLtngData
//          ProcessErrors
//          UpdateDisplay
//          Idle
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
int16_t main(void)
{
  InitSystem();                         // On resets perform uC/code/driver init
  while (1)
  {                                     // Loop continuously until reset
    ProcessTimeEvents();                // Process time events, incl UI timeouts
    ProcessKeypadInput();               // and let UI inputs over-ride timeouts
    ProcessLtngData();                  // and let GDB data over-ride scr focus
    ProcessErrors();                    // and let errors over-ride all else
    UpdateDisplay();                    // and now ready to update displayed UI
                                        // IRQ and processes set flag doNotSleep
    mGlobalIntDisable();                // Disable IRQs so we can safely
    while ( ! doNotSleep)               // check if have pressing tasking.
    {                                   // Loop here while no pressing tasks,
      mGlobalIntEnable();               // each time enabling IRQ to wake uC
      Idle();                           // from low-power state. Do NOT Sleep()!
      mGlobalIntDisable();              // When wake, immediately disable IRQ
    }                                   // so loop ctrl can check need to work.
    doNotSleep = false;                 // Immediately clear flag when exit loop
    mGlobalIntEnable();                 // and now safely re-enable IRQs
  }                                     // so they may set flag again.
  return esdErrFlags.val;               // If ever get here, return error status
} // end main routine
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
