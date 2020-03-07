//
// DamageBreakdown.cpp
// Pathal
//

#include "DamageBreakdown.hpp"
#include "Resources.hpp"

bool DamageBreakdown::SetSize(int size) {
	damages.reserve(size);
	if(damages.capacity() != size) {
		return false;   // The memory allocation failed!
	}
	return true;
}

void DamageBreakdown::CopyTo(DamageBreakdown& d) {
	d.damages = damages;
	d.total_damage = total_damage;
}

void DamageBreakdown::AddDamage(float val, int action) {
	damages.at(action) += val;
	total_damage += val;
}

void DamageBreakdown::ResetBreakdown() {
	for(int i = 0; i < damages.size(); i++) {
		damages.at(i) = 0;
	}
	for(int i = 0; i < NUMBER_OF_ACTIONS; i++) {
		damages.push_back(0);
	}
	total_damage = 0;
}

void DamageBreakdown::Print() {
	printf("\n-- Final damage breakdown and composition:\n");
	for(int i = 0; i < damages.size(); i++) {
		printf("Action %i\t dealt %f damage\n", i, damages.at(i));
	}
	printf("-- Dealing a total of %f damage\n", total_damage);
	printf("\n");
}

DamageBreakdown::DamageBreakdown() {
	DamageBreakdown((int)NUMBER_OF_ACTIONS);
}

DamageBreakdown::DamageBreakdown(int size) {
	SetSize(size);
}

DamageBreakdown::~DamageBreakdown() {
	//
}
