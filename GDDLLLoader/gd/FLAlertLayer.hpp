#pragma once

#include <cocos2d.h>

class FLAlertLayerProtocol {
    void virtual FLAlert_Clicked(void *,bool);
};

namespace FLAlertLayer {
    void* __fastcall create(FLAlertLayerProtocol*, char const*, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, char const*, char const*);
};