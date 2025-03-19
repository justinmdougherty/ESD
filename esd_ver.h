#ifndef ESD_VER_H
#define ESD_VER_H
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : esd_ver.h
 *
 *  DESCRIPTION   : Defines the string value of the ESD firmware version
 *    (with space for the related Lightning firmware's version)
 *
 * Firmware version is M.m.F.T (1-char {0..1,A..Z} each) where:
 *    M - major version, may not be backward compatible (e.g. hardware changes)
 *    m - minor version, new capabilities/features added
 *    F - version to fix bugs (not add features)
 *    T - test version to test changes before release
 *
 *  NOTE: vL_T_N_G - where code will place Lightning firmware version info
 *
 *  WRITTEN BY    : Robert Kirby, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2019/01/28, Robert Kirby, NSWC H12
 *      Remove FOG, LYNX_8, and LYNX_6 defines as move code beyond that.
 *    2018/12/17, Robert Kirby, NSWC H12
 *      Do not conditional compile firmware version, use sequencial T's
 *    2018/11/28, Robert Kirby, NSWC H12
 *      Add define for FOG to support expedient hard-code waveform switches
 *      Until Lightning can query BOLT waveforms and pass info to ESD, the ESD
 *      code design supports only one waveform at a time
 *      Use conditional compilation to finalize the firmware version designator
 *    2017/12/18, Robert Kirby, NSWC H12
 *      Add defines LYNX_6 and LYNX_8 to support easy switches between 6 & 8
 *    2017/08/30, Robert Kirby, NSWC H12
 *      Initial implementation
 */
//                   M.m.F.T (see version-ing description above)
#define FW_VER_STR "v1.6.0.D/vL_T_N_G"   // should be 17 characters string


#endif  // ESD_VER_H
