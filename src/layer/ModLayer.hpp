#pragma once

#include "../gd/GJDropDownLayer.hpp"

namespace ModLdr {
    class ModLayer : public GJDropDownLayer {
        public:
            virtual void customSetup();
            virtual void layerHidden();

            void showInfo(cocos2d::CCObject*);
            void showCredits(cocos2d::CCObject*);
            void addMod(cocos2d::CCObject*);
            void showModFolder(cocos2d::CCObject*);
            void reloadMods(cocos2d::CCObject*);
            void toggleMod(cocos2d::CCObject*);
            void showLoadedDLLs(cocos2d::CCObject*);
            void openModListPage(cocos2d::CCObject*);

            inline static int test = 0;

            bool showDLLList = false;

            static ModLayer* create();
    };
}