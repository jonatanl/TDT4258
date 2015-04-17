// Include guard
#ifndef _DRAW_H
#define _DRAW_H

#include "logic.h"


// Initialize resources
int init_draw(struct gamestate* gamestate);

// Release resources
int teardown_draw();

// Draw a line to the display
void draw_line(int sx, int sy, int ex, int ey); 

// Clear a line in the display
void clear_line(int sx, int sy, int ex, int ey); 

#endif // !_DRAW_H
