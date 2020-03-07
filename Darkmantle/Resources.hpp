//
// Resources.hpp
// Pathal
//
// Contains a long selection of enums, constants, and basic structures
// used across all other files. They have been condensed here to reduce file
// space and include statements.
//

#pragma once

#include <stdio.h>
#include <vector>
#include <iostream>

#include "DamageBreakdown.hpp"

// level 120 stat conversion rates
#define CRIT_CONVERT  72.0*100
#define HASTE_CONVERT 68.0*100
#define VERSA_CONVERT 85.0*100

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
const int NUMBER_OF_BASE_DAMAGE_TYPES = 7; // + 12 combined

enum Actions {
    ACT_MH_ATTACK,
	ACT_OH_ATTACK,
	ACT_MUTILATE,
	ACT_ENVENOM,
	ACT_RUPTURE,
	ACT_VENDETTA,
	
	ACT_SINISTER_STRIKE,
    NUMBER_OF_ACTIONS
};

enum AuraAction {
	AURA_EMPTY,
	AURA_ADD,
	AURA_EXPIRE,
	AURA_REFRESH,
	NUMBER_OF_AURA_ACTIONS
};



union values {
	float   f_remaining;
	int     i_remaining;
	AuraAction action;
};



struct TimelineEvent {
public:
    float time;
    Actions act;
    TimelineEvent* next;
    
	// stores data for future processing, we union to save space
	// ie, tle->value.action; or tle->value.f_remaining;
	values value;
};
