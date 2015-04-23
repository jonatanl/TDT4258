// Include guard
#ifndef _SPACESHIP_H
#define _SPACESHIP_H

#include "logic.h"

// Initialize a spaceship
void init_spaceship(struct spaceship** ship);

// Release the spaceship resources
void release_spaceship();

// Update the spaceship with input
void update_spaceship(uint8_t input);

#endif // !_SPACESHIP_H
