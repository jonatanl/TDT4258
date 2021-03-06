// Include guard
#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include <stdbool.h>

#include "logic.h"

//-----------------------------------------------------------
// This header contains several utility functions and macros
//-----------------------------------------------------------

// Math
#define ARG_MAX(a, b)                   (a > b ? a : b)
#define ARG_MIN(a, b)                   (a < b ? a : b)
#define RANDOM(start, end) (start + (rand() % end))

// Returns true if the intervals (p1_min, p1_max) and (p2_min, p2_max) overlaps
#define INTERSECTS(p1_min, p1_max, p2_min, p2_max)   ((p1_min >= p2_min && p1_min <= p2_max) || (p1_max >= p2_min && p1_max <= p2_max))
#define POINT_LINE_INTERSECTS(p1, p2_min, p2_max)    ((p1 >= p2_min && p1 <= p2_max))

void wrap_coordinate(int32_t* x_pos, int32_t* y_pos);

// Wraps a coordinate outside the board so that it is inside the board
void create_bounding_box(struct bounding_box* box, struct polygon* poly);

// Rotates a single integer coordinate
void rotate_coordinate_int(int32_t* xPosition, int32_t* yPosition, int32_t xCenter, int32_t yCenter, bool clockWise);

// Rotates a single floating point coordinate
void rotate_coordinate_float(float* xPosition, float* yPosition, int32_t xCenter, int32_t yCenter,  bool clockWise);

#endif // !_UTIL_H
