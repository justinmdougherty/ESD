////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : geofence.c
 *
 *  DESCRIPTION   : Define procedures which allows one to test if a geographical
 *    point is within a geozone, which may be a 'point & radius' (estimated), a
 *    polygon, or 'rectangle' (using lower left & upper right corners).
 *
 *  NOTE:  When checking a point & radius zone, scaling must be as follows:
 *  int16_t lat: 2's complement decimal degrees scaled so lsb =  90/32767 degree
 *  int16_t lon: 2's complement decimal degrees scaled so lsb = 180/32767 degree
 *  int16_t r2: radius squared in miles, so max radius is 181 miles
 *
 *  NOTE - TestInsideLlUrZone fail when zone both size 180+ degrees of longitude
 *         and crosses the Anti-Meridian (roughly the International Date Line).
 *
 *  NOTE: TestInsidePolygonZone has been seen to fail when PUT is -90,-180
 *
 *    //(1) int16_t TestInsidePtRadiusZone(geopt16_t pt, const ptradzone16_t *z)
 *    //(2) int16_t TestInsideLlUrZone(geopt16_t p, llurzone16_t z)
 *       *  int32_t TestIsLeft(geopt16_t LP1, geopt16_t LP2, geopt16_t PUT)
 *      (3) int16_t TestInsidePolygonZone(geopt16_t p, const polyzone16_t * z)
 *
 *  WRITTEN BY    : Robert Kirby, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/06/03, Robert Kirby, NSWC H12
 *      adapted from Asp code base 2019/02/19
 *      Move inline TestIsLeft() from .h to .c (due to compiler differences)
 *      Increase TestIsLeft() type casting (due to compiler/uC differences)
 *      Pass polyzone by reference (due to compiler differences)
 *      Stub out unused TestInsidePtRadiusZone() and TestInsideLlUrZone()
 *      Calculated additional LON_FACTORs for PtRadiusZones (not tested)
 *
 *  REFERENCE DOCUMENTS
 *    1. www.softsurfer.com (copyright 2001... may be freely used & modified...)
 */
#include <xc.h>             // required for Nop();
#include <stdint.h>
#include "geofence.h"

/*
//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  int16_t TestInsidePtRadiusZone(geopt16_t pt, const ptradzone16_t *z)
//  Performs an approximation check to see if a point is within a geozone
//  defined by a point-radius where max radius is 181 miles.
//
//  int16_t lat: 2's complement decimal degrees scaled so lsb =  90/32767 degree
//  int16_t lon: 2's complement decimal degrees scaled so lsb = 180/32767 degree
//  int16_t r2: radius squared in miles, so max radius is 181 miles
//
//  Distance, in miles, from a point on the earth can be approximated by formula
//
//     d = sqrt(x*x + y*y)              [NOTE this is a flat earth calculation]
//
//     where d = distance in miles
//           lat & lon are in 2's complement decimal degrees (S&W are negative)
//           x = 69.1 * (lat2 - lat1)   [NOTE 69.1 ~= miles/deg LAT]
//        and
//           y = 53.0 * (lon2 - lon1)   [NOTE 53.0 ~= miles/deg LON @ 40N]
//
//  For this application to minimize microprocessor computations compare
//     r2 to (x*x + y*y) instead of actual distance of radius
//
//  INPUT : geopt16_t pt - the point-under-test
//          const ptradzone16_t *z - geozone defined by a point & radius
//  OUTPUT: int16_t - returns 1 if approximately inside zone, otherwise 0
//  CALLS : none
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int16_t TestInsidePtRadiusZone(geopt16_t pt, const ptradzone16_t *z)
#ifdef ASP_GLOBALSTAR
//  NOTE - this has been optimized for a particular embedded application focused
//  about 40N, but is most easily modified to be generic for lat/lon and radius.
{
  // modify the following two factors according to data packing & scaling
  #define LAT_FACTOR   (69.1 *  90.0 / 32767.0)
  #define LON_FACTOR   (53.0 * 180.0 / 32767.0)

  register int16_t  x, y;               // deltas x & y
           int32_t d2;                  // distance squared

  x   = (int16_t)(LAT_FACTOR * (pt.lat - z->pt.lat));
  y   = (int16_t)(LON_FACTOR * (pt.lon - z->pt.lon));
  d2  = (int32_t)x*x + (int32_t)y*y;

  return (d2  < z->r2 ) ? 1 : 0;
} // end function TestInsidePtRadiusZone
#elif NEEDS_TESTING // S latitudes, extreme difLat or difLon, cross lon 180, etc
{
  // modify the following two factors according to data packing & scaling
  #define LAT_FACTOR   (69.1 *  90.0 / 32767.0) // 69.1 miles/deg LAT
  #define LON_FACTOR80 (12.0 * 180.0 / 32767.0) // miles/deg LON @ 80N
  #define LON_FACTOR70 (23.6 * 180.0 / 32767.0) // miles/deg LON @ 70N
  #define LON_FACTOR60 (34.6 * 180.0 / 32767.0) // miles/deg LON @ 60N
  #define LON_FACTOR50 (44.4 * 180.0 / 32767.0) // miles/deg LON @ 50N
  #define LON_FACTOR40 (53.0 * 180.0 / 32767.0) // 52.9 miles/deg LON @ 40N
  #define LON_FACTOR30 (59.8 * 180.0 / 32767.0) // miles/deg LON @ 30N
  #define LON_FACTOR20 (64.9 * 180.0 / 32767.0) // miles/deg LON @ 20N
  #define LON_FACTOR10 (68.1 * 180.0 / 32767.0) // miles/deg LON @ 10N
  #define LON_FACTOREQ (69.0 * 180.0 / 32767.0) // miles/deg LON @ equator

  register int16_t  x, y;               // delta miles x & y
           int32_t  sumLat;             // sum of latitudes
           int16_t  difLat;             // difference of latitudes
           int16_t  difLon;             // difference of longitudes
           int32_t  d2;                 // distance squared

  difLat = pt.lat - z->pt.lat;
  difLon = pt.lon - z->pt.lon;

  x = (int16_t)(LAT_FACTOR * difLat);

  sumLat = pt.lat + z->pt.lat;
  if ((150 < sumLat) || (-150 > sumLat))        { // avg lat > 75
    y = (int16_t)(LON_FACTOR80 * difLon);       }
  else if ((130 < sumLat) || (-130 > sumLat))   { // avg lat > 65
    y = (int16_t)(LON_FACTOR70 * difLon);       }
  else if ((110 < sumLat) || (-110 > sumLat))   { // avg lat > 55
    y = (int16_t)(LON_FACTOR60 * difLon);       }
  else if ((90 < sumLat) || (-90 > sumLat))     { // avg lat > 45
    y = (int16_t)(LON_FACTOR50 * difLon);       }
  else if ((70 < sumLat) || (-70 > sumLat))     { // avg lat > 35
    y = (int16_t)(LON_FACTOR40 * difLon);       }
  else if ((50 < sumLat) || (-50 > sumLat))     { // avg lat > 25
    y = (int16_t)(LON_FACTOR30 * difLon);       }
  else if ((30 < sumLat) || (-30 > sumLat))     { // avg lat > 15
    y = (int16_t)(LON_FACTOR20 * difLon);       }
  else if ((14 < sumLat) || (-14 > sumLat))     { // avg lat >  7
    y = (int16_t)(LON_FACTOR10 * difLon);       }
  else                                          { // avg lat <= 7
    y = (int16_t)(LON_FACTOREQ * difLon);       }

  d2  = (int32_t)x*x + (int32_t)y*y;

  return (d2  < z->r2 ) ? 1 : 0;
} // end function TestInsidePtRadiusZone
#endif  // NEEDS_TESTING
*/
/*
//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  int16_t TestInsideLlUrZone(geopt16_t p, llurzone16_t z)
//  Test for point in a Lower-Left Upper-Right (south-west north-east) geozone.
//
//  NOTE - this will fail cases when zone both size 180+ degrees of longitude
//         and crosses the Anti-Meridian (roughly the International Date Line).
//
//  INPUT : geopt32_t p - the point-under-test
//          llurzone32_t z - a LL-UR zone that defines geozone boundary
//  OUTPUT: int16_t - returns 1 if inside zone, otherwise 0
//  CALLS : none
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int16_t TestInsideLlUrZone(geopt16_t p, llurzone16_t z)
{
  if ((z.ll.lat < p.lat) && (p.lat < z.ur.lat))
  { // When point inside the latitude boundaries check its longitude
    if ((0 >= z.ur.lon) && (0 <= z.ll.lon))
    { // When zone's longitudes obviously span the International Date Line
      if (((p.lon >= 0) && (p.lon > z.ll.lon)) ||
          ((p.lon <= 0) && (p.lon < z.ur.lon)))
      { // pt w/E lon between LL and IDL or pt w/W lon between IDL and UR
        return 1;
      }
    }
    else if ((z.ll.lon < p.lon) && (p.lon < z.ur.lon))
    { // When zone's longitudes are less complicated (doesn't cross IDL)
      return 1;
    }
  }
  return 0;
} // end function TestInsideLlUrZone
*/

//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  int64_t TestIsLeft(geopt16_t LP1, geopt16_t LP2, geopt16_t PUT)
//  Tests if a point is Left|On|Right of an infinite line
//  NOTE: need 64-bits for full range of values given 16-bit points
//  Copyright 2001, softSurfer, ...may be freely used & modified...
//  See: the January 2001 Algorithm "Area of 2D and 3D Triangles and Polygons"
//
//  INPUT:  geopt16_t LP1, LP2 - points through which test line passes
//          geopt16_t PUT - the Point Under Test
//  OUTPUT: >0 PUT is left of line through P1 & P2
//          =0 PUT is on line
//          <0 PUT is right of line
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline int64_t TestIsLeft(geopt16_t LP1, geopt16_t LP2, geopt16_t PUT)
{
  return ( ( ((int64_t)LP2.lat - (int64_t)LP1.lat) *
             ((int64_t)PUT.lon - (int64_t)LP1.lon) ) -
           ( ((int64_t)PUT.lat - (int64_t)LP1.lat) *
             ((int64_t)LP2.lon - (int64_t)LP1.lon) ) );
} // end function TestIsLeft


//++PROCEDURE+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  int16_t TestInsidePolygonZone(geopt16_t p, const polyzone16_t * z)
//  Performs winding number test for point in a multi-vertex (polygon) geozone.
//  Copyright 2001, softSurfer.com, ...may be freely used & modified...
//
//  NOTE: has been seen to fail when PUT is -90,-180
//
//  INPUT : geopt16_t p - the point-under-test
//          polyzone16_t* - adrs of polygon zone that defines geozone boundary
//  OUTPUT: int16_t - the winding number (=0 only if P is outside z[])
//  CALLS : TestIsLeft
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int16_t TestInsidePolygonZone(geopt16_t p, const polyzone16_t * z)
{
  int16_t wn = 0;                                 // winding number counter
  int16_t i;

  // loop through all edges of the zone (polygon)
  for (i = z->n; i > 0; i--)                      // Have n+1 vertices, so i = n
  {                                               // Edge z->v[i-1] to z->v[i]
    if (z->v[i-1].lon <= p.lon)
    {
      if (z->v[i].lon > p.lon)
      {                                           // an upward crossing
        if (TestIsLeft(z->v[i-1],z->v[i],p) > 0){ // Points is left of edge
          ++wn;                                 } // have a valid up intersect
      }
    }
    else  // (z->v[i-1].lon > p.lon)
    {
      if (z->v[i].lon <= p.lon)
      {                                           // a downward crossing
        if (TestIsLeft(z->v[i-1],z->v[i],p) < 0){ // Point is right of edge
          --wn;                                 } // have a valid down intersect
      }
    }
  }
  return wn;
} // end function TestInsidePolygonZone
