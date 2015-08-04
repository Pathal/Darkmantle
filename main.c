//
//  main.c
//  Project Darkmantle
//
//  Created by Pathal.
//

///system
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <iostream.h>

#include "darkmantle.h"

///os specific
#if defined(__APPLE__) || defined(__linux__) // Apple OSX and linux (most POSIX systems)
#include "osx_tools.h"
#endif
#if defined(_WIN64) || defined(_WIN32) // Microsoft Windows
#include "win_tools.h"
#endif

int main(int argc, const char * argv[]) {
    printf("Darkmantle CLI Initializing...\n");
    //os specific time struct pointers
    void *start_time = get_system_time_struct();
    void *end_time = get_system_time_struct();
    get_system_time_in_ms(start_time);
    
    //build engine
    TimelineEvent *root_queue_node = initialize_engine();
    
    //run engine
    float *output = tree_walk_algorithm(root_queue_node, NULL);
    
    //print output
    printf("\n");
    for(int i=0; i<=NUM_CLASS_ACTIONS; i++) {
        printf("%d - %f\n", i, output[i]);
    }
    
    
    //final time check
    get_system_time_in_ms(end_time);
    printf("\n");
    printf("%f ms\n", get_runtime_from_times(start_time, end_time));
    printf("Memory usage = %ld Kbytes\n", get_memory_usage());
    
    return 0;
}



