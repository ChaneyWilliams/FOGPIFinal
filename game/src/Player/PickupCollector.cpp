#include <Player/PickupCollector.hpp>

#include <SuperPupUtilities/I_Item.hpp>
#include <SuperPupUtilities/Inventory.hpp>

#include <Canis/App.hpp>
#include <Canis/ConfigHelper.hpp>

ScriptConf pickupCollectorConf = {};

void RegisterPickupCollectorScript(App& _app)
{
    REGISTER_PROPERTY(pickupCollectorConf, PickupCollector, pickupCollider);

    DEFAULT_CONFIG_AND_REQUIRED(pickupCollectorConf, PickupCollector, SuperPupUtilities::Inventory);

    pickupCollectorConf.DEFAULT_DRAW_INSPECTOR(PickupCollector);

    _app.RegisterScript(pickupCollectorConf);
}

DEFAULT_UNREGISTER_SCRIPT(pickupCollectorConf, PickupCollector)

void PickupCollector::Create() {}

void PickupCollector::Ready() {}

void PickupCollector::Destroy() {}

void PickupCollector::Update(float _dt)
{
    (void)_dt;

    if (pickupCollider == nullptr)
        return;

    SuperPupUtilities::Inventory* inventory = entity.GetScript<SuperPupUtilities::Inventory>();
    if (inventory == nullptr)
        return;

    const std::vector<Entity*>* contacts = nullptr;
    if (pickupCollider->HasComponent<SphereCollider>())
        contacts = &pickupCollider->GetComponent<SphereCollider>().entered;
    else if (pickupCollider->HasComponent<BoxCollider>())
        contacts = &pickupCollider->GetComponent<BoxCollider>().entered;

    if (contacts == nullptr)
        return;

    for (Entity* other : *contacts)
    {
        if (other == nullptr || !other->active)
            continue;

        if (SuperPupUtilities::I_Item* item = other->GetScript<SuperPupUtilities::I_Item>())
        {
            inventory->Add(*item, 1);
            other->Destroy();
        }
    }
}
