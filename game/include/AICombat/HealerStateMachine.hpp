#pragma once

#include <Canis/Entity.hpp>

#include <SuperPupUtilities/StateMachine.hpp>
#include <AICombat/ICombatant.hpp>

#include <string>

namespace AICombat
{
    class HealerStateMachine;

    class HealerIdleState : public SuperPupUtilities::State
    {
    public:
        static constexpr const char* Name = "HealerIdleState";

        explicit HealerIdleState(SuperPupUtilities::StateMachine& _stateMachine);
        void Enter() override;
        void Update(float _dt) override;
    };

    class HealerChaseState : public SuperPupUtilities::State
    {
    public:
        static constexpr const char* Name = "HealerChaseState";
        float moveSpeed = 4.0f;

        explicit HealerChaseState(SuperPupUtilities::StateMachine& _stateMachine);
        void Enter() override;
        void Update(float _dt) override;
    };

    class HealerHammerTimeState : public SuperPupUtilities::State
    {
    public:
        static constexpr const char* Name = "HealerHammerTimeState";
        float hammerRestDegrees = 0.0f;
        float hammerSwingDegrees = 30.0f;
        float attackRange = 2.25f;
        float attackDuration = 0.75f;
        float attackDamageTime = 0.25f;

        explicit HealerHammerTimeState(SuperPupUtilities::StateMachine& _stateMachine);
        void Enter() override;
        void Update(float _dt) override;
        void Exit() override;
    };

    class HealerStateMachine : public SuperPupUtilities::StateMachine, public ICombatant
    {
    public:
        static constexpr const char* ScriptName = "AICombat::HealerStateMachine";

        std::string targetTag = "";
        float detectionRange = 20.0f;
        Canis::Vector3 bodyColliderSize = Canis::Vector3(1.0f);
        int maxHealth = 40;
        bool logStateChanges = true;
        Canis::Entity* hammerVisual = nullptr;
        Canis::AudioAssetHandle hitSfxPath1 = { .path = "assets/audio/sfx/hit_1.ogg" };
        Canis::AudioAssetHandle hitSfxPath2 = { .path = "assets/audio/sfx/hit_2.ogg" };
        float hitSfxVolume = 1.0f;
        Canis::SceneAssetHandle deathEffectPrefab = { .path = "assets/prefabs/brawler_death_particles.scene" };

        explicit HealerStateMachine(Canis::Entity& _entity);

        HealerIdleState idleState;
        HealerChaseState chaseState;
        HealerHammerTimeState hammerTimeState;

        void Create() override;
        void Ready() override;
        void Destroy() override;
        void Update(float _dt) override;

        Canis::Entity* FindClosestTarget() const;
        float DistanceTo(const Canis::Entity& _other) const;
        void FaceTarget(const Canis::Entity& _target);
        void MoveTowards(const Canis::Entity& _target, float _speed, float _dt);
        void ChangeState(const std::string& _stateName);
        const std::string& GetCurrentStateName() const override;
        float GetStateTime() const;
        float GetAttackRange() const;
        int GetCurrentHealth() const;

        void ResetHammerPose();
        void SetHammerSwing(float _normalized);
        void TakeDamage(int _damage);
        bool IsAlive() const;

    private:
        void PlayHitSfx();
        void SpawnDeathEffect();

        int m_currentHealth = 0;
        float m_stateTime = 0.0f;
        Canis::Vector4 m_baseColor = Canis::Vector4(1.0f);
        bool m_hasBaseColor = false;
        bool m_useFirstHitSfx = true;
    };

    void RegisterHealerStateMachineScript(Canis::App& _app);
    void UnRegisterHealerStateMachineScript(Canis::App& _app);
}
