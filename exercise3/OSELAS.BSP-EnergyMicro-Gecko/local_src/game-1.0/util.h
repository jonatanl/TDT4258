// Include guard
#ifndef _UTIL_H
#define _UTIL_H

//-----------------------------------------------------------
// This header contains several utility functions and macros
//-----------------------------------------------------------

// Math
#define ARG_MAX(a, b)                   (a < b ? a : b)
#define ARG_MIN(a, b)                   (a > b ? a : b)
#define RANDOM(start, end) (start + (rand() % end))

// Returns true if the intervals (p1_min, p1_max) and (p2_min, p2_max) overlaps
#define INTERSECTS(p1_min, p1_max, p2_min, p2_max)   ((p1_min >= p2_min && p1_min <= p2_max) || (p1_max >= p2_min && p1_max <= p2_max))

// Rotates a single coordinate
void rotate_coordinate_int(int32_t* xPosition, int32_t* yPosition, int32_t xCenter, int32_t yCenter, bool clockWise);
void rotate_coordinate_float(float* xPosition, float* yPosition, int32_t xCenter, int32_t yCenter,  bool clockWise);

#endif // !_UTIL_H
