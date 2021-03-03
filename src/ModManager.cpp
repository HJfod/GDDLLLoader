#include "ModManager.hpp"
#include <filesystem>
#include <algorithm>
#include <iostream>

static constexpr const wchar_t* disabledExtension = L".dis";

HMODULE injectDLL(ModLdr::Manager::Mod* _mod) {
    auto h = LoadLibraryW(_mod->wpath.c_str());

    if (h != nullptr) {
        _mod->loaded = true;
        _mod->handle = h;
    }

    return h;
};

bool isLoaded(std::wstring _fn) {
    for (auto const& mod : ModLdr::Manager::mods)
        if (mod->wpath == _fn)
            return true;
    
    return false;
}

bool ModLdr::Manager::loadMod(std::wstring _fn) {
    if (!std::filesystem::exists(_fn))
        return false;
    
    // check if mod is already loaded
    if (isLoaded(_fn))
        return false;
    
    HMODULE h = LoadLibraryW(_fn.c_str());

    if (h != nullptr) {
        mods.push_back(new Mod (_fn, Mod::Type::DLL, h));

        return true;
    }

    return false;
}

bool ModLdr::Manager::unloadMod(ModLdr::Manager::Mod* _mod) {
    auto p = std::find(mods.begin(), mods.end(), _mod);
    if (p == mods.end())
        return true;
    
    // code for removing mod goes here

    return false;
}

std::tuple<int, int, int> ModLdr::Manager::loadMods() {
    int loaded = 0;
    int total = 0;
    int disabled = 0;

    if (std::filesystem::exists(modFolder) &&
        std::filesystem::is_directory(modFolder))

            for (std::filesystem::directory_entry file : std::filesystem::directory_iterator("mods\\")) {
                std::wstring p = file.path().wstring();
                if (p.ends_with(L".dll")) {

                    if (Manager::loadMod(p))
                        loaded++;

                    total++;
                    
                } else if (p.ends_with(disabledExtension))

                    if (!isLoaded(p)) {
                        mods.push_back(new Mod(
                            p,
                            Mod::Type::DLL,
                            nullptr,
                            false
                        ));

                        total++;
                        disabled++;
                    }
            }

    else
        std::filesystem::create_directory(modFolder);

    return std::tuple<int, int, int>(loaded, total, disabled);
}

bool ModLdr::Manager::enableMod(ModLdr::Manager::Mod* _mod, bool _toggled) {
    _mod->enabled = _toggled;

    if (_toggled) {
        if (_mod->wpath.ends_with(L".dll")) {
            if (!_mod->loaded) {
                auto h = injectDLL(_mod);

                if (h == nullptr)
                    return false;
            }

            return true;
        }
        
        // if the current file extension does not exist,
        // then we know it either has the disable extension
        // or the user renamed it themselves in which case
        // we might as well just cry

        if (std::filesystem::exists(_mod->wpath))
            std::filesystem::rename(
                _mod->wpath,
                _mod->wpath.substr(0, _mod->wpath.length() - wcslen(disabledExtension))
            );

        _mod->wpath = _mod->wpath.substr(0, _mod->wpath.length() - wcslen(disabledExtension));
        _mod->path = utf8_encode(_mod->wpath);
        
        if (!_mod->loaded) {
            auto h = injectDLL(_mod);

            if (h == nullptr)
                return false;
        }
    } else {
        if (_mod->wpath.ends_with(disabledExtension))
            return true;

        if (std::filesystem::exists(_mod->wpath))
            std::filesystem::rename(
                _mod->wpath,
                _mod->wpath + disabledExtension
            );

        _mod->wpath = _mod->wpath + disabledExtension;
        _mod->path = utf8_encode(_mod->wpath);
    }

    return true;
}

void ModLdr::Manager::cleanup() {
    while(!mods.empty()) delete mods.back(), mods.pop_back();
}

