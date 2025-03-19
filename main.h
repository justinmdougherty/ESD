#ifndef MAIN_H__
#define MAIN_H__
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : main.h
 *
 *  DESCRIPTION   : Declare top level (main) attributes and methods required to
 *    implement user interface (aka ESD) of NSWCDD's Lightning BOLT hand-held.
 *
 *      (1) uint16_t main(void)
 *
 *  WRITTEN BY    : Robert Kirby, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/10/10, Robert Kirby, NSWC H12
 *      Add option to display GPS coordinates as decimal degrees (+DDD.ddddd)
 *      Move txdtycy_t to bt_waveform_traits.h (from main.h and LTNG bt_intfc.h)
 *      Implement All C.TXP options not just Norm|Max (now Min|Low|Norm|Max)
 *    2021/06/01, Robert Kirby, NSWC H12
 *      Geo-muting support: typedef wgm_t and add it to wet_t and devcfg_t
 *      Define geo-muting geozone
 *      Refactor devcfg_t .waveform to .selWfChar and add .selWfIdx
 *      Reorder declarations of typedefs
 *    2020/04/06, Robert Kirby, NSWC H12
 *      Include bt_waveform_load.h, typedef adds/deletes, etc. to support new
 *      "waveform information" interface messages that gets us away from wf
 *      load-out specific fw versions that depend on changing bt_waveform_load.h
 *      Define BUILDING_ESD for use by bt_waveform_load.h to block Ltng specific
 *      stuff that just confuses the shit out of ESD.
 *    2019/01/25, Robert Kirby, NSWC H12
 *      Comments/whitespace
 *    2018/12/17, Robert Kirby, NSWC H12
 *      Change waveform_t enums from WF_BANDx to WF_SLOTx; band not tied to slot
 *    2018/09/21, Robert Kirby, NSWC H12
 *      Add NVG and remove MED backlight setting (now have HIGH, LOW, NVG, OFF)
 *      Refactor sysErrFlags to esdErrFlags as it doesn't cover Ltng errors
 *    2017/10/20, Robert Kirby, NSWC H12
 *      Modify esd_t to get rid of ltngBit which is now duplicate info
 *    2017/08/21, Robert Kirby, NSWC H12
 *      Add uKey and inF to esd_t for flagging unique GDB Key
 *      Comments, remove stubbed out vestigal typedefs
 *    2017/07/27, Robert Kirby, NSWC H12
 *      Comments
 *    2017/04/10, Robert Kirby, NSWC H12
 *      Rearrange esd_t bits, repurpose unused esd_t.tmr45 as esd_t.ltngBit
 *    2017/04/07, Robert Kirby, NSWC H12
 *      Rename TDC_AUTO to the more appropriate TDC_NORM
 *    2017/03/24, Robert Kirby, NSWC H12
 *      Initial development as total revamp of ESD
 *
 *  REFERENCE DOCUMENTS
 *    1.  PIC24FJ128GA204 Family Data Sheet (Microchip DS30010038C)
 *    2.  ... Interface Control Document (NSWCDD Code H12)
 *    *.  Abbreviations and Acronyms... (ASME Y14.38-2007)
 *    *.  MIL-STD-12D Military Standard Abbreviations ...
 */
#define  BUILDING_ESD 1       // preempt bt_waveform_traits.h Ltng specifics
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "../Lightning/bt_waveform_traits.h"  // for waveform typedefs etc.

//----- DEFINES, ENUMS, STRUCTS, TYPEDEFS, ETC. --------------------------------
typedef union tagESD          // Different system components wrt ESD uC/circuit
{
  struct                      // Within each group, higher bit == more important
  {
    uint16_t ltng   :1;       // BIT0: Lightning intfc data
    uint16_t fw     :1;       // BIT1: generic firmware
    uint16_t sm     :1;       // BIT2: state machine firmware
    uint16_t excp   :1;       // BIT3: trap or [undefined] IRQ firmware

    uint16_t spi    :1;       // BIT4: SPI bus
    uint16_t ths    :1;       // BIT5: Temperature & Humidity Sensor
    uint16_t gasGa  :1;       // BIT6: Gas Gauge (Columb Counter)
    uint16_t nvmem  :1;       // BIT7: Non-Volatile Memory
    uint16_t i2c    :1;       // BIT8: I2C bus
    uint16_t tmr2   :1;       // BIT9: Timer-2
    uint16_t tmr3   :1;       // BITA: Timer-3
    uint16_t uc     :1;       // BITB: critical uC features (Posc, PPS, etc.)

    uint16_t uKey   :1;       // BITC: GDB unique key not loaded

    uint16_t rsvdDF :3;       // BITD:F
//  uint16_t uart1  :1;       // BITx: UART1 (connection to Lightning)
//  uint16_t vext   :1;       // BITx: external power voltage
//  uint16_t vbat   :1;       // BITx: battery power voltage
  };
  struct
  {
    uint16_t fwF    :4;       // Firmware Flags
    uint16_t hwF    :8;       // Hardware Flags
    uint16_t inF    :1;       // Infil Key Flag
    uint16_t def    :3;       // do NOT report these on System Check screen
  };
  uint8_t  u8[2];
  uint16_t val;
} esd_t;


typedef enum tagOPERATING_STATUS
{
  OS_ZEROED   = -2,   // cannot be 0xFFFF in nvmem, -2 is 0xFFFE
  OS_OFF      =  0,
  OS_ON,
} opstat_t;

typedef enum tagCOORDINATE_SYSTEM_SETTING
{
  CS_DMS,
  CS_DEC,
  CS_MGRS
} csysset_t;

typedef enum tagBACKLIGHT_SETTING
{
  BS_OFF,
  BS_NVG,
  BS_LOW,
  BS_HIGH,
} bkltset_t;

typedef enum tagEXTERNAL_COM_PORT_CONNECTION
{
  EC_NONE = '0',
  EC_HWI,
  EC_BOLT,
  EC_GDB
} extcom_t;

typedef enum tagRECEIVE_DUTY_CYCLE
{
  RDC_OFF  = '0',
  RDC_PER,
  RDC_AUTO,
  RDC_ON,
} rxdtycy_t;

typedef enum tagRESPONSE_TO_911_ACK
{
  RA_NONE = '0',
  RA_CBC,
  RA_TXM,
  RA_TXM_RXM
} rspsack_t;

typedef enum tagTRANSMIT_POWER
{ // values from BOLT ICD used in C.TXP to Ltng
  TP_LOW  = '0',      // corresponds to FP_XP_m (min)
  TP_MED,             // corresponds to FP_XP_L (low)
  TP_HIGH,            // corresponds to FP_XP_N (normal)
  TP_FULL,            // corresponds to FP_XP_X (max)
//TP_ENHANCED,
} txpwr_t;

typedef struct tagWAVEFORM_ESD_TRAITS
{ // typedefs below are from ../Lightning/bt_waveform_traits.h
  wbr_t   wbrOpt;                   // indicator of brevity range options
  wdc_t   wdcOpt;                   // indicator of duty cycle options
  wtp_t   wtpOpt;                   // indicator of TX power options
  wgm_t   wgmOpt;                   // indicator of Geo-Muting options
  uint8_t name[WF_NAME_LEN+1];      // user friendly name from D.WFI (+ '\0')
  uint8_t txId[WF_TXID_LEN+1];      // TX ID for this wf  from D.WFI (+ '\0')
} wet_t;
// Define values associated with number of "waveform info" sets
#define MAX_WFI_IDX  (MAX_WF-'0') // convert bt_waveform_traits ascii to binary
#define N_WFI     (MAX_WFI_IDX+1) // size of array of 0-based "waveform info"
#define NVLD_TXID  '?'            // char to used indicate TXID invalid


typedef struct tagDEVICE_CONFIGURATION_PARAMS
{
  uint16_t    fwKey;                // arbitrary constant value
  opstat_t    opStat;               // ON, OFF, Zeroed
  char        brevCode[4];          // 001,...,240,241,...,253
  csysset_t   cSysSet;              // Geo Coordinate System
  bkltset_t   bkltSet;              // HIGH,...,OFF
  extcom_t    extCom;               // None, external COMx
  rxdtycy_t   rxDtyCy;              // OFF,...,ON
  rspsack_t   respAuto911Ack;       // NONE, CBC, etc.
  rspsack_t   respMan911Ack;        // NONE, CBC, etc.
  ws_t        selWfChar;            // char used in C.SWF to Ltng
  uint16_t    selWfIdx;             // number(selWfChar - '0') for wfTrait[idx]
  wgm_t       geoMuting;            // NA,NEVER,OVRDN,ACTVD (devCfg never OUTGZ)
  txpwr_t     txPwr;                // HIGH, FULL
  txdtycy_t   txDtyCy;              // MUTE, SLOW, AUTO, HIGH
} devcfg_t;
#define FW_KEY  0x23DC              // arbitrary firmware key
#define FKLB    (FW_KEY & 0x00FF)   // low byte of firmware key
#define FKHB    (FW_KEY >> 8)       // high byte of firmware key


//Polygon covering zone ~1,000 miles around USA/N.Am
#define USA_NUM_PTS 15  // # of points that make zone (but v[] has n+1 points)
#define USA_VERTICES                                                          \
{                       /* LAT LSB = 0.002746666, LON LSB = 0.005493332   */  \
  {0x71C6, 0xE38E},     /*  80.0000,  -40.0000 Polar region above N.Am    */  \
  {0x71C6, 0x87A6},     /*  80.0000, -169.2500 n.Greenland to nw of Alaska*/  \
  {0x5C78, 0x87A6},     /*  65.0189, -169.2500 Tap onto Int'l Date Line   */  \
  {0x4AEE, 0x78EC},     /*  52.6863,  170.0500 Follow Int'l Date Line     */  \
  {0x4408, 0x8001},     /*  47.8353, -180.0000 Follow Int'l Date Line     */  \
  {0x4408, 0x98E4},     /*  47.8353, -145.0000 Cut back toward coast to   */  \
  {0x31C7, 0x98E4},     /*  35.0000, -145.0000 uncover off WA, OR, & n.CA */  \
  {0x31C7, 0x8001},     /*  35.0000, -180.0000 Cut back west to cover     */  \
  {0x1C72, 0x8001},     /*  20.0000, -180.0000 area around Midway Island  */  \
  {0x071C, 0x91C8},     /*   5.0000, -155.0000 and dip south of Hawaii    */  \
  {0x1555, 0x9C73},     /*  15.0000, -140.0000 Bump back north a tad to   */  \
  {0x1555, 0xDC72},     /*  15.0000,  -50.0000 go east thru Caribbean     */  \
  {0x238E, 0xD1C7},     /*  25.0000,  -65.0000 Cover PR as zig toward FL  */  \
  {0x31C7, 0xD8E4},     /*  35.0000,  -55.0000 and zag from NC as continue*/  \
  {0x4000, 0xE38E},     /*  45.0000,  -40.0000 n.e. past Nova Scotia      */  \
  {0x71C6, 0xE38E}      /*  80.0000,  -40.0000 Then back to n.Greenland   */  \
}                       /* Note that first & last points must be the same */

// Lower-Left/Upper-Right 'rectangle' container for USA_POLYZONE16
#define USA_LLUR16                                                            \
{                       /* rectangular bounds of USA_POLYZONE16           */  \
  .ll = {0x071C,0x78EB},/*   5.0000,  170.0500 Lower Left                 */  \
  .ur = {0x71C6,0xE38E} /*  80.0000,  -40.0000 Upper Right                */  \
}

//----- EXPOSED ATRRIBUTES -----------------------------------------------------
extern esd_t      volatile esdErrFlags;     // detected ESD system errors


//----- EXPOSED PROCEDURES -----------------------------------------------------


//----- MACROS -----------------------------------------------------------------


#endif  // MAIN_H__
