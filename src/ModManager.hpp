#pragma once

#include <string>
#include <vector>

namespace ModLdr {
    namespace Manager {
        inline std::vector<std::wstring> mods;
        constexpr static const char* modFolder = "mods";
        void loadMods();
        bool loadMod(std::wstring);
        bool unloadMod(std::wstring);
    }
}

