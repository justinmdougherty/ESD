////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : ltc2943.c
 *
 *  DESCRIPTION   : Defines interface to LTC2943 columb counter IC  used to
 *    implement battery gas gauge functionality.
 *
 *      (*) bool Ltc2943WriteRegister1Byte(uint8_t reg, uint8_t data)
 *      (*) bool Ltc2943WriteRegister2Bytes(reg, data1, data2)
 *      (*) bool Ltc2943ReadRegister1Byte(uint8_t reg, uint8_t * data)
 *      (*) bool Ltc2943ReadRegister2Bytes(reg, *data1, *data2 )
 *      (1) bool InitLtc2943(void)
 *      (2) bool GetLtc2943Charge(uint8_t * acrMSB, uint8_t * acrLSB)
 *      (3) bool SetLtc2943Charge(uint8_t acrMSB, uint8_t acrLSB)
 *  //  (4) bool SetLtc2943FullCharge(void);
 *
 *  NOTE - This requires micro_defs.h to define LTC2943_I2C_WRT_ADDR and
 *         LTC2943_I2C_WRT_ADDR
 *  NOTE - This requires I2C2 to be initialized/open
 *
 *  WRITTEN BY    : Nicholus Sunshine, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2017/03/24, Robert Kirby, NSWC H12
 *      Pull together and comment on what Nick (and Megan) developed for ESD
 *      Convert macros to functions and add GetLtc2943Charge to hide lowest code
 *    2017/01/24 (at the time in user.h), Megan Kozub, NSWC V14
 *      Modified gas gauge range and charge threshold values
 *      Documented max accumulation calculations
 *
 *  REFERENCE DOCUMENTS
 *    1.  LTC2487 Datasheet (Linear Technology Corporation, LT 0215 REV F)
 */
#include "micro_defs.h"
#include "i2c2.h"
#include "ltc2943.h"
#include "micro_defs.h"   // for LTC2943_I2C_WRT_ADDR etc.


#define ISWA      (LTC2943_I2C_WRT_ADDR)    // LTC2943 I2C Slave Write Address
#define ISRA      (LTC2943_I2C_RD_ADDR)     // LTC2943 I2C Slave Read Address

// Registers
#define LTC2943_STATUS                          (0X00) // default: 0bX0000001 R
#define LTC2943_CONTROL                         (0X01) // default: 0x3C      R/W
#define LTC2943_ACCUMULATED_CHARGE              (0X02) // default: 0x7FFF    R/W
#define LTC2943_ACCUMULATED_CHARGE_MSB          (0X02) // default: 0x7F      R/W
#define LTC2943_ACCUMULATED_CHARGE_LSB          (0X03) // default: 0xFF      R/W
#define LTC2943_CHARGE_THRESHOLD_HIGH           (0X04) // default: 0xFFFF    R/W
#define LTC2943_CHARGE_THRESHOLD_HIGH_MSB       (0X04) // default: 0xFF      R/W
#define LTC2943_CHARGE_THRESHOLD_HIGH_LSB       (0X05) // default: 0xFF      R/W
#define LTC2943_CHARGE_THRESHOLD_LOW            (0X06) // default: 0x0000    R/W
#define LTC2943_CHARGE_THRESHOLD_LOW_MSB        (0X06) // default: 0x00      R/W
#define LTC2943_CHARGE_THRESHOLD_LOW_LSB        (0X07) // default: 0x00      R/W
#define LTC2943_VOLTAGE                         (0X08) // default: 0x0000     R
#define LTC2943_VOLTAGE_MSB                     (0X08) // default: 0x00       R
#define LTC2943_VOLTAGE_LSB                     (0X09) // default: 0x00       R
#define LTC2943_VOLTAGE_THRESHOLD_HIGH          (0X0A) // default: 0xFFFF    R/W
#define LTC2943_VOLTAGE_THRESHOLD_HIGH_MSB      (0X0A) // default: 0xFF      R/W
#define LTC2943_VOLTAGE_THRESHOLD_HIGH_LSB      (0X0B) // default: 0xFF      R/W
#define LTC2943_VOLTAGE_THRESHOLD_LOW           (0X0C) // default: 0x0000    R/W
#define LTC2943_VOLTAGE_THRESHOLD_LOW_MSB       (0X0C) // default: 0x00      R/W
#define LTC2943_VOLTAGE_THRESHOLD_LOW_LSB       (0X0D) // default: 0x00      R/W
#define LTC2943_CURRENT                         (0X0E) // default: 0x0000     R
#define LTC2943_CURRENT_MSB                     (0X0E) // default: 0x00       R
#define LTC2943_CURRENT_LSB                     (0X0F) // default: 0x00       R
#define LTC2943_CURRENT_THRESHOLD_HIGH          (0X10) // default: 0xFFFF    R/W
#define LTC2943_CURRENT_THRESHOLD_HIGH_MSB      (0X10) // default: 0xFF      R/W
#define LTC2943_CURRENT_THRESHOLD_HIGH_LSB      (0X11) // default: 0xFF      R/W
#define LTC2943_CURRENT_THRESHOLD_LOW           (0X12) // default: 0x0000    R/W
#define LTC2943_CURRENT_THRESHOLD_LOW_MSB       (0X12) // default: 0x00      R/W
#define LTC2943_CURRENT_THRESHOLD_LOW_LSB       (0X13) // default: 0x00      R/W
#define LTC2943_TEMPERATURE                     (0X14) // default: 0x0000     R
#define LTC2943_TEMPERATURE_MSB                 (0X14) // default: 0x00       R
#define LTC2943_TEMPERATURE_LSB                 (0X15) // default: 0x00       R
#define LTC2943_TEMPERATURE_THRESHOLD_HIGH      (0X16) // default: 0xFF00    R/W
#define LTC2943_TEMPERATURE_THRESHOLD_HIGH_MSB  (0X16) // default: 0xFF      R/W
#define LTC2943_TEMPERATURE_THRESHOLD_HIGH_LSB  (0X17) // default: 0x00      R/W

// Masks (e.g. if (STATUS & MASK) { yes(); } else { no(); } )
// A[7] is reserved
#define LTC2943_CURRENT_ALERT_MASK                    (0b01000000)// A[6] def: 0
#define LTC2943_ACCUMULATED_CHARGE_OVERFLOW_UNDERFLOW (0b00100000)// A[5] def: 0
#define LTC2943_TEMPERATURE_ALERT                     (0b00010000)// A[4] def: 0
#define LTC2943_CHARGE_ALERT_HIGH                     (0b00001000)// A[3] def: 0
#define LTC2943_CHARGE_ALERT_LOW                      (0b00000100)// A[2] def: 0
#define LTC2943_VOLTAGE_ALERT                         (0b00000010)// A[1] def: 0
#define LTC2943_UNDERVOLTAGE_LOCKOUT_ALERT            (0b00000001)// A[0] def: 1


bool Ltc2943WriteRegister1Byte( uint8_t reg, uint8_t data )
{
  if ( !SendStartI2c2() )       return false;
  if ( !WriteI2c2( ISWA ) )     return false;
  if ( !WriteI2c2( reg ) )      return false;
  if ( !WriteI2c2( data ) )     return false;
  if ( !SendStopI2c2() )        return false;

  return true;
}

bool Ltc2943WriteRegister2Bytes( uint8_t reg, uint8_t data1, uint8_t data2 )
{
  if ( !SendStartI2c2() )       return false;
  if ( !WriteI2c2( ISWA ) )     return false;
  if ( !WriteI2c2( reg ) )      return false;
  if ( !WriteI2c2( data1 ) )    return false;
  if ( !WriteI2c2( data2 ) )    return false;
  if ( !SendStopI2c2() )        return false;

  return true;
}

bool Ltc2943ReadRegister1Byte( uint8_t reg, uint8_t * data )
{
  if ( !SendStartI2c2() )       return false;
  if ( !WriteI2c2( ISWA ) )     return false;
  if ( !WriteI2c2( reg ) )      return false;
  if ( !SendRestartI2c2() )     return false;
  if ( !WriteI2c2( ISRA ) )     return false;
  if ( !ReadI2c2( data ) )      return false;
  if ( !SendNackI2c2() )        return false;
  if ( !SendStopI2c2() )        return false;

  return true;
}

bool Ltc2943ReadRegister2Bytes( uint8_t reg, uint8_t * data1, uint8_t * data2 )
{
  if ( !SendStartI2c2() )       return false;
  if ( !WriteI2c2( ISWA ) )     return false;
  if ( !WriteI2c2( reg ) )      return false;
  if ( !SendRestartI2c2() )     return false;
  if ( !WriteI2c2( ISRA ) )     return false;
  if ( !ReadI2c2( data1 ) )     return false;
  if ( !SendAckI2c2() )         return false;
  if ( !ReadI2c2( data2 ) )     return false;
  if ( !SendNackI2c2() )        return false;
  if ( !SendStopI2c2() )        return false;

  return true;
}


bool InitLtc2943(void)
{
  bool isOk;

  // Auto ADC mode.  M=256.  nALCC disabled.  Analog Shutdown disabled.
  isOk  = Ltc2943WriteRegister1Byte(LTC2943_CONTROL, 0b11100000);
  isOk &= Ltc2943WriteRegister2Bytes(LTC2943_CHARGE_THRESHOLD_HIGH_MSB,
                                     LTC2943_FULL_PT_MSB, LTC2943_FULL_PT_LSB);
  isOk &= Ltc2943WriteRegister2Bytes(LTC2943_CHARGE_THRESHOLD_LOW_MSB,
                                     LTC2943_ZERO_PT_MSB, LTC2943_ZERO_PT_LSB);
  return isOk;
}


bool GetLtc2943Charge(uint8_t * acrMSB, uint8_t * acrLSB)
{
  bool isOk;
  // Auto ADC mode.  M=256.  nALCC disabled.  Analog Shutdown enabled.
  isOk  = Ltc2943ReadRegister2Bytes(LTC2943_ACCUMULATED_CHARGE_MSB,
                                    acrMSB, acrLSB);

  return isOk;
}


bool SetLtc2943Charge(uint8_t acrMSB, uint8_t acrLSB)
{
  bool isOk;
  // Auto ADC mode.  M=256.  nALCC disabled.  Analog Shutdown enabled.
  isOk  = Ltc2943WriteRegister1Byte(LTC2943_CONTROL, 0b11100001);
  // ACR = 0xMSB_LSB
  isOk &= Ltc2943WriteRegister2Bytes(LTC2943_ACCUMULATED_CHARGE_MSB,
                                     acrMSB, acrLSB);
  // Auto ADC mode.  M=256.  nALCC disabled.  Analog Shutdown disabled.
  isOk &= Ltc2943WriteRegister1Byte(LTC2943_CONTROL, 0b11100000);

  return isOk;
}


//bool SetLtc2943FullCharge(void)
//{
//  bool isOk;
//  // Auto ADC mode.  M=256.  nALCC disabled.  Analog Shutdown enabled.
//  isOk  = Ltc2943WriteRegister1Byte(LTC2943_CONTROL, 0b11100001);
//  // ACR = Full charge
//  isOk &= Ltc2943WriteRegister2Bytes(LTC2943_ACCUMULATED_CHARGE_MSB,
//                                     LTC2943_FULL_PT_MSB, LTC2943_FULL_PT_LSB);
//  // Auto ADC mode.  M=256.  nALCC disabled.  Analog Shutdown disabled.
//  isOk &= Ltc2943WriteRegister1Byte(LTC2943_CONTROL, 0b11100000);
//
//  return isOk;
//}


#undef ISWA
#undef ISRA
