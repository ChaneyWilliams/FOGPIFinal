#pragma once

#include <Canis/Entity.hpp>

class PickupCollector : public Canis::ScriptableEntity
{
public:
    static constexpr const char* ScriptName = "PickupCollector";

    Canis::Entity* pickupCollider = nullptr;

    explicit PickupCollector(Canis::Entity &_entity) : Canis::ScriptableEntity(_entity) {}

    void Create();
    void Ready();
    void Destroy();
    void Update(float _dt);
};

extern void RegisterPickupCollectorScript(Canis::App& _app);
extern void UnRegisterPickupCollectorScript(Canis::App& _app);
