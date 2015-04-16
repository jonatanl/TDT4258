
// Input defines and macros

#define SHOOT         (1 << 0)
#define ROTATE_LEFT   (1 << 1)
#define ACCELERATE    (1 << 2)
#define ROTATE_RIGHT  (1 << 3)

// returns 0 if bit mask is 0, else return some integer > 0
#define CHECK_BIT(raw_int, mask)  (raw_int & (1 << mask))
#define CHECK_SHOOT(raw_int)      (raw_int & SHOOT)
#define CHECK_LEFT(raw_int)       (raw_int & ROTATE_LEFT)
#define CHECK_RIGHT(raw_int)      (raw_int & ROTATE_RIGHT)
#define CHECK_ACC(raw_int)        (raw_int & ACCELERATE)