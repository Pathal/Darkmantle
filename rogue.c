//
//  rogue.c
//  Darkmantle
//
//  Created by Pathal.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rogue.h"
#include "darkmantle.h"
#include "stats.h"

void run_action(Event *node){
    //pulls the function pointer from the array
    //then passes the node pointer
    (*(actions[node->action_id].execute))(node);
}

void empty_function(Event *node){
    //pass, exists solely to prevent off by one checks, misreferenced functions, and enable '0' talents
}

void mh_attack(Event *node){
    printf("Action is MH_AUTOATTACK\n");
    //basic functionality
    update_power_regen(node);
    //calculate swing timer
    float swing_timer = node->event_stats.mh->speed / (node->event_stats.player_stats.haste_mult * get_multiplier_for_haste(node->event_stats.player_stats.haste));
    //add swing timer back into queue
    insert_timeline_event(make_new_timeline_event(node->time+swing_timer, MH_AUTOATTACK, NULL), node);//(node, (node.time + swing_timer, node.name, False))
    // self.queue_up_procs()
    
    //pull next event from queue
    TimelineEvent *next_event = pop_next_event(node);
    
    float crit_rate = .15 + get_percentage_for_crit(node->event_stats.player_stats.crit);
    //crit_rate += self.buffs.buff_all_crit() + self.race.get_racial_crit()
    crit_rate -= 0.03;
    
    float multistrike_rate = get_percentage_for_multistrike(node->event_stats.player_stats.multi);
    //multistrike_rate += self.buffs.multistrike_bonus()
    
    float d1 = node->event_stats.mh->speed * get_effective_attack_power() / 3.5 + (node->event_stats.mh->min_damage + node->event_stats.mh->max_damage)/2;
    d1 = (d1 * (1-crit_rate)) + (2.0 * d1 * crit_rate);
    d1 *= 1 + 2 * multistrike_rate * .3;
    add_damage_to_breakdown(d1, node);
    node->total_damage += d1;
    
    Event *o = make_new_event(next_event->action_id, next_event->time, node->total_damage, node->breakdown, node->event_stats, NULL, node, node->timeline);
    
    node->children = malloc(sizeof(Event*) * 1);
    node->children[0] = o;
    node->child_probability = malloc(sizeof(float) * 1);
    node->child_probability[0] = 1.0;
    
    node->children_populated = true;
    node->num_children = 1;
}

void oh_attack(Event *node){
    printf("Action is OH_AUTOATTACK\n");
}

//don't bother passing the name,
void build_action_struct(int index, int char_num, char* n, float gcd_size, void (*fnc), void (*fnc2)){
    actions[index].name = malloc(sizeof(char)*char_num);
    strcpy(actions[index].name, n);
    
    actions[index].gcd_size = gcd_size;
    actions[index].effect = fnc;
    actions[index].execute = fnc2;
}

void setup_class_data(){
    actions = malloc(sizeof(struct ClassAction) * NUM_CLASS_ACTIONS);
    
    //mh_autoattack is 13 chars long, plus 1 for end char
    build_action_struct(MH_AUTOATTACK, 14, "mh_autoattack\0", 0, mh_attack, mh_attack);
    
    //oh auto struct
    build_action_struct(OH_AUTOATTACK, 14, "oh_autoattack\0", 0, oh_attack, oh_attack);
}
