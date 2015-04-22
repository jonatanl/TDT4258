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
bool time_of_collision(struct asteroid* asteroid1, struct asteroid* asteroid2,
    int time_max,    // the time interval to search is [0, time_max]
    int* time_first, // time of the first intersection
    int* time_last)  // time of the last intersection
{
  struct asteroid* asteroid_temp;
  int32_t* x_coords1;
  int32_t* y_coords1;
  int32_t n_vertices1;
  int32_t* x_coords2;
  int32_t* y_coords2;
  int32_t n_vertices2;
  int32_t x_speed_rel;
  int32_t y_speed_rel;

  // To minimize the search time, the first polygon should be the one with the
  // least number of edges.
  if(asteroid1->poly.n_vertices > asteroid2->poly.n_vertices){
    asteroid_temp = asteroid1;
    asteroid1 = asteroid2;
    asteroid2 = asteroid_temp;
  }

  x_coords1 = asteroid1->poly.x_coords;
  y_coords1 = asteroid1->poly.y_coords;
  n_vertices1 = asteroid1->poly.n_vertices;
  x_coords2 = asteroid2->poly.x_coords;
  y_coords2 = asteroid2->poly.y_coords;
  n_vertices2 = asteroid2->poly.n_vertices;

  // Calculate the speed with which asteroid2 is moving towards asteroid1
  x_speed_rel = asteroid2->x_speed - asteroid1->x_speed;
  y_speed_rel = asteroid2->y_speed - asteroid1->y_speed;

  *time_first = INT32_MIN;
  *time_last  = INT32_MAX;

  int32_t edge_x;
  int32_t edge_y;
  int32_t direction_x;
  int32_t direction_y;
  int32_t min1, max1;
  int32_t min2, max2;
  int32_t speed;
  int32_t time_first_new = INT32_MIN;
  int32_t time_last_new  = INT32_MAX;

  // Search through separating axes of asteroid1
  for(int i=0, next_i; i < n_vertices1; i++){

    // Calculate the next edge vector to use as separation axis
    next_i = (i + 1) % n_vertices1;
    edge_x = x_coords1[next_i] - x_coords1[i];
    edge_y = y_coords1[next_i] - y_coords1[i];

    direction_x =  edge_y;
    direction_y = -edge_x;

    // Calculate boundary values of vertices projected onto the separation axis
    min1 = min_dot_product(direction_x, direction_y, x_coords1, y_coords1, n_vertices1);
    max1 = max_dot_product(direction_x, direction_y, x_coords1, y_coords1, n_vertices1);
    min2 = min_dot_product(direction_x, direction_y, x_coords2, y_coords2, n_vertices2);
    max2 = max_dot_product(direction_x, direction_y, x_coords2, y_coords2, n_vertices2);

    // Calculate the speed of the projection along the separation axis
    speed = dot_product(x_speed_rel, y_speed_rel, direction_x, direction_y);

    // asteroid1 is to the left of asteroid2 in the projection
    if(max2 < min1){

      if(speed <= 0 ){
        return false;
      }
      time_first_new = (min1 - max2) / speed; // rounded down
      time_last_new = (max1 - min2 + (speed - 1)) / speed; // rounded up
    }else if(max1 < min2){

      if(speed >= 0){
        return false;
      }
      time_first_new = (min2 - max1) / (-speed); // rounded down
      time_last_new = (max2 - min1 + ((-speed) - 1)) / (-speed); // rounded up
    }else{
      
      if(speed > 0){
        time_last_new = (max1 - min2 + (speed - 1)) / speed; // rounded up
      }else if(speed < 0){
        time_last_new = (min1 - max2 - ((-speed) - 1)) / (-speed); // rounded down
      }
    }

    if(time_first_new > *time_first){
      *time_first = time_first_new;
      if(*time_first > time_max){
        return false;
      }
    }
    if(time_last_new < *time_last){
      *time_last = time_last_new;
      if(*time_first > *time_last){
        return false;
      }
    }
  }
  return true;

  // Search through separating axes for asteroid2
  for(int i=0; i<n_vertices2; i++){

  }

}
