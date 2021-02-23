#pragma once

#pragma warning( push, 0 )
#include <cocos2d.h>
#pragma warning( pop )

#include "../gd/GJDropDownLayer.hpp"

namespace ModLdr {
    class ModLayer : public GJDropDownLayer {
        public:
            virtual void customSetup();

            void showInfo(cocos2d::CCObject*);
            void showCredits(cocos2d::CCObject*);
            void addMod(cocos2d::CCObject*);
            void showModFolder(cocos2d::CCObject*);

            static ModLayer* create();
    };
}