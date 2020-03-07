//
// Enemy.hpp
// Pathal
//

#pragma once

class Enemy {
protected:
	// Not actually used right now.
	float health;
	
	// for most attacks
	int armor;
	// armor constant
	int k_value;
	
	// rarely factored in
	int magic_resist;
	
	
public:
	virtual float GetPhysicalModifier() = 0;
	virtual float GetMagicalModifier() = 0;
	virtual float GetPureModifier() = 0;
};


// -- Modern Warcraft -- //
class BossEnemy : public Enemy {
	// The MOST important modifier
	float GetPhysicalModifier() {
		return (1 - armor / (armor + k_value));
	}
	
	// this is rarely anything but 1.0;
	float GetMagicalModifier() {
		return 1.0;
	}
	
	float GetPureModifier() {
		return 1.0;
	}
};


// -- Classic Warcraft -- //
class ClassicBossEnemy : public Enemy {
	// The MOST important modifier
	float GetPhysicalModifier() {
		int attacker_level = 60;
		return armor / (armor + (467.5 * attacker_level - 22167.5));
	}
	
	// this is rarely anything but 1.0;
	float GetMagicalModifier() {
		return 1.0;
	}
	
	float GetPureModifier() {
		return 1.0;
	}
};


