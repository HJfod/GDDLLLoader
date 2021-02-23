#include "offsets.hpp"
#include "ModLdr.hpp"
#include <MinHook.h>
#include "layer/SettingsLayer.hpp"
#include "layer/HelpLayer.hpp"
#include "layer/ModLayer.hpp"
#include "console.hpp"
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <locale>
#include <codecvt>

bool ModLdr::init() {
    MH_STATUS sinit = MH_Initialize();
    if (sinit != MH_STATUS::MH_OK)
        return false;

    console::load();

    SettingsLayer::loadHook();
    HelpLayer::loadHook();

    MH_STATUS s = MH_EnableHook(MH_ALL_HOOKS);

    if (s != MH_STATUS::MH_OK)
        return false;
    
    ModLdr::Manager::loadMods();

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

    if (inp.starts_with("."))
        ModLayer::test = std::stoi(inp.substr(1));

    if (inp != "e")
        ModLdr::awaitUnload();
}

