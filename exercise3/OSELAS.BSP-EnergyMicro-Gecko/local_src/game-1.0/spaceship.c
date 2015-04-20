#include "logic.h"

// Pre calculated sine and cosine to rotate 12 degrees clockwise
static ifloat clockWiseSine		= float_to_ifloat(0.97815);
static ifloat clockWiseCosine 	= float_to_ifloat(0.20791);

// Pre calculated sine and cosine to rotate 12 degrees counterclockwise
static ifloat counterWiseSine 		= float_to_ifloat(-0.20791);
static ifloat counterWiseCosine 	= float_to_ifloat(0.97815);

void rotate_clockwise(ship_object *spaceship) {
	int numberOfVertices = spaceship->poly->n_vertices;

	for (int i = 0; i < count; ++i) {
		rotate_coordinate(&spaceship->poly->x_coords[i], &spaceship->poly->y_coords[i], spaceship->x_pos, spaceship->y_pos, true);
	}	
}

void rotate_counterclockwise(ship_object *spaceship) {
	int numberOfVertices = spaceship->poly->n_vertices;

	for (int i = 0; i < count; ++i) {
		rotate_coordinate(spaceship->poly->x_coords[i], spaceship->poly->y_coords[i], spaceship->x_pos, spaceship->y_pos, false);
	}	
}

void rotate_coordinate(ifloat* xPosition, ifloat* yPosition, ifloat xCenter, ifloat yCenter, bool clockWise) {
	ifloat sine;
	ifloat cosine;

	if (clockWise) {
		sine = clockWiseSine;
		cosine = clockWiseCosine;
	} else {
		sine = counterWiseSine;
		cosine = counterWiseCosine;
	}

	ifloat xRotated = add(subtract(multiply(cosine, xPosition),
					multiply(sine, yPosition)),
					subtract(xCenter, add(multiply(xCenter, cosine), multiply(yCenter, sine))));

	ifloat yRotated = add(multiply(sine, xPosition),
					add(multiply(cosine, yPosition),
					subtract(yCenter, subtract(multiply(xCenter, sine), multiply(yCenter, cosine)))));

	*xPosition = xRotated;
	*yPosition = yRotated;
}
