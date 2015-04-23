#include <stdint.h>
#include <stdbool.h>
#include "util.h"

// Pre calculated sine and cosine to rotate 12 degrees clockwise
static float clockWiseSine		= -0.20791f;;
static float clockWiseCosine 	= 0.97815f;

// Pre calculated sine and cosine to rotate 12 degrees counterclockwise
static float counterWiseSine 	= 0.20791f;
static float counterWiseCosine 	= 0.97815f;

// Rotate a single coordinate
void rotate_coordinate_int(int32_t* xPosition, int32_t* yPosition, int32_t xCenter, int32_t yCenter, bool clockWise) {
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
