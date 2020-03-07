//
// Actor.cpp
// Pathal
//

#include "Actor.hpp"

// Actions here MUST match the Actions enum ordering
void ActorClass::PopulateActions() {
	actions.push_back(new MH_Attack()); //ACT_MH_ATTACK
	actions.push_back(new OH_Attack()); //ACT_OH_ATTACK
}

// A pure C++ action priority list
void ActorClass::ActionPriorityList() {
	//
}
