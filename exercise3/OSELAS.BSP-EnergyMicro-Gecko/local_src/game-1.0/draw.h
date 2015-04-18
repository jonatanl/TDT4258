// Include guard
#ifndef _DRAW_H
#define _DRAW_H

#include "logic.h"


// Initialize resources
int init_draw(struct gamestate* gamestate);

// Release resources
int teardown_draw(void);

// Draw asteroids and spaceship to the framebuffer
void draw_all(void);

// Clear asteroids and spaceship from the framebuffer
void clear_all(void);

#endif // !_DRAW_H
