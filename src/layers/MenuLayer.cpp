#include "MenuLayer.hpp"
#include "../ModManager.hpp"
#include "NotificationLayer.hpp"

void MenuLayer::onBellClick(cocos2d::CCObject* pSender) {
    auto nLayer = NotificationLayer::create();
    
    this->addChild(nLayer, 500);

    as<cocos2d::CCNode*>(pSender)->getParent()->removeChildByTag(50);

    ModLdr::Manager::getShared()->markNotificationsAsRead();

    nLayer->showLayer(false);
}

bool __fastcall MenuLayer::initHook(MenuLayer* _layer) {
    if (!init(_layer))
        return false;

    auto menu = getChild<cocos2d::CCMenu*>(_layer, 5);
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto bellspr = cocos2d::CCSprite::create("DL_bell.png");
    if (!bellspr)
        // if bell texture not available
        bellspr = cocos2d::CCSprite::create("GJ_infoBtn_001.png");
    bellspr->setScale(.75f);

    auto bell = gd::CCMenuItemSpriteExtra::create(
        bellspr,
        _layer,
        (cocos2d::SEL_MenuHandler)&MenuLayer::onBellClick
    );

    auto close = getChild<gd::CCMenuItemSpriteExtra*>(menu, 1);
    bell->setPosition(
        close->getPositionX(),
        close->getPositionY() - 35
    );

    menu->addChild(bell);

    auto notifs = ModLdr::Manager::getShared()->getNotifications();
    auto notifc = ModLdr::Manager::getShared()->getUnreadNotificationCount();

    if (notifc > 0) {
        auto errlabel = cocos2d::CCLabelBMFont::create(
            std::to_string(notifc).c_str(), "bigFont.fnt"
        );
        errlabel->setScale(.6f);
        errlabel->setTag(50);

        auto notifspr = cocos2d::CCSprite::create("DL_notif.png");
        if (notifspr) {
            notifspr->setScale(.7f);
            notifspr->setTag(50);

            notifspr->setPosition({
                bell->getPositionX() + 12.5f,
                bell->getPositionY() - 12.5f
            });

            menu->addChild(notifspr);

            errlabel->setPosition(
                notifspr->getScaledContentSize() / 2 + errlabel->getScaledContentSize() / 4
            );
            notifspr->addChild(errlabel);
        } else {
            errlabel->setPosition({
                bell->getPositionX() + 12.5f,
                bell->getPositionY() - 12.5f
            });

            menu->addChild(errlabel);
        }
    }

    return true;
}

