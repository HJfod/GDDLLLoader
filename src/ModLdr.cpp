#include "offsets.hpp"
#include "ModLdr.hpp"
#include <MinHook.h>
#include "layer/SettingsLayer.hpp"
#include "layer/LoadingLayer.hpp"
#include "layer/HelpLayer.hpp"
#include "layer/ModLayer.hpp"
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <locale>
#include <codecvt>

#ifdef MODLDR_CONSOLE
#include "console.hpp"
#endif

bool ModLdr::init() {
    MH_STATUS sinit = MH_Initialize();
    if (sinit != MH_STATUS::MH_OK)
        return false;

    #ifdef MODLDR_CONSOLE
    console::load();
    #endif

    SettingsLayer::loadHook();
    LoadingLayer::loadHook();
    HelpLayer::loadHook();

    MH_STATUS s = MH_EnableHook(MH_ALL_HOOKS);

    if (s != MH_STATUS::MH_OK)
        return false;

    return true;
}

void ModLdr::unload(HMODULE module) {
    MH_DisableHook(MH_ALL_HOOKS);

    Manager::cleanup();

    #ifdef MODLDR_CONSOLE
    console::unload();
    #endif

    MH_Uninitialize();

    FreeLibraryAndExitThread(module, 0);
    exit(0);
}

#ifdef MODLDR_CONSOLE
void ModLdr::awaitUnload() {
    std::string inp;
    getline(std::cin, inp);

    if (inp.starts_with("."))
        ModLayer::test = std::stoi(inp.substr(1));

    if (inp != "e")
        ModLdr::awaitUnload();
}
#endif

