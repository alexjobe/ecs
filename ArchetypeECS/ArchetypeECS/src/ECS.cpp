#include "ECS.h"
#include <cassert>

ECS::ECS() : nextEntity { 0 }, nextComponentType { 0 }
{
}

EntityID ECS::CreateEntity()
{
	EntityID newEntity = nextEntity;
	entityToArchetypeMap.insert({ newEntity, ArchetypeID() });
	++nextEntity;
	return newEntity;
}

void ECS::DestroyEntity(const EntityID& entity)
{
	if (entityToArchetypeMap.at(entity).any())
	{
		std::shared_ptr<Archetype> archetype = archetypes.at(entityToArchetypeMap.at(entity));
		archetype->EntityDestroyed(entity);
		entityToArchetypeMap.erase(entity);
	}
}

void ECS::RegisterComponentType(const char*& typeName)
{
	assert(nextComponentType < MAX_COMPONENTS && "Too many component types registered.");

	// Add component type to component type map
	componentTypes.insert({ typeName, nextComponentType });

	// Increment value for the next component type registered
	++nextComponentType;
}

ArchetypeID ECS::UpdateEntityArchetypeID(const EntityID& entity, const ComponentTypeID& componentType, const bool& isSet)
{
	ArchetypeID entityArchetype = entityToArchetypeMap.at(entity);

	// Make sure we aren't trying to add a component type more than once.
	if (isSet) { assert(entityArchetype.test(componentType) == false && "Component type added to same entity more than once."); }

	entityArchetype.set(componentType, isSet);
	entityToArchetypeMap.at(entity) = entityArchetype;

	return entityArchetype;
}

void ECS::CreateNewArchetype(const ArchetypeID& newArchetypeId)
{
	assert(archetypes.find(newArchetypeId) == archetypes.end() && "Added archetype more than once.");

	std::shared_ptr<Archetype> newArchetype = std::make_shared<Archetype>(newArchetypeId);

	archetypes.insert({ newArchetypeId, newArchetype });

	// Add new archetype to any systems that match it
	for (std::pair<const char*, std::shared_ptr<System>> pair : systems)
	{
		if ((pair.second->archetypeId & newArchetypeId) == pair.second->archetypeId)
		{
			pair.second->archetypes.push_back(archetypes.at(newArchetypeId));
		}
	}
}

void ECS::UpdateSystemArchetype(const char*& systemTypeName, ArchetypeID& systemArchetypeId)
{
	// Clear system archetype vector and update system archetype
	std::shared_ptr<System> system = systems.at(systemTypeName);
	system->archetypes.clear();
	system->archetypeId = systemArchetypeId;

	// Find any existing archetypes that contain the updated system archetype ID, and add them to the system
	for (std::pair<ArchetypeID, std::shared_ptr<Archetype>> pair : archetypes)
	{
		if ((pair.first & systemArchetypeId) == systemArchetypeId)
		{
			system->archetypes.push_back(pair.second);
		}
	}
}
