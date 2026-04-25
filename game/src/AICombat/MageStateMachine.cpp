#include <AICombat/MageStateMachine.hpp>

#include <Canis/App.hpp>
#include <Canis/AudioManager.hpp>
#include <Canis/ConfigHelper.hpp>
#include <Canis/Debug.hpp>
#include <SuperPupUtilities/Bullet.hpp>
#include <SuperPupUtilities/SimpleObjectPool.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace AICombat
{
    namespace
    {
        ScriptConf mageStateMachineConf = {};
    }

    MageIdleState::MageIdleState(SuperPupUtilities::StateMachine &_stateMachine) : State(Name, _stateMachine) {}

    void MageIdleState::Enter()
    {
        if (MageStateMachine *mageStateMachine = dynamic_cast<MageStateMachine *>(m_stateMachine))
            mageStateMachine->ResetStaffPose();
    }

    void MageIdleState::Update(float)
    {
        if (MageStateMachine *mageStateMachine = dynamic_cast<MageStateMachine *>(m_stateMachine))
        {
            if (mageStateMachine->FindClosestTarget() != nullptr)
                mageStateMachine->ChangeState(MageChaseState::Name);
        }
    }

    MageChaseState::MageChaseState(SuperPupUtilities::StateMachine &_stateMachine) : State(Name, _stateMachine) {}

    void MageChaseState::Enter()
    {
        if (MageStateMachine *mageStateMachine = dynamic_cast<MageStateMachine *>(m_stateMachine))
            mageStateMachine->ResetStaffPose();
    }

    void MageChaseState::Update(float _dt)
    {
        MageStateMachine *mageStateMachine = dynamic_cast<MageStateMachine *>(m_stateMachine);
        if (mageStateMachine == nullptr)
            return;

        Canis::Entity *target = mageStateMachine->FindClosestTarget();

        if (target == nullptr)
        {
            mageStateMachine->ChangeState(MageIdleState::Name);
            return;
        }

        mageStateMachine->FaceTarget(*target);

        if (mageStateMachine->DistanceTo(*target) <= mageStateMachine->GetAttackRange())
        {
            mageStateMachine->ChangeState(MageZapTimeState::Name);
            return;
        }

        mageStateMachine->MoveTowards(*target, moveSpeed, _dt);
    }

    MageZapTimeState::MageZapTimeState(SuperPupUtilities::StateMachine &_stateMachine) : State(Name, _stateMachine) {}

    void MageZapTimeState::Enter()
    {
        if (MageStateMachine *mageStateMachine = dynamic_cast<MageStateMachine *>(m_stateMachine))
            mageStateMachine->SetStaffSwing(0.0f);
    }

    void MageZapTimeState::Update(float _dt)
    {
        MageStateMachine *mageStateMachine = dynamic_cast<MageStateMachine *>(m_stateMachine);

        if (mageStateMachine == nullptr)
            return;

        Canis::Entity *target = mageStateMachine->FindClosestTarget();

        if (target)
            mageStateMachine->FaceTarget(*target);

        
        Canis::Transform& transform = mageStateMachine->staffVisual->GetComponent<Canis::Transform>();
        const Canis::Vector3 targetPosition = target->GetComponent<Canis::Transform>().GetGlobalPosition();
        Canis::Vector3 toTarget = targetPosition - transform.GetGlobalPosition();
        toTarget.y = 0.0f;

        if(m_fireCooldown > 0.0f)
            return;

        const float duration = std::max(attackDuration, 0.001f);
        mageStateMachine->SetStaffSwing(mageStateMachine->GetStateTime() / duration);

         if (glm::length(toTarget) <= 0.001f)
            return;

        const float angleError = RotateTowards(transform, toTarget, _dt);
        if (m_fireCooldown > 0.0f)
            m_fireCooldown -= _dt;

        if (m_fireCooldown > 0.0f)
            return;

        const float fireAngleThreshold = fireAngleThresholdDegrees * Canis::DEG2RAD;
        if (std::abs(angleError) > fireAngleThreshold)
            return;

        Fire(GetMuzzlePosition(transform), toTarget);
        m_fireCooldown = fireInterval;

        if (mageStateMachine->GetStateTime() < duration)
            return;

        if (mageStateMachine->FindClosestTarget() != nullptr)
            mageStateMachine->ChangeState(MageChaseState::Name);
        else
            mageStateMachine->ChangeState(MageIdleState::Name);
    }

    float MageZapTimeState::RotateTowards(Canis::Transform& _transform, const Canis::Vector3& _direction, float _dt) const
    {
        const Canis::Vector3 flatDirection = glm::normalize(Canis::Vector3(_direction.x, 0.0f, _direction.z));
        const float targetYaw = std::atan2(-flatDirection.x, -flatDirection.z);
        const float yawError = std::remainder(targetYaw - _transform.rotation.y, TAU);
        const float maxStep = turnSpeedDegrees * Canis::DEG2RAD * _dt;
        const float appliedStep = std::clamp(yawError, -maxStep, maxStep);

        _transform.rotation.y += appliedStep;
        return std::remainder(targetYaw - _transform.rotation.y, TAU);
    }

    void MageZapTimeState::Exit()
    {
        if (MageStateMachine *mageStateMachine = dynamic_cast<MageStateMachine *>(m_stateMachine))
            mageStateMachine->ResetStaffPose();
    }
    void MageZapTimeState::Fire(const Canis::Vector3 &_position, const Canis::Vector3 &_direction)
    {
        const Canis::Vector3 flatDirection = glm::normalize(Canis::Vector3(_direction.x, 0.0f, _direction.z));
        const float yaw = std::atan2(-flatDirection.x, -flatDirection.z);
        const Canis::Vector3 rotation = Canis::Vector3(0.0f, yaw, 0.0f);

        auto *pool = SuperPupUtilities::SimpleObjectPool::Instance;

        if (pool == nullptr)
            return;

        Canis::Entity *projectile = pool->Spawn("laser_bullet", _position, rotation);

        if (projectile == nullptr)
            return;

        if (SuperPupUtilities::Bullet *bullet = projectile->GetScript<SuperPupUtilities::Bullet>())
        {
            bullet->speed = projectileSpeed * 10.0f;
            bullet->lifeTime = projectileLifeTime;
            bullet->hitImpulse = projectileHitImpulse;
            bullet->Launch();
        }
    }
        Canis::Vector3 MageZapTimeState::GetMuzzlePosition(const Canis::Transform& _transform) const
    {
        return _transform.GetGlobalPosition()
            + (_transform.GetRight() * muzzleOffset.x)
            + (_transform.GetUp() * muzzleOffset.y)
            + (_transform.GetForward() * muzzleOffset.z);
    }

    MageStateMachine::MageStateMachine(Canis::Entity &_entity) : SuperPupUtilities::StateMachine(_entity),
                                                                 idleState(*this),
                                                                 chaseState(*this),
                                                                 zapTimeState(*this) {}

    void RegisterMageStateMachineScript(Canis::App &_app)
    {
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, targetTag);
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, detectionRange);
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, bodyColliderSize);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, chaseState, moveSpeed);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, staffRestHeight);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, staffSwingHeight);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, attackRange);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, laserPrefab);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, poolCode);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, fireInterval);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, turnSpeedDegrees);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, fireAngleThresholdDegrees);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, muzzleOffset);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, projectileSpeed);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, projectileLifeTime);
        RegisterAccessorProperty(mageStateMachineConf, AICombat::MageStateMachine, zapTimeState, projectileHitImpulse);
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, maxHealth);
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, logStateChanges);
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, staffVisual);
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, hitSfxPath1);
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, hitSfxPath2);
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, hitSfxVolume);
        REGISTER_PROPERTY(mageStateMachineConf, AICombat::MageStateMachine, deathEffectPrefab);

        DEFAULT_CONFIG_AND_REQUIRED(
            mageStateMachineConf,
            AICombat::MageStateMachine,
            Canis::Transform,
            Canis::Material,
            Canis::Model,
            Canis::Rigidbody,
            Canis::BoxCollider);

        mageStateMachineConf.DEFAULT_DRAW_INSPECTOR(AICombat::MageStateMachine);

        _app.RegisterScript(mageStateMachineConf);
    }

    DEFAULT_UNREGISTER_SCRIPT(mageStateMachineConf, MageStateMachine)

    void MageStateMachine::Create()
    {
        entity.GetComponent<Canis::Transform>();

        Canis::Rigidbody &rigidbody = entity.GetComponent<Canis::Rigidbody>();
        rigidbody.motionType = Canis::RigidbodyMotionType::KINEMATIC;
        rigidbody.useGravity = false;
        rigidbody.allowSleeping = false;
        rigidbody.linearVelocity = Canis::Vector3(0.0f);
        rigidbody.angularVelocity = Canis::Vector3(0.0f);

        entity.GetComponent<Canis::BoxCollider>().size = bodyColliderSize;

        if (entity.HasComponent<Canis::Material>())
        {
            m_baseColor = entity.GetComponent<Canis::Material>().color;
            m_hasBaseColor = true;
        }
    }

    void MageStateMachine::Ready()
    {
        if (entity.HasComponent<Canis::Material>())
        {
            m_baseColor = entity.GetComponent<Canis::Material>().color;
            m_hasBaseColor = true;
        }

        m_currentHealth = std::max(maxHealth, 1);
        m_stateTime = 0.0f;
        m_useFirstHitSfx = true;

        ClearStates();
        AddState(idleState);
        AddState(chaseState);
        AddState(zapTimeState);

        ResetStaffPose();
        ChangeState(MageIdleState::Name);
    }

    void MageStateMachine::Destroy()
    {
        staffVisual = nullptr;
        SuperPupUtilities::StateMachine::Destroy();
    }

    void MageStateMachine::Update(float _dt)
    {
        if (!IsAlive())
            return;

        m_stateTime += _dt;
        SuperPupUtilities::StateMachine::Update(_dt);
    }

    Canis::Entity *MageStateMachine::FindClosestTarget() const
    {
        if (targetTag.empty() || !entity.HasComponent<Canis::Transform>())
            return nullptr;

        const Canis::Transform &transform = entity.GetComponent<Canis::Transform>();
        const Canis::Vector3 origin = transform.GetGlobalPosition();
        Canis::Entity *closestTarget = nullptr;
        float closestDistance = detectionRange;

        for (Canis::Entity *candidate : entity.scene.GetEntitiesWithTag(targetTag))
        {
            if (candidate == nullptr || candidate == &entity || !candidate->active)
                continue;

            if (!candidate->HasComponent<Canis::Transform>())
                continue;

            if (const MageStateMachine *other = candidate->GetScript<MageStateMachine>())
            {
                if (!other->IsAlive())
                    continue;
            }

            const Canis::Vector3 candidatePosition = candidate->GetComponent<Canis::Transform>().GetGlobalPosition();
            const float distance = glm::distance(origin, candidatePosition);

            if (distance > detectionRange || distance >= closestDistance)
                continue;

            closestDistance = distance;
            closestTarget = candidate;
        }

        return closestTarget;
    }

    float MageStateMachine::DistanceTo(const Canis::Entity &_other) const
    {
        if (!entity.HasComponent<Canis::Transform>() || !_other.HasComponent<Canis::Transform>())
            return std::numeric_limits<float>::max();

        const Canis::Vector3 selfPosition = entity.GetComponent<Canis::Transform>().GetGlobalPosition();
        const Canis::Vector3 targetPosition = _other.GetComponent<Canis::Transform>().GetGlobalPosition();
        return glm::distance(selfPosition, targetPosition);
    }

    void MageStateMachine::FaceTarget(const Canis::Entity &_target)
    {
        if (!entity.HasComponent<Canis::Transform>() || !_target.HasComponent<Canis::Transform>())
            return;

        Canis::Transform &transform = entity.GetComponent<Canis::Transform>();
        const Canis::Vector3 selfPosition = transform.GetGlobalPosition();
        Canis::Vector3 direction = _target.GetComponent<Canis::Transform>().GetGlobalPosition() - selfPosition;
        direction.y = 0.0f;

        if (glm::dot(direction, direction) <= 0.0001f)
            return;

        direction = glm::normalize(direction);
        transform.rotation.y = std::atan2(-direction.x, -direction.z);
    }

    void MageStateMachine::MoveTowards(const Canis::Entity &_target, float _speed, float _dt)
    {
        if (!entity.HasComponent<Canis::Transform>() || !_target.HasComponent<Canis::Transform>())
            return;

        Canis::Transform &transform = entity.GetComponent<Canis::Transform>();
        const Canis::Vector3 selfPosition = transform.GetGlobalPosition();
        Canis::Vector3 direction = _target.GetComponent<Canis::Transform>().GetGlobalPosition() - selfPosition;
        direction.y = 0.0f;

        if (glm::dot(direction, direction) <= 0.0001f)
            return;

        direction = glm::normalize(direction);
        transform.position += direction * _speed * _dt;
    }

    void MageStateMachine::ChangeState(const std::string &_stateName)
    {
        if (SuperPupUtilities::StateMachine::GetCurrentStateName() == _stateName)
            return;

        if (!SuperPupUtilities::StateMachine::ChangeState(_stateName))
            return;

        m_stateTime = 0.0f;

        if (logStateChanges)
            Canis::Debug::Log("%s -> %s", entity.name.c_str(), _stateName.c_str());
    }

    const std::string &MageStateMachine::GetCurrentStateName() const
    {
        return SuperPupUtilities::StateMachine::GetCurrentStateName();
    }

    float MageStateMachine::GetStateTime() const
    {
        return m_stateTime;
    }

    float MageStateMachine::GetAttackRange() const
    {
        return zapTimeState.attackRange;
    }

    int MageStateMachine::GetCurrentHealth() const
    {
        return m_currentHealth;
    }

    void MageStateMachine::ResetStaffPose()
    {
        SetStaffSwing(0.0f);
    }

    void MageStateMachine::SetStaffSwing(float _normalized)
    {
        if (staffVisual == nullptr || !staffVisual->HasComponent<Canis::Transform>())
            return;

        Canis::Transform &staffTransform = staffVisual->GetComponent<Canis::Transform>();
        const float normalized = Clamp01(_normalized);
        const float swingBlend = (normalized <= 0.5f)
                                     ? normalized * 2.0f
                                     : (1.0f - normalized) * 2.0f;

        staffTransform.position.y = DEG2RAD *
                                    (zapTimeState.staffRestHeight + (zapTimeState.staffSwingHeight * swingBlend));
    }

    void MageStateMachine::TakeDamage(int _damage)
    {
        if (!IsAlive())
            return;

        const int damageToApply = std::max(_damage, 0);
        if (damageToApply <= 0)
            return;

        m_currentHealth = std::max(0, m_currentHealth - damageToApply);
        PlayHitSfx();

        if (m_hasBaseColor && entity.HasComponent<Canis::Material>())
        {
            Canis::Material &material = entity.GetComponent<Canis::Material>();
            const float healthRatio = (maxHealth > 0)
                                          ? (static_cast<float>(m_currentHealth) / static_cast<float>(maxHealth))
                                          : 0.0f;

            material.color = Canis::Vector4(
                m_baseColor.x * (0.5f + (0.5f * healthRatio)),
                m_baseColor.y * (0.5f + (0.5f * healthRatio)),
                m_baseColor.z * (0.5f + (0.5f * healthRatio)),
                m_baseColor.w);
        }
        if (m_currentHealth > 0)
            return;

        if (logStateChanges)
            Canis::Debug::Log("%s was defeated.", entity.name.c_str());

        SpawnDeathEffect();
        entity.Destroy();
    }

    void MageStateMachine::PlayHitSfx()
    {
        const Canis::AudioAssetHandle &selectedSfx = m_useFirstHitSfx ? hitSfxPath1 : hitSfxPath2;
        m_useFirstHitSfx = !m_useFirstHitSfx;

        if (selectedSfx.Empty())
            return;

        Canis::AudioManager::PlaySFX(selectedSfx, std::clamp(hitSfxVolume, 0.0f, 1.0f));
    }

    void MageStateMachine::SpawnDeathEffect()
    {
        if (deathEffectPrefab.Empty() || !entity.HasComponent<Canis::Transform>())
            return;

        const Canis::Transform &sourceTransform = entity.GetComponent<Canis::Transform>();
        const Canis::Vector3 spawnPosition = sourceTransform.GetGlobalPosition();
        const Canis::Vector3 spawnRotation = sourceTransform.GetGlobalRotation();

        for (Canis::Entity *spawnedEntity : entity.scene.Instantiate(deathEffectPrefab))
        {
            if (spawnedEntity == nullptr || !spawnedEntity->HasComponent<Canis::Transform>())
                continue;

            Canis::Transform &spawnedTransform = spawnedEntity->GetComponent<Canis::Transform>();
            spawnedTransform.position = spawnPosition;
            spawnedTransform.rotation = spawnRotation;
        }
    }

    bool MageStateMachine::IsAlive() const
    {
        return m_currentHealth > 0;
    }
}
