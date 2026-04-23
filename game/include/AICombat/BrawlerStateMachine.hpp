#pragma once

#include <Canis/Entity.hpp>

#include <SuperPupUtilities/StateMachine.hpp>
#include <AICombat/ICombatant.hpp>

#include <string>

namespace AICombat
{
    class BrawlerStateMachine;

    class BrawlerIdleState : public SuperPupUtilities::State
    {
    public:
        static constexpr const char* Name = "BrawlerIdleState";

        explicit BrawlerIdleState(SuperPupUtilities::StateMachine& _stateMachine);
        void Enter() override;
        void Update(float _dt) override;
    };

    class BrawlerChaseState : public SuperPupUtilities::State
    {
    public:
        static constexpr const char* Name = "BrawlerChaseState";
        float moveSpeed = 4.0f;

        explicit BrawlerChaseState(SuperPupUtilities::StateMachine& _stateMachine);
        void Enter() override;
        void Update(float _dt) override;
    };

    class BrawlerHammerTimeState : public SuperPupUtilities::State
    {
    public:
        static constexpr const char* Name = "BrawlerHammerTimeState";
        float hammerRestDegrees = 140.0f;
        float hammerSwingDegrees = -120.0f;
        float attackRange = 2.25f;
        float attackDuration = 0.75f;
        float attackDamageTime = 0.25f;

        explicit BrawlerHammerTimeState(SuperPupUtilities::StateMachine& _stateMachine);
        void Enter() override;
        void Update(float _dt) override;
        void Exit() override;
    };

    class BrawlerStateMachine : public SuperPupUtilities::StateMachine, public ICombatant
    {
    public:
        static constexpr const char* ScriptName = "AICombat::BrawlerStateMachine";

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

        explicit BrawlerStateMachine(Canis::Entity& _entity);

        BrawlerIdleState idleState;
        BrawlerChaseState chaseState;
        BrawlerHammerTimeState hammerTimeState;

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

    void RegisterBrawlerStateMachineScript(Canis::App& _app);
    void UnRegisterBrawlerStateMachineScript(Canis::App& _app);
}
