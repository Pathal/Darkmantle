//
//  win_tools.c
//  Darkmantle
//
//  Created by Pathal.
//

#include "win_tools.h"

void get_system_time_in_ms(struct timeval *p){
    gettimeofday(p, NULL);
}

double get_system_uptime(struct timeval *s, struct timeval *e){
    double d = (e->tv_sec - s->tv_sec) * 1000.0 + (e->tv_usec - s->tv_usec) / 1000.0;
    return d;
}