#include <cocos2d.h>
#include "../cappuccino-sdk/incl/extensions/GUI/CCControlExtension/CCScale9Sprite.h"
#include <process.h>
#include "MinHook.h"
#include "../ext/nfd.h"
#include "../src/methods.hpp"
#include <vector>
#include <Psapi.h>
#include <Shlwapi.h>
#include <algorithm>
#include <filesystem>
#include "gd/gd.hpp"

static constexpr const char* credits = "ModLdr by adaf & HJfod";

struct mod {
    HMODULE module;
    std::string name;
};

struct page {
    int index;
    int max;
};

namespace ModLdr {
    std::vector<mod*> modList;

    void load_mod(std::string path) {
		HMODULE handle = LoadLibraryA(path.c_str());

		if (handle != nullptr)
			ModLdr::modList.push_back(new mod {
				handle,
				path.substr(path.find_last_of("\\") + 1)
			});
        else
            MessageBoxA(nullptr, ("Unable to load DLL: " + path).c_str(), "Error", MB_ICONERROR);
    }

    bool is_loaded(std::string _name) {
        for (mod* mod : modList)
            if (mod->name == _name)
                return true;
        return false;
    }

    int load_mods() {
        if (!std::filesystem::is_directory("mods") || !std::filesystem::exists("mods")) 
            std::filesystem::create_directory("mods");
        
        int c = 0;

        for (std::filesystem::directory_entry file : std::filesystem::directory_iterator("mods\\")) {
            std::string p = file.path().string();
            if (methods::ewith(p, ".dll"))
                if (!is_loaded(p.substr(p.find_last_of("\\") + 1))) {
                    ModLdr::load_mod(p);
                    c++;
                }
        }

        return c;
    }
}

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
        static constexpr int l_tag = 354;
        static constexpr int m_tag = 349;
        static constexpr int opt_h = 26;
        static constexpr int page_items = 7;

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
                ModLdr::load_mod(fname);
                mods(nullptr);
                free(path);
            }
        }

        void remove_mod(cocos2d::CCObject* pSender) {
            std::string name = *(std::string*)((cocos2d::CCNode*)pSender)->getUserData();

            int ix = 0;
            for (mod* mod : ModLdr::modList) {
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
                    ModLdr::modList.erase(ModLdr::modList.begin() + ix);

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

        void refresh_mods(cocos2d::CCObject* pSender) {
            ModLdr::load_mods();

            MessageBoxA(nullptr, "Warning: Refreshing does not unload deleted mods. You need to restart GD to remove mods.", "Warning", MB_ICONWARNING);

            mods(nullptr);
        }

        void render_page(int _page, cocos2d::CCMenu* _menu) {
            if (_page < 0) return;

            _menu->removeAllChildrenWithCleanup(true);

            for (int ix = 0; ix < page_items; ix++) {
                if (ix + _page * 7 >= ModLdr::modList.size()) break;

                cocos2d::CCMenuItem* mi = cocos2d::CCMenuItem::create();
                cocos2d::ccColor4B col;
                if (ix % 2 == 0) col = { 123, 65, 31, 255 };
                else col = { 153, 85, 51, 255 };

                cocos2d::CCLayerColor* bgc = cocos2d::CCLayerColor::create(
                    col,
                    _menu->getContentSize().width,
                    opt_h
                );

                mi->setContentSize({ _menu->getContentSize().width, opt_h });

                // i can explain
                // child menu
                // c-menu
                // c-men
                // semen
                // makes perfect sense right
                cocos2d::CCMenu* semen = cocos2d::CCMenu::create();
                semen->setPosition( _menu->getContentSize().width - opt_h / 2, opt_h / 2 );

                cocos2d::CCSprite* unls = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
                CCMenuItemSpriteExtra* unlb = CCMenuItemSpriteExtra::create(
                    unls,
                    unls,
                    _menu,
                    (cocos2d::SEL_MenuHandler)&Callback::remove_mod
                );

                unlb->setUserData(new std::string(ModLdr::modList.at(ix + _page * 7)->name));

                unlb->setScale(.8);
                unlb->setPosition(0, 0);

                semen->addChild(unlb);

                cocos2d::CCLabelBMFont* txt = cocos2d::CCLabelBMFont::create(
                    ModLdr::modList.at(ix + _page * 7)->name.c_str(),
                    "bigFont.fnt"
                );

                txt->setPosition( _menu->getContentSize().width / 2, opt_h / 2 );
                txt->setScale(.5);

                bgc->addChild(txt);
                //bgc->addChild(semen);

                mi->addChild(bgc);

                mi->setPosition( 0, _menu->getContentSize().height / 2 - opt_h / 2 - opt_h * ix );

                mi->setTag(l_tag + ix + 1);

                _menu->addChild(mi);
            }
        }

        void turn_page(cocos2d::CCObject* pSender) {
            int p = *(int*)((cocos2d::CCNode*)pSender)->getUserData();

            cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();

            cocos2d::CCNode* backLayer = scene->getChildByTag(l_tag);

            if (backLayer == nullptr) {
                MessageBoxA(nullptr, "Not in menu", "Error", MB_ICONERROR);
                return;
            }

            cocos2d::CCLayerColor* layer = ((cocos2d::CCLayerColor*)backLayer
                ->getChildren()
                ->objectAtIndex(0));

            cocos2d::CCMenu* menu = (cocos2d::CCMenu*)layer->getChildByTag(m_tag);
            
            if (menu != nullptr) {
                page* cur = (page*)menu->getUserData();

                int r = 0;

                if (cur->index + p < 0)
                    r = cur->max;
                else if (cur->index + p > cur->max)
                    r = 0;
                else r = cur->index + p;

                cur->index = r;

                ((cocos2d::CCLabelBMFont*)layer->getChildByTag(m_tag + 1))->setString(
                    ("Page " + std::to_string(cur->index + 1) + "/" + std::to_string(cur->max + 1)).c_str()
                );

                render_page(r, (cocos2d::CCMenu*)menu);
            }
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

            cocos2d::CCSprite* sprSide0 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
            cocos2d::CCSprite* sprSide1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
            cocos2d::CCSprite* sprTop = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_top_001.png");
            cocos2d::CCSprite* sprBottom = cocos2d::CCSprite::createWithSpriteFrameName("GJ_table_bottom_001.png");
            cocos2d::CCSprite* sprBack = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
            cocos2d::CCSprite* sprAdd = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
            cocos2d::CCSprite* sprFold = cocos2d::CCSprite::create("DL_folder.png");
            cocos2d::CCSprite* sprPageL = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
            cocos2d::CCSprite* sprPageR = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
            cocos2d::CCSprite* sprRefresh = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");

            cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Mods", "bigFont.fnt");
            cocos2d::CCLabelBMFont* version = cocos2d::CCLabelBMFont::create("v1.1", "goldFont.fnt");

            title->setPosition( psiz.width / 2, ( z.height / 2 - psiz.height / 2 ) - sprTop->getContentSize().height / 2 + 4 );
            title->setScale(.8);

            sprTop->addChild(title);

            sprSide1->setScaleX(-1);
            sprSide0->setScaleY(3);
            sprSide1->setScaleY(3);
            sprPageR->setFlipX(true);

            sprTop->setPosition({ psiz.width / 2, psiz.height });
            sprBottom->setPosition({ psiz.width / 2, 0 });
            sprSide0->setPosition({ 6, psiz.height / 2 });
            sprSide1->setPosition({ psiz.width - 6, psiz.height / 2 });

            if (ModLdr::modList.size() > 0) {
                int max_page = ((int)std::ceil( ModLdr::modList.size() / (double)page_items )) - 1;

                cocos2d::CCLabelBMFont* pagen = cocos2d::CCLabelBMFont::create("Page ~/~", "goldFont.fnt");
                pagen->setTag(m_tag + 1);
                pagen->setScale(.5);
                pagen->setPosition(psiz.width / 2 + z.width / 2 - 50, psiz.height / 2 + z.height / 2 - 16);

                layer->addChild(pagen);

                cocos2d::CCMenu* list = cocos2d::CCMenu::create();
                list->setContentSize({ psiz.width - 30, psiz.height - 30 });
                list->setUserData(new page { 0, max_page });

                render_page(0, list);
                list->setTag(m_tag);

                // list->alignItemsVerticallyWithPadding(5.0f);

                list->setPosition( psiz.width / 2, psiz.height / 2 );

                layer->addChild(list);
            } else {
                cocos2d::CCLabelBMFont* none = cocos2d::CCLabelBMFont::create(
                    "No mods installed!\n\nInstall mods by adding them to\nthe 'mods/' folder, or by\nclicking the '+' button.",
                    "bigFont.fnt"
                );
                none->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentCenter);

                none->setPosition( psiz.width / 2, psiz.height / 2 );
                none->setScale(.6);

                layer->addChild(none);
            }

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

            CCMenuItemSpriteExtra* pageLeftBtn = CCMenuItemSpriteExtra::create(
                sprPageL,
                sprPageL,
                btns,
                (cocos2d::SEL_MenuHandler)&Callback::turn_page
            );
            pageLeftBtn->setPosition( - z.width / 2 + 64, 0 );
            pageLeftBtn->setUserData(new int(-1));
            btns->addChild(pageLeftBtn);

            CCMenuItemSpriteExtra* pageRightBtn = CCMenuItemSpriteExtra::create(
                sprPageR,
                sprPageR,
                btns,
                (cocos2d::SEL_MenuHandler)&Callback::turn_page
            );
            pageRightBtn->setPosition( z.width / 2 - 64, 0 );
            pageRightBtn->setUserData(new int(1));
            btns->addChild(pageRightBtn);

            CCMenuItemSpriteExtra* refreshBtn = CCMenuItemSpriteExtra::create(
                sprRefresh,
                sprRefresh,
                btns,
                (cocos2d::SEL_MenuHandler)&Callback::refresh_mods
            );
            refreshBtn->setPosition( - z.width / 2 + 44, - z.height / 2 + 44 );
            btns->addChild(refreshBtn);

            layer->addChild(btns);

            version->setPosition( psiz.width / 2, - 32 );
            version->setScale(.65),

            layer->addChild(version);

            backLayer->addChild(layer);

            backLayer->setTag(l_tag);

            dir->getRunningScene()->addChild(backLayer);
        }

        void test(cocos2d::CCObject* pSender) {
            //FLAlertLayer::create(nullptr, "yoo", "yoooo", "yoooooo", "yoooooooo");

            cocos2d::CCLayerColor* out = DropDownLayer::create("Swaggy");

            //DropDownLayer::show(out, "Swaggy");
        }

        void test2(cocos2d::CCObject* pSender) {
            uintptr_t l = (uintptr_t)OptionsLayer::show();

            std::cout << "yeet\n";

            std::cout << "hey it didn't crash" << std::endl;
        }
};

namespace fuck_it_i_give_up {
    void (__fastcall * cum)(cocos2d::CCObject*);
    void __fastcall dick(cocos2d::CCObject* pSender) {
        std::cout << "fml\n";

        cum(pSender);
    }
    
    void mem_init() {
        MH_CreateHook((PVOID)((uintptr_t)GetModuleHandleA(0) + 0x191C30), (LPVOID)dick, (LPVOID*)&cum);
    }
}

class ButtonSprite_mine : public cocos2d::CCSprite {
    public:
        static cocos2d::CCSprite* create(
            const char* _text,
            const char* _bsprite,
            const char* _font,
            float _tscale,
            cocos2d::CCSize _size
        ) {
            cocos2d::extension::CCScale9Sprite* backSpr = cocos2d::extension::CCScale9Sprite::create(
                _bsprite, { 0, 0, 40, 40 }
            );

            backSpr->setContentSize(_size);

            cocos2d::CCSprite* spr = cocos2d::CCSprite::create();
            spr->setContentSize(_size);

            backSpr->setPosition(_size.width / 2, _size.height / 2);
            spr->addChild(backSpr);

            cocos2d::CCLabelBMFont* text = cocos2d::CCLabelBMFont::create(_text, _font);
            text->setPosition(_size.width / 2, _size.height / 2 + 3);
            text->setScale(_tscale);
            spr->addChild(text);

            return spr;
        };
};

namespace HelpLayer {
    inline void (__fastcall* init)(cocos2d::CCNode*);
    void __fastcall initHook(cocos2d::CCNode* _self) {
        init(_self);

        cocos2d::CCSize win = cocos2d::CCDirector::sharedDirector()->getWinSize();

        cocos2d::CCNode* child = (cocos2d::CCNode*)_self->getChildren()->objectAtIndex(0);

        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(credits, "goldFont.fnt");
        label->setPosition(win.width / 2, 10);
        label->setScale(.5);

        child->addChild(label);

        return;
    }

    void mem_init() {
        uintptr_t base = (uintptr_t)GetModuleHandleA(0);

        MH_CreateHook((PVOID)(base + 0x25C7B0), (LPVOID)initHook, (LPVOID*)&init);
    }
}

namespace SettingsLayer {
    int test;
    inline void (__fastcall* init)(cocos2d::CCNode*);
    void __fastcall initHook(cocos2d::CCNode* unknown) {
        init(unknown);

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
        
        cocos2d::CCArray* menus = ((cocos2d::CCNode*)unknown->getChildren()->objectAtIndex(0))->getChildren();

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

        //ButtonSprite_spr* testSpr = ButtonSprite_spr::create("Test", 0, 0, 0, "goldFont.fnt", "GJ_button_01.png", 0);

        ButtonSprite* mods = ButtonSprite::create(
            //ButtonSprite_mine::create("Mods", "GJ_button_01.png", "goldFont.fnt", 1.0f, { lw, bh }),
            ButtonSpriteSpr::create("Mods", (float)lw, true, "goldFont.fnt", "GJ_button_01.png", bh, 1.0),
            bmenu,
            (cocos2d::SEL_MenuHandler)&Callback::test
        );

        mods->setPosition(lpos, dis);

        bmenu->addChild(mods);

        // Note to self:
        // don't try to merge projects with uncommitted required headers on github

        /*

        ButtonSprite* test = ButtonSprite::create(
            ButtonSprite_mine::create("Textures", "GJ_button_01.png", "goldFont.fnt", 1.0f, { rw, bh }),
            bmenu,
            (cocos2d::SEL_MenuHandler)&Callback::mods
        );

        test->setPosition(rpos, dis);

        bmenu->addChild(test);

        //*/

        //MessageBoxA(nullptr, "wtf", "wtf", MB_OK);

        return;
    }

    void mem_init() {
        uintptr_t base = (uintptr_t)GetModuleHandleA(0);

        MH_CreateHook((PVOID)(base + 0x1DD420), (LPVOID)SettingsLayer::initHook, (LPVOID*)&SettingsLayer::init);
    }
}

void main_loop(HMODULE module) {
    std::string _inp;
    getline(std::cin, _inp);
    if (_inp == "e") {
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        FreeConsole();

        MH_DisableHook(MH_ALL_HOOKS);
        FreeLibraryAndExitThread(module, 0);
        exit(0);
    }
    SettingsLayer::test = std::stoi(_inp);
    
    main_loop(module);
}

namespace MenuLayer {
    inline bool (__thiscall* init)(cocos2d::CCLayer* self);
    bool __fastcall initHook(cocos2d::CCLayer* _self) {
        bool in = init(_self);

        cocos2d::CCMenu* menu = ((cocos2d::CCMenu*)_self->getChildren()->objectAtIndex(3));

        cocos2d::CCNode* statsBtn = (cocos2d::CCNode*)menu->getChildren()->objectAtIndex(2);
        cocos2d::CCNode* settingsBtn = (cocos2d::CCNode*)menu->getChildren()->objectAtIndex(1);

        cocos2d::CCMenuItem* chestBtn = (cocos2d::CCMenuItem*)menu->getChildren()->objectAtIndex(4);

        menu->removeChild(chestBtn, false);

        cocos2d::CCSprite* spr = cocos2d::CCSprite::create("DL_mods.png");
        ButtonSprite* btn = ButtonSprite::create(
            spr,
            menu,
            (cocos2d::SEL_MenuHandler)&Callback::mods
        );

        menu->addChild(btn);
	    menu->alignItemsHorizontallyWithPadding(5.0f);

        menu->addChild(chestBtn);

        cocos2d::CCSprite* sp = Input::bg_create({ 250, 80 });
        sp->setPosition({ 250, 250 });
        _self->addChild(sp);

        void* si = Input::create("cum", sp, "bigFont.fnt");

        return in;
    }

    void mem_init() {
        uintptr_t base = (uintptr_t)GetModuleHandleA(0);

        MH_CreateHook((PVOID)(base + 0x1907b0), (LPVOID)initHook, (LPVOID*)&init);
    }
}

namespace ModLdr {
    bool init() {
        // Initialize hooker
        MH_STATUS ini = MH_Initialize();
        if (ini != MH_OK)
            return false;

        // create console
        if (AllocConsole() == 0)
            return false;
        
        // redirect console output
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONERR$", "w", stderr);

        MenuLayer::mem_init();
        SettingsLayer::mem_init();
        HelpLayer::mem_init();
        fuck_it_i_give_up::mem_init();

        MH_EnableHook(MH_ALL_HOOKS);

        return true;
    }
}