#ifndef LTC2943_H__
#define	LTC2943_H__
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : ltc2943.h
 *
 *  DESCRIPTION   : Declares interface to LTC2943 columb counter IC  used to
 *    implement battery gas gauge functionality.
 *
 *      (1) bool InitLtc2943(void)
 *      (2) bool GetLtc2943Charge(uint8_t * acrMSB, uint8_t * acrLSB)
 *      (3) bool SetLtc2943Charge(uint8_t acrMSB, uint8_t acrLSB)
 *  //  (4) bool SetLtc2943FullCharge(void)
 *
 *  NOTE - This requires micro_defs.h to define LTC2943_I2C_WRT_ADDR and
 *         LTC2943_I2C_WRT_ADDR
 *  NOTE - This requires I2C2 to be initialized/open
 *
 *  Battery capacity calcs based on derated Panasonic and Duracell CR123
 *    Panasonic CR123 1500 mAhr (Duracell same, most brands are lower capacity)
 *    derated to 75%  1125 mAhr
 *    derated to 70%  1050 mAhr
 *    derated to 65%   975 mAhr
 *
 *  Gas gauge range parameters:
 *      rSense = 15 mOhm (sense resistor)
 *      M = 256 (prescaler)
 *      qBat = ____ mAhr  (ex. Battery derated to 75% = 1125 mAhr)
 *
 *    Formula for calculating charge of least significant bit (qLSB):
 *        qLSB = 0.340 mAhr * (50 mOhm  / rSense)  * ( M  / 4096)
 *        qLSB = 0.340 mAhr * (50 mOhm  / 15 mOhm) * (256 / 4096)
 *        qLSB = 0.0708 mAhr
 *
 *    Therefore, IC's MAX ACCUMULATED possible = 2^16 * qLSB = 4642 mAhr
 *        [Note: 2^16 is max storage for the IC's 2 bytes]
 *
 *    Calculating MAX ACCUMULATED for specific battery capacity
 *
 *        BTRY_MAX_ACCUM = qBat / qLSB
 *
 *        derated to 75% = 1125 mAhr / 0.0708 mAhr
 *                       = 15882
 *                       = 0x3E0A
 *
 *        derated to 70% = 1050 mAhr / 0.0708 mAhr
 *                       = 14823
 *                       = 0x39E7
 *
 *        derated to 65% =  975 mAhr / 0.0708 mAhr
 *                       = 13765
 *                       = 0x35C5
 *
 *  WRITTEN BY    : Nicholus Sunshine, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2017/04/28, Robert Kirby, NSWC H12
 *      Change comments and values to match 15mOhm R309 sense resistor on
 *      new green boards built this month (old red boards had 25mOhm R309)
 *    2017/03/24, Robert Kirby, NSWC H12
 *      Pull together and comment on what Nick (and Megan) developed for ESD
 *      Convert macros to functions and expose only what is needed by consumer
 *    2017/01/24 (at the time in user.h), Megan Kozub, NSWC V14
 *      Modified gas gauge range and charge threshold values
 *      Documented max accumulation calculations
 *
 *  REFERENCE DOCUMENTS
 *    1.  LTC2487 Datasheet (Linear Technology Corporation, LT 0215 REV F)
 */
#include <stdint.h>
#include <stdbool.h>

// Battery gas gauge definitions
// Active zero and full points represent incorrect orientation of the BGG IC.
// Future HW mod will correct the orientation and the range will count
// down from 0xFFFF (vice "zero"): namely, LTC2943_FULL_PT - LTC2943_ZERO_PT
#define LTC2943_ZERO_PT (0x3E0A)          // 75% capacity CR123
//#define LTC2943_ZERO_PT (0x39E7)          // 70% capacity CR123
//#define LTC2943_ZERO_PT (0x35C5)          // 65% capacity CR123
#define LTC2943_ZERO_PT_MSB (LTC2943_ZERO_PT >> 8)
#define LTC2943_ZERO_PT_LSB (LTC2943_ZERO_PT & 0x00FF)

#define LTC2943_FULL_PT (0x0000)
#define LTC2943_FULL_PT_MSB (LTC2943_FULL_PT >> 8)
#define LTC2943_FULL_PT_LSB (LTC2943_FULL_PT & 0x00FF)

#define LTC2943_BATTERY_RANGE (LTC2943_ZERO_PT - LTC2943_FULL_PT)


bool InitLtc2943(void);
bool GetLtc2943Charge(uint8_t * acrMSB, uint8_t * acrLSB);
bool SetLtc2943Charge(uint8_t acrMSB, uint8_t acrLSB);
//bool SetLtc2943FullCharge(void);


/*
 * COMMENTS FOR PRE-2017/04/28 CODE BASED ON RED BOARD's 25mOhm R309
 *
 *  Gas gauge range parameters:
 *      rSense = 25 mOhm (sense resistor)
 *      M = 256 (prescaler)
 *      qBat = ____ mAhr  (ex. Battery derated to 75% = 1125 mAhr)
 *
 *    Formula for calculating charge of least significant bit (qLSB):
 *        qLSB = 0.340 mAhr * (50 mOhm  / rSense)  * ( M  / 4096)
 *        qLSB = 0.340 mAhr * (50 mOhm  / 25 mOhm) * (256 / 4096)
 *        qLSB = 0.0425 mAhr
 *
 *    Therefore, IC's MAX ACCUMULATED possible = 2^16 * qLSB = 2785 mAhr
 *        [Note: 2^16 is max storage for the IC's 2 bytes]
 *
 *    Calculating MAX ACCUMULATED for specific battery capacity
 *
 *        BTRY_MAX_ACCUM = qBat / qLSB
 *
 *        derated to 75% = 1125 mAhr / 0.0425 mAhr
 *                       = 26470
 *                       = 0x6766
 *
 *        derated to 70% = 1050 mAhr / 0.0425 mAhr
 *                       = 24706
 *                       = 0x6082
 *
 *        derated to 65% =  975 mAhr / 0.0425 mAhr
 *                       = 22941
 *                       = 0x599D
 */
#endif	// LTC2943_H__
