////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : coords.c
 *
 *  DESCRIPTION   : contains procedure definitions required to convert between
 *    geographic coordinate systems and compute range & bearing
 *
 *    (*) MGRS_1989_STD void LatLonToUTM(lat, lon, * mgrsZones, * mgrsCoords)
 *    (*) MGRS_2014_STD double ComputeUtmX(double lamda, double phi)
 *    (*) MGRS_2014_STD double ComputeUtmY(double lamda, double phi)
 *    (*) MGRS_2014_STD void LatLonToUTM(lat, lon, * mgrsZones, * mgrsCoords)
 *    (*) ComputeUpsX(double lamda, double phi, double k90)
 *    (*) ComputeUpsY(double lamda, double phi, double k90)
 *    (*) void LatLonToUPS(lat, lon, * mgrsZones, * mgrsCoords)
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
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/09/23, Robert Kirby, NSWC H12
 *      Add HexDegToDblDeg(), ClearCoords(), SetCoordsFromDecLatLon(),
 *      DecLatLonToDblLatLon(), DblLatLonToDMS(), and CalcRngBrg()
 *      Rework LatLonToMGRS() to DblLatLonToMGRS()
 *      and LatLonToGeopt16() to CoordsToGeopt16()
 *      Delete now obsolete ValidateDMS()
 *      Cosmetic changes to conditional compile for MGRS standard in use
 *    2021/06/01, Robert Kirby, NSWC H12
 *      Add LatLonToGeopt16() to support geo-muting
 *    2017/04/18, Robert Kirby, NSWC H12
 *      Table of Contents (*) above
 *    2017/04/07, Robert Kirby, NSWC H12
 *      Left justify MGRS data when overwriting latitude and longitude
 *      In LatLonToMGRS() minimize code overwriting lat/lon, now after single if
 *      Add space between Grid-Zone Designator and 100km Square Identifier
 *      In LatLonToUPS() rename eastingLetter to more appropriate eastingStr
 *      Correct date in reference to old MGRS standard, i.e.  1984 to 1989
 *    2017/03/31, Robert Kirby, NSWC H12
 *      Copy method description comment blocks from the .h file
 *      Prototype non-consumer methods in this file rather than in the .h
 *      Place implementation specific defines here rather than in the .h
 *      Merge, for conditional compilation, both 2014 and 1989 versions
 *      Call ValidateDMS of both latitude & longitude in one 'if' statement
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
#include <dsp.h>                // for PI optimized to XC16 double
#include <float.h>              // for DBL_MIN
#include <math.h>               // for trig functions
#include <stdio.h>              // for sprintf
#include <stdlib.h>             // for strtoul (atoi)
#include <stdbool.h>            // for bool true false
#include <string.h>             // for strcmp, memcpy, etc.
#include "coords.h"


#define DEG_TO_RAD(dblDeg)  ((dblDeg) * PI/180.0)
#define RAD_TO_DEG(dblRad)  ((dblRad) * 180.0/PI)

// GPS uses WGS84.  WGS84 radii in meters:
//    equatorial Re  = 6378137   (semi-major axis)
//    polar      Rp ~= 6356752   (semi-minor axis)
//    mean       Rm ~= 6371009 = ((2*Re + Rp)/3)
#define MER_M   6371008.7714            // WGS-84 Mean Earth Radius in meters
#define ME_MPD  DEG_TO_RAD(MER_M)       // WGS-84 Mean Earth meters per degree
#define WGS_84_SEMI_MAJOR_AXIS 6378137  // WGS-84 Equatorial radius
//#define WGS_84_SEMI_MINOR_AXIS 6356752.314245
#define e  0.081819190842621494335
#define e2 0.0066943799901413169961
#define K0_UTM 0.9996
#define K0_UPS 0.994

//Currently using WGS 84 ellipsoid reference
#define alpha WGS_84_SEMI_MAJOR_AXIS
//#define beta WGS_84_SEMI_MINOR_AXIS


#if (MGRS_STD == 1989)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Code specifically for MGRS_1989_STD
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
 * LatLonToUTM MGRS_1989_STD
 * Inputs: latitude (decimal degrees)
 *         longitude (decimal degrees)
 * Outputs: MGRS strings
 * Function: Converts the latitude longitude to UTM using old methods
 *           and then formats for Military Grid Reference System (MGRS)
 */
static void LatLonToUTM(double latitude, double longitude, char* mgrsZones, char* mgrsCoords)
{
  if(latitude < -80 || latitude >= 84) {
    mgrsZones = " ";
    mgrsCoords = " ";
    return; //needs to use UPS instead
  }

  int zone = (int)((longitude + 180) / 6) + 1;
  if(latitude < 0)
  {
    zone = -zone;
  }

  if(zone == 31 && 56 <= latitude && latitude < 64 && longitude >= 3)
  {
    zone = 32;
  }
  else if(zone == 32 && latitude >= 72)
  {
    if(longitude < 9)
    {
      zone = 31;
    }
    else
    {
      zone = 33;
    }
  }
  else if(zone == 34 && latitude >= 72)
  {
    if(longitude < 21)
    {
      zone = 33;
    }
    else
    {
      zone = 35;
    }
  }
  else if(zone == 36 && latitude >= 72)
  {
    if(longitude < 33)
    {
      zone = 35;
    }
    else
    {
      zone = 37;
    }
  }

  char *letters = "CDEFGHJKLMNPQRSTUVWXX";
  int latBandIndex = 9 + (int)(latitude / 8);
  if(latitude > 0)
  {
    latBandIndex++;
  }

  char latitudeBandLetter = letters[latBandIndex];

  double phi = latitude * PI / 180;
  double lamda0 = (-183 + (6 * abs(zone))) * PI / 180;


  double ePrimeSquared = e * e / (1 - e * e); //eccentricity prime squared

  double N = alpha / sqrt(1 - pow(e * sin(phi), 2));
  double T = pow(tan(phi), 2);
  double C = ePrimeSquared * pow(cos(phi), 2);
  //double A = ((longitude * PI / 180) - (lamda0 * PI / 180)) * cos(phi);
  double A = ((longitude * PI / 180) - lamda0) * cos(phi);

  double M = (1 - (e2 / 4) - (3 * pow(e2, 2) / 64) - (5 * pow(e2, 3) / 256)) * phi;
  M -= ((3 * e2 / 8) + (3 * pow(e2, 2) / 32) + (45 * pow(e2, 3) / 1024)) * sin(2 * phi);
  M += ((15 * pow(e2, 2) / 256) + (45 * pow(e2, 3) / 1024)) * sin(4 * phi);
  M -= (35 * pow(e2, 3) / 3072) * sin(6 * phi);
  M *= alpha;

  //easting relative to central meridian
  double east = (K0_UTM * N * (A + (1.0 - T + C) * pow(A, 3) / 6.0
              + (5.0 - (18.0 * T) + pow(T, 2)
              + (72.0 * C) - (58.0 * ePrimeSquared)) * pow(A, 5) / 120.0));

  east += 500000.0; //add false easting

  long easting = floor(east);

  //northing from equator
  double north = (K0_UTM * (M + N * tan(phi)
               * (pow(A, 2) / 2 + (5 - T + 9 * C + 4 * pow(C, 2))
               * pow(A, 4) / 24 + (61 - 58 * T + pow(T, 2)
               + 600 * C - 330 * ePrimeSquared) * pow(A, 6) / 720)));

  if(north < 0)                   {     //if southern hemisphere
    north += 10000000.0;          }     //add false northing

  long northing = floor(north);

  if(latitude < 0)
  {
    northing = 10000000 + northing;
  }

  int slicesFromOrigin = floor(northing / 100000.0);

  if(!(zone % 2)) //if zone is not odd
  {
    slicesFromOrigin += 5; //start w/ 'F'
  }

  letters = "ABCDEFGHJKLMNPQRSTUVWXYZ";
  int sliceId;
  char eastingLetter;

  if(easting > 800000)                        {
    sliceId = 7;                              }
  else                                        {
    sliceId = ((easting / 100000.0) - 1.0);   }

  if((zone % 3) == 0)                         {
    eastingLetter = letters[16 + sliceId];    }
  else if((zone % 3) == 1)                    {
    eastingLetter = letters[sliceId];         }
  else                                        {
    eastingLetter = letters[8 + sliceId];     }

  char northingLetter = letters[(int)(slicesFromOrigin - 20 * floor(slicesFromOrigin / 20))]; //start with 'A'

  easting = (easting % 100000) / 10;
  northing = (northing % 100000) /10;

  sprintf(mgrsZones,  "%02d%c %c%c", abs(zone), latitudeBandLetter, eastingLetter, northingLetter);
  sprintf(mgrsCoords, "%04ld %04ld", easting, northing);
} // end LatLonToUTM MGRS_1989_STD

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// end MGRS_1989_STD specific code
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#elif (MGRS_STD == 2014)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Code specifically for MGRS_2014_STD
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define R4 6367449.1458234153093
#define a2 8.3773182062446983032E-04
#define a4 7.608527773572489156E-07


/*
 * ComputeUtmX MGRS_2014_STD
 * Inputs: lamda (radians)
 *         phi (radians)
 * Outputs: x value
 * Function: Calculates x of the Krueger series based conversion
 *           from latitude/longitude to Universal Tranverse Mercator (UTM)
 */
static double ComputeUtmX(double lamda, double phi)
{
  double sinPhi = sin(phi);
  double sinLamda = sin(lamda);

  double P = pow(((1 + (e * sinPhi)) / (1 - (e * sinPhi))), (e / 2)); // exp(e * Atanh(e * sinPhi))
  //chi (X) is "conformal latitude"
  double cosChi = 2 * cos(phi) / ((1 + sinPhi) / P + (1 - sinPhi) / P);
  double sinChi = ((1 + sinPhi) / P - (1 - sinPhi) / P) / ((1 + sinPhi) / P + (1 - sinPhi) / P);

  long double u = log((1.0 + (cosChi * sinLamda)) / (1.0 - (cosChi * sinLamda))) / 2; //atanh(cosChi * sinLamda);
  double v = atan2(sinChi, cosChi * cos(lamda));

  return R4 * (u + (a2 * sinh(2 * u) * cos(2 * v)) + (a4 * sinh(4 * u) * cos(4 * v)));
} // end ComputeUtmX MGRS_2014_STD


/*
 * ComputeUtmY MGRS_2014_STD
 * Inputs: lamda (radians)
 *         phi (radians)
 * Outputs: y value
 * Function: Calculates y of the Krueger series based conversion
 *           from latitude/longitude to Universal Tranverse Mercator (UTM)
 */
static double ComputeUtmY(double lamda, double phi)
{
  double sinPhi = sin(phi);
  double sinLamda = sin(lamda);

  double P = pow(((1 + (e * sinPhi)) / (1 - (e * sinPhi))), (e / 2)); // exp(e * Atanh(e * sinPhi))
  //chi (X) is "conformal latitude"
  double cosChi = 2 * cos(phi) / ((1 + sinPhi) / P + (1 - sinPhi) / P);
  double sinChi = ((1 + sinPhi) / P - (1 - sinPhi) / P) / ((1 + sinPhi) / P + (1 - sinPhi) / P);

  double u = log((1 + (cosChi * sinLamda)) / (1 - (cosChi * sinLamda))) / 2; //atanh(cosChi * sinLamda);
  double v = atan2(sinChi, cosChi * cos(lamda));

  return R4 * (v + (a2 * cosh(2 * u) * sin(2 * v)) + (a4 * cosh(4 * u) * sin(4 * v)));
} // end ComputeUtmY MGRS_2014_STD


/*
 * LatLonToUTM MGRS_2014_STD
 * Inputs: latitude (decimal degrees)
 *         longitude (decimal degrees)
 * Outputs: MGRS strings
 * Function: Converts the latitude longitude to UTM
 *           and then formats for Military Grid Reference System (MGRS)
 */
static void LatLonToUTM(double latitude, double longitude, char* mgrsZones, char* mgrsCoords)
{
  if(latitude < -80 || latitude >= 84) {
    mgrsZones = " ";
    mgrsCoords = " ";
    return; //needs to use UPS instead
  }

  int zone = (int)((longitude + 180) / 6) + 1;
  if(latitude < 0)
  {
    zone = -zone;
  }

  if(zone == 31 && 56 <= latitude && latitude < 64 && longitude >= 3)
  {
    zone = 32;
  }
  else if(zone == 32 && latitude >= 72)
  {
    if(longitude < 9)
    {
      zone = 31;
    }
    else
    {
      zone = 33;
    }
  }
  else if(zone == 34 && latitude >= 72)
  {
    if(longitude < 21)
    {
      zone = 33;
    }
    else
    {
      zone = 35;
    }
  }
  else if(zone == 36 && latitude >= 72)
  {
    if(longitude < 33)
    {
      zone = 35;
    }
    else
    {
      zone = 37;
    }
  }

  char *letters = "CDEFGHJKLMNPQRSTUVWXX";
  int latBandIndex = 9 + (int)(latitude / 8);
  if(latitude > 0)
  {
    latBandIndex++;
  }

  char latitudeBandLetter = letters[latBandIndex];

  double phi = latitude * PI / 180;
  double lamda = longitude * PI / 180;
  double lamda0 = (-183 + (6 * abs(zone))) * PI / 180;
  double x_origin = 500000; //"false easting"
  double y_origin = 0; //"false northing"
  if(zone < 0)                      {
    y_origin= 10000000;             }

  double x_cm = x_origin - K0_UTM * ComputeUtmX(0.0, 0.0);
  double y_eq = y_origin - K0_UTM * ComputeUtmY(0.0, 0.0);

  double x_utm = K0_UTM * ComputeUtmX(lamda - lamda0, phi) + x_cm;
  double y_utm = K0_UTM * ComputeUtmY(lamda - lamda0, phi) + y_eq;

  long easting = (long)x_utm % 100000 / 10; //floor(mod(x_utm, 10^5) / 10^(5-4))
  long northing = (long)y_utm % 100000 / 10; //floor(mod(y_utm, 10^5) / 10^(5-4))

  char* eastLetters = "STUVWXYZ";

  if(abs(zone) % 3 == 1)            {
    eastLetters = "ABCDEFGH";       }
  else if(abs(zone) % 3 == 2)       {
    eastLetters = "JKLMNPQR";       }

  char eastingLetter = eastLetters[(int) (x_utm / 100000) - 1];

  char* northLetters = "FGHJKLMNPQRSTUVABCDE";
  if(zone % 2)
  {
    northLetters = "ABCDEFGHJKLMNPQRSTUV";
  }

  char northingLetter = northLetters[((long) y_utm % 2000000 / 100000)];

  sprintf(mgrsZones,  "%02d%c %c%c", abs(zone), latitudeBandLetter, eastingLetter, northingLetter);
  sprintf(mgrsCoords, "%04ld %04ld", easting, northing);
} // end LatLonToUTM MGRS_2014_STD

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// end MGRS_2014_STD specific code
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#else
  #error MGRS_STD must be either 1989 or 2014
#endif // MGRS_STD


/*
 * ComputeUpsX
 * Inputs: lamda (radians)
 *         phi (radians)
 * Outputs: x value
 * Function: Calculates x of the Krueger series based conversion
 *           from latitude/longitude to Universal Polar Stereographic (UPS)
 */
static double ComputeUpsX(double lamda, double phi, double k90)
{
  double sinPhi = sin(phi);

  double P = pow(((1 + (e * sinPhi)) / (1 - (e * sinPhi))), (e / 2)); // exp(e * Atanh(e * sinPhi))
  //chi (X) is "conformal latitude"
  double cosChi = 2 * cos(phi) / ((1 + sinPhi) / P + (1 - sinPhi) / P);
  double sinChi = ((1 + sinPhi) / P - (1 - sinPhi) / P) / ((1 + sinPhi) / P + (1 - sinPhi) / P);

  return ((2 * alpha * sin(lamda) * cosChi) / (k90 * (1 + sinChi)));
} // end ComputeUpsX


/*
 * ComputeUpsY
 * Inputs: lamda (radians)
 *         phi (radians)
 * Outputs: y value
 * Function: Calculates y of the Krueger series based conversion
 *           from latitude/longitude to Universal Polar Stereographic (UPS)
 */
static double ComputeUpsY(double lamda, double phi, double k90)
{
  double sinPhi = sin(phi);

  double P = pow(((1 + (e * sinPhi)) / (1 - (e * sinPhi))), (e / 2)); // exp(e * Atanh(e * sinPhi))
  //chi (X) is "conformal latitude"
  double cosChi = 2 * cos(phi) / ((1 + sinPhi) / P + (1 - sinPhi) / P);
  double sinChi = ((1 + sinPhi) / P - (1 - sinPhi) / P) / ((1 + sinPhi) / P + (1 - sinPhi) / P);

  return ((-2 * alpha * cos(lamda) * cosChi) / (k90 * (1 + sinChi)));
} // end ComputeUpsY


/*
 * LatLonToUPS
 * Inputs: latitude (decimal degrees)
 *         longitude (decimal degrees)
 * Outputs: MGRS strings
 * Function: Converts the latitude longitude to UPS
 *           and then formats for Military Grid Reference System (MGRS)
 */
static void LatLonToUPS(double latitude, double longitude,
                        char* mgrsZones, char* mgrsCoords)
{
  int zone = 0; //denote error
  if(latitude >= 84)
  {
    zone = 1;
  }
  else if(latitude < -80)
  {
    zone = -1;
  }

  if(zone == 0)
  {
    mgrsZones = " ";
    mgrsCoords = " ";
    return; //needs to use UTM instead
  }

  double phi = latitude * PI / 180;
  double lamda = longitude * PI / 180;
  double lamda0 = 0;
  double x_pole = 2000000;
  double y_pole = 2000000;
  double x, y;

  char* eastingStr = "";
  char northingLetter;

  double k90 = sqrt(pow((1 + e), (1 + e)) * pow((1 - e), (1 - e)));

  if(zone == -1) //south pole
  {
    x = K0_UPS * ComputeUpsX(lamda - lamda0, (-1 * phi), k90) + x_pole;
    y = (-1 * K0_UPS) * ComputeUpsY(lamda - lamda0, (-1 * phi), k90) + y_pole;

    char* eastingLetters[24] = { "AJ", "AK", "AL", "AP", "AQ", "AR", "AS", "AT", "AU", "AX", "AY", "AZ", "BA", "BB", "BC", "BF", "BG", "BH", "BJ", "BK", "BL", "BP", "BQ", "BR" };
    char* northingLetters = "ABCDEFGHJKLMNPQRSTUVWXYZ";

    eastingStr = eastingLetters[(int)(x / 100000) - 8];
    northingLetter = northingLetters[(int)(y / 100000) - 8];
  }
  else //north pole
  {
    x = K0_UPS * ComputeUpsX(lamda - lamda0, phi, k90) + x_pole;
    y = K0_UPS * ComputeUpsY(lamda - lamda0, phi, k90) + y_pole;

    char* eastingLetters[14] = { "YR", "YS", "YT", "YU", "YX", "YY", "YZ", "ZA", "ZB", "ZC", "ZF", "ZG", "ZH", "ZJ" };
    char* northingLetters = "ABCDEFGHJKLMNP";

    eastingStr = eastingLetters[(int)(x / 100000) - 13];
    northingLetter = northingLetters[(int)(y / 100000) - 13];
  }

  long easting = (long)x % 100000 / 10; //floor(mod(x, 10^5) / 10^(5-4))
  long northing = (long)y % 100000 / 10; //floor(mod(y, 10^5) / 10^(5-4))

  sprintf(mgrsZones,  "  %s %c", eastingStr, northingLetter);
  sprintf(mgrsCoords, "%04ld %04ld", easting, northing);
} // end LatLonToUPS


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  double HexDegToDblDeg(char* hexDeg, bool isLat)
//  Converts hexadecimal representation of 32-bit scaled degrees to double.
//
//  INPUT : char * - hextext representation of scaled degrees value to convert
//          bool   - isLat indicates if this is a latitude (for range checks)
//  OUTPUT: double - returns COORD_ERR on error, otherwise degrees represented
//  CALLS : strtoul
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
double HexDegToDblDeg(char* hexDeg, bool isLat)
{
  #define HEX_DEG_SCALER (360.0 / (double)UINT32_MAX)
  char*     endPtr;
  uint32_t  scaled;           // strtol w/int32_t doesn't always work w/ hexDeg
  double    dbl;

  scaled = strtoul(hexDeg, &endPtr, 16);
  if (0x80000000 & scaled)
  { // When scaled value represents negative coordinate, make it so
    scaled *= -1; // scaled = ~scaled; scaled++;
    dbl = ((double)scaled) * (-HEX_DEG_SCALER);
  }
  else
  { // When scaled value represents non-negative there is nothing special to do
    dbl = ((double)scaled) * HEX_DEG_SCALER;
  }

  if ((endPtr != (hexDeg + 8))                    ||    // wrong # hex digits,
      (isLat && ((-90.0 > dbl) || (90.0 < dbl)))  ||    // out-of-range lat, or
      ((-180.0 > dbl) || (180.0 < dbl)))              { // out-of-range lon:
    return COORD_ERR;                                 } // return error value.
  else                                                { // Otherwise:
    return dbl;                                       } // return calculated deg
} // end function HexDegToDblDeg


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void ClearCoords(coords_t *pCoord)
//  Clears/Initializes a coords_t structure to the geo-strings ' ' filled and
//  the double values are set to COORD_ERR.
//
//  INPUT : coords_t * - structure to clear
//  OUTPUT: NONE (other than the updating structure)
//  CALLS : memset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ClearCoords(coords_t *pCoord)
{
  memset((void*)pCoord,' ',(sizeof(coords_t) - (2*sizeof(double))));
  pCoord->decLat[COORD_LEN]   = '\0';   // memset filled struct with ' '
  pCoord->decLon[COORD_LEN]   = '\0';   // now need to NULL terminate
  pCoord->dmsLat[COORD_LEN]   = '\0';   // every string and set doubles
  pCoord->dmsLon[COORD_LEN]   = '\0';   // to special 'error' value
  pCoord->mgrsGzd[COORD_LEN]  = '\0';
  pCoord->mgrs10m[COORD_LEN]  = '\0';
  pCoord->dblLat              = COORD_ERR;
  pCoord->dblLon              = COORD_ERR;
} // end routine ClearCoords


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool SetCoordsFromDecLatLon(coords_t *pCoord, char *pLat, char *pLon)
//  Populates entire coords_t structure based on lat & lon input of fixed-width
//  form <sDDD.ddddd> (e.g. +012.01234 or -180.00000).
//
//  INPUT : coords_t *pCoord - structure to populate
//          char     *pLat   - latitude  in fixed width form <sDDD.ddddd>
//          char     *pLon   - longitude in fixed width form <sDDD.ddddd>
//  OUTPUT: bool - returns true if input lat/lon representation valid
//          updates structure (invokes ClearCoords() if lat/lon invalid)
//  CALLS : memcpy
//          DecLatLonToDblLatLon
//          ClearCoords
//          DblLatLonToDMS
//          DblLatLonToMGRS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool SetCoordsFromDecLatLon(coords_t *pCoord, char *pLat, char *pLon)
{
  memcpy((void*)pCoord->decLat, (const void*)pLat,  COORD_LEN);
  memcpy((void*)pCoord->decLon, (const void*)pLon,  COORD_LEN);
  DecLatLonToDblLatLon(pCoord);
  if (COORD_ERR == pCoord->dblLat || COORD_ERR == pCoord->dblLon)
  {
    ClearCoords(pCoord);
    return false;
  }
  else
  {
    DblLatLonToDMS(pCoord);
    DblLatLonToMGRS(pCoord);
    return true;
  }
} // end function SetCoordsFromDecLatLon


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void DecLatLonToDblLatLon(coords_t *pCoord)
//  Converts fixed length sDDD.DDDDD representation of decimal degrees to double
//
//  INPUT : coords_t *pCoord - structure to update
//  OUTPUT: updates structure using COORD_ERR if appropriate
//  CALLS : strtod
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DecLatLonToDblLatLon(coords_t *pCoord)
{
  char*   endPtr;
  double  dbl;

  dbl = strtod(pCoord->decLat, &endPtr);
  if ((endPtr != (pCoord->decLat + 10)) ||    // When wrong # digits/form
      ((-90.0 > dbl) || (90.0 < dbl)))      { // or out-of-range lat
    dbl = COORD_ERR;                        } // then set error value.
  pCoord->dblLat = dbl;

  dbl = strtod(pCoord->decLon, &endPtr);
  if ((endPtr != (pCoord->decLon + 10)) ||    // When wrong # digits/form
      ((-180.0 > dbl) || (180.0 < dbl)))    { // or out-of-range lon
    dbl = COORD_ERR;                        } // then set error value.
  pCoord->dblLon = dbl;
} // end function DecLatLonToDblLatLon


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void DblLatLonToDMS(coords_t *pCoord)
//  Converts lat and lon to corresponding fixed length HDDD.MM.SS characters.
//
//  INPUT : coords_t *pCoord - structure to update
//  OUTPUT: updates structure lat/lon with <HDDD.MM.SS> chars
//  CALLS : fabs
//          sprintf
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DblLatLonToDMS(coords_t *pCoord)
{
  double  dblLat = pCoord->dblLat;      // local:smaller code & preserves pCoord
  double  dblLon = pCoord->dblLon;
  char    caLat[COORD_LEN+1] = "";
  char    caLon[COORD_LEN+1] = "";
  bool    isNeg;                        // i.e. is SOUTH or WEST ?
  uint16_t d, m, s;                     // degrees, minutes, seconds

  if (( -90.0 <= dblLat) && ( 90.0 >= dblLat) &&
      (-180.0 <= dblLon) && (180.0 >= dblLon))
  {
    isNeg = (0 > dblLat);
    dblLat = fabs(dblLat);
    d = dblLat;  dblLat = (dblLat - d) * 60;  // do NOT round 'd' up
    m = dblLat;  dblLat = (dblLat - m) * 60;  // do NOT round 'm' up
    s = dblLat + 0.5;                         // but DO round 's' up
    sprintf(caLat, "%04d.%02d.%02d", d, m, s);
    caLat[0] = (isNeg) ? 'S' : 'N';

    isNeg = (0 > dblLon);
    dblLon = fabs(dblLon);
    d = dblLon;  dblLon = (dblLon - d) * 60;
    m = dblLon;  dblLon = (dblLon - m) * 60;
    s = dblLon + 0.5;
    sprintf(caLon, "%04d.%02d.%02d", d, m, s);
    caLon[0] = (isNeg) ? 'W' : 'E';
  }
  sprintf(pCoord->dmsLat, "%-10s", caLat);
  sprintf(pCoord->dmsLon, "%-10s", caLon);
} // end routine DblLatLonToDMS


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void DblLatLonToMGRS(coords_t *pCoord)
//  Converts lat/lon coordinate to geopt16_t without verifying inputs.
//
//  INPUT : coords_t *pCoord - structure to update
//  OUTPUT: updates structure mgrsGzd and mgrs10m
//  CALLS : LatLonToUPS
//          LatLonToUTM
//          sprintf
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DblLatLonToMGRS(coords_t *pCoord)
{
  char mgrsZones[COORD_LEN+1]  = "";
  char mgrsCoords[COORD_LEN+1] = "";

  if (( -90.0 <= pCoord->dblLat) && ( 90.0 >= pCoord->dblLat) &&
      (-180.0 <= pCoord->dblLon) && (180.0 >= pCoord->dblLon))
  {
    double lclDblLon = pCoord->dblLon;            // do NOT mod pCoord->dblLon
    if(lclDblLon == 180.0 || lclDblLon == -180.0)
    { //adjust edge case
      lclDblLon = 0;
    }

    if(pCoord->dblLat >= 84 || pCoord->dblLat < -80)
    {
      LatLonToUPS(pCoord->dblLat, lclDblLon, mgrsZones, mgrsCoords);
    }
    else
    {
      LatLonToUTM(pCoord->dblLat, lclDblLon, mgrsZones, mgrsCoords);
    }
  }
  sprintf(pCoord->mgrsGzd, "%-10s", mgrsZones);
  sprintf(pCoord->mgrs10m, "%-10s", mgrsCoords);
} // end routine DblLatLonToMGRS


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  void CoordsToGeopt16(coords_t *pCoord, geopt16_t* geoPt)
//  Converts coordinate in coords_t to geopt16_t without verifying inputs.
//
//  INPUT : coords_t *pCoord - struct holding inputs dblLat and dblLon
//          geopt16_t* geoPt - struct to be updated if conversion successful
//  OUTPUT: updates geoPt
//  CALLS : NONE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CoordsToGeopt16(coords_t *pCoord, geopt16_t* geoPt)
{ // geopt16_t uses 32767 in scaling because lon -90 does not wrap to +90
  geoPt->lat = (int16_t)(pCoord->dblLat * 32767.0 /  90.0);
  geoPt->lon = (int16_t)(pCoord->dblLon * 32767.0 / 180.0);
} // end routine CoordsToGeopt16


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  bool CalcRngBrg(double frLat, double frLon,
//                  double toLat, double toLon,
//                  char*  range, char*  bearing)
//  Calculate the geodetic vector from one lat/lon to another, placing resultant
//  range and bearing as 3-characters ASCII into buffers while returning whether
//  or not the range is in kilometers (or meters).
//
//  NOTE - Using pseudo-Vincenty's inverse formula with special checks.
//  Haversine has antipodal issue and larger code.  Polar Flat Earth is no good
//  with chaotic error conditions. Both 'Spherical Law of Cosines' and
//  'Equirectangular Approximation' require 64-bit 'long double' trigonometry,
//  especially since we MUST work with small distances.
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
//  CALLS : fabs, sin, cos, atan, atan2
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CalcRngBrg(double frLat, double frLon,
                double toLat, double toLon,
                char*  rng,   char* brg)
{
  int    i;
  bool   isKm   = false;

  if (( -90.0 <= frLat) && ( 90.0 >= frLat) &&
      (-180.0 <= frLon) && (180.0 >= frLon) &&
      ( -90.0 <= toLat) && ( 90.0 >= toLat) &&
      (-180.0 <= toLon) && (180.0 >= toLon) &&
      (NULL   != rng)   && (NULL  != brg))
  { // All input validity checks passed so calc range & bearing
    double dblBrg;                      // final answer to be 0-359 degrees
    double dblRng;                      // final answer to be in meters

    // Calculate range used in 3 special if-else cases not doing spherical trig
    dblRng  = fabs(toLat - frLat) * ME_MPD; // calc before ~polar adjustments

    // when frLat or toLat within ~1.5 meters of pole just set value to pole
    if (0.0000135 > (90.0 - fabs(frLat)))   {
      frLat = (0 < frLat) ? 90.0 : -90.0;   }
    if (0.0000135 > (90.0 - fabs(toLat)))   {
      toLat = (0 < toLat) ? 90.0 : -90.0;   }

    if ((frLat == toLat) && (frLon == toLon))
    { // <same points> is a simple case to handle (and it shall be done first)
      dblBrg = 0.0;                     // At same place (or within 1.5m @ pole)
    }                                   // Range calculated before polar adjust
    else if (90.0 == fabs(frLat))
    { // when coming from pole: easily avoid divide-by-zero when finding range
      dblBrg = (0 > frLat) ? 0 : 180;   // due N or S from applicable pole
    }                                   // Range calculated before polar adjust
    else if (90.0 == fabs(toLat))
    { // when going to pole: brg easily calculated, but check after frLat
      dblBrg = (0 < toLat) ? 0 : 180;   // due N or S to applicable pole
    }                                   // Range calculated before polar adjust
    else
    { // Perform spherical earth calculations using WGS-84 Mean Earth Radius
      // Pre-calculate radians, sines, and cosines used in bearing and/or range
      double phiF, sinPf, cosPf;        // radian relatives of frLat
      double phiT, sinPt, cosPt;        // radian relatives of toLat
      double deltaLamda, sinDl, cosDl;  // radian relatives of Lon change
      double f,g,h;                     // re-used combinations of trig values

      deltaLamda = DEG_TO_RAD(toLon - frLon);
      sinDl      = sin(deltaLamda);
      cosDl      = cos(deltaLamda);
      phiF       = DEG_TO_RAD(frLat);
      sinPf      = sin(phiF);
      cosPf      = cos(phiF);
      phiT       = DEG_TO_RAD(toLat);
      sinPt      = sin(phiT);
      cosPt      = cos(phiT);

      f = (cosPt * sinDl);
      g = (cosPf * sinPt) - (sinPf * cosPt * cosDl);
      h = (sinPf * sinPt) + (cosPf * cosPt * cosDl);
      if (0 == h)         {             // Protect against divide-by-zero
        h = DBL_MIN;      }             // in range calculation

      // Calculate Great-Circle route initial bearing (FAZ - forward azimuth)
      // Never calls atan2(0,0) since checked for <polar points> & <same points>
      dblBrg = RAD_TO_DEG(atan2(f,g));  // -180 to 180 degrees (at end 0 to 359)

      // Use special case of Vincenty formula for ellipsoid w/ equal major and
      // minor axes to calculate Great-Circle central angle * distance in meters
      dblRng  = atan(sqrtf(f*f + g*g) / h);   // Central angle in radians, but
      if (0 > dblRng)     {                   // when atan returns a negative
        dblRng += PI;     }                   // normalize back to positive.
      dblRng *= MER_M;                        // Then put final answer in meters
    } // end if-else the various ways of deriving range & bearing

    // Now it is time to use dblRng and dblBrg to set range & bearing 'strings'
    uint16_t u16;                       // for rounding doubles to final values

    if(1000 <= dblRng)
    {                                   // When range is 1 km or more
      isKm    = true;                   // note that and
      dblRng /= 1000;                   // convert meters to km
    }

    if (1000 <= dblRng)                 // When > 999 km, would over-run display
    {                                   // so indicate range > 999 km ala excel
      for (i = 3; i && rng; i--, rng++)   {
        *rng = '#';                       }
    }
    else
    {
      if (isKm && (9.9495 >= dblRng))   // When single-digit kilometers don't
      {                                 // round up to double digit, generate a
        sprintf(rng, "%01.1f", dblRng); // 3-char K.m to fit display Range
      }
      else
      {                                 // When meters or multi-digit km
        u16 = dblRng + 0.5;             // Round up when convert double to int
        if (999 < u16)              {   // but don't allow rounding out of range
          u16--;                    }   // of 3-digits available on display when
        sprintf(rng, "%03d", u16);      // generate left 0-padded range string
      }
    }

    if (0.0 > dblBrg)   {               // As needed convert a negative
      dblBrg += 360.0;  }               // bearing to 180...360 equivalent.
    u16 = dblBrg + 0.5;                 // Round up when convert double to int
    if (360 <= u16)               {     // but don't allow rounding out of range
      u16 = 0;                    }     // by converting 360 back to 0.
    sprintf(brg, "%03d", u16);          // Generate left 0-padded bearing string
  }
  else
  { // invalid input(s)... garbage in; garbage out (checking ptr also not bad)
    for (i = 3; i && rng; i--, rng++)   { // while not a NULL pointer
      *rng = '!';                       } // set range to indicate error
    for (i = 3; i && brg; i--, brg++)   { // then do the same w/ bearing
      *brg = '!';                       } // (two loops because two pointers)
  }
  return isKm;
} // end function CalcRngBrg
