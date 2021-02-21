#include <cocos2d.h>
#include <MinHook.h>

namespace ModLdr {
    bool init();
    void unload(HMODULE);
    void awaitUnload();
}