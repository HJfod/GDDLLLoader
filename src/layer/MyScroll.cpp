#include "MyScroll.hpp"

void HJScroll::ccTouchBegan() {
    std::cout << "y";
}

void HJScroll::ccTouchMoved() {
    std::cout << "a";
}

void HJScroll::ccTouchEnded() {
    std::cout << "y!\n";
}

HJScroll* HJScroll::create() {
    std::cout << "yeet!" << std::endl;

    auto p = new HJScroll();

    if (p && p->init()) {
        p->autorelease();

        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

