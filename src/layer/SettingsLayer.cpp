#include "SettingsLayer.hpp"
#include "ModLayer.hpp"

static cocos2d::CCNode* getc(cocos2d::CCNode* const& _o, int _ix) {
    return static_cast<cocos2d::CCNode*>(_o->getChildren()->objectAtIndex(_ix));
}

void __fastcall ModLdr::SettingsLayer::initHook(cocos2d::CCNode* _layer) {
    init_(_layer);

    /*
    0 : child layer
        0 : background
        1 : back button
        2 : left chain
        3 : right chain
        4 : BUTTONS
            0-6 : these all follow the order you'd expect
        5 : music slider text
        6 : music slider
        7 : sfx slider text
        8 : sfx slider
        9 : vault icon
    */
    
    cocos2d::CCArray* menus = ((cocos2d::CCNode*)_layer->getChildren()->objectAtIndex(0))->getChildren();

    cocos2d::CCMenu* bmenu = (cocos2d::CCMenu*)menus->objectAtIndex(4);

    cocos2d::CCArray* buttons = bmenu->getChildren();

    // let us compress these mfs

    float condense = 6;
    float liftsfx = 4;
    float dis;

    float lpos, rpos, lw, rw, bh, smp, ssp;

    { // top row
        float y = ((cocos2d::CCNode*)buttons->objectAtIndex(0))->getPositionY();

        y += condense;

        ((cocos2d::CCNode*)buttons->objectAtIndex(0))->setPositionY(y);
        ((cocos2d::CCNode*)buttons->objectAtIndex(1))->setPositionY(y);

        dis = y;
    }

    { // middle row
        float y = ((cocos2d::CCNode*)buttons->objectAtIndex(2))->getPositionY();

        y += condense * 2;

        ((cocos2d::CCNode*)buttons->objectAtIndex(2))->setPositionY(y);
        ((cocos2d::CCNode*)buttons->objectAtIndex(3))->setPositionY(y);

        dis -= y;
    }

    { // bottom row
        cocos2d::CCNode* rateB = (cocos2d::CCNode*)buttons->objectAtIndex(4);
        cocos2d::CCNode* songsB = (cocos2d::CCNode*)buttons->objectAtIndex(5);
        cocos2d::CCNode* helpB = (cocos2d::CCNode*)buttons->objectAtIndex(6);

        float y = rateB->getPositionY();

        float gap = helpB->getPositionX() - songsB->getPositionX();

        lpos = songsB->getPositionX();  // rateB->getPositionX();
        lw = songsB->getContentSize().width;  // rateB->getContentSize().width;
        bh = rateB->getContentSize().height;
        rpos = helpB->getPositionX() - gap / 2;

        gap -= songsB->getContentSize().width / 2 + helpB->getContentSize().width / 2;

        rw = helpB->getContentSize().width + songsB->getContentSize().width + gap;

        y += condense * 3;

        rateB->setPositionY(y);
        songsB->setPositionY(y);
        helpB->setPositionY(y);

        dis = y - dis;
    }

    { // music slider
        cocos2d::CCNode* sldr = (cocos2d::CCNode*)menus->objectAtIndex(6);
        float x = sldr->getPositionX();

        smp = sldr->getPositionY();

        sldr->setPositionX(x + 40);
    }

    { // music slider text
        cocos2d::CCNode* txt = (cocos2d::CCNode*)menus->objectAtIndex(5);
        float x = txt->getPositionX();

        txt->setPosition(x - 110, smp);
    }

    { // sfx slider
        cocos2d::CCNode* sldr = (cocos2d::CCNode*)menus->objectAtIndex(8);
        float x = sldr->getPositionX();

        ssp = sldr->getPositionY();

        sldr->setPosition(x + 40, ssp + liftsfx);
    }

    { // sfx slider text
        cocos2d::CCNode* txt = (cocos2d::CCNode*)menus->objectAtIndex(7);
        float x = txt->getPositionX();

        txt->setPosition(x - 110, ssp + liftsfx);
    }

    ButtonSprite* mods = ButtonSprite::create(
        ButtonSpriteSpr::create("Mods", (int)lw, false, "goldFont.fnt", "GJ_button_01.png", bh, 1.0),
        bmenu,
        (cocos2d::SEL_MenuHandler)&SettingsLayer::showModList
    );

    mods->setPosition(lpos, dis);
    mods->setUserData(_layer);

    bmenu->addChild(mods);

    return;
}

void ModLdr::SettingsLayer::showModList(cocos2d::CCObject* pSender) {
    auto l = ModLayer::create();
    auto p = static_cast<cocos2d::CCNode*>(pSender);

    cocos2d::CCDirector::sharedDirector()->getRunningScene()->addChild(l);

    auto slayer = static_cast<GJDropDownLayer*>(p->getUserData());
    
    slayer->hideLayer(false);

    // hardcoded delay let's go
    // i found that there's some callback function
    // at slayer + 0x214 called when the hide animation
    // finishes, but couldn't figure out how i could add
    // shit to it

    l->runAction(
        cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(.5),
            cocos2d::CCCallFunc::create(l, static_cast<cocos2d::SEL_CallFunc>(
                &GJDropDownLayer::showLayerB
            )),
            nullptr
        )
    );  //*/
}

