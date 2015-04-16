
// Input defines and macros

#define SHOOT         (1 << 0)
#define ROTATE_LEFT   (1 << 1)
#define ACCELERATE    (1 << 2)
#define ROTATE_RIGHT  (1 << 3)

// returns 0 if bit bit_number is 0, else return some integer > 0
#define CHECK_BIT(raw_int, bit_number)  (raw_int & (1 << bit_number))
#define FLIP_BIT(raw_int, bit_number)   (raw_int ^ (1 << bit_number))
#define SET_BIT(raw_int, bit_number)    (raw_int | (1 << bit_number))

#define CHECK_SHOOT(raw_int)            (raw_int & SHOOT)
#define CHECK_LEFT(raw_int)             (raw_int & ROTATE_LEFT)
#define CHECK_RIGHT(raw_int)            (raw_int & ROTATE_RIGHT)
#define CHECK_ACC(raw_int)              (raw_int & ACCELERATE)

#define SET_SHOOT(raw_int)              (raw_int | SHOOT)
#define SET_LEFT(raw_int)               (raw_int | ROTATE_LEFT)
#define SET_RIGHT(raw_int)              (raw_int | ROTATE_RIGHT)
#define SET_ACC(raw_int)                (raw_int | ACCELERATE)

#define FLIP_SHOOT(raw_int)             (raw_int ^ SHOOT)
#define FLIP_LEFT(raw_int)              (raw_int ^ ROTATE_LEFT)
#define FLIP_RIGHT(raw_int)             (raw_int ^ ROTATE_RIGHT)
#define FLIP_ACC(raw_int)               (raw_int ^ ACCELERATE)
