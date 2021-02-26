#pragma once

#include "../offsets.hpp"

#pragma runtime_checks("s", off)

class CustomListView : public cocos2d::CCLayer {
    public:
        virtual void setupList() {
            reinterpret_cast<void(__fastcall*)(
                cocos2d::CCNode*
            )>(
                ModLdr::base + 0x58870
            )(this);
        };
        virtual cocos2d::CCNode* getListCell(std::string str) {
            return reinterpret_cast<cocos2d::CCNode*(__thiscall*)(
                cocos2d::CCNode*, std::string
            )>(
                ModLdr::base + 0x58050
            )(this, str);
        };
        virtual void loadCell(cocos2d::CCNode* cell, int ix) {
            reinterpret_cast<void(__thiscall*)(
                cocos2d::CCNode*, cocos2d::CCNode*, int
            )>(
                ModLdr::base + 0x585C0
            )(this, cell, ix);
        };

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

        bool init(cocos2d::CCArray* _arr, float _w, float _h, CustomListView* _idk, int _n) {

            // width and height are stored in xmm registers
            __asm {
                movss xmm2, [_h]
                movss xmm3, [_w]
            }

            std::cout << "dbg0\n";

            auto ret = reinterpret_cast<bool(__thiscall*)(
                cocos2d::CCNode*, cocos2d::CCArray*, CustomListView*, int
            )>(
                ModLdr::base + 0x10C20
            )(
                this, _arr, _idk, _n
            );

            // clean stack
            __asm add esp, 0xA

            std::cout << "dbg1\n";

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

class InheritedView : public CustomListView {
    public:
        inline virtual void loadCell(cocos2d::CCNode*, int ix) override {
            std::cout << ix << "\n";
        }

        static InheritedView* create(cocos2d::CCArray* _a, float _w, float _h, int _bt) {
            InheritedView* pRet = new InheritedView();
            // InheritedView is inherited from CustomListView

            if (pRet && pRet->init(_a, _w, _h, nullptr, _bt)) {
                std::cout << "yoot\n";
                
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
};
#pragma runtime_checks("s", restore)

