#include <limits.h>
#include <stdbool.h>

#include "logic.h"
#include "geometry.h"


// Returns the dot product between (x1, y1) and (x2, y2)
int32_t inline dot_product(int32_t x1, int32_t y1, int32_t x2, int32_t y2){
  return (x1 * x2) + (y1 * y2);
} 

// Returns the smallest dot product between (x, y) and a point in x_coords / y_coords
int32_t min_dot_product(int32_t x, int32_t y, int32_t* x_coords, int32_t* y_coords, int n_vertices)
{
  int32_t min = INT32_MAX;
  int32_t product;

  for(int i=0; i<n_vertices; i++){
    product = dot_product(x, x_coords[i], y, y_coords[i]);
    if(min > product){
      min = product;
    } 
  }
  return min;
}

// Returns the largest dot product between (x, y) and a point in x_coords / y_coords
int32_t max_dot_product(int32_t x, int32_t y, int32_t* x_coords, int32_t* y_coords, int n_vertices)
{
  int32_t max = INT32_MIN;
  int32_t product;

  for(int i=0; i<n_vertices; i++){
    product = dot_product(x, x_coords[i], y, y_coords[i]);
    if(max < product){
      max = product;
    } 
  }
  return max;
}

// Running time is O(m x log(n)) where m and n is the number of edges in each polygon.
bool do_get_intersection_time(
    struct polygon* poly1,  // the polygons to check for intersection
    struct polygon* poly2,  //
    int32_t x_speed1,     // the speed of the polygons
    int32_t y_speed1,     //
    int32_t x_speed2,     // 
    int32_t y_speed2,     //
    int time_max,           // the time interval to search is [0, time_max]
    int* time_first_max,  // time of first and last intersection
    int* time_last_min)   //
{
  // The polygon vertices
  int32_t* x_coords1;
  int32_t* y_coords1;
  int32_t n_vertices1;
  int32_t* x_coords2;
  int32_t* y_coords2;
  int32_t n_vertices2;

  // The direction that defines a separation axis
  int32_t direction_x;
  int32_t direction_y;

  // The minimum and maximum boundary values of vertices projected onto the separation axis
  int32_t min1, max1;
  int32_t min2, max2;
  int32_t speed;

  // Time of first and last intersection for a single separation axis
  int32_t time_first = INT32_MIN;
  int32_t time_last  = INT32_MAX;

  // Store references to the polygon vertex lists for faster access
  x_coords1   = poly1->x_coords;
  y_coords1   = poly1->y_coords;
  n_vertices1 = poly1->n_vertices;
  x_coords2   = poly2->x_coords;
  y_coords2   = poly2->y_coords;
  n_vertices2 = poly2->n_vertices;

  // Initialize results
  *time_first_max = INT32_MIN;
  *time_last_min  = INT32_MAX;

  // Search through separating axes of polygon1
  for(int i=0; i < n_vertices1; i++){

    // Calculate the next separation axis
    direction_x =  y_coords1[(i + 1) % n_vertices1] - y_coords1[i];
    direction_y = -x_coords1[(i + 1) % n_vertices1] - x_coords1[i];

    // Calculate the minimum and maximum values of the polygon vertices
    // projected onto the separation axis
    min1 = min_dot_product(direction_x, direction_y, x_coords1, y_coords1, n_vertices1);
    max1 = max_dot_product(direction_x, direction_y, x_coords1, y_coords1, n_vertices1);
    min2 = min_dot_product(direction_x, direction_y, x_coords2, y_coords2, n_vertices2);
    max2 = max_dot_product(direction_x, direction_y, x_coords2, y_coords2, n_vertices2);

    // Calculate the speed with which the projection of polygon2 moves along
    // the separation axis relative to the speed of the projection of polygon1
    speed = dot_product(
        x_speed2 - x_speed1,
        y_speed2 - y_speed1,
        direction_x,
        direction_y);

    // Find the first and last time of intersection for the current separation axis
    if(max2 < min1){
     
      // polygon2 is on the negative side of polygon1 in the projection 
      if(speed <= 0){ return false; }
      time_first = (min1 - max2) / speed; // rounded down
      time_last = (max1 - min2 + (speed - 1)) / speed; // rounded up
    }else if(max1 < min2){ 
      
      // polygon2 is on the positive side of polygon1 in the projection 
      if(speed >= 0){ return false; }
      time_first = (min2 - max1) / (-speed); // rounded down
      time_last = (max2 - min1 + ((-speed) - 1)) / (-speed); // rounded up
    }else{
      
      // polygon2 and polygon1 overlaps in the projection 
      if(speed > 0){
        time_last = (max1 - min2 + (speed - 1)) / speed; // rounded up
      }else if(speed < 0){
        time_last = (min1 - max2 - ((-speed) - 1)) / (-speed); // rounded down
      }
    }

    // If the first time of intersection along the current separation axis is
    // larger, update the first time of intersection
    if(time_first > *time_first_max){
      *time_first_max = time_first;
      if(*time_first_max > time_max){
        return false;
      }
    }

    // If the last time of intersection along the current separation axis is
    // smaller, update the last time of intersection
    if(time_last < *time_last_min){
      *time_last_min = time_last;
      if(*time_first_max > *time_last_min){
        return false;
      }
    }
  }
  return true;

  // Search through separating axes for polygon2
  for(int i=0; i<n_vertices2; i++){

  }
}
