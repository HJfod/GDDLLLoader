#include "ButtonSprite.hpp"

ButtonSprite* ButtonSprite::create(cocos2d::CCSprite* spr, cocos2d::CCMenu* targ, cocos2d::SEL_MenuHandler click) {
    auto pRet = reinterpret_cast<ButtonSprite*(__thiscall*)(cocos2d::CCSprite*, cocos2d::CCMenu*, cocos2d::SEL_MenuHandler)>
        ((uintptr_t)GetModuleHandleA(0) + 0x18EE0)(spr, targ, click);

    __asm add esp, 0x8
    return pRet;
}