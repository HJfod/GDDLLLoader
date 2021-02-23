#pragma once

#include <cocos2d.h>
#include <MinHook.h>
#include "ModManager.hpp"

namespace ModLdr {
    bool init();
    void unload(HMODULE);
    void awaitUnload();
}