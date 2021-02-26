#include "ModLayer.hpp"
#include "../offsets.hpp"
#include "../ModManager.hpp"
#include "../gd/ButtonSprite.hpp"
#include "../gd/FLAlertLayer.hpp"
#include "../gd/CustomListView.hpp"
#include "../gd/GJListLayer.hpp"
#include "../gd/CCScrollLayerExt.hpp"
#include <direct.h>
#include <MinHook.h>

static cocos2d::CCMenuItem* addc(
    cocos2d::CCMenu* _p,
    cocos2d::CCSprite* _spr,
    cocos2d::CCPoint _pos,
    void (ModLdr::ModLayer::*_cb)(cocos2d::CCObject*)
) {
    ButtonSprite* b = ButtonSprite::create(
        _spr,
        _p,
        (cocos2d::SEL_MenuHandler)_cb
    );

    b->setPosition(_pos);

    _p->addChild(b);

    return b;
}

#define _NODE(x) static_cast<cocos2d::CCNode*>(x)
#define _CHILD(x) getChildren()->objectAtIndex(x)

static cocos2d::CCLayerColor* testt(const char* t) {
    auto layer = cocos2d::CCLayerColor::create({ 255, 0, 0, 255 }, 150, 20);

    auto bm = cocos2d::CCLabelBMFont::create(t, "bigFont.fnt");

    layer->addChild(bm);

    return layer;
}

class testing {
    public:
        void (__thiscall* testc)(void*, void*, int);
        static void __fastcall testh(void* ecx, void* edx, void* stack0, int stack1) {
            std::cout << "yoink" << stack1 << "\n";
        }
        static void init() {
            MH_CreateHook(
                (LPVOID)(ModLdr::base + 0x10ff0),
                (LPVOID)testing::testh,
                nullptr
            );

            MH_EnableHook(MH_ALL_HOOKS);

            std::cout << "inited\n";
        }
};

void ModLdr::ModLayer::showModFolder(cocos2d::CCObject*) {
    char buff[FILENAME_MAX];
    _getcwd(buff, FILENAME_MAX);
    std::string path(buff);

    path = path + "\\" + ModLdr::Manager::modFolder;
    ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

void ModLdr::ModLayer::addMod(cocos2d::CCObject* pSender) {
    nfdchar_t* path = nullptr;
    const nfdchar_t* filter = "dll";
    nfdresult_t res = NFD_OpenDialog(filter, nullptr, &path);

    if (res == NFD_OKAY) {
        //std::string fname = std::string(path);
        //fname = fname.substr(fname.find_last_of("\\"));
        //fname = "mods\\" + fname;
        //methods::fcopy(path, fname);
        //ModLdr::load_mod(fname);
        //mods(nullptr);
        free(path);
    }
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

void ModLdr::ModLayer::customSetup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto lrSize  = this->m_pListLayer->getScaledContentSize();

    if (ModLdr::Manager::mods.size() != 0) {
        auto noneText = cocos2d::CCLabelBMFont::create("No mods installed!", "bigFont.fnt");

        noneText->setPosition(lrSize.width / 2, lrSize.height / 2);
        noneText->setScale(.75);
        
        this->m_pListLayer->addChild(noneText);
    } else {
        auto arr = cocos2d::CCArray::create();

        arr->addObject(cocos2d::CCString::create("hey"));
        arr->addObject(cocos2d::CCString::create("hey"));
        arr->addObject(cocos2d::CCString::create("hey"));
        arr->addObject(cocos2d::CCString::create("hey"));
        arr->addObject(cocos2d::CCString::create("hey"));

        auto modListView = InheritedView::create(arr, 200.0, 70.0, 0x0);

        /*
        for (unsigned int i = 0; i < arr->count(); i++) {
            auto c = static_cast<cocos2d::CCLayerColor*>(arr->objectAtIndex(i));

            auto cc = _NODE(modListView->_CHILD(0));

            c->setPosition(100.0, 70.0 - i * 30.0);

            cc->addChild(c);
        }   //*/

        modListView->setPosition(60, 60);

        this->m_pListLayer->addChild(modListView);
    }

    addc(
        this->m_pButtonMenu,
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        { 0, -winSize.height + 48 },
        &ModLdr::ModLayer::showInfo
    );

    auto credits = addc(
        this->m_pButtonMenu,
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        { 36, -winSize.height + 48 },
        &ModLdr::ModLayer::showCredits
    );
    credits->setColor({ 255, 0, 255 });

    addc(
        this->m_pButtonMenu,
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png"),
        { winSize.width - 70, -winSize.height + 64 },
        &ModLdr::ModLayer::addMod
    );

    addc(
        this->m_pButtonMenu,
        cocos2d::CCSprite::create("DL_folder.png"),
        { winSize.width - 70, -winSize.height + 128 },
        &ModLdr::ModLayer::showModFolder
    );
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

