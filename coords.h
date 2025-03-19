#ifndef COORDS_H
#define COORDS_H
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : coords.h
 *
 *  DESCRIPTION   : defines the macros and declares procedures required to
 *    convert between geographic coordinate systems and compute range & bearing
 *
 *    (1) double HexDegToDblDeg(char* hexDeg, bool isLat)
 *    (2) void ClearCoords(coords_t *coord)
 *    (3) bool SetCoordsFromDecLatLon(coords_t *pCoord, char *pLat, char *pLon)
 *    (4) void DecLatLonToDblLatLon(coords_t *pCoord)
 *    (5) void DblLatLonToDMS(coords_t *pCoord)
 *    (6) void DblLatLonToMGRS(coords_t *pCoord)
 *    (7) void CoordsToGeopt16(coords_t *pCoord, geopt16_t* geoPt)
 *    (8) bool CalcRngBrg(double frLat, double frLon,
 *                        double toLat, double toLon,
 *                        char*  rng,   char* brg)
 *
 *  NOTE - Currently utilizing WGS-84 ellipsoid reference.
 *
 *  NOTE: The 2014 MGRS standard (ref 1) provides more correct answers...
 *        but 1989 MGRS standard (ref 3) is the de facto standard...
 *        Use #define MGRS_STD below to select standard to be used (compiled)
 *
 *  NOTE - Reference (1) denotes the most recent "version" of calculating
 *         MGRS from Latitude/Longitude. It utilizes an extention
 *         of the Krueger series which should result in accuracy to
 *         a few nanometers and run about 5-6 faster than the
 *         exact methods by Thompson and Lee (Ref 2).
 *
 *  NOTE - MGRS, if following the standards in Reference (1) will output a
 *         single string without any whitespace. USNG (also according to Ref 1),
 *         also outputs a single string but with whitespace for ease of reading.
 *         This implementation is a variation of the USNG output: two strings
 *         are output (which overwrite the space of latitude and longitude)
 *         and resemble the following format for UTM:
 *              "47T DK    "
 *              "5963 2839 "
 *         For polar regions (UPS), the output resembles the following format:
 *              "  ZJ H    "
 *              "5963 2839 "
 *
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/10/06, Robert Kirby, NSWC H12
 *      Add HexDegToDblDeg(), ClearCoords(), SetCoordsFromDecLatLon(),
 *      DecLatLonToDblLatLon(), DblLatLonToDMS(), and CalcRngBrg()
 *      Rework LatLonToMGRS() to DblLatLonToMGRS()
 *      and LatLonToGeopt16() to CoordsToGeopt16()
 *      Delete now obsolete ValidateDMS()
 *      Move #define selecting MGRS standard conditional compile to this file
 *      Move #define COORD_LEN from main.c to this file, #define COORD_ERR
 *      Typedef coords_t for use in main.c and coords.c
 *    2021/06/01, Robert Kirby, NSWC H12
 *      Add LatLonToGeopt16() to support geo-muting, update comments
 *    2017/04/07, Robert Kirby, NSWC H12
 *      Correct UPS Grid Zone Designator string in comment above
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Remove method prototypes consumer should never need.
 *      Move implementation specific defines to the .c file
 *      Merge, for conditional compilation, both 2014 and 1984 versions
 *      Comment update.
 *    2017/03/02, Megan Kozub, NSWC V14
 *      Initial development
 *
 *  REFERENCE DOCUMENTS
 *    1. National Geospatial-Intelligence Agency Standardization Document
 *       Implementation Practice
 *       "The Universal Grids and the Transverse Mercator and Polar
 *       Stereographic Map Projections"
 *       Version 2 - 2014-03-25
 *       http://earth-info.nga.mil/GandG/publications/NGA_SIG_0012_02_0_0_UTMUPS
 *       /NGA.SIG.0012_2.0.0_UTMUPS.pdf
 *
 *    2. "Transverse Mercator with an accuracy of a few nanometers"
 *       Charles Karney (2011) Journal of Geodesy Vol. 85, No. 8, pp475-485
 *       https://arxiv.org/pdf/1002.1417.pdf
 *
 *    3. Defense Mapping Agency (DMA) Technical Manual DMATM 8358.2
 *       "The Universal Grids: Universal Transverse Mercator (UTM) and
 *       Universal Polar Stereographic (UPS)"
 *       1989
 *       http://earth-info.nga.mil/GandG/publications/tm8358.2/TM8358_2.pdf
 *
 *    4. U.S. Geological Survey Professional Paper 1395
 *       "Map Projections - A Working Manual"
 *       1987
 *       https://pubs.usgs.gov/pp/1395/report.pdf
 *
 *    5. Map Projections Used by the U.S. Geological Survey
 *       "Geological Survey Bulletin 1532"
 *       Second Edition - 1984
 *
 *    6. Great-Circle Navigation - Course
 *       https://en.wikipedia.org/wiki/Great-circle_navigation#Course
 *       Great-Circle Distance - Formulae
 *       https://en.wikipedia.org/wiki/Great-circle_distance#Formulae
 *       CQSRG WGS-84 World Geodetic System Distance Calculator
 *       https://www.cqsrg.org/tools/GCDistance/
 *       Calculate distance, bearing and more between Latitude/Longitude points
 *       http://www.movable-type.co.uk/scripts/latlong.html
 *       Distance and Azimuths Between Two Sets of Coordinates
 *       https://www.fcc.gov/media/radio/distance-and-azimuths
 */

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Define either    1989  - or -  2014
#define MGRS_STD    1989      // 1989 is the defacto standard even though ...
//#define MGRS_STD    2014    // ... 2014 is gives more-correct answers
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  double HexDegToDblDeg(char* hexDeg, bool isLat)
//  Converts hexadecimal representation of 32-bit scaled degrees to double.
//
//  INPUT : char * - hextext representation of scaled degrees value to convert
//          bool   - isLat indicates if this is a latitude (for range checks)
//  OUTPUT: double - returns COORD_ERR on error, otherwise degrees represented
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ClearCoords(coords_t *pCoord)
//  Clears/Initializes a coords_t structure to the geo-strings ' ' filled and
//  the double values are set to COORD_ERR.
//
//  INPUT : coords_t * - structure to clear
//  OUTPUT: NONE (other than the updating structure)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool SetCoordsFromDecLatLon(coords_t *pCoord, char *pLat, char *pLon)
//  Populates entire coords_t structure based on lat & lon input of fixed-width
//  form <sDDD.ddddd> (e.g. +012.01234 or -180.00000).
//
//  INPUT : coords_t *pCoord - structure to populate
//          char     *pLat   - latitude  in fixed width form <sDDD.ddddd>
//          char     *pLon   - longitude in fixed width form <sDDD.ddddd>
//  OUTPUT: bool - returns true if input lat/lon representation valid
//          updates structure (invokes ClearCoords() if lat/lon invalid)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void DecLatLonToDblLatLon(coords_t *pCoord)
//  Converts fixed length sDDD.DDDDD representation of decimal degrees to double
//
//  INPUT : coords_t *pCoord - structure to update
//  OUTPUT: updates structure using COORD_ERR if appropriate
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void DblLatLonToDMS(coords_t *pCoord)
//  Converts lat and lon to corresponding fixed length HDDD.MM.SS characters.
//
//  INPUT : coords_t *pCoord - structure to update
//  OUTPUT: updates structure lat/lon with <HDDD.MM.SS> chars
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void DblLatLonToMGRS(coords_t *pCoord)
//  Converts lat/lon coordinate to geopt16_t without verifying inputs.
//
//  INPUT : coords_t *pCoord - structure to update
//  OUTPUT: updates structure mgrsGzd and mgrs10m
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void CoordsToGeopt16(coords_t *pCoord, geopt16_t* geoPt)
//  Converts coordinate in coords_t to geopt16_t without verifying inputs.
//
//  INPUT : coords_t *pCoord - struct holding inputs dblLat and dblLon
//          geopt16_t* geoPt - struct to be updated if conversion successful
//  OUTPUT: updates geoPt
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool CalcRngBrg(double frLat, double frLon,
//                  double toLat, double toLon,
//                  char*  range, char*  bearing)
//  Calculate the geodetic vector from one lat/lon to another, placing resultant
//  range and bearing as 3-characters ASCII into buffers while returning whether
//  or not the range is in kilometers (or meters).
//
//  INPUT : double frLat - input start point latitude  ( -90.0 <= lat <=  +90.0)
//          double frLon - input start point longitude (-180.0 <= lon <= +180.0)
//          double toLat - input end point latitude  ( -90.0 <= lat <=  +90.0)
//          double toLon - input end point longitude (-180.0 <= lon <= +180.0)
//          char*  range - return 3-char range (meters, km, or ### [OutOfRange])
//          char*bearing - return 3-char bearing in degrees (000-359)
//  OUTPUT: always updates range and bearing char buffers...
//          returns false if range is less than 1 km, otherwise true; plus:...
//          if range is > 999 km the range buffer will be filled w/ '#' chars...
//          if there's an error both range & bearing buffers get filled w/ '!'
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <stdbool.h>
#include "geofence.h"     // for geopt16_t

//----- DEFINES, ENUMS, STRUCTS, TYPEDEFS, ETC. --------------------------------
#define COORD_ERR -666.666// special value to indicate error in coord_t
#define COORD_LEN   10    // LAT/LON, or padded MGRS chars displayed on line
typedef char geostr_t[COORD_LEN+1]; // add byte for NULL terminator

typedef struct tagGEO_COORDINATE_DATA
{ // order of fields matter because memset used to clear out the geostr_t fields
  geostr_t  decLat;       // <+DDD.ddddd> ~1m LAT
  geostr_t  decLon;       // <+DDD.ddddd> ~1m LON
  geostr_t  dmsLat;       // <NDDD.MM.SS> ~31m LAT
  geostr_t  dmsLon;       // <EDDD.MM.SS> ~31m LON
  geostr_t  mgrsGzd;      // <GZsi      >  10m MGRS grid-zone
  geostr_t  mgrs10m;      // <1234 5678 >  10m MGRS easting & northing
  double    dblLat;       // XC16 v1.24 double is the same as float
  double    dblLon;       //
} coords_t;


//----- EXPOSED ATTRIBUTES -----------------------------------------------------


//----- EXPOSED PROCEDURES -----------------------------------------------------
double HexDegToDblDeg(char* hexDeg, bool isLat);
void ClearCoords(coords_t *coord);
bool SetCoordsFromDecLatLon(coords_t *pCoord, char *pLat, char *pLon);
void DecLatLonToDblLatLon(coords_t *pCoord);
void DblLatLonToDMS(coords_t *pCoord);
void DblLatLonToMGRS(coords_t *pCoord);
void CoordsToGeopt16(coords_t *pCoord, geopt16_t* geoPt);
bool CalcRngBrg(double frLat,double frLon,double toLat,double toLon,char* rng,char* brg);


//----- MACROS -----------------------------------------------------------------


#endif  // COORDS_H