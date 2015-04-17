
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

// Currently holds 360 sine vals
int8_t sine_LUT[] = {
0,2,4,6,8,11,13,15,17,19,22,24,26,28,30,32,34,37,39,41,43,45,47,49,51,53,55,57,59,61,63,65,67,69,71,72,74,76,78,79,81,83,84,86,88,89,91,92,94,95,97,98,100,101,102,104,105,106,107,108,109,111,112,113,114,115,116,116,117,118,119,120,120,121,122,122,123,123,124,124,125,125,125,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,125,125,125,124,124,123,123,122,122,121,120,120,119,118,117,116,116,115,114,113,112,111,110,108,107,106,105,104,102,101,100,98,97,95,94,92,91,89,88,86,85,83,81,79,78,76,74,72,71,69,67,65,63,61,59,57,55,53,51,49,47,45,43,41,39,37,35,32,30,28,26,24,22,19,17,15,13,11,8,6,4,2,0,-2,-4,-6,-8,-10,-13,-15,-17,-19,-21,-24,-26,-28,-30,-32,-34,-37,-39,-41,-43,-45,-47,-49,-51,-53,-55,-57,-59,-61,-63,-65,-67,-69,-70,-72,-74,-76,-78,-79,-81,-83,-84,-86,-88,-89,-91,-92,-94,-95,-97,-98,-100,-101,-102,-103,-105,-106,-107,-108,-109,-111,-112,-113,-114,-115,-115,-116,-117,-118,-119,-120,-120,-121,-122,-122,-123,-123,-124,-124,-125,-125,-125,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-126,-125,-125,-125,-124,-124,-123,-123,-122,-122,-121,-120,-120,-119,-118,-117,-116,-116,-115,-114,-113,-112,-111,-110,-108,-107,-106,-105,-104,-102,-101,-100,-98,-97,-95,-94,-92,-91,-89,-88,-86,-85,-83,-81,-80,-78,-76,-74,-72,-71,-69,-67,-65,-63,-61,-59,-57,-55,-53,-51,-49,-47,-45,-43,-41,-39,-37,-35,-33,-30,-28,-26,-24,-22,-20,-17,-15,-13,-11,-9,-6,-4,0}
