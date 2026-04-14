#pragma once

#include <Canis/Entity.hpp>

namespace Canis
{
    class App;
    struct UIActionContext;
}

namespace RollABall
{
    class StartMenu : public Canis::ScriptableEntity
    {
    public:
        static constexpr const char* ScriptName = "RollABall::StartMenu";

        Canis::SceneAssetHandle gameScene;
        Canis::SceneAssetHandle ecsSampleScene;

        explicit StartMenu(Canis::Entity& _entity) : Canis::ScriptableEntity(_entity) {}

        void Create() override;
        void Ready() override;
        void Destroy() override;
        void Update(float _dt) override;

        void Play(const Canis::UIActionContext& _context);
        void PlayECSExample(const Canis::UIActionContext& _context);
        void Quit(const Canis::UIActionContext& _context);
    };

    void RegisterStartMenuScript(Canis::App& _app);
    void UnRegisterStartMenuScript(Canis::App& _app);
}
