#include "src/ModLdr.hpp"
#include <Windows.h>

DWORD WINAPI my_thread(void* hModule) {
    if (!ModLdr::init())
        ModLdr::unload(reinterpret_cast<HMODULE>(hModule));

    ModLdr::awaitUnload();
    
    ModLdr::unload(reinterpret_cast<HMODULE>(hModule));
    
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(0, 0x1000, my_thread, hModule, 0, 0);
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    
    return true;
}

