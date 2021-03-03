#include "DLLLayer.hpp"
#include "../offsets.hpp"
#include "../gd/CustomListView.hpp"
#include "../gd/GJListLayer.hpp"
#include "../gd/FLAlertLayer.hpp"
#include "../ModManager.hpp"
#include <psapi.h>
#include <process.h>

cocos2d::CCMenu* createListItem(std::string _path) {
    auto menu = cocos2d::CCMenu::create();

    std::string name = _path.substr(_path.find_last_of("\\") + 1);

    auto bm = cocos2d::CCLabelBMFont::create(
        name.c_str(), "bigFont.fnt"
    );

    bm->limitLabelWidth(250.0f, 1.0f, .3f);

    auto text = cocos2d::CCMenuItemLabel::create(
        bm,
        menu,
        (cocos2d::SEL_MenuHandler)&ModLdr::DLLLayer::showInfo
    );

    text->setScale(.65f);

    text->setUserData(reinterpret_cast<void*>(
        new std::string(_path)
    ));

    text->setPositionX(text->getScaledContentSize().width / 2 + 20);

    menu->addChild(text);

    return menu;
}

void ModLdr::DLLLayer::showInfo(cocos2d::CCObject* pSender) {
    auto data = *reinterpret_cast<std::string*>(
        static_cast<cocos2d::CCNode*>(pSender)->getUserData()
    );

    auto fl = FLAlertLayer::create(
        nullptr, "DLL Path", "OK", nullptr, 350.0, 0, 0, data
    );

    fl->show();
}

cocos2d::CCArray* listLoadedDLLs() {
    HMODULE hMods[1024];
    DWORD cbNeeded;

    auto array = cocos2d::CCArray::create();

    DWORD pid = GetCurrentProcessId();
    HANDLE proc = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        false,
        pid
    );

    if (EnumProcessModules(proc, hMods, sizeof(hMods), &cbNeeded))
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            WCHAR szModName[MAX_PATH];

            if (GetModuleFileNameExW(
                proc,
                hMods[i],
                szModName,
                sizeof(szModName) / sizeof(TCHAR)
            )) {
                array->addObject(
                    createListItem(utf8_encode(szModName))
                );
            }
        }
    else {
        auto fl = FLAlertLayer::create(
            nullptr, "Error", "OK", nullptr, 240.0, 0, 0,
            "Unable to load <cl>DLL</c>s! <co>Error code</c>: <cc>"
            + std::to_string(GetLastError()) + "</c>"
        );

        fl->show();
    }
    
    return array;
}

void ModLdr::DLLLayer::customSetup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto lrSize  = this->m_pListLayer->getScaledContentSize();

    this->m_pListLayer->setColor(listBGLight);
    this->m_pListLayer->setOpacity(255);

    auto array = listLoadedDLLs();

    auto modListView = CustomListView::create(array, lrSize.width, lrSize.height + 25, 0x0);

    auto list  = getChild<cocos2d::CCLayer*>(modListView, 0);
    auto listc = getChild<cocos2d::CCLayerColor*>(list, 0);

    for (unsigned int i = 0; i < array->count(); i++) {
        auto childpar = getChild<cocos2d::CCLayer*>(listc, i);

        auto childbg = getChild<cocos2d::CCLayerColor*>(childpar, 0);

        childbg->setOpacity(255);
        childbg->setZOrder(100);

        if (i % 2 == 0)
            childbg->setColor(listBGDark);
        else
            childbg->setColor(listBGLight);

        auto childfg = getChild<cocos2d::CCLayer*>(childpar, 1);

        auto text = static_cast<cocos2d::CCMenu*>(array->objectAtIndex(i));

        text->setPosition(
            0,
            childpar->getScaledContentSize().height / 2
        );

        childfg->addChild(text);
        childfg->setZOrder(101);

        // create the border between items
        auto border = cocos2d::CCLayerColor::create({ 0, 0, 0, 150 }, lrSize.width, 1);

        border->setPosition(0, 0);
        border->setZOrder(500);

        childpar->addChild(border);
    }

    std::stringstream s;
    s << "Total: " << array->count();

    auto textCount = cocos2d::CCLabelBMFont::create(
        s.str().c_str(), "goldFont.fnt"
    );

    textCount->setPositionX(winSize.width - 70);
    textCount->setScale(.5f);

    this->m_pButtonMenu->addChild(textCount);

    this->m_pListLayer->addChild(modListView);
}

ModLdr::DLLLayer* ModLdr::DLLLayer::create() {
    DLLLayer* pRet = new DLLLayer();
    if (pRet && pRet->init("Loaded DLLs")) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

