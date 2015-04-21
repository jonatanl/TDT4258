
// Input defines and macros
#define SHOOT         (1 << 0)
#define ROTATE_LEFT   (1 << 1)
#define ACCELERATE    (1 << 2)
#define ROTATE_RIGHT  (1 << 3)

// Suggested functionality
#define PAUSE         (1 << 4)
#define EXIT          (1 << 5)
#define CRASH         (1 << 6)  // Maybe it's useful to have a button crashing the game

// returns 0 if bit bit_number is 0, else return some integer > 0
#define CHECK_BIT(raw_int, bit_number)  (raw_int & (1 << bit_number))
#define FLIP_BIT(raw_int, bit_number)   (raw_int ^ (1 << bit_number))
#define SET_BIT(raw_int, bit_number)    (raw_int | (1 << bit_number))

#define CHECK_SHOOT(raw_int)            (raw_int & SHOOT)
#define CHECK_LEFT(raw_int)             (raw_int & ROTATE_LEFT)
#define CHECK_RIGHT(raw_int)            (raw_int & ROTATE_RIGHT)
#define CHECK_ACC(raw_int)              (raw_int & ACCELERATE)
#define CHECK_PAUSE(raw_int)            (raw_int & PAUSE)
#define CHECK_CRASH(raw_int)            (raw_int & CRASH)
#define CHECK_EXIT(raw_int)             (raw_int & EXIT)


#define SET_SHOOT(raw_int)              (raw_int | SHOOT)
#define SET_LEFT(raw_int)               (raw_int | ROTATE_LEFT)
#define SET_RIGHT(raw_int)              (raw_int | ROTATE_RIGHT)
#define SET_ACC(raw_int)                (raw_int | ACCELERATE)

#define FLIP_SHOOT(raw_int)             (raw_int ^ SHOOT)
#define FLIP_LEFT(raw_int)              (raw_int ^ ROTATE_LEFT)
#define FLIP_RIGHT(raw_int)             (raw_int ^ ROTATE_RIGHT)
#define FLIP_ACC(raw_int)               (raw_int ^ ACCELERATE)

#define ARG_MAX(a, b)                   (a < b ? a : b)
#define ARG_MIN(a, b)                   (a > b ? a : b)