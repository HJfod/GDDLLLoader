#include "ModLdr.hpp"

#pragma warning( push, 0 )
#include <cocos2d.h>
#pragma warning( pop )

#include <MinHook.h>
#include "layer/SettingsLayer.hpp"
#include "console.hpp"

bool ModLdr::init() {
    MH_STATUS sinit = MH_Initialize();
    if (sinit != MH_STATUS::MH_OK)
        return false;

    console::load();

    SettingsLayer::loadHook();

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

