//
// Resources.hpp
// Pathal
//

#pragma once

#include <stdio.h>
#include <vector>
#include <iostream>

enum ResourceTypes {
    PRIMARY_RESOURCE,
    SECONDARY_RESOURCE,
    TERTIARY_RESOURCE,
    NUMBER_OF_RESOURCES
};

enum DamageTypes {
    DAMAGE_PHYSICAL = 1 << 0,
    DAMAGE_HOLY     = 1 << 1,
    DAMAGE_FIRE     = 1 << 2,
    DAMAGE_NATURE   = 1 << 3,
    DAMAGE_FROST    = 1 << 4,
    DAMAGE_SHADOW   = 1 << 5,
    DAMAGE_ARCANE   = 1 << 6,
    
    // 2 combined
    DAMAGE_FIRESTORM    = DAMAGE_FIRE | DAMAGE_NATURE,
    DAMAGE_FLAMESTRIKE  = DAMAGE_FIRE | DAMAGE_PHYSICAL,
    DAMAGE_RADIANT      = DAMAGE_FIRE | DAMAGE_HOLY,
    DAMAGE_SHADOWFLAME  = DAMAGE_FIRE | DAMAGE_SHADOW,
    DAMAGE_PLAGUE       = DAMAGE_NATURE | DAMAGE_SHADOW,
    DAMAGE_SHADOWSTRIKE = DAMAGE_SHADOW | DAMAGE_PHYSICAL,
    DAMAGE_TWILIGHT     = DAMAGE_SHADOW | DAMAGE_HOLY,
    DAMAGE_ASTRAL       = DAMAGE_ARCANE | DAMAGE_NATURE,
    DAMAGE_SPELLFROST   = DAMAGE_ARCANE | DAMAGE_FROST,
    DAMAGE_SPELLSTRIKE  = DAMAGE_ARCANE | DAMAGE_PHYSICAL,
    
    // triple
    DAMAGE_ELEMENTAL    = DAMAGE_FIRE | DAMAGE_FROST | DAMAGE_NATURE,
    
    // all combined
    DAMAGE_CHAOS        = DAMAGE_PHYSICAL | DAMAGE_HOLY | DAMAGE_FIRE | DAMAGE_NATURE | DAMAGE_FROST | DAMAGE_SHADOW | DAMAGE_ARCANE
};
const int NUMBER_OF_BASE_DAMAGE_TYPES = 7; //12 combined

enum Actions {
    ACT_MH_ATTACK,
	ACT_OH_ATTACK,
    NUMBER_OF_ACTIONS
};


class TimelineEvent {
public:
    float time;
    Actions act;
    TimelineEvent* next;
    
    union vals {
        float   f_remaining;
        int     i_remaining;
    } value;
};

class DamageBreakdown {
public:
    std::vector<float> damages;
    float total_damage;
    
    bool SetSize(int size) {
        damages.reserve(size);
        if(damages.capacity() != size) {
            return false;   // The memory allocation failed!
        }
        return true;
    }
    
    void CopyTo(DamageBreakdown& d) {
        d.damages = damages;
        d.total_damage = total_damage;
    }
    
    void AddDamage(float val, int action) {
        damages.at(action) += val;
        total_damage += val;
    }
    
    void ResetBreakdown() {
        for(int i = 0; i < damages.size(); i++) {
            damages.at(i) = 0;
        }
		for(int i = 0; i < NUMBER_OF_ACTIONS; i++) {
			damages.push_back(0);
		}
        total_damage = 0;
    }
    
    DamageBreakdown() {
        DamageBreakdown((int)NUMBER_OF_ACTIONS);
    }
    
    DamageBreakdown(int size) {
        SetSize(size);
    }
    
    ~DamageBreakdown() {
        //
    }
};

struct EventState {
	//state related
	double time;
    DamageBreakdown breakdown;
    TimelineEvent* timeline = nullptr;
	float percentage;
	
	
    // tree related
	int current_child = 0;
    std::vector<EventState*> children;
	bool processed = false;
    // TODO: Union?
    EventState* parent = nullptr; // for navigating the tree
    EventState* next = nullptr;   // for recycling
};


// -- Attack -- //

class Attack {
public:
    float Modifier_AP, base_damage;
    float duration;
    
    float GCD = 1.5;
    
    
    virtual bool CanCast(EventState * event = nullptr);
    
    virtual bool ConsumeResources(EventState * event = nullptr);

    virtual bool Execute(EventState * event = nullptr) = 0;
};


// -- MH_Attack -- //

class MH_Attack : public Attack {
    float GCD = 1.02f;
    
public:
    bool Execute(EventState * event);
};

class OH_Attack : public Attack {
	float GCD = 1.02f;
public:
	bool Execute(EventState * event);
};
