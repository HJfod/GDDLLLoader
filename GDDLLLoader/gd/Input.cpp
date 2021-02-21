#include "Input.hpp"

cocos2d::CCSprite* Input::bg_create(cocos2d::CCSize _r) {
    cocos2d::extension::CCScale9Sprite* s9s = cocos2d::extension::CCScale9Sprite::create(
        "square02_001.png", { 0, 0, 80, 80 }
    );

    s9s->setContentSize(_r);

    cocos2d::CCSprite* s = cocos2d::CCSprite::create();
    s->setContentSize(_r);

    s9s->setPosition(_r.width / 2, _r.height / 2);

    s->addChild(s9s);

    return s;
}

cocos2d::CCNode* __fastcall Input::create(const char* _text, void* _par, const char* _fnt) {
    return reinterpret_cast<cocos2d::CCNode* (__fastcall*)(const char*, void*, const char*)>
        ((uintptr_t)GetModuleHandleA(0) + 0x20D90)(_text, _par, _fnt);
}