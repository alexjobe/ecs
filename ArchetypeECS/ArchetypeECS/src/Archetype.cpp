#include "Archetype.h"

Archetype::Archetype(ArchetypeID archetypeId) : archetypeId { archetypeId }, numEntities { 0 }
{
}

size_t Archetype::GetNumEntities() const
{
	return numEntities;
}

void Archetype::RemoveComponent(const EntityID& entity, const char*& componentTypeName)
{
	componentVectors.at(componentTypeName)->RemoveComponent(entity);
}

void Archetype::EntityDestroyed(const EntityID& entity)
{
	for (std::pair<const char*, std::shared_ptr<IComponentVector>> pair : componentVectors)
	{
		pair.second->EntityDestroyed(entity);
	}
	numEntities--;
}

std::shared_ptr<IComponentVector> Archetype::GetComponentVector(const char*& componentTypeName)
{
	return componentVectors.at(componentTypeName);
}

void Archetype::TransferComponents(const EntityID& entity, std::shared_ptr<Archetype> newArchetype)
{
	for (std::pair<const char*, std::shared_ptr<IComponentVector>> pair : componentVectors)
	{
		if (newArchetype->componentVectors.count(pair.first) == 0)
		{
			// If the new archetype does not contain a ComponentVector of this type yet, create it
			newArchetype->componentVectors.insert({ pair.first, pair.second->CreateEmptyClone() });
		}

		// Copy entity's component from this archetype to new archetype
		pair.second->TransferComponents(entity, newArchetype->GetComponentVector(pair.first));

		// Once component is moved, remove it from this archetype
		RemoveComponent(entity, pair.first);
	}
}