//
//  win_tools.h
//  Darkmantle
//
//  Created by Pathal.
//

#ifndef __Darkmantle__win_tools__
#define __Darkmantle__win_tools__

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
void get_system_time_in_ms(struct timeval*);
double get_system_uptime(struct timeval*, struct timeval*);

#endif /* defined(__Darkmantle__win_tools__) */
