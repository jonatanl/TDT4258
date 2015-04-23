// Include guard
#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include <stdint.h>
#include <stdbool.h>

#include "logic.h"

// Returns the dot product of (x1, y1) and (x2, y2).
int32_t dot_product(int32_t x1, int32_t y1, int32_t x2, int32_t y2);


// Returns the intersection between two lines in (ix, iy). If the lines do not
// intersect, the result is undefined.
//
// ASSERT: No line end-points are equal, and the lines are not parallel. 
void get_line_intersection(
    int32_t line1_x1,
    int32_t line1_y1,
    int32_t line1_x2,
    int32_t line1_y2,
    int32_t line2_x1,
    int32_t line2_y1,
    int32_t line2_x2,
    int32_t line2_y2,
    int32_t* ix,
    int32_t* iy);


// Finds the time of intersection for two moving convex polygons.
//
// Returns 'true' if the two polygons intersect in the time interval [0,
// time_max], and stores the time of the first and last intersection in the
// variables 'time_first_max' and 'time_last_min'. If 'false' is returned, the
// value of these variables is undefined.
bool get_intersection_time(
    struct polygon* poly1,  // the polygons to check for intersection
    struct polygon* poly2,  //
    int32_t x_speed1,     // the speed of the polygons
    int32_t y_speed1,     //
    int32_t x_speed2,     // 
    int32_t y_speed2,     //
    int time_max,           // the time interval to search is [0, time_max]
    int* time_first_max,  // time of first and last intersection
    int* time_last_min);   //

#endif // !_GEOMETRY_H
