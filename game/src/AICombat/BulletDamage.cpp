#include <AICombat/BulletDamage.hpp>

#include <Canis/App.hpp>
#include <Canis/Debug.hpp>
#include <Canis/ConfigHelper.hpp>

#include <algorithm>

namespace AICombat
{
    namespace
    {
        ScriptConf bulletDamageConf = {};
    }

    void RegisterBulletDamageScript(Canis::App& _app)
    {
        //REGISTER_PROPERTY(bulletDamageConf, AICombat::BulletDamage, owner);
        //REGISTER_PROPERTY(bulletDamageConf, AICombat::BulletDamage, sensorSize);
        //REGISTER_PROPERTY(bulletDamageConf, AICombat::BulletDamage, damage);
        //REGISTER_PROPERTY(bulletDamageConf, AICombat::BulletDamage, targetTag);

        //DEFAULT_CONFIG_AND_REQUIRED(
        //    bulletDamageConf,
        //    AICombat::BulletDamage,
        //    Canis::Transform,
        //    Canis::Rigidbody,
        //    Canis::BoxCollider);
//
        //bulletDamageConf.DEFAULT_DRAW_INSPECTOR(AICombat::BulletDamage);
//
        //_app.RegisterScript(bulletDamageConf);
    }

    DEFAULT_UNREGISTER_SCRIPT(bulletDamageConf, BulletDamage)

    void BulletDamage::Create()
    {
        //entity.GetComponent<Canis::Transform>();
//
        //Canis::Rigidbody& rigidbody = entity.GetComponent<Canis::Rigidbody>();
        //rigidbody.motionType = Canis::RigidbodyMotionType::STATIC;
        //rigidbody.useGravity = false;
        //rigidbody.isSensor = true;
        //rigidbody.allowSleeping = false;
        //rigidbody.linearVelocity = Canis::Vector3(0.0f);
        //rigidbody.angularVelocity = Canis::Vector3(0.0f);
//
        //entity.GetComponent<Canis::BoxCollider>().size = sensorSize;
    }

    void BulletDamage::Ready()
    {

       // if (targetTag.empty())
       // {
       //     if (owner)
       //         targetTag = owner->targetTag;
       // }
    }

    void BulletDamage::Update(float)
    {
        //CheckSensorEnter();
    }

    void BulletDamage::CheckSensorEnter()
    {
        //if (!entity.HasComponents<Canis::BoxCollider, Canis::Rigidbody>())
        //    return;
//
        //if (owner == nullptr || !owner->IsAlive())
        //{
        //    m_hitTargetsThisSwing.clear();
        //    return;
        //}
//
//
//
        //for (Canis::Entity* other : entity.GetComponent<Canis::BoxCollider>().entered)
        //{
        //    if (other == nullptr || !other->active)
        //        continue;
//
        //    AICombat::ICombatant* targetStateMachine = other->GetScript<AICombat::ICombatant>();
        //    if (targetStateMachine == nullptr || !targetStateMachine->IsAlive())
        //        continue;
//
        //    if (other->tag != targetTag)
        //        continue;
//
        //    Debug::Log("HYUP");
        //    targetStateMachine->TakeDamage(damage);
        //    m_hitTargetsThisSwing.push_back(other);
        //}
    }

}
