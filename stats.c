//
//  stats.c
//  Darkmantle
//
//  Created by Pathal.
//

#include "stats.h"

float get_multiplier_for_haste(int rating){
    return 1 + rating / (90.0 * 100);
}

float get_percentage_for_crit(int rating){
    return rating / (110.0 * 100);
}

float get_value_for_mastery(int rating){
    return 8 + rating / (110.0 * 100);
}

float get_percentage_for_multistrike(int rating){
    return rating / (66.0 * 100);
}

float get_multiplier_for_versatility(int rating){
    return 1 + rating / (130.0 * 100);
}