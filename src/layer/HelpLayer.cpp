#include "HelpLayer.hpp"

void __fastcall HelpLayer::initHook(cocos2d::CCNode* _self) {
    init(_self);

    cocos2d::CCSize win = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCNode* child = (cocos2d::CCNode*)_self->getChildren()->objectAtIndex(0);

    cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(credits, "goldFont.fnt");
    label->setPosition(win.width / 2, 10);
    label->setScale(.5);

    child->addChild(label);

    return;
}