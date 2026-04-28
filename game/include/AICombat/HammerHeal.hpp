#pragma once

#include <Canis/Entity.hpp>

#include <AICombat/HealerStateMachine.hpp>
#include <vector>

namespace AICombat
{

    class HammerHeal : public Canis::ScriptableEntity
    {
    public:
        static constexpr const char* ScriptName = "AICombat::HammerHeal";

        Canis::Entity* owner = nullptr;
        Canis::Vector3 sensorSize = Canis::Vector3(1.0f);
        int damage = 10;
        std::string targetTag = "";

        explicit HammerHeal(Canis::Entity& _entity) : Canis::ScriptableEntity(_entity) {}

        void Create() override;
        void Ready() override;
        void Update(float _dt) override;

        void CheckSensorEnter();

    private:
        HealerStateMachine* GetOwnerStateMachine();
        Canis::Entity* FindOwnerFromHierarchy() const;
        bool HasDamagedThisSwing(Canis::Entity& _target) const;

        std::vector<Canis::Entity*> m_hitTargetsThisSwing = {};
    };

    void RegisterHammerHealScript(Canis::App& _app);
    void UnRegisterHammerHealScript(Canis::App& _app);
}
