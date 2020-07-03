#pragma once

#include "Types.h"
#include "ComponentVector.h"
#include <unordered_map>
#include <memory>

class Archetype
{
public:
	Archetype(ArchetypeID archetypeId);

	friend class ECS;
	
	size_t GetNumEntities() const;

	template<typename T>
	std::vector<T>& GetComponentVector()
	{
		const char* componentTypeName = typeid(T).name();

		return std::static_pointer_cast<ComponentVector<T>>(componentVectors.at(componentTypeName))->GetVector();
	}

private:
	ArchetypeID archetypeId;
	size_t numEntities;
	std::unordered_map<const char*, std::shared_ptr<IComponentVector>> componentVectors;

	void RemoveComponent(const EntityID& entity, const char*& componentTypeName);
	void EntityDestroyed(const EntityID& entity);
	std::shared_ptr<IComponentVector> GetComponentVector(const char*& componentTypeName);
	void TransferComponents(const EntityID& entity, std::shared_ptr<Archetype> newArchetype);

	template<typename T>
	void AddComponent(EntityID entity, T component)
	{
		const char* componentTypeName = typeid(T).name();

		// If the component vector doesn't exist yet, create it
		if (componentVectors.find(componentTypeName) == componentVectors.end())
		{
			componentVectors.insert({ componentTypeName, std::make_shared<ComponentVector<T>>() });
		}

		// Add component to component vector
		std::static_pointer_cast<ComponentVector<T>>(componentVectors.at(componentTypeName))->AddComponent(entity, component);
	}
};