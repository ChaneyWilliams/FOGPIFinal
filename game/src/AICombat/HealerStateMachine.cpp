#include <AICombat/HealerStateMachine.hpp>
#include <AICombat/BrawlerStateMachine.hpp>
#include <AICombat/MageStateMachine.hpp>
#include <Canis/App.hpp>
#include <Canis/AudioManager.hpp>
#include <Canis/ConfigHelper.hpp>
#include <Canis/Debug.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace AICombat
{
    namespace
    {
        ScriptConf healerStateMachineConf = {};
    }

    HealerIdleState::HealerIdleState(SuperPupUtilities::StateMachine &_stateMachine) : State(Name, _stateMachine) {}

    void HealerIdleState::Enter()
    {
        if (HealerStateMachine *brawlerStatMachine = dynamic_cast<HealerStateMachine *>(m_stateMachine))
            brawlerStatMachine->ResetHammerPose();
    }

    void HealerIdleState::Update(float)
    {
        if (HealerStateMachine *brawlerStatMachine = dynamic_cast<HealerStateMachine *>(m_stateMachine))
        {
            if (brawlerStatMachine->FindClosestTarget() != nullptr)
                brawlerStatMachine->ChangeState(HealerChaseState::Name);
        }
    }

    HealerChaseState::HealerChaseState(SuperPupUtilities::StateMachine &_stateMachine) : State(Name, _stateMachine) {}

    void HealerChaseState::Enter()
    {
        if (HealerStateMachine *brawlerStatMachine = dynamic_cast<HealerStateMachine *>(m_stateMachine))
            brawlerStatMachine->ResetHammerPose();
    }

    void HealerChaseState::Update(float _dt)
    {
        HealerStateMachine *brawlerStatMachine = dynamic_cast<HealerStateMachine *>(m_stateMachine);
        if (brawlerStatMachine == nullptr)
            return;

        Canis::Entity *target = brawlerStatMachine->FindClosestTarget();

        if (target == nullptr)
        {
            brawlerStatMachine->ChangeState(HealerIdleState::Name);
            return;
        }

        brawlerStatMachine->FaceTarget(*target);

        if (brawlerStatMachine->DistanceTo(*target) <= brawlerStatMachine->GetAttackRange())
        {
            brawlerStatMachine->ChangeState(HealerHammerTimeState::Name);
            return;
        }

        brawlerStatMachine->MoveTowards(*target, moveSpeed, _dt);
    }

    HealerHammerTimeState::HealerHammerTimeState(SuperPupUtilities::StateMachine &_stateMachine) : State(Name, _stateMachine) {}

    void HealerHammerTimeState::Enter()
    {
        if (HealerStateMachine *brawlerStatMachine = dynamic_cast<HealerStateMachine *>(m_stateMachine))
            brawlerStatMachine->SetHammerSwing(0.0f);
    }

    void HealerHammerTimeState::Update(float)
    {
        HealerStateMachine *brawlerStatMachine = dynamic_cast<HealerStateMachine *>(m_stateMachine);
        if (brawlerStatMachine == nullptr)
            return;

        if (Canis::Entity *target = brawlerStatMachine->FindClosestTarget())
            brawlerStatMachine->FaceTarget(*target);

        const float duration = std::max(attackDuration, 0.001f);
        brawlerStatMachine->SetHammerSwing(brawlerStatMachine->GetStateTime() / duration);

        if (brawlerStatMachine->GetStateTime() < duration)
            return;

        if (brawlerStatMachine->FindClosestTarget() != nullptr)
            brawlerStatMachine->ChangeState(HealerChaseState::Name);
        else
            brawlerStatMachine->ChangeState(HealerIdleState::Name);
    }

    void HealerHammerTimeState::Exit()
    {
        if (HealerStateMachine *brawlerStatMachine = dynamic_cast<HealerStateMachine *>(m_stateMachine))
            brawlerStatMachine->ResetHammerPose();
    }

    HealerStateMachine::HealerStateMachine(Canis::Entity &_entity) : SuperPupUtilities::StateMachine(_entity),
                                                                     idleState(*this),
                                                                     chaseState(*this),
                                                                     hammerTimeState(*this) {}

    void RegisterHealerStateMachineScript(Canis::App &_app)
    {
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, targetTag);
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, detectionRange);
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, bodyColliderSize);
        RegisterAccessorProperty(healerStateMachineConf, AICombat::HealerStateMachine, chaseState, moveSpeed);
        RegisterAccessorProperty(healerStateMachineConf, AICombat::HealerStateMachine, hammerTimeState, hammerRestDegrees);
        RegisterAccessorProperty(healerStateMachineConf, AICombat::HealerStateMachine, hammerTimeState, hammerSwingDegrees);
        RegisterAccessorProperty(healerStateMachineConf, AICombat::HealerStateMachine, hammerTimeState, attackRange);
        RegisterAccessorProperty(healerStateMachineConf, AICombat::HealerStateMachine, hammerTimeState, attackDuration);
        RegisterAccessorProperty(healerStateMachineConf, AICombat::HealerStateMachine, hammerTimeState, attackDamageTime);
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, maxHealth);
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, logStateChanges);
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, hammerVisual);
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, hitSfxPath1);
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, hitSfxPath2);
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, hitSfxVolume);
        REGISTER_PROPERTY(healerStateMachineConf, AICombat::HealerStateMachine, deathEffectPrefab);

        DEFAULT_CONFIG_AND_REQUIRED(
            healerStateMachineConf,
            AICombat::HealerStateMachine,
            Canis::Transform,
            Canis::Material,
            Canis::Model,
            Canis::Rigidbody,
            Canis::BoxCollider);

        healerStateMachineConf.DEFAULT_DRAW_INSPECTOR(AICombat::HealerStateMachine);

        _app.RegisterScript(healerStateMachineConf);
    }

    DEFAULT_UNREGISTER_SCRIPT(healerStateMachineConf, HealerStateMachine)

    void HealerStateMachine::Create()
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

    void HealerStateMachine::Ready()
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
        AddState(hammerTimeState);

        ResetHammerPose();
        ChangeState(HealerIdleState::Name);
    }

    void HealerStateMachine::Destroy()
    {
        hammerVisual = nullptr;
        SuperPupUtilities::StateMachine::Destroy();
    }

    void HealerStateMachine::Update(float _dt)
    {
        if (!IsAlive())
            return;

        m_stateTime += _dt;
        SuperPupUtilities::StateMachine::Update(_dt);
    }

    Canis::Entity *HealerStateMachine::FindClosestTarget() const
    {
        if (targetTag.empty() || !entity.HasComponent<Canis::Transform>())
            return nullptr;

        const Canis::Transform &transform = entity.GetComponent<Canis::Transform>();
        const Canis::Vector3 origin = transform.GetGlobalPosition();

        Canis::Entity *bestTarget = nullptr;
        float closestDistance = detectionRange;
        int lowestHealth = INT_MAX;

        for (Canis::Entity *candidate : entity.scene.GetEntitiesWithTag(targetTag))
        {
            if (candidate == nullptr || candidate == &entity || !candidate->active)
                continue;

            if (!candidate->HasComponent<Canis::Transform>())
                continue;

            int health = -1;
            int maxHealth = -1;
            bool isAlive = false;

            if (HealerStateMachine *h = candidate->GetScript<HealerStateMachine>())
            {
                health = h->GetCurrentHealth();
                maxHealth = h->maxHealth;
                isAlive = h->IsAlive();
            }
            else if (BrawlerStateMachine *b = candidate->GetScript<BrawlerStateMachine>())
            {
                health = b->GetCurrentHealth();
                maxHealth = b->maxHealth;
                isAlive = b->IsAlive();
            }
            else if (MageStateMachine *m = candidate->GetScript<MageStateMachine>())
            {
                health = m->GetCurrentHealth();
                maxHealth = m->maxHealth;
                isAlive = m->IsAlive();
            }
            else
            {
                continue;
            }

            if (!isAlive || health >= maxHealth)
                continue;

            const Canis::Vector3 candidatePosition =
                candidate->GetComponent<Canis::Transform>().GetGlobalPosition();

            float distance = glm::distance(origin, candidatePosition);

            if (distance > detectionRange)
                continue;

            if (health < lowestHealth ||
                (health == lowestHealth && distance < closestDistance))
            {
                lowestHealth = health;
                closestDistance = distance;
                bestTarget = candidate;
            }
        }

        return bestTarget;
    }

    float HealerStateMachine::DistanceTo(const Canis::Entity &_other) const
    {
        if (!entity.HasComponent<Canis::Transform>() || !_other.HasComponent<Canis::Transform>())
            return std::numeric_limits<float>::max();

        const Canis::Vector3 selfPosition = entity.GetComponent<Canis::Transform>().GetGlobalPosition();
        const Canis::Vector3 targetPosition = _other.GetComponent<Canis::Transform>().GetGlobalPosition();
        return glm::distance(selfPosition, targetPosition);
    }

    void HealerStateMachine::FaceTarget(const Canis::Entity &_target)
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

    void HealerStateMachine::MoveTowards(const Canis::Entity &_target, float _speed, float _dt)
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

    void HealerStateMachine::ChangeState(const std::string &_stateName)
    {
        if (SuperPupUtilities::StateMachine::GetCurrentStateName() == _stateName)
            return;

        if (!SuperPupUtilities::StateMachine::ChangeState(_stateName))
            return;

        m_stateTime = 0.0f;

        if (logStateChanges)
            Canis::Debug::Log("%s -> %s", entity.name.c_str(), _stateName.c_str());
    }

    const std::string &HealerStateMachine::GetCurrentStateName() const
    {
        return SuperPupUtilities::StateMachine::GetCurrentStateName();
    }

    float HealerStateMachine::GetStateTime() const
    {
        return m_stateTime;
    }

    float HealerStateMachine::GetAttackRange() const
    {
        return hammerTimeState.attackRange;
    }

    int HealerStateMachine::GetCurrentHealth() const
    {
        return m_currentHealth;
    }

    void HealerStateMachine::ResetHammerPose()
    {
        SetHammerSwing(0.0f);
    }

    void HealerStateMachine::SetHammerSwing(float _normalized)
    {
        if (hammerVisual == nullptr || !hammerVisual->HasComponent<Canis::Transform>())
            return;

        Canis::Transform &hammerTransform = hammerVisual->GetComponent<Canis::Transform>();
        const float normalized = Clamp01(_normalized);


        const float angle = normalized * 2.0f * PI;


        const float swing = sinf(angle);

        hammerTransform.rotation.x = DEG2RAD *
                                     (hammerTimeState.hammerRestDegrees +
                                      hammerTimeState.hammerSwingDegrees * swing);
    }

    void HealerStateMachine::TakeDamage(int _damage)
    {

        if (!IsAlive())
            return;

        const int damageToApply = _damage;

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

    void HealerStateMachine::PlayHitSfx()
    {
        const Canis::AudioAssetHandle &selectedSfx = m_useFirstHitSfx ? hitSfxPath1 : hitSfxPath2;
        m_useFirstHitSfx = !m_useFirstHitSfx;

        if (selectedSfx.Empty())
            return;

        Canis::AudioManager::PlaySFX(selectedSfx, std::clamp(hitSfxVolume, 0.0f, 1.0f));
    }

    void HealerStateMachine::SpawnDeathEffect()
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

    bool HealerStateMachine::IsAlive() const
    {
        return m_currentHealth > 0;
    }
}
