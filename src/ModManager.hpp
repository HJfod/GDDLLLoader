#pragma once

#include <string>
#include <vector>
#include <Windows.h>

inline std::string utf8_encode(const std::wstring &wstr) {
    if( wstr.empty() ) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

inline std::wstring utf8_decode(const std::string &str) {
    if( str.empty() ) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

namespace ModLdr {
    namespace Manager {
        struct Mod {
            enum Type {
                DLL,
                ModLdrDLL
            };

            std::string name;
            std::wstring wname;

            std::string path;
            std::wstring wpath;

            bool enabled;
            bool loaded;
            Type type;

            HMODULE handle;

            Mod(
                std::wstring _wpath,
                Type _type,
                HMODULE _handle = nullptr,
                bool _enabled = true
            ) {
                this->wpath = _wpath;
                this->path = utf8_encode(_wpath);

                std::wstring wn;
                if (_wpath.find(L"\\") != std::wstring::npos)
                    wn = this->wpath.substr(this->wpath.find_last_of(L"\\") + 1);
                else wn = this->wpath;

                this->wname = wn.substr(0, wn.find_first_of(L"."));
                this->name = utf8_encode(this->wname);

                this->enabled = _enabled;
                this->type = _type;

                this->loaded = _enabled;
                this->handle = _handle;
            }
        };

        inline std::vector<Mod*> mods;
        constexpr static const char* modFolder = "mods";

        std::tuple<int, int, int> loadMods();
        bool loadMod(std::wstring);
        bool unloadMod(Mod*);
        bool enableMod(Mod*, bool);
        void cleanup();

        Mod* getMod(std::wstring);
        Mod* getMod(std::string);
    }
}

