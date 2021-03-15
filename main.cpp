 #include "src/ModLdr.hpp"
#include <Windows.h>

void  __declspec(dllexport) zstub(){}

DWORD WINAPI my_thread(void* hModule) {
    if (!ModLdr::init())
        ModLdr::unload(reinterpret_cast<HMODULE>(hModule));

    #ifdef MODLDR_CONSOLE
    ModLdr::awaitUnload();
    
    ModLdr::unload(reinterpret_cast<HMODULE>(hModule));
    #endif
    
    return 0;
}

DWORD WINAPI unload_thread(void* hModule) {
    ModLdr::unload(reinterpret_cast<HMODULE>(hModule));

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(0, 0x1000, my_thread, hModule, 0, 0);
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH: break;
        case DLL_PROCESS_DETACH:
            #ifndef MODLDR_CONSOLE
                CreateThread(0, 0x1000, unload_thread, hModule, 0, 0);
            #endif
            break;
    }
    
    return true;
}

