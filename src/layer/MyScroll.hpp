#pragma once

#include "../offsets.hpp"

class HJScroll : public cocos2d::CCMenu {
    public:
        virtual void ccTouchBegan();
        virtual void ccTouchEnded();
        virtual void ccTouchMoved();

        static HJScroll* create();
};

