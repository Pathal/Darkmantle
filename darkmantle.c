//
//  darkmantle.c
//  Darkmantle
//
//  Created by Pathal.
//

#include "darkmantle.h"

//'global' variables
//just a struct with the engine settings
Settings *engine_settings;

//methods
struct Event* make_new_event(int id, float t, int total_d, int breakdown[], PlayerData ev_stats, Event **childs, Event *par, TimelineEvent *event_queue){
    //if the recycling is empty, just return a new one.
    //node0.parent = node1, node1.parent=node2, etc
    Event *node;
    
    //if there's no list to pull from, then make one
    if(engine_settings->recycling_bin == NULL){
        node = malloc(sizeof (struct Event));
    } else {
        node = engine_settings->recycling_bin;
        engine_settings->recycling_bin = node->parent;
    }
    
    node->action_id = id;
    node->time = t;
    node->total_damage = total_d;
    //the arrays will always be NUM_CLASS_ACTIONS long
    node->breakdown = malloc(sizeof(int) * NUM_CLASS_ACTIONS);
    node->final_breakdown = malloc(sizeof(int) * NUM_CLASS_ACTIONS);
    memcpy(node->breakdown, breakdown, sizeof(*breakdown) * NUM_CLASS_ACTIONS);
    for (int i = 0; i <= NUM_CLASS_ACTIONS; i++) {
        node->final_breakdown[i] = 0;
    }
    node->event_stats = ev_stats;
    node->children = childs;
    node->parent = par;
    node->timeline = clone_timeline(event_queue);
    
    node->current_child = 0;
    node->num_children = 0;
    node->child_probability = NULL;
    node->children_populated = false;
    
    return node;
}

struct TimelineEvent* make_new_timeline_event(float t, int id, char *extra){
    //if the recycling is empty, just return a new one.
    //node0.parent = node1, node1.parent=node2, etc
    TimelineEvent *node;
    
    //if there's no list to pull from, then make one
    if(engine_settings->recycling_bin_timeline == NULL){
        node = malloc(sizeof (struct TimelineEvent));
    } else {
        node = engine_settings->recycling_bin_timeline;
        engine_settings->recycling_bin_timeline = node->next;
    }
    
    node->time = t;
    node->action_id = id;
    node->extra = extra;
    node->next = NULL;
    
    return node;
}

struct Weapon* make_new_weapon(min, max, spd, type){
    Weapon *wep = malloc(sizeof(struct Weapon));
    
    wep->min_damage = min;
    wep->max_damage = max;
    wep->speed = spd;
    wep->type = type;
    
    return wep;
}

void recycle_timeline_event(TimelineEvent *event){
    event->next = engine_settings->recycling_bin_timeline;
    engine_settings->recycling_bin_timeline = event;
}

//recycles the entire linked list instead of just one event
void recycle_timeline_event_chain(TimelineEvent *event){
    TimelineEvent *start;
    while(event->next != NULL){
        start = event->next;
        recycle_timeline_event(event);
        event = start;
    }
    recycle_timeline_event(event);
}

void recycle_event(Event *node){
    recycle_timeline_event_chain(node->timeline);
    node->timeline = NULL;
    free(node->breakdown);
    free(node->final_breakdown);
    
    node->parent = engine_settings->recycling_bin;
    engine_settings->recycling_bin = node;
}

void del_event(Event *node){
    //shallow delete
    free(node);
}

struct Stats* clone_stats_struct(Stats *old_node){
    Stats *node = malloc(sizeof (struct Stats));
    //do the cloning
    *node = *old_node;
    return node; //return the reference so the memory isn't lost
}

bool end_calc_branch(float current_time, int total_damage_done){
    if(engine_settings->end_condition_type == TIME_BASED && engine_settings->end_condition_value < current_time){
        printf("Fork ends at: %f seconds (%d damage)", current_time, total_damage_done);
        return true;
    }
    if(engine_settings->end_condition_type == DAMAGE_BASED && engine_settings->end_condition_value < total_damage_done){
        printf("Fork ends at: %d damage (%f seconds)", total_damage_done, current_time);
        return true;
    }
    return false;
}

TimelineEvent* clone_timeline(TimelineEvent *node){
    //node is reusable, *node is not
    if(node == NULL){
        return NULL;
    }
    
    //build the base node for the linked list
    TimelineEvent *root = make_new_timeline_event(node->time, node->action_id, node->extra);
    TimelineEvent *current_node = root;
    
    while(node->next != NULL){
        //if there's no next node, stop
        
        //make a new node then assign it
        current_node->next = make_new_timeline_event(node->time, node->action_id, node->extra);
        
        //move on to the next one
        current_node = current_node->next;
        node = node->next;
    }
    
    return root;
}

void insert_timeline_event(TimelineEvent *event, Event *node){
    //if the linked list is empty
    if(node->timeline == NULL){
        node->timeline = event;
        return;
    }
    
    //if it should be before the first entry
    if (event->time < node->timeline->time) {
        event->next = node->timeline;
        node->timeline = event;
        return;
    }
    
    //iterate over list
    TimelineEvent *current_node = node->timeline;
    while(current_node->next != NULL){
        if (current_node->time < event->time && event->time < current_node->next->time){
            event->next = current_node->next;
            current_node->next = event;
            return;
        }
        current_node = current_node->next;
    }
    
    //append if it doesn't fit somewhere
    current_node->next = event;
    return;
}

TimelineEvent* remove_first_timeline_event(Event *node){
    TimelineEvent *event = node->timeline;
    node->timeline = event->next;
    
    return event;
}

//automatically determines the index and checks if the talent matches the value
bool player_has_talent(enum class_talents talent){
    if(engine_settings->talents[talent/3] == talent){
        return true;
    }
    return false;
}

void try_to_populate(Event *node){
    //if the branch is done calculating
    if(end_calc_branch(node->time, node->total_damage)){
        //node->final_breakdown = node->breakdown;
        return;
    }
    //if the children have already been populated
    if(node->children_populated){
        return;
    }
    printf("running action...\n");
    //run the appropriate action/event-method on the node
    run_action(node);
}

bool if_node_has_next_child(Event *node){
    if(node->current_child >= node->num_children){
        printf("checking next child: %d - %d\n", node->current_child, node->num_children);
        return false;
    }
    return true;
}

Event* get_next_child(Event *node){
    //increment the counter before returning, but grab the correct child (not the next one)
    node->current_child += 1;
    return (node->children[node->current_child-1]);
}

TimelineEvent* pop_next_event(Event *node){
    TimelineEvent *tmp = node->timeline;
    node->timeline = node->timeline->next;
    return tmp;
}

void recycle_last_child(Event *node){
    recycle_event(node->children[(node->current_child-1)]);
}

void send_data_to_parent(Event *node){
    //TODO
}

void update_power_regen(Event *node){
    //node->time - node->event_stats.last_event;
    
    //TODO: add haste auras
    float regen = node->event_stats.regen_power * (node->event_stats.player_stats.haste_mult * node->event_stats.player_stats.haste);
    regen *= (node->time - node->event_stats.last_event);
    node->event_stats.current_power += regen;
    //if power is outside the bounds, snap back
    if(node->event_stats.current_power > node->event_stats.max_power){
        node->event_stats.current_power = node->event_stats.max_power;
    } else if (node->event_stats.current_power < node->event_stats.min_power) {
        node->event_stats.current_power = node->event_stats.min_power;
    }
    
    node->event_stats.last_event = node->time;
}

int get_effective_attack_power(){
    //TODO
    return 4000;
}

void add_damage_to_breakdown(int damage, Event *node){
    node->breakdown[node->action_id] += damage;
}

float* tree_walk_algorithm(TimelineEvent *event_queue, PlayerData *player_stats){
    //should return damage breakown
    //breakdown = {} //empty dict
    int *breakdown = malloc(sizeof(int) * NUM_CLASS_ACTIONS);
    float *final_breakdown = malloc(sizeof(float) * NUM_CLASS_ACTIONS);
    int total_d = 0;
    
    //dummy/test generated data, to be replaced by player_stats input
    Stats base_stats = {.agi = 2900, .crit = 800, .haste = 1200, .mast = 1200, .vers = 400, .multi = 800,
                        .agi_mult = 1.0, .crit_mult = 1.0, .haste_mult = 1.0, .mast_mult = 1.0, .vers_mult = 1.0, .multi_mult = 1.0};
    
    Weapon *mh_wep = make_new_weapon(1076, 2000, 2.6, WEAPON_MACE_1H);//124371 - Hammer of Wicked Infusion
    Weapon *oh_wep = mh_wep; //reuse, why not
    
    PlayerData root_player_data = {.player_stats = base_stats, .mh = mh_wep, .oh = oh_wep,
                        .min_power=0, .min_power_secondary=0, .max_power=100, .max_power_secondary=5, .current_power=100, .current_power_secondary=0,
                        .stance=1, .damage_mult=1.0, .haste_value=1.0, .attack_speed=1.0, .player_level=100, .target_level=103,
                        .last_event=0, .regen_power=10, .regen_power_secondary=0};
    
    
    //TODO: This pile of crap
    
    //get first event from event_queue
    //make root Event struct from it
    Event *current_node = make_new_event(event_queue->action_id, event_queue->time, total_d, breakdown, root_player_data, NULL, NULL, event_queue->next);
    //recycle_timeline_event(event_queue);
    insert_timeline_event(make_new_timeline_event(2.6, MH_AUTOATTACK, ""), current_node);
    
    
    int l = 0; //iteration number
    while(true) {
        l++;
        printf("\nCurrent node - id:%d time:%f iteration:%d\n", current_node->action_id, current_node->time, l);
        if(l % 1000 == 0){
            printf("iteration %d\n", l);
        }
        
        //try to populate the children nodes
        try_to_populate(current_node);
        
        //if the current node has a next child
        if(if_node_has_next_child(current_node)){
            current_node = get_next_child(current_node);
        } else {
            //if the parent is null/none/empty, we must be done
            if(current_node->parent == NULL){
                return final_breakdown;
            }
            //send the data to the parent's final breakdown
            send_data_to_parent(current_node);
            //set the parent as the current node
            current_node = current_node->parent;
            //send the child we just left to the recycling bin
            recycle_last_child(current_node);
        }
    }
    
    return final_breakdown;
}

TimelineEvent* initialize_engine(){
    engine_settings = malloc(sizeof(struct Settings)); //creates the settings struct in global space
    
    // read file from args string, file contains settings for runtime
    
    //basic init
    engine_settings->recycling_bin = NULL; //to point to a linked list of reusable events
    engine_settings->recycling_bin_timeline = NULL;
    setup_class_data();
    engine_settings->end_condition_type = TIME_BASED;
    engine_settings->end_condition_value = 10;
    engine_settings->apl_cd = .1;
    engine_settings->latency = .05; //50ms
    engine_settings->spec = SPEC_COMBAT;
    
    //talents
    char *tal = "3000021"; //seal fate, mfd, lemonzest in this temp list
    engine_settings->talents = malloc(sizeof(int) * (NUM_CLASS_TALENTS/3));
    for(int i = 0; tal[i] != '\0'; i++) {
        //for each character in the string
        //set the appropriate index in the enum array to that talent's number
        //talent number is equal to: (char_index*3) + char_number
        //(int)(tal[i]-'0') generates integer value for the char, type cast to avoid warnings
        //0 is an empty talent
        if(tal[i] != '0'){
            engine_settings->talents[i] = i*3 + (int)(tal[i]-'0');
        } else {
            engine_settings->talents[i] = 0;
        }
        printf("Talent: %d\n", engine_settings->talents[i]);
    }
    
    printf("Loading spec...\n\n");
    
    //build event queue
    //TODO: APL should go first
    TimelineEvent *root_queue_node = make_new_timeline_event(0, MH_AUTOATTACK, "");
    //root_queue_node->next = make_new_timeline_event(0.01, OH_AUTOATTACK, "");
    
    //run tree walking algorithm
    return root_queue_node;
}