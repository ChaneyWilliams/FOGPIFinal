#pragma once

#include <Canis/System.hpp>

namespace ECSExample
{
    class SamplePlayerSystem : public Canis::System
    {
    public:
        void Create() override;
        void Update(entt::registry& _registry, float _deltaTime) override;
    };

    class SpinSystem : public Canis::System
    {
    public:
        void Create() override;
        void Update(entt::registry& _registry, float _deltaTime) override;
    };

    class HoverSystem : public Canis::System
    {
    public:
        void Create() override;
        void Update(entt::registry& _registry, float _deltaTime) override;
    };

    class CollectibleSystem : public Canis::System
    {
    public:
        void Create() override;
        void Update(entt::registry& _registry, float _deltaTime) override;
    };

    class SampleHUDSystem : public Canis::System
    {
    public:
        void Create() override;
        void Update(entt::registry& _registry, float _deltaTime) override;
    };

    void RegisterSamplePlayerSystem(Canis::App& _app);
    void UnRegisterSamplePlayerSystem(Canis::App& _app);

    void RegisterSpinSystem(Canis::App& _app);
    void UnRegisterSpinSystem(Canis::App& _app);

    void RegisterHoverSystem(Canis::App& _app);
    void UnRegisterHoverSystem(Canis::App& _app);

    void RegisterCollectibleSystem(Canis::App& _app);
    void UnRegisterCollectibleSystem(Canis::App& _app);

    void RegisterSampleHUDSystem(Canis::App& _app);
    void UnRegisterSampleHUDSystem(Canis::App& _app);
}
