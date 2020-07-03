#pragma once

#include "Archetype.h"
#include "Types.h"
#include <vector>
#include <memory>

class System
{
public:
	friend class ECS;
	std::vector<std::shared_ptr<Archetype>> archetypes;
private:
	ArchetypeID archetypeId;
};