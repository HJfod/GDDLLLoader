#include "FLAlertLayer.hpp"

void* __fastcall FLAlertLayer::create(
    FLAlertLayerProtocol* d0,
    char const* d1,
    std::basic_string<char, std::char_traits<char>, std::allocator<char> > d2,
    char const* d3,
    char const* d4
) {
    return reinterpret_cast<void*(__fastcall*)(FLAlertLayerProtocol*, char const*, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, char const*, char const*)>
        ((uintptr_t)GetModuleHandleA(0) + 0x227E0)(d0, d1, d2, d3, d4);
}

// 0x23560 show