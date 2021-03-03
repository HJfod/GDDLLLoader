#pragma once

#include "../gd/GJDropDownLayer.hpp"

namespace ModLdr {
    class DLLLayer : public GJDropDownLayer {
        public:
            virtual void customSetup();

            void showInfo(cocos2d::CCObject*);

            static DLLLayer* create();
    };
}