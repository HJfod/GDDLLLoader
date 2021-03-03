#pragma once

#include "../offsets.hpp"
#include "../gd/ButtonSprite.hpp"
#include "../gd/FLAlertLayer.hpp"
#include "../gd/GJDropDownLayer.hpp"

#include <MinHook.h>

namespace ModLdr {
    class LoadingLayer : public cocos2d::CCLayer {
        public:
            static inline void (__thiscall* init_)(cocos2d::CCLayer*, char);
            static void __fastcall initHook(cocos2d::CCLayer*, void*, char);

            static inline void loadHook() {
                MH_CreateHook(
                    (PVOID)(offsets::LoadingLayerInit),
                    (LPVOID)LoadingLayer::initHook,
                    (LPVOID*)&LoadingLayer::init_
                );
            }
    };
}

