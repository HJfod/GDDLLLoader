#pragma once

#include "../offsets.hpp"

#include <MinHook.h>

namespace HelpLayer {
    inline void (__fastcall* init)(cocos2d::CCNode*);
    void __fastcall initHook(cocos2d::CCNode*);
    inline static constexpr const char* credits =
        "ModLdr by Adaf & HJfod. Special thanks to PoweredByPie";

    inline void loadHook() {
        MH_CreateHook((PVOID)(ModLdr::offsets::HelpLayerInit), (LPVOID)initHook, (LPVOID*)&init);
    }
}