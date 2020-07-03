#pragma once

#include "Types.h"
#include "Archetype.h"
#include "System.h"
#include <vector>
#include <unordered_map>
#include <memory>

class ECS
{
public:
	ECS();
	EntityID CreateEntity();
	void DestroyEntity(const EntityID& entity);

	template<typename T>
	void AddComponentToEntity(EntityID entity, T component)
	{
		const char* typeName = typeid(T).name();

		// If component type is not found in component type map, register it
		if (componentTypes.find(typeName) == componentTypes.end())
		{
			RegisterComponentType(typeName);
		}

		ComponentTypeID componentTypeId = componentTypes.at(typeName);

		AddComponentToEntityArchetype(entity, component, componentTypeId);
	}

	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) == systems.end() && "Added system more than once.");

		std::shared_ptr<T> system = std::make_shared<T>();

		systems.insert({ typeName, system });

		systemToArchetypeMap.insert({ typeName, ArchetypeID() });

		return system;
	}

	// Registers component of type "C" with system of type "S"
	template<typename S, typename C>
	void AddComponentToSystem()
	{
		const char* systemTypeName = typeid(S).name();
		assert(systems.find(systemTypeName) != systems.end() && "System has not been registered.");

		const char* componentTypeName = typeid(C).name();
		if (componentTypes.find(componentTypeName) == componentTypes.end())
		{
			RegisterComponentType(componentTypeName);
		}

		// Set component in system's archetype ID
		ComponentTypeID componentTypeId = componentTypes.at(componentTypeName);
		ArchetypeID systemArchetypeId = systemToArchetypeMap.at(systemTypeName).set(componentTypeId, true);

		UpdateSystemArchetype(systemTypeName, systemArchetypeId);
	}

private:
	EntityID nextEntity;
	ComponentTypeID nextComponentType;

	std::unordered_map<EntityID, ArchetypeID> entityToArchetypeMap;
	std::unordered_map<ArchetypeID, std::shared_ptr<Archetype>> archetypes;
	std::unordered_map<const char*, ComponentTypeID> componentTypes;
	std::unordered_map<const char*, std::shared_ptr<System>> systems;
	std::unordered_map<const char*, ArchetypeID> systemToArchetypeMap;

	void RegisterComponentType(const char*& typeName);
	ArchetypeID UpdateEntityArchetypeID(const EntityID& entity, const ComponentTypeID& componentType, const bool& isSet);
	void CreateNewArchetype(const ArchetypeID& newArchetypeId);
	void UpdateSystemArchetype(const char*& systemTypeName, ArchetypeID& systemArchetypeId);

	template<typename T>
	void AddComponentToEntityArchetype(const EntityID& entity, T& component, const ComponentTypeID& componentType)
	{
		// Update entity archetype ID
		ArchetypeID oldArchetypeId = entityToArchetypeMap.at(entity);
		ArchetypeID newArchetypeId = UpdateEntityArchetypeID(entity, componentType, true);

		// If archetype does not exist, create it
		if (archetypes.find(newArchetypeId) == archetypes.end())
		{
			CreateNewArchetype(newArchetypeId);
		}

		// Get the new archetype
		std::shared_ptr<Archetype> newArchetype = archetypes.at(newArchetypeId);
		newArchetype->numEntities++;

		// Move existing components from entity's old archetype to the new archetype
		if (oldArchetypeId.any())
		{
			std::shared_ptr<Archetype> oldArchetype = archetypes.at(oldArchetypeId);
			oldArchetype->numEntities--;
			oldArchetype->TransferComponents(entity, newArchetype);
		}

		// Add new component to the entity's new archetype
		newArchetype->AddComponent(entity, component);
	}
};