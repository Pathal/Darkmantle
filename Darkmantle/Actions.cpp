//
// Actions.cpp
// Pathal
//

#include "Actions.hpp"
#include "Engine.hpp"

// -- Attack -- //

bool Attack::CanCast(EventState * event) {
    return true;
};

bool Attack::ConsumeResources(EventState * event) {
    return true;
};


// -- MH_Attack -- //

bool MH_Attack::Execute(EventState * event) {
    if(!event) {
        // there was an issue, abort
        return false;
    }
    
    // pull resources
	// well... we would if there were any to pull here
	
	// compute effects and damages
	float damage = 2.0f;
        
    // spawn appropriate children, clone from given event state
	EventState * c1 = Engine::getInstance()->PullEvent(event);
	
	c1->breakdown.AddDamage(damage, ACT_MH_ATTACK);
	
	// queue up next MH swing
	TimelineEvent * next_swing = Engine::getInstance()->PullTimelineEvent();
	next_swing->time = event->time + 0.9f;
	next_swing->act = ACT_MH_ATTACK;
	Engine::getInstance()->InsertTimelineEventIntoState(next_swing, c1);
	
	c1->percentage = 1.0f;
	
	event->children.push_back(c1);
    
    return true;
}


// -- OH_Attack -- //

bool OH_Attack::Execute(EventState * event) {
	if(!event) {
        // there was an issue, abort
        return false;
    }
	
	// compute effects and damages
	float damage = 1.5f;
        
    // spawn appropriate children, clone from given event state
	EventState * c1 = Engine::getInstance()->PullEvent(event);
	
	c1->breakdown.AddDamage(damage, ACT_OH_ATTACK);
	
	// queue up next MH swing
	TimelineEvent * next_swing = Engine::getInstance()->PullTimelineEvent();
	next_swing->time = event->time + 0.8f;
	next_swing->act = ACT_OH_ATTACK;
	Engine::getInstance()->InsertTimelineEventIntoState(next_swing, c1);
	
	c1->percentage = 1.0f;
	
	event->children.push_back(c1);
    
    return true;
}


// -- Sinister Strike -- //

bool SinisterStrike::Execute(EventState * event) {
	if(!event) {
        // there was an issue, abort
        return false;
    }
	
	// compute effects and damages
	float damage = 5.0f;
        
    // spawn appropriate children, clone from given event state
	EventState * c1 = Engine::getInstance()->PullEvent(event);
	c1->breakdown.AddDamage(damage, ACT_SINISTER_STRIKE);
	c1->percentage = 1.0f;
	
	// consume the resources
	// generate the resources
	
	// queue up the APL again
	
	event->children.push_back(c1);
    
    return true;
}
