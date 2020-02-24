//
//  main.cpp
//  Darkmantle
//

#include <iostream>
#include <stdio.h>
#include <stdexcept>
#include <vector>

#include "Resources.hpp"
#include "Engine.hpp"


int main(int argc, const char * argv[]) {
    std::cout << "Starting Darkmantle...\n";
    
    // Initialize statics
    Engine * e = Engine::getInstance();
	e->PopulateActions(); // fills out the actions list (aka initialize)
    
    // build timeline
    TimelineEvent * tle = new TimelineEvent;
    tle->time = 0.0;
    tle->act = ACT_MH_ATTACK;
	
	tle->next = new TimelineEvent;
	tle->next->act = ACT_OH_ATTACK;
	tle->next->time = 0.1;
	tle->next->next = nullptr;

    // lets do this
	e->Run(tle);
    
    std::cout << "Darkmantle complete.\n";
    return 0;
}
