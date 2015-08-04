//
//  osx_tools.c
//  Darkmantle
//
//  Created by Pathal.
//

#include "osx_tools.h"

struct timeval* get_system_time_struct(){
    return malloc(sizeof(struct timeval));
}

void get_system_time_in_ms(struct timeval *p){
    gettimeofday(p, NULL);
}

double get_runtime_from_times(struct timeval *s, struct timeval *e){
    return (e->tv_sec - s->tv_sec) * 1000.0 + (e->tv_usec - s->tv_usec) / 1000.0;
}

long get_memory_usage(){
    //returns Kbytes
    struct rusage r_usage;
    getrusage(RUSAGE_SELF,&r_usage);
    return r_usage.ru_maxrss/1000;
}

