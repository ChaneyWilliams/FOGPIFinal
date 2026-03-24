#include <Environment/Block.hpp>

#include <Canis/App.hpp>
#include <Canis/ConfigHelper.hpp>


ScriptConf blockConf = {};

void RegisterBlockScript(App& _app)
{
    REGISTER_PROPERTY(blockConf, Block, blockType);
    
    DEFAULT_CONFIG_AND_REQUIRED(blockConf, Block, RectTransform);

    blockConf.DEFAULT_DRAW_INSPECTOR(Block);

    _app.RegisterScript(blockConf);
}

DEFAULT_UNREGISTER_SCRIPT(blockConf, Block)

void Block::Create() {}

void Block::Ready() {}

void Block::Destroy() {}

void Block::Update(float _dt) {}
