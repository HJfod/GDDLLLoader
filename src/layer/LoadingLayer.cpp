#include "LoadingLayer.hpp"
#include "../ModManager.hpp"
#include <sstream>

void __fastcall ModLdr::LoadingLayer::initHook(cocos2d::CCLayer* _layer, void*, char _bool) {
    init_(_layer, _bool);

    std::stringstream infoText;
    
    auto load = ModLdr::Manager::loadMods();

    infoText << "Running ModLdr " << ModLdr::version
        << " - "
        << std::get<0>(load) << "/"
        << std::get<1>(load) << " loaded"
        << " (" << std::get<2>(load) << " disabled)";

    auto label = cocos2d::CCLabelBMFont::create(
        infoText.str().c_str(),
        "goldFont.fnt"
    );

    label->setScale(.4);

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    label->setPosition(
        winSize.width / 2,
        20
    );

    _layer->addChild(label);

    return;
}


