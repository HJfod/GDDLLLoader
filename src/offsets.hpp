#pragma once

#ifndef INCLUDE_COCOS
    #define INCLUDE_COCOS
    #pragma warning( push, 0 )
    #include <cocos2d.h>
    #pragma warning( pop )
#endif

#define PAD(size) char pad[size] = {};

#define offset_type inline const uintptr_t

namespace ModLdr {
    offset_type base = (uintptr_t)GetModuleHandleA(0);

    namespace offsets {
        offset_type MenuLayerInit           = base + 0x1907b0;
        offset_type SettingsLayerInit       = base + 0x1DD420;
        offset_type HelpLayerInit           = base + 0x25C7B0;

        offset_type ButtonSpriteCreate      = base + 0x137D0;
        offset_type ButtonCreate            = base + 0x18EE0;

        offset_type FLAlertLayerCreate      = base + 0x22680;
        offset_type FLAlertLayerCreate2     = base + 0x227E0;
        offset_type FLAlertLayerShow        = base + 0x23560;

        offset_type DropDownLayerInit       = base + 0x113530;
        offset_type DropDownLayerShow       = base + 0x1139C0;
        offset_type DropDownLayerHide       = base + 0x113A90;
    }
}