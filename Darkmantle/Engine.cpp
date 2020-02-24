//
// Engine.cpp
// Pathal
//

#include "Engine.hpp"

Engine::Engine() {
    recycled_eventstates = nullptr;
    recycled_timelineevents = nullptr;
	
	resource_lower[PRIMARY_RESOURCE]   = 0;
	resource_lower[SECONDARY_RESOURCE] = 0;
	resource_lower[TERTIARY_RESOURCE]  = 0;
	
	resource_upper[PRIMARY_RESOURCE]   = 100;
	resource_upper[SECONDARY_RESOURCE] = 5;
	resource_upper[TERTIARY_RESOURCE]  = 100;
}

Engine::~Engine() {
    while (recycled_eventstates) {
        EventState* pop = recycled_eventstates;
        recycled_eventstates = recycled_eventstates->next;
        delete pop;
    }
    while (recycled_timelineevents) {
        TimelineEvent* pop = recycled_timelineevents;
        recycled_timelineevents = recycled_timelineevents->next;
        delete pop;
    }
}

Engine* Engine::getInstance() {
    if(!instance) {
        instance = new Engine;
    }
    return instance;
}

bool Engine::ReserveEvents(int num) {
    EventState * states = new EventState[num];
    if(!states) {
       return false; // There was a memory allocation failure!
    }
    // shuffle the array into the linked list
    for(int i = 0; i < num; i++) {
       // recycled_states;
       states[i].next = recycled_eventstates;
       recycled_eventstates = &(states[i]);
    }
    eventstate_batches.push_back(states);
    return true;
}

EventState* Engine::PullEvent(EventState * e) {
    EventState* node = nullptr;
	if(recycled_eventstates){
		node = recycled_eventstates;
		recycled_eventstates = node->next;
	} else {
		node = new EventState;
	}
	
	if (e){
		// copy the data over
		e->breakdown.CopyTo(node->breakdown);
		node->parent = e;
		node->timeline = CloneTimeline(e->timeline);
	} else {
		// otherwise it's brand new
		node->breakdown.ResetBreakdown();
		node->parent = nullptr;
		node->timeline = nullptr;
	}
	node->children.clear();
	node->percentage = 1.0f;
	node->current_child = 0;
	node->time = 0.0f;
	node->next = nullptr;

	return node;
}

// TODO: Populate from a Actor CLASS
void Engine::PopulateActions() {
    actions.push_back(new MH_Attack()); //ACT_MH_ATTACK
	actions.push_back(new OH_Attack()); //ACT_OH_ATTACK
}

bool Engine::HasNextChild(EventState * e) {
	if(!e) {
		return false;
	}
	if(e->current_child < e->children.size()){
		return true;
	}
    return false;
}

EventState* Engine::GetNextChild(EventState * e) {
	if(e->current_child < e->children.size()){
		return e->children.at(e->current_child++);
	}
    return nullptr;
}

void Engine::SendDataToParent(EventState * e) {
	for(int i = 0; i < NUMBER_OF_ACTIONS; i++) {
		float val = e->breakdown.damages.at(i) * e->percentage;
		e->parent->breakdown.AddDamage(val, i);
	}
    return;
}

void Engine::RecycleNode(EventState * e) {
	e->next = recycled_eventstates;
	recycled_eventstates = e;
}

TimelineEvent* Engine::PullTimelineEvent(TimelineEvent * e) {
	TimelineEvent* node = nullptr;
	if(recycled_timelineevents){
		node = recycled_timelineevents;
		recycled_timelineevents = node->next;
	} else {
		node = new TimelineEvent;
	}
	
	// if we were passed a node, we copy from it
	if (e){
		node->time = e->time;
		node->act = e->act;
		node->value = e->value; // union copy
	} else {
		// lets blank the data in case then
		node->time = 0;
		node->act = ACT_MH_ATTACK;
	}
	node->next = nullptr;
	
	return node;
}

// A deep copy of a given timeline
TimelineEvent* Engine::CloneTimeline(TimelineEvent * e) {
	if(!e) {
		return nullptr;
	}
	
	//build the base node for the linked list
	TimelineEvent *root = PullTimelineEvent(e); // shallow copy of e
    TimelineEvent *current_node = root;
    
	e = e->next;
    while(e != NULL){
        //make a new node then assign it
		current_node->next = PullTimelineEvent(e); //shallow copy e again
        
        //move on to the next one
        current_node = current_node->next;
        e = e->next;
    }
    
    return root;
}

void Engine::RecycleTimelineEvent(TimelineEvent * e){
	if(!e) {
		// I think you forgot something?
		return;
	}
	
	e->next = recycled_timelineevents;
	recycled_timelineevents = e;
}

TimelineEvent* Engine::GetNextTimelineEvent(EventState * e){
	if(!e) {
		// I think you forgot something?
		return nullptr;
	}
	
	TimelineEvent * node = e->timeline;
	e->timeline = e->timeline->next;
	
	return node;
}

void Engine::InsertTimelineEventIntoState(TimelineEvent * e, EventState * state) {
	if (!e || !state) {
		return;
	}
	
	// We have an empty timeline
	if(state->timeline == nullptr) {
		state->timeline = e;
		return;
	}
	
	// if the event should go first
	if(e->time < state->timeline->time) {
		e->next = state->timeline;
        state->timeline = e;
        return;
	}
	
	//iterate over list
    TimelineEvent *current_node = state->timeline;
    while(current_node->next != NULL){
        if (current_node->time < e->time && e->time < current_node->next->time){
            e->next = current_node->next;
            current_node->next = e;
            return;
        }
        current_node = current_node->next;
    }
    
    //append if it doesn't fit somewhere
    current_node->next = e;
    return;
}




// return false for an error, return true in any other situation
bool Engine::ProcessNode(EventState * event) {
	if (!event) {
		// We don't have anything to work with here!?!
		return false;
	}
	
	if(!event->timeline) {
		// The given TimelineEvent is blank, how are we supposed to process anything?
		// this is a very bad error
		return false;
	}
	
	if(event->processed) {
		// we already populated children and distributed effects, just return
		return true;
	}
	
	if(config.IsOver(event)) {
		//We're actually done with the branch, we've hit an end condition
		return true;
	}
	
	TimelineEvent * task = event->timeline;
	event->time = task->time;
	event->timeline = task->next;
	if(!actions.at(task->act)->Execute(event)) {
		// Something went wrong while executing the attack
		// this is probably due to some desynchronized pointer
		return false;
	}
	RecycleTimelineEvent(task);
	event->processed = true;
	
	return true;
}


// The core of the functionality
// Builds and traverses the tree, stores
void Engine::Run(TimelineEvent * e) {
    //get first event from event_queue
    //make root Event struct from it
    EventState * current_node = PullEvent();
	current_node->timeline = e;
    
    long l = 0; // iteration number
    long max_iterations = LONG_MAX;
    while (l < max_iterations){
		printf("\nCurrent node - iteration:%li - addr:%li\n", l, (long)current_node);
        
        // try to populate the children nodes
        // ie, execute the behaviour
		if(!current_node->processed) {
			printf("Doing action %i at time %f\n", current_node->timeline->act, current_node->timeline->time);
			if(!ProcessNode(current_node)){
				// There was an error in the process!
				// End the calculations to discourage problems.
				printf("PROCESSING ERROR on node addr, time t, action a\n");
				return;
			}
		}
        
        //if the current node has another child
        if(HasNextChild(current_node)){
            current_node = GetNextChild(current_node);
        } else {
            // if the parent is null, and no children left, we're done!
			// we've finished the root node!
            if (current_node->parent == nullptr) {
				current_node->breakdown.CopyTo(final_breakdown);
                return;
            }
            
            //send the data to the parent's final breakdown
            SendDataToParent(current_node);
            //set the parent as the current node
            EventState* recyclable = current_node;
            current_node = current_node->parent;
            //send the child we just left to the recycling bin
            RecycleNode(recyclable);
        }
    }
	
	// We're done calculating. Congratulations!
	
	return;
}

// singleton requirement, C++ is ugly sometimes
Engine *Engine::instance = 0;
