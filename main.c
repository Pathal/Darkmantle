//
//  main.c
//  Project Darkmantle
//
//  Created by Pathal.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <iostream.h>

#include <sys/time.h>
#include <sys/resource.h>

#include "rogue.h"
#include "resources.h" //still not working for no reason

//'global' variables
Event *recycling_bin;        //next node is the child reference
Settings *engine_settings;   //

//declarations

//methods
struct Event* make_new_event(char *n, float t, int total_d, Stats *ev_stats, Event *childs, Event *par){
    //if the recycling is empty, just return a new one.
    //node0.parent = node1, node1.parent=node2, etc
    Event *node;
    if(recycling_bin == NULL){
        node = malloc(sizeof (struct Event));
    } else {
        node = recycling_bin;
        recycling_bin = node->parent;
    }
    
    node->name = n;
    node->time = t;
    node->total_damage = total_d;
    node->event_stats = ev_stats;
    node->children = childs;
    node->parent = par;
    
    return node;
}

void recycle_event(Event *node){
    node->parent = recycling_bin;
    recycling_bin = node;
}

void del_event(Event *node){
    //frees up the memory for a node, does not delete child nodes, only event references
    //name and event_stats are not references to other events, but strings and event subdata
    free(node->name);
    free(node->event_stats);
    free(node);
}

struct Stats* clone_stats_struct(){
    Stats *node = malloc(sizeof (struct Stats));
    //do the cloning
    
    return node;
}

bool end_calc_branch(float current_time, int total_damage_done){
    if(engine_settings->end_condition_type == TIME_BASED && engine_settings->end_condition_value < current_time){
        //printf("Fork ends at: %f seconds (%d damage)", current_time, total_damage_done);
        //print '--------'
        return true;
    }
    if(engine_settings->end_condition_type == DAMAGE_BASED && engine_settings->end_condition_value < total_damage_done){
        //printf("Fork ends at: %d damage (%f seconds)", total_damage_done, current_time);
        //print '--------'
        return true;
    }
    return false;
}

void try_to_populate(Event *node){
    //if the branch is done calculating
    if(end_calc_branch(node->time, node->total_damage)){
        //node.final_breakdown = node.breakdown
        //return
    }
    //if the children have already been populated
        //return
    //run the appropriate action/event-method on the node
}

void tree_walk_algorithm(Event event_queue){
    //should return damage breakown
    //breakdown = {} //emptry dict
    int total_d = 0;
    float t = 0;
    
    Stats base_stats = {.agi = 2900, .crit = 800, .haste = 1200, .mast = 1200, .vers = 400, .multi = 800};
    
    Event *current_node = make_new_event("", t, total_d, &base_stats, NULL, NULL);
    
    printf("%f", current_node->time);
    
    //get first event from event_queue
    //make root Event struct from it
    
    int l = 0; //iteration number
    while (1) {
        l++;
        if(l % 1000 == 0){
            printf("iteration %d", l);
        }
        
        //try to populate the children nodes
        try_to_populate(current_node);
        
        //if the current node has a next child
            //get it
        //else
            //if the parent is null/none/empty, we must be done
                //print out info
                //return info from method
            //send the data to the parent's final breakdown
            //set the parent as the current node
            //send the child we just left to the recycling bin
        break;
    }
}

int main(int argc, const char * argv[]) {
    printf("Darkmantle Initializing...\n");
    struct timeval t1, t2;
    struct rusage r_usage;
    gettimeofday(&t1, NULL);
    
    //basic init
    recycling_bin = NULL; //to point to a linked list of reusable events
    
    
    // read file from args string, file contains settings for runtime
    engine_settings = malloc(sizeof(struct Settings)); //creates the settings struct in global space
    
    
    printf("Loading spec...\n");
    
    //run tree walking algorithm
    
    //print output
    
    
    //final time check
    gettimeofday(&t2, NULL);
    printf("\n%f ms\n", (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0);
    getrusage(RUSAGE_SELF,&r_usage);
    printf("Memory usage = %ld Kbytes\n", r_usage.ru_maxrss/1000);
    return 0;
}



