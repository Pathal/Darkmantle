//
// Scene.hpp
// Pathal
//

#pragma once

#include "Actor.hpp"
#include "Enemy.hpp"

struct Scene {
	std::vector<ActorClass *> actors_list;
	Enemy * target;
};
