#include <cocos2d.h>
#include <process.h>
#include "MinHook.h"
#include "../ext/nfd.h"
#include "../src/methods.hpp"
#include <vector>
#include <Psapi.h>
#include <Shlwapi.h>

struct mod {
    HMODULE module;
    std::string name;
};

namespace MenuLayer {
    std::vector<mod*> modList;

    void load_mod(std::string path) {
		HMODULE handle = LoadLibraryA(path.c_str());

		if (handle != nullptr)
			MenuLayer::modList.push_back(new mod {
				handle,
				path.substr(path.find_last_of("\\") + 1)
			});
        else
            MessageBoxA(nullptr, ("Unable to load DLL: " + path).c_str(), "Error", MB_ICONERROR);
    }
}

struct mod_name {
    std::string name;

    mod_name(std::string _name) {
        this->name = _name;
    }
};

bool UnloadDLL(DWORD ProcessID, const char* DllName) {
    HMODULE hMod;
    HMODULE hMods[0x400];
    DWORD dwNeeded;
    HANDLE Proc;
    LPVOID FreeLibAddy;
    
    if (!ProcessID)
        return false;
    
    Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
    if (!Proc)
        return false;
    
    EnumProcessModules(Proc, hMods, 0x400, &dwNeeded);
    for (unsigned int i = 0; i < (dwNeeded / sizeof(DWORD)); i++) {
        char szPath[MAX_PATH] = "";
        GetModuleFileNameExA(Proc, hMods[i], szPath, MAX_PATH);
        PathStripPathA(szPath);
    
        if (!_stricmp(szPath, DllName))
            hMod = hMods[i];
    }
    
    FreeLibAddy = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");
    HANDLE hThread = CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)FreeLibAddy, (LPVOID)hMod, NULL, NULL);
    WaitForSingleObjectEx(hThread, INFINITE, FALSE);
    CloseHandle(Proc);
    FreeLibraryAndExitThread(hMod, 0);
    return true;
}

class Callback {
    public:
        static constexpr int l_tag = 420;
        static constexpr int opt_h = 40;
        static std::string modtorem;

        void close_mod_menu(cocos2d::CCObject* pSender) {
            cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
            
            if (scene->getChildByTag(l_tag) != nullptr)
                scene->removeChild(
                    scene->getChildByTag(l_tag)
                );
        }

        void add_mod(cocos2d::CCObject* pSender) {
            nfdchar_t* path = nullptr;
            const nfdchar_t* filter = "dll";
            nfdresult_t res = NFD_OpenDialog(filter, nullptr, &path);

            if (res == NFD_OKAY) {
                std::string fname = std::string(path);
                fname = fname.substr(fname.find_last_of("\\"));
                fname = "mods\\" + fname;
                methods::fcopy(path, fname);
                MenuLayer::load_mod(fname);
                mods(nullptr);
                free(path);
            }
        }

        void remove_mod(cocos2d::CCObject* pSender) {
            std::string name = ((mod_name*)((cocos2d::CCNode*)pSender)->getUserData())->name;

            int ix = 0;
            for (mod* mod : MenuLayer::modList) {
                if (mod->name == name) {
                    HMODULE modu = GetModuleHandleA(mod->name.c_str());
                    
                    if (modu == nullptr) {
                        MessageBoxA(
                            nullptr,
                            "Error: Could not find mod handle!",
                            "Unable to remove mod",
                            MB_OK
                        );
                        break;
                    }
                    
                    if (!UnloadDLL(_getpid(), mod->name.c_str())) {
                        MessageBoxA(
                            nullptr,
                            "Error: Unable to unload!",
                            "Unable to remove mod",
                            MB_OK
                        );
                        break;
                    }
                    
                    MessageBoxA(nullptr, "feetus", "yeetus", MB_OK);
                    MenuLayer::modList.erase(MenuLayer::modList.begin() + ix);

                    cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
                    
                    if (scene->getChildByTag(l_tag) != nullptr)
                        scene->removeChild(
                            scene->getChildByTag(l_tag + ix + 1)
                        );
                    break;
                }
                ix++;
            }
        }

        void open_mod_folder(cocos2d::CCObject* pSender) {
            std::string path = methods::workdir() + "\\mods";
            ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
        }

        void mods(cocos2d::CCObject* pSender) {
            close_mod_menu(nullptr);

            cocos2d::CCDirector* dir = cocos2d::CCDirector::sharedDirector();
            cocos2d::CCSize z = dir->getWinSize();

            cocos2d::CCSize psiz = z / 1.5;

            cocos2d::CCLayerColor* backLayer = cocos2d::CCLayerColor::create({ 0, 0, 0, 180 });

            cocos2d::CCLayerColor* layer = cocos2d::CCLayerColor::create(
                { 0, 0, 0, 150 },
                psiz.width,
                psiz.height
            );

            layer->setPosition({ z.width / 2 - psiz.width / 2, z.height / 2 - psiz.height / 2 });

            // sides: GJ_table_side_001.png
            // top: GJ_table_top_001.png
            // bottom: GJ_table_bottom_001.png
            // backbtn: GJ_arrow_03_001.png

            cocos2d::CCSprite* sprSide0 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
            cocos2d::CCSprite* sprSide1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
            cocos2d::CCSprite* sprTop = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_top_001.png");
            cocos2d::CCSprite* sprBottom = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_bottom_001.png");
            cocos2d::CCSprite* sprBack = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
            cocos2d::CCSprite* sprAdd = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
            cocos2d::CCSprite* sprFold = cocos2d::CCSprite::createWithSpriteFrameName("GJ_myServerBtn_001.png");

            cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Mods", "bigFont.fnt");

            title->setPosition( psiz.width / 2, ( z.height / 2 - psiz.height / 2 ) - sprTop->getContentSize().height / 2 + 5 );
            title->setScale(.8);

            sprTop->addChild(title);

            sprSide1->setScaleX(-1);
            sprSide0->setScaleY(3);
            sprSide1->setScaleY(3);

            sprTop->setPosition({ psiz.width / 2, psiz.height });
            sprBottom->setPosition({ psiz.width / 2, 0 });
            sprSide0->setPosition({ 8, psiz.height / 2 });
            sprSide1->setPosition({ psiz.width - 8, psiz.height / 2 });

            cocos2d::CCPoint* pn = new cocos2d::CCPoint( psiz.width - 30, psiz.height -30 );
            cocos2d::CCDrawNode* clipspr = cocos2d::CCDrawNode::create();
            clipspr->setContentSize({ psiz.width - 30, psiz.height - 30 });
            clipspr->drawPolygon(pn, 4, { 255, 255, 255, 255 }, 1, { 255, 255, 255, 255 });
            cocos2d::CCClippingNode* clip = cocos2d::CCClippingNode::create(clipspr);
            clip->setContentSize({ psiz.width - 30, psiz.height - 30 });

            cocos2d::CCMenu* list = cocos2d::CCMenu::create();
            list->setContentSize({ psiz.width - 30, psiz.height - 30 });

            int ix = 0;
            for (mod* mod : MenuLayer::modList) {
                cocos2d::CCMenuItem* mi = cocos2d::CCMenuItem::create();
                cocos2d::ccColor4B col;
                if (ix % 2 == 0) col = { 123, 65, 31, 255 };
                else col = { 153, 85, 51, 255 };

                cocos2d::CCLayerColor* bgc = cocos2d::CCLayerColor::create(
                    col,
                    list->getContentSize().width,
                    opt_h
                );

                mi->setContentSize({ list->getContentSize().width, opt_h });

                // i can explain
                // child menu
                // c-menu
                // c-men
                // semen
                // makes perfect sense right
                cocos2d::CCMenu* semen = cocos2d::CCMenu::create();
                semen->setPosition( list->getContentSize().width - opt_h / 2, opt_h / 2 );

                cocos2d::CCSprite* unls = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
                CCMenuItemSpriteExtra* unlb = CCMenuItemSpriteExtra::create(
                    unls,
                    unls,
                    list,
                    (cocos2d::SEL_MenuHandler)&Callback::remove_mod
                );

                unlb->setUserData(new mod_name(mod->name));

                unlb->setScale(.8);
                unlb->setPosition(0, 0);

                semen->addChild(unlb);

                cocos2d::CCLabelBMFont* txt = cocos2d::CCLabelBMFont::create(mod->name.c_str(), "bigFont.fnt");

                txt->setPosition( list->getContentSize().width / 2, opt_h / 2 );
                txt->setScale(.6);

                bgc->addChild(txt);
                //bgc->addChild(semen);

                mi->addChild(bgc);

                mi->setPosition( 0, list->getContentSize().height / 2 - opt_h / 2 - opt_h * ix );

                mi->setTag(l_tag + ix + 1);

                list->addChild(mi);

                ix++;
            }

            // list->alignItemsVerticallyWithPadding(5.0f);

            list->setPosition( psiz.width / 2, psiz.height / 2 );

            //clip->addChild(list);
            layer->addChild(list);

            layer->addChild(sprSide0);
            layer->addChild(sprSide1);
            layer->addChild(sprTop);
            layer->addChild(sprBottom);

            layer->setScale(0);

            layer->runAction(cocos2d::CCEaseBackOut::create(
                cocos2d::CCScaleTo::create(.25, 1)
            ));

            cocos2d::CCMenu* btns = cocos2d::CCMenu::create();
            btns->setPosition( psiz.width / 2, psiz.height / 2 );

            CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(
                sprBack,
                sprBack,
                btns,
                (cocos2d::SEL_MenuHandler)&Callback::close_mod_menu
            );
            backBtn->setPosition( - z.width / 2 + 32, z.height / 2 - 32 );

            btns->addChild(backBtn);

            CCMenuItemSpriteExtra* addBtn = CCMenuItemSpriteExtra::create(
                sprAdd,
                sprAdd,
                btns,
                (cocos2d::SEL_MenuHandler)&Callback::add_mod
            );
            addBtn->setPosition( z.width / 2 - 44, - z.height / 2 + 44 );

            btns->addChild(addBtn);

            CCMenuItemSpriteExtra* folderBtn = CCMenuItemSpriteExtra::create(
                sprFold,
                sprFold,
                btns,
                (cocos2d::SEL_MenuHandler)&Callback::open_mod_folder
            );
            folderBtn->setPosition( z.width / 2 - 44, - z.height / 2 + 104 );

            btns->addChild(folderBtn);

            layer->addChild(btns);

            backLayer->addChild(layer);

            backLayer->setTag(l_tag);

            dir->getRunningScene()->addChild(backLayer);
        }
};

namespace MenuLayer {
    inline bool (__thiscall* init)(cocos2d::CCLayer* self);
    bool __fastcall initHook(cocos2d::CCLayer* _self) {
        bool in = init(_self);

        cocos2d::CCMenu* menu = ((cocos2d::CCMenu*)_self->getChildren()->objectAtIndex(3));

        cocos2d::CCNode* statsBtn = (cocos2d::CCNode*)menu->getChildren()->objectAtIndex(2);
        cocos2d::CCNode* settingsBtn = (cocos2d::CCNode*)menu->getChildren()->objectAtIndex(1);

        cocos2d::CCMenuItem* chestBtn = (cocos2d::CCMenuItem*)menu->getChildren()->objectAtIndex(4);

        menu->removeChild(chestBtn, false);

        cocos2d::CCSprite* spr = cocos2d::CCSprite::create("mods.png");
        CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(
            spr,
            spr,
            menu,
            (cocos2d::SEL_MenuHandler)&Callback::mods
        );

        menu->addChild(btn);
	    menu->alignItemsHorizontallyWithPadding(5.0f);

        menu->addChild(chestBtn);

        return in;
    }

    bool mem_init() {
        // Initialize hooker
        MH_STATUS ini = MH_Initialize();
        if (ini != MH_OK)
            return false;

        uintptr_t base = (uintptr_t)GetModuleHandleA(0);

        MH_CreateHook((PVOID)(base + 0x1907b0), (LPVOID)MenuLayer::initHook, (LPVOID*)&MenuLayer::init);

        MH_EnableHook(MH_ALL_HOOKS);

        return true;
    }
}