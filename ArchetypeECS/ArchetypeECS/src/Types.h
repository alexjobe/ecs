#pragma once

#include <bitset>

using EntityID = unsigned int;

using ComponentTypeID = unsigned int;
const ComponentTypeID MAX_COMPONENTS = 32;

// Represents what components make up an archetype
using ArchetypeID = std::bitset<MAX_COMPONENTS>;