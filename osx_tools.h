//
//  osx_tools.h
//  Darkmantle
//
//  Created by Pathal.
//

#ifndef Darkmantle_osx_tools_h
#define Darkmantle_osx_tools_h

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
struct timeval* get_system_time_struct();
void get_system_time_in_ms(struct timeval*);
double get_runtime_from_times(struct timeval*, struct timeval*);
long get_memory_usage();

#endif
