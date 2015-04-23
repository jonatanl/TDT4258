#include <stdbool.h>

#include "spaceship.h"
#include "logic.h"
#include "util.h"

// Rotate all vertices in ship clockwise
void rotate_spaceship(struct ship_object *spaceship, bool clockwise) {
	int numberOfVertices = spaceship->poly.n_vertices;

	for (int i = 0; i < numberOfVertices; ++i) {
		rotate_coordinate_int(&spaceship->poly.x_coords[i], &spaceship->poly.y_coords[i], 0, 0, clockwise);
	}
  rotate_coordinate_float(&spaceship->x_orientation, &spaceship->y_orientation, 0, 0, clockwise);
}
