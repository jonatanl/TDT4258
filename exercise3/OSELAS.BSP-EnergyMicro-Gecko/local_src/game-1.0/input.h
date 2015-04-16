// Implementation of the input module

// Include guard
#ifndef _INPUT_H
#define _INPUT_H

#include <stdint.h>

// Defines the meaning of the bits in processed input
#define SHOOT         (1 << 0)
#define ROTATE_LEFT   (1 << 1)
#define ACCELERATE    (1 << 2)
#define ROTATE_RIGHT  (1 << 3)

// Return the processed gamepad input as a bitfield of actions
uint8_t get_processed_input();

struct input{
    uint8_t shoot : 1;
    uint8_t turn_left : 1;
    uint8_t accelerate : 1;
    uint8_t turn_right : 1;
};

typedef struct input input;

#endif // !_INPUT_H
