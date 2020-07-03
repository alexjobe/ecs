#include "ECS.h"
#include <iostream>

struct Position
{
    int x = 0;
    int y = 0;
};

struct PlayerController
{
    bool isMoving = false;
};

struct Velocity
{
    float x = 0;
    float y = 0;
};

struct CharacterTraits
{
    std::string name = "";
    std::string race = "";
};

struct EnemyAI
{
    bool isAttacking = false;
};

class PhysicsSystem : public System
{
public:
    void PrintPhysicsComponents()
    {
        // The physics system iterates through each archetype that contains Position and Velocity 
        // components. Each system has a vector of pointers to the archetypes it cares about
        std::cout << "Physics System" << std::endl << std::endl;
        for (std::shared_ptr<Archetype> archetype : archetypes)
        {
            std::vector<Position>& positions = archetype->GetComponentVector<Position>();
            std::vector<Velocity>& velocities = archetype->GetComponentVector<Velocity>();

            for (size_t i = 0; i < archetype->GetNumEntities(); ++i)
            {
                // Print components for each entity in the archetype
                std::cout << "  Position: " << positions[i].x << ", " << positions[i].y << std::endl;
                std::cout << "  Velocity: " << velocities[i].x << ", " << velocities[i].y << std::endl << std::endl;
            }
        }
    }
};

class CharacterInfoSystem : public System
{
public:
    void PrintInfo()
    {
        std::cout << "Character Info System" << std::endl << std::endl;
        for (std::shared_ptr<Archetype> archetype : archetypes)
        {
            std::vector<CharacterTraits>& traits = archetype->GetComponentVector<CharacterTraits>();
            std::vector<Position>& positions = archetype->GetComponentVector<Position>();

            for (size_t i = 0; i < archetype->GetNumEntities(); ++i)
            {
                std::cout << "  Name: " << traits[i].name << std::endl;
                std::cout << "  Race: " << traits[i].race << std::endl;
                std::cout << "  Position: " << positions[i].x << ", " << positions[i].y << std::endl << std::endl;
            }
        }
    }
};

class EnemySystem : public System
{

};

int main(int argc, char* args[])
{
    ECS ECS_Engine;

    // Here we register two systems with the ECS engine.
    auto physicsSystem = ECS_Engine.RegisterSystem<PhysicsSystem>();
    auto charInfoSystem = ECS_Engine.RegisterSystem<CharacterInfoSystem>();

    // We then register which components each system cares about.
    ECS_Engine.AddComponentToSystem<PhysicsSystem, Position>();
    ECS_Engine.AddComponentToSystem<PhysicsSystem, Velocity>();

    ECS_Engine.AddComponentToSystem<CharacterInfoSystem, CharacterTraits>();
    ECS_Engine.AddComponentToSystem<CharacterInfoSystem, Position>();

    // Create two entities. EntityID is just an int.
    EntityID player = ECS_Engine.CreateEntity();
    EntityID goblin = ECS_Engine.CreateEntity();

    // Add components to each entity. Notice that goblin does not have a Velocity.
    ECS_Engine.AddComponentToEntity(player, Position{ 234, 456 });
    ECS_Engine.AddComponentToEntity(player, Velocity{ 3.4f , 6.7f });
    ECS_Engine.AddComponentToEntity(player, CharacterTraits{ "Bob", "Human" });

    ECS_Engine.AddComponentToEntity(goblin, Position{ 567 , 876 });
    ECS_Engine.AddComponentToEntity(goblin, CharacterTraits{ "Griblub", "Goblin" });

    // Call functions from each system. The Physics system should only print
    // player's components, since goblin does not have a Velocity.
    charInfoSystem->PrintInfo();
    physicsSystem->PrintPhysicsComponents();

    return 0;
}