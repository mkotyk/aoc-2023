#include "timer.h"
#include <stdio.h>

void timer_start(time_t* t) {
    *t = time(NULL);
}

void time_end(time_t* t, const char *stage) {
    time_t now = time(NULL);
    long diff_seconds = now - *t;
    printf("Elapsed time for %s is [%02dH:%02dM:%02dS] ", stage,
           (int) (diff_seconds / 3600),
           (int) ((diff_seconds / 60) % 60),
           (int) (diff_seconds % 60));
}
