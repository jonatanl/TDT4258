// Include guard
#ifndef _DRAW_H
#define _DRAW_H

#include "logic.h"

// Initialize resources
int init_draw(struct gamestate* gamestate);

// Release resources
int release_draw(void);

// Draw asteroids and spaceship to the framebuffer
void draw_all(void);

// Clear asteroids and spaceship from the framebuffer
void clear_all(void);

// Refreshes the whole display
// NOTE: might be inefficient
void update_display(void);

#endif // !_DRAW_H
