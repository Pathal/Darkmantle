//
//  resources.h
//  Darkmantle
//
//  Created by Pathal.
//

#ifndef __Darkmantle__resources__
#define __Darkmantle__resources__

#include <stdio.h>
#include <stdbool.h>

enum expiration {
    TIME_BASED,
    DAMAGE_BASED
};

enum PlayerState {
    STATE_NORMAL,         //standing around doing nothing
    STATE_COMBAT,         //in combat, but doing nothing
    STATE_CASTING,        //out of combat, but casting (prefight casts)
    STATE_COMBAT_CASTING, //normal casting in combat
};

enum WeaponType {
    //1.7
    WEAPON_DAGGER,
    //2.4
    WEAPON_SWORD_1H,
    WEAPON_MACE_1H,
    WEAPON_AXE_1H,
    WEAPON_FIST,
    //2.8
    WEAPON_GUN,
    WEAPON_BOW,
    WEAPON_XBOW,
    //3.3
    WEAPON_SWORD_2H,
    WEAPON_MACE_2H,
    WEAPON_FIST_2H,
    WEAPON_AXE_2H,
    WEAPON_STAFF,
    WEAPON_POLEARM,
};

enum specialization {
    //death knight
    //druid
    //hunter
    //mage
    //monk
    //paladin
    //priest
    //rogue
    SPEC_ASSASSINATION,
    SPEC_COMBAT,
    SPEC_SUBTELTY,
    //shaman
    //warlock
    //warrior
    NUM_SPECS
};

typedef struct ClassAction {
    char *name; //string for output purposes
    //primary resource cost (-1 for all, >=0 otherwise)
    int primary_resource_cost;
    //secondary resource cost (-1 for all, >=0 otherwise)
    int secondary_resource_cost;
    float gcd_size;      //
    void (*execute)();   //consumes the resources
    void (*effect)();    //does the damage/mechanic (but doesn't generate child nodes)
    //effect is useful for things like DFA or sinister calling procs for rogues
} class_action;

typedef struct Stats {
    float primary_mult;
    int agi;
    float agi_mult;
    int str;
    float str_mult;
    int intel; //lol can't do 'int int;'
    float intel_mult;
    int stam;
    float stam_mult;
    int spi;
    float spi_mult;
    
    //ratings
    int crit;  //crit
    float crit_mult;
    int haste; //haste
    float haste_mult;
    int mast;  //mastery
    float mast_mult;
    int vers;  //versatility
    float vers_mult;
    int multi; //multistrike
    float multi_mult;
    int readi; //readiness (it still technically exists...)
    float readi_mult;
    
    //tertiary
    int speed;
    int leech;
    int avoid;
} Stats;

typedef struct Weapon {
    int min_damage;
    int max_damage;
    int speed;
    enum WeaponType type;
} Weapon;

typedef struct PlayerData {
    struct Stats player_stats;   //things like raw rating
    struct Weapon *mh;
    struct Weapon *oh;
    
    int min_power;
    int min_power_secondary;
    int max_power;
    int max_power_secondary;
    int current_power;
    int current_power_secondary;
    
    int stance;                  //wow.gamepedia.com/Stance, rogue: 0 normal, 1 stealth, 2 vanish, 3 ShD
    float damage_mult;           //
    float haste_value;
    float attack_speed;
    
    //enemy data
    int player_level;
    int target_level;
    
    //auras...?
    
    float last_event;            //the time of the last event calculated, useful for power regen calcs
    float regen_power;
    float regen_power_secondary;
} PlayerData;

typedef struct TimelineEvent {
    float time;                 //time that the event is supposed to occur
    int action_id;              //id for the event, use a class_actions enum
    char *extra;                //extra information about the even, eg remove_aura needs to know what to remove
    struct TimelineEvent *next; //references the next event in the timeline, linked lists don't need to be resized constantly
} TimelineEvent;

typedef struct Event {
    //engine
    int action_id; //contains the class action enum
    //breakdown; //array of ints, length of NUM_CLASS_ACTIONS
    float time; //float time of occurance
    TimelineEvent *timeline; //linked list
    int total_damage; //int
    ///state_values
    struct PlayerData event_stats;
    struct Event **children; //actually an array of pointers
    struct Event *parent;
    //child data
    int current_child;       //should start at 0
    int num_children;
    bool children_populated; //whether the children have been generated
    float *child_probability;//the likelihood of this child occuring
    //damage accumulation
    int *breakdown;          //array
    float *final_breakdown;  //array
    //extra
    char *extra_str;         //string of extra information
} Event;

typedef struct Settings {
    enum specialization spec; //enum for tracking what spec the player is
    enum expiration end_condition_type;
    float end_condition_value;
    float latency; //delay for actions, useful for GCD and switching from APL to action
    bool export_example; //print out a log of a potential sequence of events
    float apl_cd; //should be about .5s for now
    
    //array of ints, actually used for array of talent enums
    //each tier on the grid is one index in this array
    //just passing the talent enum to player_has_talent()
    //will automatically determine the array index
    int *talents;
    
    //next node is the child reference
    //could do recycling_bin->child->child->child
    Event *recycling_bin;
    
    //next node is the next reference
    //could do recycling_bin_timeline->next->next->next
    TimelineEvent *recycling_bin_timeline;
} Settings;

#endif /* defined(__Darkmantle__resources__) */
