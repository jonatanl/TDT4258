#include <stdint.h>
#include <stdbool.h>
#include "util.h"

// Pre calculated sine and cosine to rotate 12 degrees clockwise
static float clockWiseSine		= -0.20791f;;
static float clockWiseCosine 	= 0.97815f;

// Pre calculated sine and cosine to rotate 12 degrees counterclockwise
static float counterWiseSine 	= 0.20791f;
static float counterWiseCosine 	= 0.97815f;

void wrap_coordinate(int32_t* x_pos, int32_t* y_pos){
  *x_pos = ((*x_pos % DEFAULT_WORLD_X_DIM) + DEFAULT_WORLD_X_DIM) % DEFAULT_WORLD_X_DIM;
  *y_pos = ((*y_pos % DEFAULT_WORLD_Y_DIM) + DEFAULT_WORLD_Y_DIM) % DEFAULT_WORLD_Y_DIM;
}

void create_bounding_box(struct bounding_box* box, struct polygon* poly){
    box->x_left_upper  = poly->x_coords[0];
    box->x_right_lower = poly->x_coords[0];
    box->y_left_upper  = poly->y_coords[0];
    box->y_right_lower = poly->y_coords[0];
    
    for(int i = 1; i < poly->n_vertices; i++){
        box->x_left_upper  = ARG_MIN(poly->x_coords[i], box->x_left_upper);
        box->x_right_lower = ARG_MAX(poly->x_coords[i], box->x_right_lower);
        box->y_left_upper  = ARG_MIN(poly->y_coords[i], box->y_left_upper);
        box->y_right_lower = ARG_MAX(poly->y_coords[i], box->y_right_lower);
    }
}

void rotate_coordinate_int(int32_t* xPosition, int32_t* yPosition, int32_t xCenter, int32_t yCenter, bool clockWise){
	float sine;
	float cosine;

	// Different sine and cosine valuse depending on which way to rotate
	if (clockWise) {
		sine = clockWiseSine;
		cosine = clockWiseCosine;
	} else {
		sine = counterWiseSine;
		cosine = counterWiseCosine;
	}

	float xRotated = cosine * (*xPosition)
					- sine * (*yPosition)
					+ xCenter -xCenter * cosine + yCenter * sine;

	float yRotated = sine * (*xPosition)
					+ cosine * (*yPosition)
					+ yCenter - xCenter * sine - yCenter * cosine;

	*xPosition = (int32_t) xRotated;
	*yPosition = (int32_t) yRotated;
}

void rotate_coordinate_float(float* xPosition, float* yPosition, int32_t xCenter, int32_t yCenter,  bool clockWise) {
	float sine;
	float cosine;

	// Different sine and cosine valuse depending on which way to rotate
	if (clockWise) {
		sine = clockWiseSine;
		cosine = clockWiseCosine;
	} else {
		sine = counterWiseSine;
		cosine = counterWiseCosine;
	}

	float xRotated = cosine * (*xPosition)
					- sine * (*yPosition)
					+ xCenter -xCenter * cosine + yCenter * sine;

	float yRotated = sine * (*xPosition)
					+ cosine * (*yPosition)
					+ yCenter - xCenter * sine - yCenter * cosine;

	*xPosition = xRotated;
	*yPosition = yRotated;
}
