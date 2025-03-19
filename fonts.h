#ifndef FONTS_H
#define	FONTS_H
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : fonts.h
 *
 *  DESCRIPTION   : Define various font widths, heights, and size in bytes;
 *    declare the various constant font values -- all custom to be used with the
 *    UC1701X LCD display.  The naming convention is as follows:
 *
 * <FONT NAME>_<WIDTH>X<HEIGHT>_<PARAMETER>
 *
 * TERMINAL and VERDANA are alphanumeric fonts.
 * BATTERY and ENVELOPE are not so much fonts but sets of graphics to display
 * battery charge levels (i.e. fuel gauge) and an envelope icon.
 * Display BATTERY icons by calling LCDWriteFont.
 * Display ENVELOPE incon by calling LCDWriteStringTerminal6X8 w/ ENVELOPE_STR.
 *
 * WIDTH, in pixels, when multiplied by (1 column / 1 pixel), reveals the number
 * of display columns, which translates to the number of bytes per page (row).
 *
 * HEIGHT, in pixels, when multiplied by (1 page (row) / 8 pixels), reveals the
 * number of display pages, which translates to the number of bytes per column.
 * The height must be a multiple of 8.
 *
 *  WRITTEN BY    : Nicholus Sunshine, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/06/01, Robert Kirby, NSWC H12
 *      Add VERDANA_34X56_SPACE, stub out VERDANA_34X56_EQUALS
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Change VERDANA_34X64 to VERDANA_34X56, saves memory by getting rid of
 *      wasted white space above characters and allows displaying a row of 6X8
 *      Rename TERMINAL_6X8_ENVELOPE_LEFT/RIGHT to ENVELOPE_6X8_LEFT/RIGHT
 *      Rename defined WIDTHs and HEIGHTs
 *      Make BATTERY icons same width as three TERMINAL_6X8 for alignment ie 18
 *      Comments on what Nick developed for ESD
 *
 *  REFERENCE DOCUMENTS
 *    1.
 */
#include <xc.h>
#include <stdint.h>


#define WIDTH_6X8    (6)                // 6 bytes wide
#define HEIGHT_6X8   (1)                // 1 byte (page) high

#define WIDTH_12X16  (12)               // 12 bytes wide
#define HEIGHT_12X16 (2)                // 2 bytes (pages) high

#define WIDTH_18X8    (18)              // 18 bytes wide
#define HEIGHT_18X8   (1)               // 1 byte (page) high

#define WIDTH_34X56   (34)              // 34 bytes wide
#define HEIGHT_34X56  (7)               // 7 bytes (pages) high

// Stand-ins for envelope symbols, each half of envelope is 6x8 pixles
#define ENVELOPE_LEFT_CHAR    '\xF9'
#define ENVELOPE_RIGHT_CHAR   '\xFA'
#define ENVELOPE_STR          "\xF9\xFA"


// ENVELOPE 6X8
extern const uint8_t ENVELOPE_6X8_LEFT[];   // Left-side unread msg icon
extern const uint8_t ENVELOPE_6X8_RIGHT[];  // Right-side unread msg icon

// TERMINAL 6X8
extern const uint8_t TERMINAL_6X8_SPACE[];
extern const uint8_t TERMINAL_6X8_EXCLAMATION[];
extern const uint8_t TERMINAL_6X8_DOUBLEQUOTE[];
extern const uint8_t TERMINAL_6X8_HASH[];
extern const uint8_t TERMINAL_6X8_DOLLAR[];
extern const uint8_t TERMINAL_6X8_PERCENT[];
extern const uint8_t TERMINAL_6X8_AMPERSAND[];
extern const uint8_t TERMINAL_6X8_SINGLEQUOTE[];
extern const uint8_t TERMINAL_6X8_OPENPAREN[];
extern const uint8_t TERMINAL_6X8_CLOSEPAREN[];
extern const uint8_t TERMINAL_6X8_ASTERISK[];
extern const uint8_t TERMINAL_6X8_PLUS[];
extern const uint8_t TERMINAL_6X8_COMMA[];
extern const uint8_t TERMINAL_6X8_MINUS[];
extern const uint8_t TERMINAL_6X8_PERIOD[];
extern const uint8_t TERMINAL_6X8_FWDSLASH[];
extern const uint8_t TERMINAL_6X8_0[];
extern const uint8_t TERMINAL_6X8_1[];
extern const uint8_t TERMINAL_6X8_2[];
extern const uint8_t TERMINAL_6X8_3[];
extern const uint8_t TERMINAL_6X8_4[];
extern const uint8_t TERMINAL_6X8_5[];
extern const uint8_t TERMINAL_6X8_6[];
extern const uint8_t TERMINAL_6X8_7[];
extern const uint8_t TERMINAL_6X8_8[];
extern const uint8_t TERMINAL_6X8_9[];
extern const uint8_t TERMINAL_6X8_COLON[];
extern const uint8_t TERMINAL_6X8_SEMICOLON[];
extern const uint8_t TERMINAL_6X8_LESS[];
extern const uint8_t TERMINAL_6X8_EQUALS[];
extern const uint8_t TERMINAL_6X8_GREATER[];
extern const uint8_t TERMINAL_6X8_QUESTION[];
extern const uint8_t TERMINAL_6X8_AT[];
extern const uint8_t TERMINAL_6X8_A[];
extern const uint8_t TERMINAL_6X8_B[];
extern const uint8_t TERMINAL_6X8_C[];
extern const uint8_t TERMINAL_6X8_D[];
extern const uint8_t TERMINAL_6X8_E[];
extern const uint8_t TERMINAL_6X8_F[];
extern const uint8_t TERMINAL_6X8_G[];
extern const uint8_t TERMINAL_6X8_H[];
extern const uint8_t TERMINAL_6X8_I[];
extern const uint8_t TERMINAL_6X8_J[];
extern const uint8_t TERMINAL_6X8_K[];
extern const uint8_t TERMINAL_6X8_L[];
extern const uint8_t TERMINAL_6X8_M[];
extern const uint8_t TERMINAL_6X8_N[];
extern const uint8_t TERMINAL_6X8_O[];
extern const uint8_t TERMINAL_6X8_P[];
extern const uint8_t TERMINAL_6X8_Q[];
extern const uint8_t TERMINAL_6X8_R[];
extern const uint8_t TERMINAL_6X8_S[];
extern const uint8_t TERMINAL_6X8_T[];
extern const uint8_t TERMINAL_6X8_U[];
extern const uint8_t TERMINAL_6X8_V[];
extern const uint8_t TERMINAL_6X8_W[];
extern const uint8_t TERMINAL_6X8_X[];
extern const uint8_t TERMINAL_6X8_Y[];
extern const uint8_t TERMINAL_6X8_Z[];
extern const uint8_t TERMINAL_6X8_OPENBRACKET[];
extern const uint8_t TERMINAL_6X8_BACKSLASH[];
extern const uint8_t TERMINAL_6X8_CLOSEDBRACKET[];
extern const uint8_t TERMINAL_6X8_CARET[];
extern const uint8_t TERMINAL_6X8_UNDERSCORE[];
extern const uint8_t TERMINAL_6X8_GRAVE[];
extern const uint8_t TERMINAL_6X8_a[];
extern const uint8_t TERMINAL_6X8_b[];
extern const uint8_t TERMINAL_6X8_c[];
extern const uint8_t TERMINAL_6X8_d[];
extern const uint8_t TERMINAL_6X8_e[];
extern const uint8_t TERMINAL_6X8_f[];
extern const uint8_t TERMINAL_6X8_g[];
extern const uint8_t TERMINAL_6X8_h[];
extern const uint8_t TERMINAL_6X8_i[];
extern const uint8_t TERMINAL_6X8_j[];
extern const uint8_t TERMINAL_6X8_k[];
extern const uint8_t TERMINAL_6X8_l[];
extern const uint8_t TERMINAL_6X8_m[];
extern const uint8_t TERMINAL_6X8_n[];
extern const uint8_t TERMINAL_6X8_o[];
extern const uint8_t TERMINAL_6X8_p[];
extern const uint8_t TERMINAL_6X8_q[];
extern const uint8_t TERMINAL_6X8_r[];
extern const uint8_t TERMINAL_6X8_s[];
extern const uint8_t TERMINAL_6X8_t[];
extern const uint8_t TERMINAL_6X8_u[];
extern const uint8_t TERMINAL_6X8_v[];
extern const uint8_t TERMINAL_6X8_w[];
extern const uint8_t TERMINAL_6X8_x[];
extern const uint8_t TERMINAL_6X8_y[];
extern const uint8_t TERMINAL_6X8_z[];
extern const uint8_t TERMINAL_6X8_OPENBRACE[];
extern const uint8_t TERMINAL_6X8_CLOSEDBRACE[];
extern const uint8_t TERMINAL_6X8_VERTBAR[];

// TERMINAL 12X16
extern const uint8_t TERMINAL_12X16_SPACE[];
extern const uint8_t TERMINAL_12X16_EXCLAMATION[];
extern const uint8_t TERMINAL_12X16_DOUBLEQUOTE[];
extern const uint8_t TERMINAL_12X16_HASH[];
extern const uint8_t TERMINAL_12X16_DOLLAR[];
extern const uint8_t TERMINAL_12X16_PERCENT[];
extern const uint8_t TERMINAL_12X16_AMPERSAND[];
extern const uint8_t TERMINAL_12X16_SINGLEQUOTE[];
extern const uint8_t TERMINAL_12X16_OPENPAREN[];
extern const uint8_t TERMINAL_12X16_CLOSEPAREN[];
extern const uint8_t TERMINAL_12X16_ASTERISK[];
extern const uint8_t TERMINAL_12X16_PLUS[];
extern const uint8_t TERMINAL_12X16_COMMA[];
extern const uint8_t TERMINAL_12X16_MINUS[];
extern const uint8_t TERMINAL_12X16_PERIOD[];
extern const uint8_t TERMINAL_12X16_FWDSLASH[];
extern const uint8_t TERMINAL_12X16_0[];
extern const uint8_t TERMINAL_12X16_1[];
extern const uint8_t TERMINAL_12X16_2[];
extern const uint8_t TERMINAL_12X16_3[];
extern const uint8_t TERMINAL_12X16_4[];
extern const uint8_t TERMINAL_12X16_5[];
extern const uint8_t TERMINAL_12X16_6[];
extern const uint8_t TERMINAL_12X16_7[];
extern const uint8_t TERMINAL_12X16_8[];
extern const uint8_t TERMINAL_12X16_9[];
extern const uint8_t TERMINAL_12X16_COLON[];
extern const uint8_t TERMINAL_12X16_SEMICOLON[];
extern const uint8_t TERMINAL_12X16_LESS[];
extern const uint8_t TERMINAL_12X16_EQUALS[];
extern const uint8_t TERMINAL_12X16_GREATER[];
extern const uint8_t TERMINAL_12X16_QUESTION[];
extern const uint8_t TERMINAL_12X16_AT[];
extern const uint8_t TERMINAL_12X16_A[];
extern const uint8_t TERMINAL_12X16_B[];
extern const uint8_t TERMINAL_12X16_C[];
extern const uint8_t TERMINAL_12X16_D[];
extern const uint8_t TERMINAL_12X16_E[];
extern const uint8_t TERMINAL_12X16_F[];
extern const uint8_t TERMINAL_12X16_G[];
extern const uint8_t TERMINAL_12X16_H[];
extern const uint8_t TERMINAL_12X16_I[];
extern const uint8_t TERMINAL_12X16_J[];
extern const uint8_t TERMINAL_12X16_K[];
extern const uint8_t TERMINAL_12X16_L[];
extern const uint8_t TERMINAL_12X16_M[];
extern const uint8_t TERMINAL_12X16_N[];
extern const uint8_t TERMINAL_12X16_O[];
extern const uint8_t TERMINAL_12X16_P[];
extern const uint8_t TERMINAL_12X16_Q[];
extern const uint8_t TERMINAL_12X16_R[];
extern const uint8_t TERMINAL_12X16_S[];
extern const uint8_t TERMINAL_12X16_T[];
extern const uint8_t TERMINAL_12X16_U[];
extern const uint8_t TERMINAL_12X16_V[];
extern const uint8_t TERMINAL_12X16_W[];
extern const uint8_t TERMINAL_12X16_X[];
extern const uint8_t TERMINAL_12X16_Y[];
extern const uint8_t TERMINAL_12X16_Z[];
extern const uint8_t TERMINAL_12X16_OPENBRACKET[];
extern const uint8_t TERMINAL_12X16_BACKSLASH[];
extern const uint8_t TERMINAL_12X16_CLOSEDBRACKET[];
extern const uint8_t TERMINAL_12X16_CARET[];
extern const uint8_t TERMINAL_12X16_UNDERSCORE[];
extern const uint8_t TERMINAL_12X16_GRAVE[];
extern const uint8_t TERMINAL_12X16_a[];
extern const uint8_t TERMINAL_12X16_b[];
extern const uint8_t TERMINAL_12X16_c[];
extern const uint8_t TERMINAL_12X16_d[];
extern const uint8_t TERMINAL_12X16_e[];
extern const uint8_t TERMINAL_12X16_f[];
extern const uint8_t TERMINAL_12X16_g[];
extern const uint8_t TERMINAL_12X16_h[];
extern const uint8_t TERMINAL_12X16_i[];
extern const uint8_t TERMINAL_12X16_j[];
extern const uint8_t TERMINAL_12X16_k[];
extern const uint8_t TERMINAL_12X16_l[];
extern const uint8_t TERMINAL_12X16_m[];
extern const uint8_t TERMINAL_12X16_n[];
extern const uint8_t TERMINAL_12X16_o[];
extern const uint8_t TERMINAL_12X16_p[];
extern const uint8_t TERMINAL_12X16_q[];
extern const uint8_t TERMINAL_12X16_r[];
extern const uint8_t TERMINAL_12X16_s[];
extern const uint8_t TERMINAL_12X16_t[];
extern const uint8_t TERMINAL_12X16_u[];
extern const uint8_t TERMINAL_12X16_v[];
extern const uint8_t TERMINAL_12X16_w[];
extern const uint8_t TERMINAL_12X16_x[];
extern const uint8_t TERMINAL_12X16_y[];
extern const uint8_t TERMINAL_12X16_z[];
extern const uint8_t TERMINAL_12X16_OPENBRACE[];
extern const uint8_t TERMINAL_12X16_VERTBAR[];
extern const uint8_t TERMINAL_12X16_CLOSEDBRACE[];
extern const uint8_t TERMINAL_12X16_TILDE[];

// BATTERY_18X8
extern const uint8_t BATTERY_18X8_0[];
extern const uint8_t BATTERY_18X8_1[];
extern const uint8_t BATTERY_18X8_2[];
extern const uint8_t BATTERY_18X8_3[];
extern const uint8_t BATTERY_18X8_4[];
extern const uint8_t BATTERY_18X8_5[];
extern const uint8_t BATTERY_18X8_6[];
extern const uint8_t BATTERY_18X8_7[];
extern const uint8_t BATTERY_18X8_8[];
extern const uint8_t BATTERY_18X8_9[];
extern const uint8_t BATTERY_18X8_10[];
extern const uint8_t BATTERY_18X8_11[];

// VERDANA 34X56
extern const uint8_t VERDANA_34X56_SPACE[];
extern const uint8_t VERDANA_34X56_0[];
extern const uint8_t VERDANA_34X56_1[];
extern const uint8_t VERDANA_34X56_2[];
extern const uint8_t VERDANA_34X56_3[];
extern const uint8_t VERDANA_34X56_4[];
extern const uint8_t VERDANA_34X56_5[];
extern const uint8_t VERDANA_34X56_6[];
extern const uint8_t VERDANA_34X56_7[];
extern const uint8_t VERDANA_34X56_8[];
extern const uint8_t VERDANA_34X56_9[];
// NRS 20160811: Colon, semicolon, less-than, equals, and greater-than are not
// needed in the program right now.  Save program space by not defining them.
//extern const uint8_t VERDANA_34X56_COLON[];
//extern const uint8_t VERDANA_34X56_SEMICOLON[];
//extern const uint8_t VERDANA_34X56_LESS[];
//extern const uint8_t VERDANA_34X56_EQUALS[];
//extern const uint8_t VERDANA_34X56_GREATER[];

#endif /* FONTS_H */
