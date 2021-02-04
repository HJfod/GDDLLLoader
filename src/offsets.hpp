#pragma once

#include <Windows.h>
#include <type_traits>

namespace gd {
    namespace functions {
        void (__fastcall * GetObjectString)(void*, void*, void*) = nullptr;
        void (__fastcall * DeselectObjects)(uintptr_t) = nullptr;
        intptr_t (__fastcall * ShowPopup)(void*, const char*, std::string, const char*, const char*);
    }
}

namespace offsets {
    uintptr_t base;
    uintptr_t gamemanager;

    uintptr_t MenuLayerInit;
    uintptr_t EditLevelLayerInit;
    uintptr_t LocalLevelList;
    uintptr_t EditorLayerInit;
    uintptr_t PopupShow;
    uintptr_t HelpMenuInit;

    uintptr_t GetObjectStringAddr;
    uintptr_t DeselectAddr;

    void load() {
        base = (uintptr_t)GetModuleHandleA(0);
        gamemanager = base + 0x3222D0;

        MenuLayerInit = base + 0x1907b0;
        EditLevelLayerInit = base + 0x6F5D0;
        LocalLevelList = base + 0x15A040;
        PopupShow = base + 0x227E0;
        HelpMenuInit = base + 0x25CB70;
        EditorLayerInit = base + 0x76310;

        gd::functions::ShowPopup = (intptr_t (__fastcall *)(void*, const char*, std::string, const char*, const char*))PopupShow;

        GetObjectStringAddr = base + 0xED0C0;
        DeselectAddr = base + 0x86AF0;

        gd::functions::GetObjectString = (void (__fastcall *)(void*, void*, void*))GetObjectStringAddr;
        gd::functions::DeselectObjects = (void (__fastcall *)(uintptr_t))DeselectAddr;

        // base + 0x10C3A       appears to be the CCLayer::init of an item in a GD select list
        // base + 0x159FA0      appears to be the create function for a GD select list
        // base + 0x15A85D      at this point, if EAX == 0x62 then we're loading local levels

        // base + 0x19600       this is the 90% certainly thet function for creating checboxes'
        // base + 0x422730
        // base + 0x1CCBF0      level create function (the 'new' button)

        // base + 0x838A0       ButtonSprite::init apparently
    };
};