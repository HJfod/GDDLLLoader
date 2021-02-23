#pragma once

#include "../offsets.hpp"

#pragma warning( push, 0 )
#include <cocos2d.h>
#pragma warning( pop )

#define PAD(size) char pad[size] = {};

class CustomListView : public cocos2d::CCLayer {
    public:
        static CustomListView* create(cocos2d::CCArray* _arr, float _f, float _2, int _n, intptr_t _type) {
            return nullptr;
        }
};

