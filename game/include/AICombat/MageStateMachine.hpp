#pragma once

#include <Canis/Entity.hpp>

#include <SuperPupUtilities/StateMachine.hpp>
#include <AICombat/ICombatant.hpp>

#include <string>

namespace AICombat
{
    class MageStateMachine;

    class MageIdleState : public SuperPupUtilities::State
    {
    public:
        static constexpr const char *Name = "MageIdleState";

        explicit MageIdleState(SuperPupUtilities::StateMachine &_stateMachine);
        void Enter() override;
        void Update(float _dt) override;
    };

    class MageChaseState : public SuperPupUtilities::State
    {
    public:
        static constexpr const char *Name = "MageChaseState";
        float moveSpeed = 4.0f;

        explicit MageChaseState(SuperPupUtilities::StateMachine &_stateMachine);
        void Enter() override;
        void Update(float _dt) override;
    };

    class MageZapTimeState : public SuperPupUtilities::State
    {
    public:
        static constexpr const char *Name = "MageZapTimeState";
        float staffRestHeight = 0.0f;
        float staffSwingHeight = 10.0f;
        float attackRange = 10.0f;
        float attackDuration = 0.75f;
        float attackDamageTime = 0.25f;
        Canis::SceneAssetHandle laserPrefab = {};
        std::string poolCode = "laser_bullet";
        float fireInterval = 1.75f;
        float turnSpeedDegrees = 120.0f;
        float fireAngleThresholdDegrees = 8.0f;
        Canis::Vector3 muzzleOffset = Canis::Vector3(0.0f, 0.15f, 1.0f);
        float projectileSpeed = 14.0f;
        float projectileLifeTime = 4.0f;
        float projectileHitImpulse = 6.0f;
         float m_fireCooldown = 0.0f;


        explicit MageZapTimeState(SuperPupUtilities::StateMachine &_stateMachine);
        void Enter() override;
        void Update(float _dt) override;
        void Exit() override;

    private:
        Canis::Vector3 GetMuzzlePosition(const Canis::Transform& _transform) const;
        float RotateTowards(Canis::Transform& _transform, const Canis::Vector3& _direction, float _dt) const;
        void Fire(const Canis::Vector3& _position, const Canis::Vector3& _direction);
    };

    class MageStateMachine : public SuperPupUtilities::StateMachine, public ICombatant
    {
    public:
        static constexpr const char *ScriptName = "AICombat::MageStateMachine";

        std::string targetTag = "";
        float detectionRange = 20.0f;
        Canis::Vector3 bodyColliderSize = Canis::Vector3(1.0f);
        int maxHealth = 40;
        bool logStateChanges = true;
        Canis::Entity *staffVisual = nullptr;
        Canis::AudioAssetHandle hitSfxPath1 = {.path = "assets/audio/sfx/hit_1.ogg"};
        Canis::AudioAssetHandle hitSfxPath2 = {.path = "assets/audio/sfx/hit_2.ogg"};
        float hitSfxVolume = 1.0f;
        Canis::SceneAssetHandle deathEffectPrefab = {.path = "assets/prefabs/brawler_death_particles.scene"};

        explicit MageStateMachine(Canis::Entity &_entity);

        MageIdleState idleState;
        MageChaseState chaseState;
        MageZapTimeState zapTimeState;

        void Create() override;
        void Ready() override;
        void Destroy() override;
        void Update(float _dt) override;

        Canis::Entity *FindClosestTarget() const;
        float DistanceTo(const Canis::Entity &_other) const;
        void FaceTarget(const Canis::Entity &_target);
        void MoveTowards(const Canis::Entity &_target, float _speed, float _dt);
        void ChangeState(const std::string &_stateName);
        const std::string &GetCurrentStateName() const override;
        float GetStateTime() const;
        float GetAttackRange() const;
        int GetCurrentHealth() const;

        void ResetStaffPose();
        void SetStaffSwing(float _normalized);
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

    void RegisterMageStateMachineScript(Canis::App &_app);
    void UnRegisterMageStateMachineScript(Canis::App &_app);
}
