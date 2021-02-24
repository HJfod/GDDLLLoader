#pragma once

#include "../offsets.hpp"

#pragma runtime_checks("s", off)
class CustomListView : public cocos2d::CCLayer {
    public:
        static CustomListView* create(cocos2d::CCArray* _arr, float _w, float _h, int _n) {
            
            // size of the listview is passed in xmm1 & xmm2
            
            __asm {
                movss xmm1, [_h]
                movss xmm2, [_w]
            }

            auto ret = reinterpret_cast<CustomListView*(__thiscall*)(cocos2d::CCArray*, intptr_t)>(
                ModLdr::base + 0x57f90
            )(_arr, _n);

            // fix stack
            __asm add esp, 0x4

            return ret;
        }
};
#pragma runtime_checks("s", restore)

