//
//  resources.h
//  Darkmantle
//
//  Created by Pathal
//

#ifndef __Darkmantle__resources__
#define __Darkmantle__resources__

#include <stdio.h>
#include <stdbool.h>

enum expiration {
    TIME_BASED,
    DAMAGE_BASED
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
};

typedef struct Stats {
    int agi;
    int str;
    int intellect; //lol can't do 'int int;'
    int stam;
    int spi;
    
    //ratings
    int crit;  //crit
    int haste; //haste
    int mast;  //mastery
    int vers;  //versatility
    int multi; //multistrike
    int readi; //readiness (it still technically exists...)
    
    //tertiary
    int speed;
    int leech;
    int avoid;
} Stats;

typedef struct Settings {
    enum specialization spec; //enum for tracking what spec the player is
    enum expiration end_condition_type;
    float end_condition_value;
    float latency; //delay for actions, useful for GCD and switching from APL to action
    bool export_example; //print out a log of a potential sequence of events
    float apl_cd; //should be about .5s for now
    
} Settings;

typedef struct Event {
    //engine
    char *name; //string defining object (could be pointer to action stuct?)
    //breakdown; //map/table/dict <string, float>
    float time; //float timer of
    //timeline //varying length array of
    int total_damage; //int
    ///state_values
    struct Stats *event_stats;
    struct Event *children; //actually an array
    struct Event *parent;
    //extra
    char *extra_str; //string of extra information
} Event;

typedef struct TimelineEvent {
    float time;                 //time that the event is supposed to occur
    char *name;                 //name of the event type
    char *extra;                //extra information about the even, eg remove_aura needs to know what to remove
    struct TimelineEvent *next; //references the next event in the timeline, linked lists don't need to be resized constantly
} TimelineEvent;


#endif /* defined(__Darkmantle__resources__) */
