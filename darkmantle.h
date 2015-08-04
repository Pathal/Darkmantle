//
//  darkmantle.h
//  Darkmantle
//
//  Created by Pathal.
//

#ifndef __Darkmantle__darkmantle__
#define __Darkmantle__darkmantle__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include <iostream.h>
#include <sys/time.h>
#include <sys/resource.h>

///project
#include "rogue.h"
#include "resources.h"

struct Event* make_new_event(int, float, int, int*, PlayerData, Event**, Event*, TimelineEvent*);
struct TimelineEvent* make_new_timeline_event(float, int, char*);
TimelineEvent* initialize_engine();
void recycle_timeline_event(TimelineEvent*);
TimelineEvent* clone_timeline(TimelineEvent*);
float* tree_walk_algorithm(TimelineEvent*, PlayerData*);
void update_power_regen(Event*);
void insert_timeline_event(TimelineEvent*, Event*);
TimelineEvent* pop_next_event(Event*);

int get_effective_attack_power();
void add_damage_to_breakdown(int, Event*);

#endif /* defined(__Darkmantle__darkmantle__) */
