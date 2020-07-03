#pragma once

#include "Types.h"
#include <vector>
#include <unordered_map>
#include <cassert>
#include <memory>

class IComponentVector
{
public:
	virtual ~IComponentVector() = default;
	virtual void RemoveComponent(const EntityID& entity) = 0;
	virtual void TransferComponents(const EntityID& entity, std::shared_ptr<IComponentVector> newVector) = 0;
	virtual void EntityDestroyed(const EntityID& entity) = 0;
	virtual std::shared_ptr<IComponentVector> CreateEmptyClone() const = 0;
};

template<typename T>
class ComponentVector : public IComponentVector
{
public:
	void AddComponent(const EntityID& entity, const T& component)
	{
		assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component type added to same entity more than once.");

		// Put new entry at end and update the maps
		size_t index = componentVector.size();
		componentVector.push_back(component);
		entityToIndexMap.insert({ entity, index });
		indexToEntityMap.insert({ index, entity });
	}

	void RemoveComponent(const EntityID& entity)
	{
		assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Attempting to remove component that does not exist.");

		// Copy last component into deleted component's place to maintain density
		size_t indexOfRemovedEntity = entityToIndexMap.at(entity);
		size_t indexOfLastEntity = componentVector.size() - 1;

		componentVector[indexOfRemovedEntity] = componentVector[indexOfLastEntity];
		componentVector.pop_back();

		// Update map to point to moved spot
		EntityID lastEntity = indexToEntityMap[indexOfLastEntity];
		entityToIndexMap.at(lastEntity) = indexOfRemovedEntity;
		indexToEntityMap.at(indexOfRemovedEntity) = lastEntity;

		entityToIndexMap.erase(entity);
		indexToEntityMap.erase(indexOfLastEntity);
	}

	std::vector<T>& GetVector()
	{
		return componentVector;
	}

	T& GetComponent(const EntityID& entity)
	{
		assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Attempting to retrieve component that does not exist.");

		// Return a reference to the entity's component
		return componentVector[entityToIndexMap.at(entity)];
	}

	void EntityDestroyed(const EntityID& entity)
	{
		if (entityToIndexMap.find(entity) != entityToIndexMap.end())
		{
			// Remove component if it exists
			RemoveComponent(entity);
		}
	}

	std::shared_ptr<IComponentVector> CreateEmptyClone() const
	{
		// Returns an empty ComponentVector that matches this one's type
		return std::make_shared<ComponentVector<T>>();
	}

	void TransferComponents(const EntityID& entity, std::shared_ptr<IComponentVector> newVector)
	{
		std::shared_ptr<ComponentVector<T>> newPtr = std::static_pointer_cast<ComponentVector<T>>(newVector);
		newPtr->AddComponent(entity, this->GetComponent(entity));
	}

private:
	std::vector<T> componentVector;

	// Map from entity ID to vector index.
	std::unordered_map<EntityID, size_t> entityToIndexMap;

	// Map from vector index to entity ID.
	std::unordered_map<size_t, EntityID> indexToEntityMap;
};