#include <RollABall/StartMenu.hpp>

#include <Canis/App.hpp>
#include <Canis/ConfigHelper.hpp>

namespace RollABall
{
    namespace
    {
        Canis::ScriptConf scriptConf = {};
    }

    void RegisterStartMenuScript(Canis::App& _app)
    {
        REGISTER_PROPERTY(scriptConf, RollABall::StartMenu, gameScene);
        REGISTER_PROPERTY(scriptConf, RollABall::StartMenu, ecsSampleScene);

        DEFAULT_CONFIG(scriptConf, RollABall::StartMenu);

        RegisterUIAction<RollABall::StartMenu>(scriptConf, "Play", &RollABall::StartMenu::Play);
        RegisterUIAction<RollABall::StartMenu>(scriptConf, "PlayECSExample", &RollABall::StartMenu::PlayECSExample);
        RegisterUIAction<RollABall::StartMenu>(scriptConf, "Quit", &RollABall::StartMenu::Quit);

        scriptConf.DEFAULT_DRAW_INSPECTOR(RollABall::StartMenu);

        _app.RegisterScript(scriptConf);
    }

    DEFAULT_UNREGISTER_SCRIPT(scriptConf, StartMenu)

    void StartMenu::Create() {}

    void StartMenu::Ready() {}

    void StartMenu::Destroy() {}

    void StartMenu::Update(float) {}

    void StartMenu::Play(const Canis::UIActionContext&)
    {
        entity.scene.app->LoadScene(gameScene);
    }

    void StartMenu::PlayECSExample(const Canis::UIActionContext&)
    {
        entity.scene.app->LoadScene(ecsSampleScene);
    }

    void StartMenu::Quit(const Canis::UIActionContext&)
    {
        entity.scene.QuitGame();
    }
}
