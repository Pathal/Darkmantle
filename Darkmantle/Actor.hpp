//
// Actor.hpp
// Pathal
//

#pragma once

#include <algorithm>
#include <iterator>

#include "Resources.hpp"
#include "Actions.hpp"
#include "Talents.hpp"

enum Auras {
	AURA_SLICE_AND_DICE,
	AURA_ADRENALINE_RUSH,
	NUMBER_OF_AURAS
};

struct ActorStats {
	int agi, intel, stam; // primary stats
	int crit, haste, mastery, multi, versatility, bonus_armor, pvp_power, pvp_resil;
	int avoid, speed, leech;
};

// Can't call them class Class, so we'll just make them all ActorClass
class ActorClass {
public:
	int resource_lower[NUMBER_OF_RESOURCES] = {0,0,0};
	int resource_upper[NUMBER_OF_RESOURCES] = {100,5,100};
	
	std::vector<Action *> actions;
	Talents * talent_list;
	
	
	// Actions here MUST match the Actions enum ordering
	void PopulateActions();
	
	// A pure C++ action priority list
	void ActionPriorityList();
};

class ActorState {
public:
	ActorClass * actor_type = nullptr;
	
	ActorStats stats;
	ActorStats bonus_stats;
	
	ActorState * next = nullptr; // used strictly for recycling;
	
	std::vector<Aura> auras;
	
	int resource[NUMBER_OF_RESOURCES] = {100,0,100};
	
public:
	
	bool AuraActive(enum Auras) {
		return false; // TODO: Actually Implement
	}
	
	
	float GetCritRate() {
		return 0.05 + (stats.crit + bonus_stats.crit)/(CRIT_CONVERT); // #define, must include parenthesis
	}
	
	float GetAttackPower() {
		return 0.0 + (stats.agi + bonus_stats.agi)*2.0;
	}
	
	float GetAttackSpeedModifier() {
		float att_speed = GetHasteMultiplier();
		if(AuraActive(AURA_SLICE_AND_DICE)){
			att_speed *= Aura::EffectValue();
		}
		return att_speed;
	}
	
	float GetHasteMultiplier() {
		return 1.0 + (stats.haste + bonus_stats.haste)/(HASTE_CONVERT); // #define, must include parenthesis
	}
	
	// adds a positive value from the resource array
	// does not verify amount sign
	void AddResource(ResourceTypes resource_type, int amount) {
		resource[resource_type] += amount;
		if(resource[resource_type] > actor_type->resource_upper[resource_type]){
			resource[resource_type] = actor_type->resource_upper[resource_type];
		}
		if(resource[resource_type] < actor_type->resource_lower[resource_type]){
			resource[resource_type] = actor_type->resource_lower[resource_type];
		}
	}
	
	// subtracts a positive value from the resource array
	// does not verify amount sign
	void ConsumeResource(ResourceTypes resource_type, int amount) {
		resource[resource_type] -= amount;
		if(resource[resource_type] > actor_type->resource_upper[resource_type]){
			resource[resource_type] = actor_type->resource_upper[resource_type];
		}
		if(resource[resource_type] < actor_type->resource_lower[resource_type]){
			resource[resource_type] = actor_type->resource_lower[resource_type];
		}
	}
	
	void CopyTo(ActorState & other) {
		other.actor_type = actor_type;
		other.stats = stats;
		std::copy(std::begin(resource), std::end(resource), std::begin(other.resource));
		other.next = nullptr;
	}
};


class RogueActor : public ActorClass {
	//
};
