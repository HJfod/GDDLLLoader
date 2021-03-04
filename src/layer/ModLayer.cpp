#include "ModLayer.hpp"
#include "../offsets.hpp"
#include "../ModManager.hpp"
#include "../gd/ButtonSprite.hpp"
#include "../gd/FLAlertLayer.hpp"
#include "../gd/CustomListView.hpp"
#include "../gd/GJListLayer.hpp"
#include "../gd/CCScrollLayerExt.hpp"
#include "../gd/CCMenuItemToggler.hpp"
#include "ModInfoLayer.hpp"
#include "DLLLayer.hpp"
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
    CCMenuItemSpriteExtraGD* b = CCMenuItemSpriteExtraGD::create(
        _spr,
        _p,
        (cocos2d::SEL_MenuHandler)_cb
    );

    b->setPosition(_pos);

    _p->addChild(b);

    return b;
}

class ModListItem {
    ModLdr::Manager::Mod* m_mod;
    int m_index;

    public:
        static const int labelTag = 5;

        cocos2d::CCMenu* getActual(cocos2d::CCSize _size) {
            auto menu = cocos2d::CCMenu::create();

            menu->setContentSize(_size);

            auto bm = cocos2d::CCLabelBMFont::create(this->m_mod->name.c_str(), "bigFont.fnt");

            bm->limitLabelWidth(200.0f, .75f, .2f);

            auto bmItem = cocos2d::CCMenuItemLabel::create(
                bm, menu, (cocos2d::SEL_MenuHandler)&ModListItem::showInfo
            );

            bmItem->setScale(.75);

            bmItem->setPosition(
                - _size.width / 2 + bmItem->getScaledContentSize().width / 2 + 20,
                4
            );

            if (!this->m_mod->loaded)
                bmItem->setColor({ 150, 150, 150 });
            
            bmItem->setTag(labelTag);

            bmItem->setUserData(reinterpret_cast<void*>(this->m_mod));

            menu->addChild(bmItem);


            auto enabledToggle = CCMenuItemToggler::createWithScale(
                menu,
                (cocos2d::SEL_MenuHandler)&ModLdr::ModLayer::toggleMod,
                .8f
            );

            enabledToggle->toggle(this->m_mod->enabled);

            enabledToggle->setUserData(reinterpret_cast<void*>(this->m_mod));

            enabledToggle->setPositionX(_size.width / 2 - 30);

            menu->addChild(enabledToggle);

            /*

            auto moveUpSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_upBtn_001.png");
            moveUpSprite->setScale(.7f);
            moveUpSprite->setRotation(0);

            auto upButton = CCMenuItemSpriteExtraGD::create(
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

            auto downButton = CCMenuItemSpriteExtraGD::create(
                moveDownSprite,
                menu,
                nullptr
            );

            downButton->setPositionX(_size.width / 2 - 60); // -80
            downButton->setPositionY(-7);

            menu->addChild(downButton);

            //*/


            menu->setPosition(
                _size.width / 2,
                _size.height / 2
            );
            

            return menu;
        }

        void showInfo(cocos2d::CCObject* pSender) {
            auto mod = reinterpret_cast<ModLdr::Manager::Mod*>(
                static_cast<cocos2d::CCNode*>(pSender)->getUserData()
            );

            ModLdr::ModInfoLayer::create(mod)->show();

            /*
            auto fl = FLAlertLayer::create(
                nullptr, "Info", "OK", nullptr, 350.0f, 0, 0, info.c_str()
            );

            fl->show();

            //*/
        }

        ModListItem(ModLdr::Manager::Mod* _mod, int _ix) {
            this->m_mod = _mod;
            this->m_index = _ix;
        }
};

static constexpr const int modListTag = 420;
static constexpr const int noModsTag = 421;

bool shownInfoAboutBasicDLLs = false;

void renderList(GJListLayer* _list, cocos2d::CCSize _size) {
    if (_list->getChildByTag(modListTag) != nullptr)
        _list->removeChildByTag(modListTag);

    if (_list->getChildByTag(noModsTag) != nullptr)
        _list->removeChildByTag(noModsTag);

    // wow this code is terrible
    // but it works so eh

    std::vector<ModListItem*> actualList;

    // allocate space for modlist
    actualList.resize(ModLdr::Manager::mods.size());

    int modIndex = 0;
    // turn modlist into list of modlistitem
    std::transform(
        ModLdr::Manager::mods.begin(),
        ModLdr::Manager::mods.end(),
        actualList.begin(),
        [&modIndex](ModLdr::Manager::Mod* _mod) -> ModListItem* {
            return new ModListItem(
                _mod,
                modIndex++
            );
        }
    );

    // dummy array for the modlistview
    // this is needed so we get the right amount of
    // children in the listview
    auto dummyArray = cocos2d::CCArray::create();

    for (auto l : actualList)
        dummyArray->addObject(new cocos2d::CCObject());

    // why do i add 25 to the height? because for
    // some god-knows reason the list is otherwise
    // wrongly positioned! why? don't know!
    auto modListView = CustomListView::create(dummyArray, _size.width, _size.height + 25, 0x0);

    // get children
    auto list  = getChild<cocos2d::CCLayer*>(modListView, 0);
    auto listc = getChild<cocos2d::CCLayerColor*>(list, 0);

    // height of list items
    float height = 40.0f;

    for (unsigned int i = 0; i < dummyArray->count(); i++) {
        // get list item by index
        auto childpar = getChild<cocos2d::CCLayer*>(listc, i);
        
        // resize & reposition child to match our
        // desired height
        childpar->setPositionY(childpar->getPositionY() * ( height / childpar->getContentSize().height ));
        childpar->setContentSize({ childpar->getContentSize().width, height });

        // get child background layer
        auto childbg = getChild<cocos2d::CCLayerColor*>(childpar, 0);

        // setZOrder is because GD for some reason
        // draws lines wherever the fuck it wants
        // in the childpar node, so to fix that we
        // just make sure we're drawing on top of it
        childbg->setOpacity(255);
        childbg->setZOrder(100);

        // make every other bg the darker version
        if (i % 2 == 0)
            // good ol' hardcoded color values
            childbg->setColor(listBGDark);
        else
            childbg->setColor(listBGLight);

        // resize background to match new item height
        childbg->setContentSize(childpar->getScaledContentSize());

        // get child foreground layer
        auto childfg = getChild<cocos2d::CCLayer*>(childpar, 1);

        // generate content for layer from our own
        // ModMenuItem class
        childfg->addChild(actualList.at(i)->getActual(childpar->getScaledContentSize()));
        childfg->setZOrder(101);

        // resize foreground to match new item height
        childfg->setContentSize(childpar->getScaledContentSize());

        // create the border between items
        auto border = cocos2d::CCLayerColor::create({ 0, 0, 0, 150 }, _size.width, 1);

        border->setPosition(0, 0);
        border->setZOrder(500);

        childpar->addChild(border);
    }

    // resize list to match the size of our new resized list items
    listc->setContentSize({ listc->getContentSize().width, dummyArray->count() * height });

    // move list scroll position to the top
    listc->setPositionY(dummyArray->count() * -height + _size.height);

    // set tag so we can remove the modlist on refresh
    modListView->setTag(modListTag);

    _list->addChild(modListView);

    // clean up list
    while(!actualList.empty()) delete actualList.back(), actualList.pop_back();
}

void ModLdr::ModLayer::toggleMod(cocos2d::CCObject* pSender) {
    auto send = static_cast<CCMenuItemToggler*>(pSender);

    auto mod = reinterpret_cast<Manager::Mod*>(send->getUserData());

    if (mod->type == Manager::Mod::Type::DLL)
        if (!shownInfoAboutBasicDLLs) {
            auto fl = FLAlertLayer::create(
                nullptr, "Warning", "OK", nullptr, 320.0f, 0, 0,
                "<cc>Warning</c>: While standard <cj>DLL</c>s can be <cg>enabled</c> " \
                "live, they can not be <cr>disabled</c> without a <co>restart</c>."
            );

            fl->show();

            shownInfoAboutBasicDLLs = true;
        }
    
    bool load = mod->loaded;

    if (!ModLdr::Manager::enableMod(
        mod, send->isToggled()
    )) {
        auto fl = FLAlertLayer::create(
            nullptr, "Warning", "OK", nullptr, 320.0f, 0, 0,
            "<cc>Error</c>: Unable to load DLL!" \
            "(A <co>restart</c> will probably fix the issue.)"
        );

        fl->show();
    } else if (!load && send->isToggled())
        static_cast<cocos2d::CCMenuItemLabel*>(
            send->getParent()->getChildByTag(ModListItem::labelTag)
        )->setColor({ 255, 255, 255 });
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

        list->setColor(listBGLight);
        list->setOpacity(255);
        renderList(list, list->getScaledContentSize());

        FLAlertLayer::create(
            nullptr, "Warning", "OK", nullptr, 250.0, 0, 0,
            "<co>Warning</c>: Due to an unknown <cc>bug</c>, you need to re-enter " \
            "the <cg>layer</c> for the list to properly work."
        )->show();

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
        nullptr, "Credits", "OK", nullptr, 320.0, 0, 0,
        "Developed by <cl>HJfod</c>\n"                      \
        "Based on <cr>GDDLLLoader</c> by <cg>adaf</c>\n\n"    \
        "<co>Extra special thank you to</c> <cy>PoweredByPie</c>\n\n" \
        "Version <cg>" + std::string(ModLdr::version) + "</c>"
    );

    f->show();
}

void ModLdr::ModLayer::reloadMods(cocos2d::CCObject* pSender) {
    Manager::loadMods();

    auto list = reinterpret_cast<GJListLayer*>(
        static_cast<cocos2d::CCNode*>(pSender)->getUserData()
    );

    list->setColor(listBGLight);
    list->setOpacity(255);
    renderList(list, list->getScaledContentSize());

    FLAlertLayer::create(
        nullptr, "Warning", "OK", nullptr, 250.0, 0, 0,
        "<co>Warning</c>: Due to an unknown <cc>bug</c>, you need to re-enter" \
        "the <cg>layer</c> for the list to properly work."
    )->show();
}

void ModLdr::ModLayer::showLoadedDLLs(cocos2d::CCObject* pSender) {
    auto layer = reinterpret_cast<ModLayer*>(
        static_cast<cocos2d::CCNode*>(pSender)->getUserData()
    );

    layer->showDLLList = true;

    layer->hideLayer(false);
}

void ModLdr::ModLayer::customSetup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto lrSize  = this->m_pListLayer->getScaledContentSize();

    if (ModLdr::Manager::mods.size() == 0) {
        auto noneText = cocos2d::CCLabelBMFont::create("No mods installed!", "bigFont.fnt");

        noneText->setPosition(lrSize.width / 2, lrSize.height / 2);
        noneText->setScale(.75);

        noneText->setTag(noModsTag);
        
        this->m_pListLayer->addChild(noneText);
    } else {
        this->m_pListLayer->setColor(listBGLight);
        this->m_pListLayer->setOpacity(255);
        renderList(this->m_pListLayer, lrSize);
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

    auto dlls = addc(
        this->m_pButtonMenu,
        ButtonSprite::create("View Loaded DLLs", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 0.8f),
        { winSize.width / 2 - this->m_pButtonMenu->getPositionX(), -winSize.height + 48 },
        &ModLdr::ModLayer::showLoadedDLLs
    );
    dlls->setUserData(this);
}

void ModLdr::ModLayer::layerHidden() {
    if (this->showDLLList) {
        auto l = DLLLayer::create();

        cocos2d::CCDirector::sharedDirector()->getRunningScene()->addChild(l);

        l->showLayer(false);

        this->showDLLList = false;
    }

    GJDropDownLayer::layerHidden();
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

