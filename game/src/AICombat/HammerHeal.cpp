#include <AICombat/HammerHeal.hpp>

#include <Canis/App.hpp>
#include <Canis/ConfigHelper.hpp>

#include <algorithm>

namespace AICombat
{
    namespace
    {
        ScriptConf hammerHealConf = {};
    }

    void RegisterHammerHealScript(Canis::App& _app)
    {
        REGISTER_PROPERTY(hammerHealConf, AICombat::HammerHeal, owner);
        REGISTER_PROPERTY(hammerHealConf, AICombat::HammerHeal, sensorSize);
        REGISTER_PROPERTY(hammerHealConf, AICombat::HammerHeal, damage);
        REGISTER_PROPERTY(hammerHealConf, AICombat::HammerHeal, targetTag);

        DEFAULT_CONFIG_AND_REQUIRED(
            hammerHealConf,
            AICombat::HammerHeal,
            Canis::Transform,
            Canis::Rigidbody,
            Canis::BoxCollider);

        hammerHealConf.DEFAULT_DRAW_INSPECTOR(AICombat::HammerHeal);

        _app.RegisterScript(hammerHealConf);
    }

    DEFAULT_UNREGISTER_SCRIPT(hammerHealConf, HammerHeal)

    void HammerHeal::Create()
    {
        entity.GetComponent<Canis::Transform>();

        Canis::Rigidbody& rigidbody = entity.GetComponent<Canis::Rigidbody>();
        rigidbody.motionType = Canis::RigidbodyMotionType::STATIC;
        rigidbody.useGravity = false;
        rigidbody.isSensor = true;
        rigidbody.allowSleeping = false;
        rigidbody.linearVelocity = Canis::Vector3(0.0f);
        rigidbody.angularVelocity = Canis::Vector3(0.0f);

        entity.GetComponent<Canis::BoxCollider>().size = sensorSize;
    }

    void HammerHeal::Ready()
    {
        if (owner == nullptr)
            owner = FindOwnerFromHierarchy();

        if (targetTag.empty())
        {
            if (HealerStateMachine* ownerStateMachine = GetOwnerStateMachine())
                targetTag = ownerStateMachine->targetTag;
        }
    }

    void HammerHeal::Update(float)
    {
        CheckSensorEnter();
    }

    void HammerHeal::CheckSensorEnter()
    {
        if (!entity.HasComponents<Canis::BoxCollider, Canis::Rigidbody>())
            return;

        HealerStateMachine* ownerStateMachine = GetOwnerStateMachine();
        if (ownerStateMachine == nullptr || !ownerStateMachine->IsAlive())
        {
            m_hitTargetsThisSwing.clear();
            return;
        }

        const bool damageWindowOpen =
            ownerStateMachine->GetCurrentStateName() == HealerHammerTimeState::Name &&
            ownerStateMachine->GetStateTime() >= ownerStateMachine->hammerTimeState.attackDamageTime;

        if (!damageWindowOpen)
        {
            m_hitTargetsThisSwing.clear();
            return;
        }

        for (Canis::Entity* other : entity.GetComponent<Canis::BoxCollider>().entered)
        {
            if (other == nullptr || !other->active || other == owner || HasDamagedThisSwing(*other))
                continue;

            AICombat::ICombatant* targetStateMachine = other->GetScript<AICombat::ICombatant>();
            if (targetStateMachine == nullptr || !targetStateMachine->IsAlive())
                continue;

            if (other->tag != targetTag)
                continue;

            targetStateMachine->TakeDamage(-damage);
            m_hitTargetsThisSwing.push_back(other);
        }
    }

    HealerStateMachine* HammerHeal::GetOwnerStateMachine()
    {
        if (owner == nullptr)
            owner = FindOwnerFromHierarchy();

        if (owner == nullptr || !owner->active)
            return nullptr;

        return owner->GetScript<HealerStateMachine>();
    }

    Canis::Entity* HammerHeal::FindOwnerFromHierarchy() const
    {
        if (!entity.HasComponent<Canis::Transform>())
            return nullptr;

        Canis::Entity* current = entity.GetComponent<Canis::Transform>().parent;
        while (current != nullptr)
        {
            if (current->HasScript<AICombat::ICombatant>())
                return current;

            if (!current->HasComponent<Canis::Transform>())
                break;

            current = current->GetComponent<Canis::Transform>().parent;
        }

        return nullptr;
    }

    bool HammerHeal::HasDamagedThisSwing(Canis::Entity& _target) const
    {
        return std::find(m_hitTargetsThisSwing.begin(), m_hitTargetsThisSwing.end(), &_target)
            != m_hitTargetsThisSwing.end();
    }
}
