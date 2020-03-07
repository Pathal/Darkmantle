//
// Actions.hpp
// Pathal
//

#pragma once

#include "Resources.hpp"
#include "EventState.hpp"

// -- Attack -- //

class Action {
public:
	virtual bool Execute(EventState * event = nullptr) = 0;
};


// Note: It's up to the Add() function to decide how and when to call Refresh()
// in the event that the timeline is not properly evaluated
class Aura : public Action {
public:
	virtual void Add(EventState * event = nullptr) 		= 0;
	virtual void Refresh(EventState * event = nullptr)	= 0;
	virtual void Expire(EventState * event = nullptr)	= 0;
	
	static float EffectValue(EventState * event = nullptr) {
		return 1.0f;
	}
	
	virtual bool Execute(EventState * event) {
		if(!event){
			return false;
		}
		
		switch(event->value.action) {
			case AURA_ADD:
				Add(event);
				break;
			case AURA_EXPIRE:
				Expire(event);
				break;
			case AURA_REFRESH:
				Refresh(event);
				break;
				
			default:
				// There must have been an issue in setting up the aura event
				return false;
		}
		return true;
	}
};




class Attack : public Action {
public:
    float Modifier_AP, base_damage;
    float duration;
    
    float GCD = 1.5;
	int resource_cost[NUMBER_OF_RESOURCES] = {0,0,0};
    
    
    virtual bool CanCast(EventState * event = nullptr);
    
    virtual bool ConsumeResources(EventState * event = nullptr);

    virtual bool Execute(EventState * event = nullptr) = 0;
};



// ---------- Universal ---------- //

// -- MH_Attack -- //

class MH_Attack : public Attack {
public:
    bool Execute(EventState * event);
};

// -- OH_Attack -- //
class OH_Attack : public Attack {
public:
	bool Execute(EventState * event);
};




// ---------- Assassination ---------- //

// -- Mutilate -- //
class Mutilate : public Attack {
	Mutilate() {
		resource_cost[PRIMARY_RESOURCE] = 50;
	}
	
	bool Execute(EventState * event);
};

// -- Envenom -- //
class Envenom : public Attack {
	Envenom() {
		resource_cost[PRIMARY_RESOURCE] = 35;
	}
	
	bool Execute(EventState * event);
};

// -- Rupture -- //
class Rupture : public Attack {
	Rupture() {
		resource_cost[PRIMARY_RESOURCE] = 25;
	}
	
	bool Execute(EventState * event);
};




// ---------- Outlaw ---------- //

// -- Sinister_Strike -- //
class SinisterStrike : public Attack {
	SinisterStrike() {
		resource_cost[PRIMARY_RESOURCE] = 45;
	}
	
	bool Execute(EventState * event);
};
