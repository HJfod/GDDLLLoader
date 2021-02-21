#include "ModLdr.hpp"
#include <cocos2d.h>
#include <MinHook.h>
#include "layer/SettingsLayer.hpp"
#include "console.hpp"

void* (__fastcall * retFLAL)(void*, const char*, const char*, const char*, float, void*, float, void*) = nullptr;

void* __fastcall FLAL(void* _par, const char* _title, const char* _b1, const char* _b2, float _f, void* _u, float _f2, void* _cap) {
    std::cout << _title << " | " << _b1 << " | " << _b2 << " | " << _cap << "\n";

    return retFLAL(_par, _title, _b1, _b2, _f, _u, _f2, _cap);
}

bool ModLdr::init() {
    MH_STATUS sinit = MH_Initialize();
    if (sinit != MH_STATUS::MH_OK)
        return false;

    console::load();

    SettingsLayer::loadHook();

    //MH_CreateHook((PVOID)offsets::FLAlertLayerCreate2, (LPVOID)FLAL, (LPVOID*)&retFLAL);

    MH_STATUS s = MH_EnableHook(MH_ALL_HOOKS);

    if (s != MH_STATUS::MH_OK)
        return false;

    return true;
}

void ModLdr::unload(HMODULE module) {
    MH_DisableHook(MH_ALL_HOOKS);

    console::unload();

    MH_Uninitialize();

    FreeLibraryAndExitThread(module, 0);
    exit(0);
}

void ModLdr::awaitUnload() {
    std::string inp;
    getline(std::cin, inp);

    if (inp != "e")
        ModLdr::awaitUnload();
}

