//
// EventState.hpp
// Pathal
//

#pragma once

#include "DamageBreakdown.hpp"
#include "Resources.hpp"

struct EventState {
	//state related
	double time;
    DamageBreakdown breakdown;
    DamageBreakdown final_breakdown;
    TimelineEvent* timeline = nullptr;
	float percentage;
	values value;
	
	
	
    // tree related
	int current_child = 0;
    std::vector<EventState*> children;
	bool processed = false;
    // TODO: Union?
    EventState* parent = nullptr; // for navigating the tree
    EventState* next = nullptr;   // for recycling
};
