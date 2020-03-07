//
// Engine.cpp
// Pathal
//

#include "Engine.hpp"

Engine::Engine() {
    recycled_eventstates = nullptr;
    recycled_timelineevents = nullptr;
	recycled_actorstates = nullptr;
	
	target = nullptr;
	
	//std::thread eventstate_allocator(    &Engine::EventStateAllocator,	this);
	//std::thread timelineevent_allocator( &Engine::TimelineEventAllocator,	this);
	//std::thread actorstate_allocator(    &Engine::ActorStateAllocator,	this);
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
	while (recycled_actorstates) {
        ActorState* pop = recycled_actorstates;
        recycled_actorstates = recycled_actorstates->next;
        delete pop;
    }
}

// Recycling allocators, constantly running in the background
void Engine::EventStateAllocator(Engine * e) {
	// while true, keep scanning for 
}
void Engine::TimelineEventAllocator(Engine * e) {
	//
}
void Engine::ActorStateAllocator(Engine * e) {
	//
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
	
	mutex_eventstate.lock();
	eventstate_pool_size += num;
    // shuffle the array into the linked list
    for(int i = 0; i < num; i++) {
       // recycled_states;
       states[i].next = recycled_eventstates;
       recycled_eventstates = &(states[i]);
    }
    eventstate_batches.push_back(states);
	mutex_eventstate.unlock();

	return true;
}

EventState* Engine::PullEvent(EventState * e) {
    EventState* node = nullptr;
	if(!recycled_eventstates) {
		ReserveEvents(EVENTSTATE_RESERVE_SIZE);
	}

	mutex_eventstate.lock();
	eventstate_pool_size--;
	node = recycled_eventstates;
	recycled_eventstates = node->next;
	mutex_eventstate.unlock();

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
	node->final_breakdown.ResetBreakdown();
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
		float val = e->final_breakdown.damages.at(i) * e->percentage;
		e->parent->final_breakdown.AddDamage(val, i);
	}
    return;
}

void Engine::RecycleNode(EventState * e) {
	e->next = recycled_eventstates;
	recycled_eventstates = e;
}

bool Engine::ReserveTimelineEvents(int num) {
    TimelineEvent * states = new TimelineEvent[num];
    if(!states) {
       return false; // There was a memory allocation failure!
    }
	
	mutex_timeline.lock();
	timelineevent_pool_size += num;
    // shuffle the array into the linked list
    for(int i = 0; i < num; i++) {
       // recycled_states;
       states[i].next = recycled_timelineevents;
       recycled_timelineevents = &(states[i]);
    }
    timelineevent_batches.push_back(states);
	mutex_timeline.unlock();

	return true;
}

TimelineEvent* Engine::PullTimelineEvent(TimelineEvent * e) {
	TimelineEvent* node = nullptr;
	if(!recycled_timelineevents){
		ReserveTimelineEvents(EVENTSTATE_RESERVE_SIZE);
	}
	
	mutex_timeline.lock();
	timelineevent_pool_size--;
	node = recycled_timelineevents;
	recycled_timelineevents = node->next;
	mutex_timeline.unlock();
	
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
	
	// this function copies data to the return breakdown that gets passed
	// upward to the parent, should be checked last
	if(config.IsOver(event)) {
		// well we're done, so copy the breakdown to the final breakdown
		// for "send to parent"
		event->breakdown.CopyTo(event->final_breakdown);
		return true;
	}
	
	if(event->processed) {
		// we already populated children and distributed effects, just return
		return true;
	}
	
	
	// copy timeline event data into the event state
	TimelineEvent * task = event->timeline;
	event->time = task->time;
	event->timeline = task->next;
	event->value = task->value;
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
		if(verbose){
			printf("\nCurrent node - iteration:%li - addr:%li\n", l, (long)current_node);
		}
        
        // try to populate the children nodes
        // ie, execute the behaviour
		if(!current_node->processed) {
			if(verbose){
				printf("Doing action %i at time %f\n", current_node->timeline->act, current_node->timeline->time);
			}
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
				current_node->final_breakdown.CopyTo(final_breakdown);
				final_breakdown.Print();
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
		
		l++; // iteration increment
    }
	
	// Wait... we probably should have already returned.
	return;
}

// singleton requirement, C++ is ugly sometimes
Engine *Engine::instance = 0;
