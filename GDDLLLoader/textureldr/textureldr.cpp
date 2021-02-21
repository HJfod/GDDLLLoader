#include "textureldr.hpp"
#include "list.hpp"
#include <filesystem>
#include "ButtonSprite.hpp"

namespace textureldr {
    void __fastcall exitScene(cocos2d::CCObject*);
    void __fastcall apply(cocos2d::CCObject*);

    namespace vars {
        list quality = { "Quality", 1 };

        extern listExt applied;
        listExt all = { "Available", 10, &applied };
        listExt applied = { "Applied", 10, &all };
    }

    namespace hooks {
        void __fastcall loadingFinished(cocos2d::CCScene* LoadingLayer) {
            *reinterpret_cast<bool*>(reinterpret_cast<char*>(LoadingLayer) + 0x138) = false;
            listManager::exit();
            return gates::loadingFinished(LoadingLayer);
        }

        void __fastcall dataLoaded(cocos2d::CCObject* GameManager, void*, void* DS_Dictionary) {
            using namespace cocos2d;
            using namespace vars;

            auto fileUtils = CCFileUtils::sharedFileUtils();
            //remove all search paths that have been added.
            fileUtils->removeAllPaths();
            gates::addSearchPath(fileUtils, "Resources");
            //call dataLoaded.
            gates::dataLoaded(GameManager, DS_Dictionary);
            //add all of 'em back lol
            fileUtils->removeAllPaths();
            for (std::string i : applied.getVector()) {
                gates::addSearchPath(fileUtils, ("packs/" + i).c_str());
            }
            gates::addSearchPath(fileUtils, "Resources");
        }

        void __fastcall trySaveGame(cocos2d::CCObject* AppDelegate) {
            listManager::save();
            return gates::trySaveGame(AppDelegate);
        }

        void __fastcall addSearchPath(cocos2d::CCFileUtils* _this, void*, const char* path) {
            using namespace vars;

            //to refresh menuLoop.mp3.
            gd::MenuLayer::fadeInMusic(" ");
            for (std::string i : applied.getVector()) {
                gates::addSearchPath(_this, ("packs/" + i).c_str());
            }
            return gates::addSearchPath(_this, path);
        }
    }

    void __fastcall getPacks(cocos2d::CCObject* pSender) {
        using namespace std::filesystem;
        using namespace cocos2d;
        using namespace vars;

        std::vector<std::string> packsList = {};

        /*all characters allowed by stock bigFont.fnt and goldFont.fnt.
        * excludes '\', '/', ':', '*', '?', '"', '<', '>', and '|' since windows
        * folders can't have those symbols, and '•' since it acts weird and i can't
        * be bothered lol
        */
        constexpr std::string_view filter = " !#$%&'()+,-.0123456789;=@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{}~";
        int invalid = 0;

        log::info("Searching for packs...");
        path packs = current_path() / "packs";
        if (exists(packs)) {
            if (is_directory(packs)) {
                log::info("Packs directory found. Iterating...");
                for (directory_entry pack : directory_iterator{ packs }) {
                    if (is_directory(pack)) {
                        bool valid = true;
                        for (auto c : pack.path().filename().u8string()) {
                            if (filter.find(c) == filter.npos) {
                                valid = false;
                                break;
                            }
                        }

                        if (valid)
                            packsList.push_back(pack.path().filename().string());
                        else
                            ++invalid;
                    }
                }

                log::info(packsList.size(), " packs found.");
            }
            else
                MessageBoxA(0, "ERROR: packs is an existing file.\n please remove it to use textureldr.", "textureldr", MB_OK | MB_ICONERROR);
        }
        else {
            //log::error("No packs directory found. Creating new directory...");
            create_directories(packs);
        }
            
        int added = 0;
        int removed = 0;

        if (!(all.getVector().empty() && applied.getVector().empty())) {
            added = all.ifNotFound(packsList, true);
            removed = all.ifNotFound(packsList, false);
        }
        else {
            all.setVector(packsList);
            added = packsList.size();
        }

        if (pSender) {
            auto director = CCDirector::sharedDirector();
            auto winSize = director->getWinSize();

            std::stringstream text = {};
            if (added > 0 || removed > 0) {
                text << added << ' ' << ((added == 1) ? "pack" : "packs") << " added.\n";
                text << removed << ' ' << ((removed == 1) ? "pack" : "packs") << " removed.\n";
            }
            else {
                text << "Nothing changed!\n";
            }

            if (invalid > 0) {
                text << '\n';
                if (invalid == 1) {
                    text << "1 pack had an invalid name, and was ignored.\n";
                }
                else {
                    text << invalid << " packs had invalid names, and were ignored.\n";
                }
            }

            auto label = CCLabelBMFont::create(text.str().c_str(), "goldFont.fnt");
            label->setScale(0.7f);
            label->setAlignment(kCCTextAlignmentCenter);
            label->setPosition(winSize.width / 2, winSize.height / 2 + 50.0f);
            director->getRunningScene()->addChild(label);

            //workaround; idfk how va_arg functions work when compiled
            auto arr = CCArray::create();
            arr->addObject(CCFadeIn::create(0.5f));
            arr->addObject(CCDelayTime::create(1.0f));
            arr->addObject(CCFadeOut::create(0.5f));
            arr->addObject(CCCallFunc::create(
                label,
                //this is a removeMeAndCleanup callback that rob setup so imma use it
                reinterpret_cast<void(__thiscall*)()>(gd::base + 0x10A3A)
            ));
            //END workaround

            //change this to use CCSequence::create(CCObject*, ...) plz
            label->runAction(CCSequence::create(arr));
        }
    }

	void __fastcall enterScene(cocos2d::CCObject* pSender) {
        using namespace gd;
        using namespace cocos2d;

        //create scene here
        auto director = CCDirector::sharedDirector();
        CCSize winSize = director->getWinSize();
        auto ldrScene = CCScene::create();

        auto bg = CCSprite::create("GJ_gradientBG.png");
        bg->setAnchorPoint({ 0.0f, 0.0f });
        ldrScene->addChild(bg);
        bg->setScaleX(36.267f);
        bg->setScaleY(2.063f);
        bg->setPosition({ -5.0f, -5.0f });
        //bg->setColor({ 0, 102, 255 });
        
        listManager::enter(ldrScene);

        auto miscBtns = CCMenu::create();
        ldrScene->addChild(miscBtns);
        ButtonSprite* applyBtn = ButtonSprite::create(
            CCSprite::create("GJ_button_04.png"),
            miscBtns,
            apply
        );
        miscBtns->addChild(applyBtn);
        miscBtns->addChild(CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"));
        
        auto backBtn = ButtonSprite::create(
            CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
            miscBtns,
            exitScene
        );
        backBtn->setPosition(-winSize.width / 2 + 25.0f, winSize.height / 2 - 25.0f);
        miscBtns->addChild(backBtn);

        auto reloadBtn = ButtonSprite::create(
            CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
            miscBtns,
            getPacks
        );
        reloadBtn->setPosition(winSize.width / 2 - 35.0f, -winSize.height / 2 + 35.0f);
        reloadBtn->setScale(1.1f);
        miscBtns->addChild(reloadBtn);

        director->replaceScene(CCTransitionFade::create(0.5f, ldrScene));
	}

	void __fastcall exitScene(cocos2d::CCObject* pSender) {
        using namespace cocos2d;
        using namespace gd;
        using namespace vars;

        listManager::exit();

        auto menuLayer = MenuLayer::create();

        auto menuLayerScene = CCScene::create();
        menuLayerScene->addChild(menuLayer);

        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, menuLayerScene));
	}

    void __fastcall apply (cocos2d::CCObject* pSender) {
        using namespace cocos2d;
        using namespace vars;

        auto director = CCDirector::sharedDirector();
        director->updateContentScale(static_cast<TextureQuality>(quality.getCurrentIndex() + 1));
        
        auto GameManager = gd::GameManager::sharedState();
        *reinterpret_cast<int*>
            (reinterpret_cast<char*>(GameManager) + 0x2E4) = quality.getCurrentIndex() + 1;

        GameManager->reloadAll(false, false, true);
    }

    bool init() {
        using namespace vars;
        
        listManager::setSaveTargets("packs/config.dat", "packs/backup.dat");

        if (!listManager::load()) {
            getPacks(nullptr);
            quality.setVector({ "Low", "Medium", "High" });
            //set to high for convenience sake. TODO: maybe autodetect, or use gd's autodetect?
            quality.setOffset(2);
        }
        quality.setPosition(0.0f, -130.0f);
        all.setPosition(-120.0f, 95.0f);
        applied.setPosition(120.0f, 95.0f);

        return true;
    }
}