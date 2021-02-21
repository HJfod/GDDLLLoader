#include "DropDownLayer.hpp"

cocos2d::CCLayerColor* __fastcall DropDownLayer::create(const char* title) {
    auto pRet = reinterpret_cast<cocos2d::CCLayerColor*(__fastcall*)(const char*)>
        ((uintptr_t)GetModuleHandleA(0) + 0x113530)(title);

    __asm add esp, 0x8
    return pRet;
}

void* OptionsLayer::show() {
    return reinterpret_cast<void*(*)()>((uintptr_t)GetModuleHandleA(0) + 0x1dd310)();
    // 0x191C30 optionslayer
}

// 0x438470 create
// 0x113530 show

// 0x1dd310 show settingslayer
