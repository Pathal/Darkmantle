//
// DamageBreakdown.cpp
// Pathal
//

#pragma once

#include <vector>

class DamageBreakdown {
public:
    std::vector<float> damages;
    float total_damage;
    
	
	
	bool SetSize(int size);
    
	void CopyTo(DamageBreakdown& d);
    
	void AddDamage(float val, int action);
    
	void ResetBreakdown();
	
	void Print();
    
	DamageBreakdown();
    
	DamageBreakdown(int size);
    
	~DamageBreakdown();
};
