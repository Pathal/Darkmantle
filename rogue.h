//
//  rogue.h
//  Darkmantle
//
//  Created by Pathal.
//

#ifndef Darkmantle_rogue_h
#define Darkmantle_rogue_h

#include "resources.h"

//an array of the class action structs
class_action *actions;

void empty_function(Event*);

void mh_attack(Event*);
void oh_attack(Event*);
void slice_and_dice(Event*);

void sinister_strike(Event*);
void eviscerate(Event*);
void revealing_strike(Event*);
void main_gauche(Event*);

enum class_actions {
    //general
    MH_AUTOATTACK,
    OH_AUTOATTACK,
    SLICE_AND_DICE,
    
    //assassination
    
    //combat
    SINISTER_STRIKE,
    EVISCERATE,
    REVEALING_STRIKE,
    MAIN_GAUCHE,
    
    //subtlety
    NUM_CLASS_ACTIONS //automtically set to the number of enums in the list
};

enum class_talents {
    EMPTY_TALENT,
    //15
    TALENT_NIGHTSTALKER,
    TALENT_SUBTERFUGE,
    TALENT_SHADOW_FOCUS,
    //30
    TALENT_DEADLY_THROW,
    TALENT_NERVE_STRIKE,
    TALENT_COMBAT_READINESS,
    //45
    TALENT_CHEAT_DEATH,
    TALENT_LEECHING_POISON,
    TALENT_ELUSIVENESS,
    //60
    TALENT_CLOAK_AND_DAGGER,
    TALENT_SHADOWSTEP,
    TALENT_BURST_OF_SPEED,
    //75
    TALENT_PREY_ON_THE_WEAK,
    TALENT_INTERNAL_BLEEDING,
    TALENT_DIRTY_TRICKS,
    //90
    TALENT_SHURIKEN_TOSS,
    TALENT_MARKED_FOR_DEATH,
    TALENT_ANTICIPATION,
    //100
    TALENT_LEMON_ZEST,
    TALENT_SHADOW_REFLECTION,
    TALENT_DEATH_FROM_ABOVE,
    //
    NUM_CLASS_TALENTS
};


//pulls the action enum int from the action_id in the node, then executes that
void run_action(Event*);

//builds the arrays containing potential actions for the desired class (rogue)
void setup_class_data();

#endif