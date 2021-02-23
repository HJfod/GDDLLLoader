#include "ModLayer.hpp"
#include "../ModManager.hpp"
#include "../gd/ButtonSprite.hpp"
#include "../gd/FLAlertLayer.hpp"
#include "../gd/CustomListView.hpp"
#include <direct.h>

void ModLdr::ModLayer::showModFolder(cocos2d::CCObject*) {
    char buff[FILENAME_MAX];
    _getcwd(buff, FILENAME_MAX);
    std::string path(buff);

    path = path + "\\" + ModLdr::Manager::modFolder;
    ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

void ModLdr::ModLayer::addMod(cocos2d::CCObject* pSender) {
    
}

void ModLdr::ModLayer::customSetup() {
    GJListLayer* lr = this->m_pListLayer;

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto lrSize  = lr->getScaledContentSize();

    if (ModLdr::Manager::mods.size() == 0) {
        auto l = cocos2d::CCLabelBMFont::create("No mods installed!", "bigFont.fnt");

        l->setPosition(lrSize.width / 2, lrSize.height / 2);
        l->setScale(.75);
        
        this->m_pListLayer->addChild(l);
    } else {
        auto arr = cocos2d::CCArray::create();

        arr->addObject(cocos2d::CCLabelBMFont::create("test text", "bigFont.fnt"));
        arr->addObject(cocos2d::CCLabelBMFont::create("test 2", "bigFont.fnt"));
        arr->addObject(cocos2d::CCLabelBMFont::create("yaypogs", "bigFont.fnt"));

        //CustomListView* c = CustomListView::create(arr, 365.0, 150.0, 4, 0);
    }

    auto infoButton = ButtonSprite::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this->m_pButtonMenu,
        (cocos2d::SEL_MenuHandler)&ModLdr::ModLayer::showInfo
    );

    infoButton->setPosition(0, -winSize.height + 48);

    this->m_pButtonMenu->addChild(infoButton);


    auto creditsButton = ButtonSprite::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this->m_pButtonMenu,
        (cocos2d::SEL_MenuHandler)&ModLdr::ModLayer::showCredits
    );

    creditsButton->setPosition(40, -winSize.height + 48);
    creditsButton->setColor({ 255, 0, 255 });

    this->m_pButtonMenu->addChild(creditsButton);


    auto addButton = ButtonSprite::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png"),
        this->m_pButtonMenu,
        (cocos2d::SEL_MenuHandler)&ModLdr::ModLayer::addMod
    );

    addButton->setPosition(winSize.width - 70, -winSize.height + 64);

    this->m_pButtonMenu->addChild(addButton);


    auto folderButton = ButtonSprite::create(
        cocos2d::CCSprite::create("DL_folder.png"),
        this->m_pButtonMenu,
        (cocos2d::SEL_MenuHandler)&ModLdr::ModLayer::showModFolder
    );

    folderButton->setPosition(winSize.width - 70, -winSize.height + 128);

    this->m_pButtonMenu->addChild(folderButton);
}

void ModLdr::ModLayer::showInfo(cocos2d::CCObject* pSender) {
    auto f = FLAlertLayer::create(
        nullptr, "Info", "OK", nullptr, 350.0, 0, 0,
        "<cy>Mods</c> are <ca>DLLs</c> that are loaded into GD on startup.\n\n"  \
        "You can install mods by dragging them into the <co>mods/</c> folder, "   \
        "or by pressing the <cr>'+'</c> button."
    );

    f->show();
}

void ModLdr::ModLayer::showCredits(cocos2d::CCObject* pSender) {
    auto f = FLAlertLayer::create(
        nullptr, "Credits", "OK", nullptr, 300.0, 0, 0,
        "Developed by <cl>HJfod</c>\n"                      \
        "Based on <cr>GDDLLLoader</c> by <cg>adaf</c>\n\n"    \
        "<co>Extra special thank you to</c> <cy>PoweredByPie</c>"
    );

    f->show();
}

ModLdr::ModLayer* ModLdr::ModLayer::create() {
    ModLayer* pRet = new ModLayer();
    if (pRet && pRet->init("Mods")) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

