#include <stdbool.h>
#include "logic.h"
#include "spaceship.h"

// Private methods
void rotate_coordinate(int32_t* xPosition, int32_t* yPosition, int32_t xCenter, int32_t yCenter, bool clockWise);

// Pre calculated sine and cosine to rotate 12 degrees clockwise
static float clockWiseSine		= -0.20791f;;
static float clockWiseCosine 	= 0.97815f;

// Pre calculated sine and cosine to rotate 12 degrees counterclockwise
static float counterWiseSine 	= 0.20791f;
static float counterWiseCosine 	= 0.97815f;

// Rotate all vertices in ship clockwise
void rotate_clockwise(ship_object *spaceship) {
	int numberOfVertices = spaceship->poly.n_vertices;

	for (int i = 0; i < numberOfVertices; ++i) {
		rotate_coordinate(&spaceship->poly.x_coords[i], &spaceship->poly.y_coords[i], 0, 0, true);
	}
  rotate_float_coordinate(&spaceship->x_orientation, &spaceship->y_orientation, 0, 0, true);
}

// Rotate all vertices in ship counterclockwise
void rotate_counterclockwise(ship_object *spaceship) {
	int numberOfVertices = spaceship->poly.n_vertices;

	for (int i = 0; i < numberOfVertices; ++i) {
		rotate_coordinate(&spaceship->poly.x_coords[i], &spaceship->poly.y_coords[i], 0, 0, false);
	}	
  rotate_float_coordinate(&spaceship->x_orientation, &spaceship->y_orientation, 0, 0, false);
}

// Rotate a single coordinate
void rotate_coordinate(int32_t* xPosition, int32_t* yPosition, int32_t xCenter, int32_t yCenter, bool clockWise) {
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

// Rotate a single coordinate
void rotate_float_coordinate(float* xPosition, float* yPosition, int32_t xCenter, int32_t yCenter,  bool clockWise) {
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
