#pragma once

#include <cocos2d.h>
#include <MinHook.h>
#include "../offsets.hpp"
#include "../gd/ButtonSprite.hpp"

namespace ModLdr {
    namespace SettingsLayer {
        int test;
        inline void (__fastcall* init)(cocos2d::CCNode*);
        void __fastcall initHook(cocos2d::CCNode*);

        inline void loadHook() {
            MH_CreateHook((PVOID)(offsets::SettingsLayerInit), (LPVOID)SettingsLayer::initHook, (LPVOID*)&SettingsLayer::init);
        }
    }
}

