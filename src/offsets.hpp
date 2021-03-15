#pragma once

#ifndef INCLUDE_SUBMODULES
    #define INCLUDE_SUBMODULES
    #pragma warning( push, 0 )
    #include <cocos2d.h>
    #include <gd.h>
    #include <MinHook.h>
    #include <nfd.h>
    #pragma warning( pop )
#endif

#define MODLDR_CONSOLE

typedef const uintptr_t offset_type;

static constexpr const cocos2d::ccColor3B listBGLight { 194, 114, 62 };
static constexpr const cocos2d::ccColor3B listBGDark  { 161, 88,  44 };

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

static constexpr unsigned int h$(const char* str, int h = 0) {
    return !str[h] ? 5381 : (h$(str, h+1) * 33) ^ str[h];
}

template<typename T>
static T getChild(cocos2d::CCNode* x, int i) {
    return static_cast<T>(x->getChildren()->objectAtIndex(i));
}

template <typename ret>
ret ReadChain(std::vector<uintptr_t>_offs, ret _if_fucky = ret()) {
    uintptr_t* read = reinterpret_cast<std::uintptr_t*>(_offs.at(0));
    bool not_valid = false;

    for (unsigned int i = 1; i < _offs.size() - 1; i++)
        if (read == nullptr) {
            not_valid = true; break;
        } else
            read = reinterpret_cast<std::uintptr_t*>(*read + _offs.at(i));
    
    if (!not_valid)
        return *(ret*)(*read + _offs[_offs.size() - 1]);
    else return _if_fucky;
}

template <typename T, typename R>
T as(R const v) { return reinterpret_cast<T>(v); }

template <typename T, typename R>
constexpr static MH_STATUS makeHook(uintptr_t _addr, T _func, R _tramp) {
    return MH_CreateHook(
        reinterpret_cast<PVOID>(_addr),
        reinterpret_cast<LPVOID>(_func),
        reinterpret_cast<LPVOID*>(_tramp)
    );
}

namespace ModLdr {
    static constexpr const char* version = "v1.3 DEV";
    static constexpr const int plistVersion = 1;

    namespace offsets {
        offset_type MenuLayerInit           = gd::base + 0x1907b0;
        offset_type SettingsLayerInit       = gd::base + 0x1DD420;
        offset_type HelpLayerInit           = gd::base + 0x25C7B0;
        offset_type LoadingLayerInit        = gd::base + 0x18c080;

        offset_type ButtonSpriteCreate      = gd::base + 0x137D0;
        offset_type ButtonCreate            = gd::base + 0x18EE0;

        offset_type FLAlertLayerCreate      = gd::base + 0x22680;
        offset_type FLAlertLayerCreate2     = gd::base + 0x227E0;
        offset_type FLAlertLayerShow        = gd::base + 0x23560;

        offset_type DropDownLayerInit       = gd::base + 0x113530;
        offset_type DropDownLayerShow       = gd::base + 0x1139C0;
        offset_type DropDownLayerHide       = gd::base + 0x113A90;
    }
}