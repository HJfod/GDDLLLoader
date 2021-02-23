#pragma once

#include "../offsets.hpp"

#pragma warning( push, 0 )
#include <cocos2d.h>
#pragma warning( pop )

#define PAD(size) char pad[size] = {};

class CustomListView : public cocos2d::CCLayer {
    static CustomListView* create(cocos2d::CCLayer* layer) {
        return reinterpret_cast<CustomListView*(__fastcall*)(cocos2d::CCLayer*)>(
            ModLdr::base + 0x10A80
        )(layer);
    }
};