// Implementation of the input module

// Include guard
#ifndef _INPUT_H
#define _INPUT_H

// Specifies the meaning of the bits in processed game input
#define INPUT_ROTATE_LEFT   (1 << 0)
#define INPUT_ROTATE_RIGHT  (1 << 2)
#define INPUT_ACCELERATE    (1 << 3)
#define INPUT_SHOOT         (1 << 1)
#define INPUT_PAUSE         (1 << 4) // suggested functionality
#define INPUT_EXIT          (1 << 5) //
#define INPUT_DEBUG         (1 << 6)

// returns 0 if bit bit_number is 0, else return some integer > 0
#define CHECK_BIT(raw_int, bit_number)  (raw_int & (1 << bit_number))
#define FLIP_BIT(raw_int, bit_number)   (raw_int ^ (1 << bit_number))
#define SET_BIT(raw_int, bit_number)    (raw_int | (1 << bit_number))

#define CHECK_SHOOT(raw_int)            (raw_int & INPUT_SHOOT)
#define CHECK_LEFT(raw_int)             (raw_int & INPUT_ROTATE_LEFT)
#define CHECK_RIGHT(raw_int)            (raw_int & INPUT_ROTATE_RIGHT)
#define CHECK_ACC(raw_int)              (raw_int & INPUT_ACCELERATE)
#define CHECK_PAUSE(raw_int)            (raw_int & INPUT_PAUSE)
#define CHECK_DEBUG(raw_int)            (raw_int & INPUT_DEBUG)
#define CHECK_EXIT(raw_int)             (raw_int & INPUT_EXIT)

#define SET_SHOOT(raw_int)              (raw_int | INPUT_SHOOT)
#define SET_LEFT(raw_int)               (raw_int | INPUT_ROTATE_LEFT)
#define SET_RIGHT(raw_int)              (raw_int | INPUT_ROTATE_RIGHT)
#define SET_ACC(raw_int)                (raw_int | INPUT_ACCELERATE)

#define FLIP_SHOOT(raw_int)             (raw_int ^ INPUT_SHOOT)
#define FLIP_LEFT(raw_int)              (raw_int ^ INPUT_ROTATE_LEFT)
#define FLIP_RIGHT(raw_int)             (raw_int ^ INPUT_ROTATE_RIGHT)
#define FLIP_ACC(raw_int)               (raw_int ^ INPUT_ACCELERATE)

#include <stdint.h>

// Return the processed gamepad input as a bitfield of actions
uint8_t get_processed_input();
uint8_t get_input();

// Initialize and release module
int init_input();
int release_input();

#endif // !_INPUT_H
