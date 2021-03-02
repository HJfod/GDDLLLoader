#include "ModLayer.hpp"
#include "../offsets.hpp"
#include "../ModManager.hpp"
#include "../gd/ButtonSprite.hpp"
#include "../gd/FLAlertLayer.hpp"
#include "../gd/CustomListView.hpp"
#include "../gd/GJListLayer.hpp"
#include "../gd/CCScrollLayerExt.hpp"
#include "../gd/CCMenuItemToggler.hpp"
#include <direct.h>
#include <MinHook.h>
#include <algorithm>
#include <filesystem>

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

std::string utf8_encode(const std::wstring &wstr) {
    if( wstr.empty() ) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring utf8_decode(const std::string &str) {
    if( str.empty() ) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

template<typename T>
static T getChild(cocos2d::CCNode* x, int i) {
    return static_cast<T>(x->getChildren()->objectAtIndex(i));
}

class ModListItem {
    std::string m_text;
    std::string m_info;

    public:
        cocos2d::CCLayer* getActual(cocos2d::CCSize _size) {
            auto layer = cocos2d::CCLayer::create();

            auto menu = cocos2d::CCMenu::create();

            layer->setContentSize(_size);

            auto bm = cocos2d::CCLabelBMFont::create(this->m_text.c_str(), "bigFont.fnt");

            bm->limitLabelWidth(200.0f, .75f, .2f);

            auto bmItem = cocos2d::CCMenuItemLabel::create(
                bm, menu, (cocos2d::SEL_MenuHandler)&ModListItem::showInfo
            );

            bmItem->setScale(.75);

            bmItem->setPosition(
                - _size.width / 2 + bmItem->getScaledContentSize().width / 2 + 20,
                4
            );

            bmItem->setUserData((void*)(new std::string(this->m_info)));

            menu->addChild(bmItem);


            auto enabledToggle = CCMenuItemToggler::createWithScale(menu, nullptr, .8f);

            enabledToggle->setPositionX(_size.width / 2 - 30);

            menu->addChild(enabledToggle);


            auto moveUpSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_upBtn_001.png");
            moveUpSprite->setScale(.7f);
            moveUpSprite->setRotation(0);

            auto upButton = ButtonSprite::create(
                moveUpSprite,
                menu,
                nullptr
            );

            upButton->setPositionX(_size.width / 2 - 60);
            upButton->setPositionY(7);

            menu->addChild(upButton);


            auto moveDownSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_upBtn_001.png");
            moveDownSprite->setScale(.7f);
            moveDownSprite->setRotation(180);

            auto downButton = ButtonSprite::create(
                moveDownSprite,
                menu,
                nullptr
            );

            downButton->setPositionX(_size.width / 2 - 60); // -80
            downButton->setPositionY(-7);

            menu->addChild(downButton);


            menu->setPosition(
                _size.width / 2,
                _size.height / 2
            );

            layer->addChild(menu);
            

            return layer;
        }

        void showInfo(cocos2d::CCObject* pSender) {
            auto fl = FLAlertLayer::create(
                nullptr, "Info", "OK", nullptr, 240.0f, 0, 0,
                reinterpret_cast<std::string*>(static_cast<cocos2d::CCNode*>(pSender)->getUserData())->c_str()
            );

            fl->show();
        }

        ModListItem(std::string _text, std::string _info = "No info available... somehow") {
            this->m_text = _text;
            this->m_info = _info;
        }
};

static constexpr const int modListTag = 420;

// DO THIS FOR DISABLING MODS
// "like it renames the dll to .dll.disabled" -mat

void renderList(GJListLayer* _list, cocos2d::CCSize _size) {
    if (_list->getChildByTag(modListTag) != nullptr)
        _list->removeChildByTag(modListTag);

    std::vector<ModListItem*> actualList;

    actualList.resize(ModLdr::Manager::mods.size());

    std::transform(
        ModLdr::Manager::mods.begin(),
        ModLdr::Manager::mods.end(),
        actualList.begin(),
        [](std::wstring _str) -> ModListItem* {
            std::string str = utf8_encode(_str);
            std::string name;
            
            if (str.find("\\") != std::string::npos)
                name = str.substr(str.find_last_of("\\") + 1);
            else name = str;

            return new ModListItem(
                // substr to remove the .dll
                name.substr(0, name.find_last_of(".")),
                ("Path: <cy>" + str + "</c>")
            );
        }
    );

    auto dummyArray = cocos2d::CCArray::create();

    for (auto l : actualList)
        dummyArray->addObject(new cocos2d::CCObject());

    auto modListView = CustomListView::create(dummyArray, _size.width, _size.height + 25, 0x0);

    for (unsigned int i = 0; i < dummyArray->count(); i++) {
        auto list  = getChild<cocos2d::CCLayer*>(modListView, 0);
        auto listc = getChild<cocos2d::CCLayerColor*>(list, 0);

        auto childpar = getChild<cocos2d::CCLayer*>(listc, i);

        auto childbg = getChild<cocos2d::CCLayerColor*>(childpar, 0);

        childbg->setOpacity(255);

        if (i % 2 == 0)
            // good ol' hardcoded color values
            childbg->setColor({ 161, 88, 44 });
        else
            childbg->setColor({ 194, 114, 62 });

        
        auto childfg = getChild<cocos2d::CCLayer*>(childpar, 1);

        childfg->addChild(actualList.at(i)->getActual(childbg->getScaledContentSize()));


        auto border = cocos2d::CCLayerColor::create({ 0, 0, 0, 150 }, _size.width, 1);

        border->setPosition(0, 0);

        childpar->addChild(border);
    }

    modListView->setTag(modListTag);

    _list->addChild(modListView);

    while(!actualList.empty()) delete actualList.back(), actualList.pop_back();
}

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
        std::string fname = std::string(path);
        fname = fname.substr(fname.find_last_of("\\"));
        fname = "mods\\" + fname;
        std::filesystem::copy_file(path, fname);
        ModLdr::Manager::loadMod(utf8_decode(fname));

        auto list = reinterpret_cast<GJListLayer*>(
            static_cast<cocos2d::CCNode*>(pSender)->getUserData()
        );

        renderList(list, list->getScaledContentSize());

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

void ModLdr::ModLayer::reloadMods(cocos2d::CCObject* pSender) {
    Manager::loadMods();

    auto list = reinterpret_cast<GJListLayer*>(
        static_cast<cocos2d::CCNode*>(pSender)->getUserData()
    );

    renderList(list, list->getScaledContentSize());
}

void ModLdr::ModLayer::customSetup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto lrSize  = this->m_pListLayer->getScaledContentSize();

    if (ModLdr::Manager::mods.size() == 0) {
        auto noneText = cocos2d::CCLabelBMFont::create("No mods installed!", "bigFont.fnt");

        noneText->setPosition(lrSize.width / 2, lrSize.height / 2);
        noneText->setScale(.75);
        
        this->m_pListLayer->addChild(noneText);
    } else
        renderList(this->m_pListLayer, lrSize);

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

    auto add = addc(
        this->m_pButtonMenu,
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png"),
        { winSize.width - 70, -winSize.height + 64 },
        &ModLdr::ModLayer::addMod
    );
    add->setUserData(this->m_pListLayer);

    addc(
        this->m_pButtonMenu,
        cocos2d::CCSprite::create("DL_folder.png"),
        { winSize.width - 70, -winSize.height + 128 },
        &ModLdr::ModLayer::showModFolder
    );

    auto update = addc(
        this->m_pButtonMenu,
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        { winSize.width - 70, -winSize.height + 192 },
        &ModLdr::ModLayer::reloadMods
    );
    update->setUserData(this->m_pListLayer);
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

