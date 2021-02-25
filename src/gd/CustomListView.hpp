#pragma once

#include "../offsets.hpp"

#pragma runtime_checks("s", off)

class CustomListView : public cocos2d::CCLayer {
    public:
        // TableViewDelegate vtable i believe
        virtual void something0();
        virtual void something1();
        virtual void something2();
        virtual void something3();
        virtual void something4();
        virtual void something5();
        virtual void something6();

        // BoomListView vtable i believe
        virtual void getCountIG(int);
        virtual bool another1();
        virtual void another2();
        virtual cocos2d::CCObject* getChild(int);

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

        CustomListView() {
            reinterpret_cast<void(__thiscall*)(CustomListView*)>(
				ModLdr::base + 0x57e60
			)(this);

        }

        ~CustomListView() {
            reinterpret_cast<void(__thiscall*)(CustomListView*, bool)>(
				ModLdr::base + 0x57ec0
			)(this, true);
        }
};
#pragma runtime_checks("s", restore)

