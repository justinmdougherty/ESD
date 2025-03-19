#ifndef GEOFENCE_H__
#define GEOFENCE_H__
////////////////////////////////////////////////////////////////////////////////
//        Property of United States of America - For Official Use Only        //
////////////////////////////////////////////////////////////////////////////////
/*
 *  FILE NAME     : geofence.h
 *
 *  DESCRIPTION   : Define procedures which allows one to test if a geographical
 *    point is within a geozone, which may be a 'point & radius' (estimated), a
 *    'rectangle' (using lower left & upper right corners), or a polygon.
 *
 *    //(1) int16_t TestInsidePtRadiusZone(geopt16_t pt, const ptradzone16_t *z)
 *    //(2) int16_t TestInsideLlUrZone(geopt16_t p, llurzone16_t z)
 *      (3) int16_t TestInsidePolygonZone(geopt16_t p, const polyzone16_t * z)
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
 *  WRITTEN BY    : Robert Kirby, NSWC H12
 *  MODIFICATIONS (in reverse chronological order)
 *    2021/06/02, Robert Kirby, NSWC H12
 *      adapted from Asp code base 2019/02/19
 *      Move inline TestIsLeft() from .h to .c (due to compiler differences)
 *      Modify polyzone16_t so that .v is a flexible array at end of struct
 *      Pass polyzone by reference (due to compiler differences)
 *      Stub out unused TestInsidePtRadiusZone() and TestInsideLlUrZone()
 *
 *  REFERENCE DOCUMENTS
 *    1.  Oxford Atlas of the World
 *    2.  https://www.nhc.noaa.gov/gccalc.shtml
 *    3.  https://ithoughthecamewithyou.com/post/international-date-line-longitude-latitute-coordinates
 */
//**PROCEDURES******************************************************************
//  int16_t TestInsidePtRadiusZone(geopt16_t pt, const ptradzone16_t *z)
//  Performs an approximation check to see if a point is within a geozone
//  defined by a point-radius where max radius is 181 miles.
//
//  NOTE - this has been optimized for a particular embedded application focused
//  about 40N, but is most easily modified to be generic for lat/lon and radius.
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
//  OUTPUT: int16_t - returns 1 if approximately inside fence, otherwise 0
//******************************************************************************
//  int16_t TestInsideLlUrZone(geopt16_t p, llurzone16_t z)
//  Test for point in a Lower-Left Upper-Right (south-west north-east) geozone.
//
//  NOTE - this will fail cases when zone both size 180+ degrees of longitude
//         and crosses the Anti-Meridian (roughly the International Date Line).
//
//  INPUT : geopt16_t p - the point-under-test
//          llurzone16_t z - a LL-UR zone that defines geozone boundary
//  OUTPUT: int16_t - returns 1 if inside zone, otherwise 0
//******************************************************************************
//  int16_t TestInsidePolygonZone(geopt16_t p, const polyzone16_t * z)
//  Performs winding number test for point in a multi-vertex (polygon) geozone.
//  Copyright 2001, softSurfer.com, ...may be freely used & modified...
//
//  INPUT : geopt16_t p - the point-under-test
//          const polyzone16_t - a polygon zone that defines geozone boundary
//  OUTPUT: int16_t - the winding number (=0 only if P is outside z[])
//******************************************************************************
#include <stdint.h>

//----- DEFINES, ENUMS, STRUCTS, TYPEDEFS, ETC. --------------------------------
typedef enum tagGEOZONE_CATEGORY
{
  GC_PTRAD = 1,       // Point-Radius zone (where radius is already squared)
  GC_LLUR,            // Lower-Left Upper-Right (south-west north-east)
  GC_POLY,            // polygon with 'n' vertices
  GC_UNUSED = 0
} geocat_t;

typedef struct tagGEOPT16
{                     // 16-bit based storage for geographical point
  int16_t lat;        // latitude of geographical point
  int16_t lon;        // longitude of geographical point
} geopt16_t;          // packing/scaling of values not specified here

typedef struct tagPTRADZONE16
{                     // 16-bit based storage for geographical zone
  geopt16_t pt;       // center point of geozone
  int16_t   r2;       // radius squared, resulting max radius is 181
} ptradzone16_t;      // packing/scaling of values not specified here

typedef struct tagLLURZONE16
{                     // 16-bit based storage for geographical zone
  geopt16_t ll;       // lower-left (South-West most) corner of geozone
  geopt16_t ur;       // upper-right (North-East most) corner of geozone
} llurzone16_t;       // packing/scaling of values not specified here

typedef struct tagPOLYZONE16
{                     // 16-bit based storage for geographical zone
  int16_t    n;       // # of points that make zone (but v[] has n+1 points)
  geopt16_t  v[];     // points to (n+1) vertex zone (polygon) w/ v[n] == v[0]
} polyzone16_t;       // packing/scaling of geopt values not specified here

//----- EXPOSED ATTRIBUTES -----------------------------------------------------

//----- EXPOSED PROCEDURES -----------------------------------------------------
//int16_t TestInsidePtRadiusZone(geopt16_t p, const ptradzone16_t *z);
//int16_t TestInsideLlUrZone(geopt16_t p, const llurzone16_t z);
int16_t TestInsidePolygonZone(geopt16_t p, const polyzone16_t * z);

//----- MACROS -----------------------------------------------------------------


#endif  // GEOFENCE_H__
