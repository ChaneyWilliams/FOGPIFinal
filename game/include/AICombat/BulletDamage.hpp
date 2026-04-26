#pragma once

#include <Canis/Entity.hpp>

#include <AICombat/MageStateMachine.hpp>
#include <vector>

namespace AICombat
{

    class BulletDamage : public Canis::ScriptableEntity
    {
    public:
        static constexpr const char* ScriptName = "AICombat::BulletDamage";

        MageStateMachine* owner = nullptr;
        Canis::Vector3 sensorSize = Canis::Vector3(1.0f);
        int damage = 10;
        std::string targetTag = "";

        explicit BulletDamage(Canis::Entity& _entity) : Canis::ScriptableEntity(_entity) {}

        void Create() override;
        void Ready() override;
        void Update(float _dt) override;

        void CheckSensorEnter();

    private:

        std::vector<Canis::Entity*> m_hitTargetsThisSwing = {};
    };

    void RegisterBulletDamageScript(Canis::App& _app);
    void UnRegisterBulletDamageScript(Canis::App& _app);
}
