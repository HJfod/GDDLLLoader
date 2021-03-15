#pragma once

#include "../offsets.hpp"

class MenuLayer : public cocos2d::CCLayer {
    protected:
        static inline bool (__fastcall* init)(MenuLayer*);
        static bool __fastcall initHook(MenuLayer*);

        void onBellClick(cocos2d::CCObject*);

    public:
        static bool load() {
            return makeHook(
                gd::base + 0x1907b0,
                MenuLayer::initHook,
                &MenuLayer::init
            );
        }
};

