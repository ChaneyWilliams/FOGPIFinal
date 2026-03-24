#pragma once

#include <Canis/Entity.hpp>

enum BlockType {
    ROCK = 0,
    GOLD = 1,
    URANIUM = 2
};

class Block : public Canis::ScriptableEntity
{
private:

public:
    static constexpr const char* ScriptName = "Block";

    int blockType = BlockType::ROCK;

    Block(Canis::Entity &_entity) : Canis::ScriptableEntity(_entity) {}

    void Create();
    void Ready();
    void Destroy();
    void Update(float _dt);
};

extern void RegisterBlockScript(Canis::App& _app);
extern void UnRegisterBlockScript(Canis::App& _app);