#pragma once

#include <Canis/Entity.hpp>

#include <AICombat/BrawlerStateMachine.hpp>

#include <vector>

namespace AICombat
{

    class HammerDamage : public Canis::ScriptableEntity
    {
    public:
        static constexpr const char *ScriptName = "AICombat::HammerDamage";

        Canis::Entity *owner = nullptr;
        Canis::Vector3 sensorSize = Canis::Vector3(1.0f);
        int damage = 10;
        std::string targetTag = "";
        Canis::AudioAssetHandle bonkSfxPath1 = {.path = "assets/audio/sfx/bonk.ogg"};
        Canis::AudioAssetHandle bonkSfxPath2 = {.path = "assets/audio/sfx/bonk1.ogg"};
        float bonkSfxVolume = 1.0f;

        explicit HammerDamage(Canis::Entity &_entity) : Canis::ScriptableEntity(_entity) {}

        void Create() override;
        void Ready() override;
        void Update(float _dt) override;

        void CheckSensorEnter();

    private:
        void PlayBonksSfx();
        BrawlerStateMachine *GetOwnerStateMachine();
        Canis::Entity *FindOwnerFromHierarchy() const;
        bool HasDamagedThisSwing(Canis::Entity &_target) const;

        std::vector<Canis::Entity *> m_hitTargetsThisSwing = {};
        bool m_usedBonk1Sound = true;
    };

    void RegisterHammerDamageScript(Canis::App &_app);
    void UnRegisterHammerDamageScript(Canis::App &_app);
}
