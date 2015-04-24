#define DEBUG
#include <limits.h>
#include <stdbool.h>

#include "debug.h"
#include "logic.h"
#include "geometry.h"


// Returns the dot product between (x1, y1) and (x2, y2)
int32_t dot_product(int32_t x1, int32_t y1, int32_t x2, int32_t y2){
  return (x1 * x2) + (y1 * y2);
}
 

// Returns the largest and the smallest dot product between (x, y) and a point in x_coords / y_coords
void min_max_dot_product(int32_t x, int32_t y,
    int32_t* x_coords, int32_t* y_coords, int n_vertices, int* min, int* max)
{
  int32_t product = dot_product(x, y, x_coords[0], y_coords[0]);
  *min = product;
  *max = product;
  for(int i=1; i<n_vertices; i++){
    product = dot_product(x, y, x_coords[i], y_coords[i]);
    if(*min > product){ *min = product; } 
    if(*max < product){ *max = product; }
  }
}


void inline rotate_clockwise90(int32_t* x, int32_t* y)
{
  int temp = *x;
  *x = *y;
  *y = -temp; 
}


void inline rotate_counterclockwise90(int32_t* x, int32_t* y)
{
  int temp = *x;
  *x = -y[0];
  *y = temp; 
}


bool inline inside_interval(int32_t n, int32_t i1, int32_t i2)
{
  // NOTE: Avoids branching by using '|' instead of '&&'
  return ((i1 <= n) && (n <= i2)) | ((i2 <= n) && (n <= i1));
}


bool inline intersects_line_line(
    int32_t line1_x1,
    int32_t line1_y1,
    int32_t line1_x2,
    int32_t line1_y2,
    int32_t line2_x1,
    int32_t line2_y1,
    int32_t line2_x2,
    int32_t line2_y2
    )
{
  int32_t line1_dx = line1_x2 - line1_x1;
  int32_t line1_dy = line1_y2 - line1_y1;
  int32_t line2_dx = line2_x2 - line2_x1;
  int32_t line2_dy = line2_y2 - line2_y1;
  int32_t dot1 = ((line2_x2 - line1_x1) * (line1_dy))  + ((line2_y2 - line1_y1) * (-line1_dx));
  int32_t dot2 = ((line2_x1 - line1_x1) * (line1_dy))  + ((line2_y1 - line1_y1) * (-line1_dx));
  int32_t dot3 = ((line1_x1 - line2_x1) * (line2_dy))  + ((line1_y1 - line2_y1) * (-line2_dx));
  int32_t dot4 = ((line1_x2 - line2_x1) * (line2_dy))  + ((line1_y2 - line2_y1) * (-line2_dx));
  return inside_interval(0, dot1, dot2) && inside_interval(0, dot3, dot4);
}


void inline get_intersection_line_line(
    int32_t line1_x1,
    int32_t line1_y1,
    int32_t line1_x2,
    int32_t line1_y2,
    int32_t line2_x1,
    int32_t line2_y1,
    int32_t line2_x2,
    int32_t line2_y2,
    int32_t* ix,
    int32_t* iy)
{
  int32_t line1_dx = line1_x2 - line1_x1;
  int32_t line1_dy = line1_y2 - line1_y1;
  int32_t line2_dx = line2_x2 - line2_x1;
  int32_t line2_dy = line2_y2 - line2_y1;
  int line1_t;
  line1_t  = (line2_y1 - line1_y1) * line1_dx - (line2_x1 - line1_x1) * line2_dy;
  line1_t /= (line2_dx * line1_dy) - (line2_dy * line1_dx);
  *ix = line1_x1 + (line1_dx * line1_t);
  *iy = line1_y1 + (line1_dy * line1_t);
}


// NOTE: Running time is O(m x n) where m and n is the number of edges in each
// polygon. This can be improved to O(m x log(n)) by implementing a binary
// search in min_max_dot_product(). However, for small polygons the improvement
// will be small. 
bool get_intersection_time_poly_poly(
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

  // Search through separating axes of polygon1 and polygon2
  for(int i1=0, i2=0; (i1 + i2) != (n_vertices1 + n_vertices2);){

    // Calculate the next separation axis as a normal vector to the current
    // polygon edge. First pick from polygon1, then from polygon2.
    if(i1 < n_vertices1){ // pick a separation axis from polygon1
      direction_x =  y_coords1[(i1 + 1) % n_vertices1] - y_coords1[i1];
      direction_y = -x_coords1[(i1 + 1) % n_vertices1] - x_coords1[i1];
      i1++;
    }else{ // pick a separation axis from polygon2
      direction_x =  y_coords2[(i2 + 1) % n_vertices2] - y_coords2[i2];
      direction_y = -x_coords2[(i2 + 1) % n_vertices2] - x_coords2[i2];
      i2++;
    }

    // Calculate the minimum and maximum values of the polygon vertices
    // projected onto the separation axis
    min_max_dot_product(direction_x, direction_y, x_coords1, y_coords1, n_vertices1, &min1, &max1);
    min_max_dot_product(direction_x, direction_y, x_coords2, y_coords2, n_vertices2, &min2, &max2);

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
      if(speed <= 0){ game_debug("1\n"); return false; }
      time_first = (min1 - max2) / speed; // rounded down
      time_last  = (max1 - min2 + (speed - 1)) / speed; // rounded up
    }else if(max1 < min2){ 
      
      // polygon2 is on the positive side of polygon1 in the projection 
      if(speed >= 0){ game_debug("2\n"); return false; }
      time_first = (min2 - max1) / (-speed); // rounded down
      time_last  = (max2 - min1 + ((-speed) - 1)) / (-speed); // rounded up
    }else{
      
      // polygon2 and polygon1 overlaps in the projection 
      if(speed > 0){
        time_last = (max1 - min2 + (speed - 1)) / speed; // rounded up
      }else if(speed < 0){
        time_last = (max2 - min1 + ((-speed) - 1)) / (-speed); // rounded up
      }
    }

    // If the first time of intersection along the current separation axis is a
    // new maximum, update the first time of intersection
    if(time_first > *time_first_max){
      *time_first_max = time_first;
      if(*time_first_max > time_max){
        game_debug("3\n"); return false;
      }
    }

    // If the last time of intersection along the current separation axis is a
    // new minimum, update the last time of intersection
    if(time_last < *time_last_min){
      *time_last_min = time_last;
      if(*time_first_max > *time_last_min){
        game_debug("4\n"); return false;
      }
    }
  }

  // Return true if no separating axis was found
  return true;
}


// Test function for get_intersection_time().
void test_get_intersection_time_poly_poly(){

  // Test polygon 1
  int32_t n_vertices1 = 6;
  int32_t x_coords1[6] = { 80,  20,  20,  80, 100, 120};
  int32_t y_coords1[6] = { 20,  50, 110, 170, 160, 140};
  struct polygon test_poly1 = {
    .n_vertices = n_vertices1,
    .x_coords = &x_coords1[0],
    .y_coords = &y_coords1[0],
  };
  int32_t x_speed1 =  2;
  int32_t y_speed1 = -1;

  // Test polygon 2
  int32_t n_vertices2 = 6;
  int32_t x_coords2[6] = {240, 240, 250, 270, 270, 260};
  int32_t y_coords2[6] = { 20,  50,  60,  60,  20,  10};
  struct polygon test_poly2 = {
    .n_vertices = n_vertices2,
    .x_coords = &x_coords2[0],
    .y_coords = &y_coords2[0],
  };
  int32_t x_speed2 = -1;
  int32_t y_speed2 = -1;

  // Test variables for collision between polygon 1 and 2
  int32_t time_max_1_2 = 1000;
  bool has_intersection_1_2;
  int intersection_1_2_first;
  int intersection_1_2_last;

  // Test for intersection between polygon 1 and 2
  // Expected intersection times: first = 50, last = ?
  game_debug("Testing for intersection between polygon 1 and 2 ... \n");
  has_intersection_1_2 = get_intersection_time_poly_poly(
      &test_poly1,
      &test_poly2,
      x_speed1,
      y_speed1,
      x_speed2,
      y_speed2,
      time_max_1_2,
      &intersection_1_2_first,
      &intersection_1_2_last
  );
  game_debug("has_intersection       : %d\n", has_intersection_1_2);
  game_debug("intersection_1_2_first : %d (expected somewhat less than 50)\n", intersection_1_2_first);
  game_debug("intersection_1_2_last  : %d\n", intersection_1_2_last);
}
