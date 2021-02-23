#pragma once

#include "../offsets.hpp"

class CustomListView : public cocos2d::CCLayer {
    public:
        static CustomListView* create(cocos2d::CCArray* _arr, float _w, float _h, int _n, intptr_t _type) {
            
            // size of the listview is passed in xmm1 & xmm2
            __asm {
                movss xmm1, [_w]
                movss xmm2, [_h]
            }

            // the array is passed in ecx and then there's
            // some number passed in edx (assuming it's the
            // type or something)
            // + there's some number on the top of the stack

            auto ret = reinterpret_cast<CustomListView*(__thiscall*)(cocos2d::CCArray*, int)>(
                ModLdr::base + 0x57f90
            )(_arr, _n);

            return ret;
        }
};

