#include "ModManager.hpp"
#include <filesystem>
#include <algorithm>
#include <Windows.h>

bool ModLdr::Manager::loadMod(std::wstring _fn) {
    if (!std::filesystem::exists(_fn))
        return false;
    
    // check if mod is already loaded
    if (std::find(mods.begin(), mods.end(), _fn) != mods.end())
        return true;
    
    HMODULE h = LoadLibraryW(_fn.c_str());

    if (h != nullptr) {
        mods.push_back(_fn);

        return true;
    }

    return false;
}

bool ModLdr::Manager::unloadMod(std::wstring _name) {
    auto p = std::find(mods.begin(), mods.end(), _name);
    if (p == mods.end())
        return true;
    
    // code for removing mod goes here

    return false;
}

void ModLdr::Manager::loadMods() {
    if (std::filesystem::exists(modFolder) &&
        std::filesystem::is_directory(modFolder))

            for (std::filesystem::directory_entry file : std::filesystem::directory_iterator("mods\\")) {
                std::wstring p = file.path().wstring();
                if (p.ends_with(L".dll"))
                    Manager::loadMod(p);
            }

    else
        std::filesystem::create_directory(modFolder);
}
