
#define ARG_MAX(a, b)                   (a < b ? a : b)
#define ARG_MIN(a, b)                   (a > b ? a : b)

#define INTERSECTS(p1_min, p1_max, p2_min, p2_max)   ((p1_min >= p2_min && p1_min <= p2_max) || (p1_max >= p2_min && p1_max <= p2_max))
#define RANDOM(start, end) (start + (rand() % end))
