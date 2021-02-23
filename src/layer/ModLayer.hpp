#pragma once

#pragma warning( push, 0 )
#include <cocos2d.h>
#pragma warning( pop )

#include "../gd/GJDropDownLayer.hpp"

namespace ModLdr {
    class ModLayer : public GJDropDownLayer {
        public:
            void customSetup() override;
            void setup();

            void showInfo(cocos2d::CCObject*);

            static ModLayer* create();
    };
}