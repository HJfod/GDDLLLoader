#pragma once

#include <cocos2d.h>
#include <MinHook.h>
#include "../offsets.hpp"
#include "../gd/ButtonSprite.hpp"
#include "../gd/FLAlertLayer.hpp"

namespace ModLdr {
    class SettingsLayer {
        public:
            static inline int test;
            static inline void (__fastcall* init)(cocos2d::CCNode*);
            static void __fastcall initHook(cocos2d::CCNode*);
            
            // this whole thing is a class full of static
            // methods just so i can add the button callback
            // here lol

            void showModList(cocos2d::CCObject*);

            static inline void loadHook() {
                MH_CreateHook((PVOID)(offsets::SettingsLayerInit), (LPVOID)SettingsLayer::initHook, (LPVOID*)&SettingsLayer::init);
            }
    };
}

