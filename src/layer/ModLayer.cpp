#include "ModLayer.hpp"
#include "../gd/ButtonSprite.hpp"
#include "../gd/FLAlertLayer.hpp"
#include "../gd/CustomListView.hpp"

void ModLdr::ModLayer::setup() {
    auto l = cocos2d::CCLabelBMFont::create("No mods installed!", "bigFont.fnt");

    CustomListView* lr = this->m_pListLayer;

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto lrSize  = lr->getScaledContentSize();

    l->setPosition(lrSize.width / 2, lrSize.height / 2);
    l->setScale(.75);

    this->m_pListLayer->addChild(l);

    auto b = ButtonSprite::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this->m_pButtonMenu,
        (cocos2d::SEL_MenuHandler)&ModLdr::ModLayer::showInfo
    );

    b->setPosition(16, -winSize.height + 48);

    this->m_pButtonMenu->addChild(b);
}

void ModLdr::ModLayer::showInfo(cocos2d::CCObject* pSender) {
    auto f = FLAlertLayer::create(
        nullptr, "Info", "OK", nullptr, 350.0, 0, 0,
        "<cy>Mods</c> are <cc>DLLs</c> that are loaded into GD on startup.\n\n"  \
        "You can install mods by dragging them into the <co>mods/</c> folder,"   \
        "or by pressing the <cr>'+'</c> button."
    );

    f->show();
}

ModLdr::ModLayer* ModLdr::ModLayer::create() {
    ModLayer* m = static_cast<ModLayer*>(GJDropDownLayer::create("Mods"));

    m->setup();

    return m;
}

