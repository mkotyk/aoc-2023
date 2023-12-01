#ifndef AOC_2023_TIMER_H
#define AOC_2023_TIMER_H
#include <time.h>

void timer_start(time_t* t);
void time_end(time_t* t, const char *stage);

#endif
