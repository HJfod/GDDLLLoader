#include "ModInfoLayer.hpp"
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "../gd/ButtonSprite.hpp"
#include "../gd/CCMenuItemToggler.hpp"
#include "ModLayer.hpp"

bool ModLdr::ModInfoLayer::init(Manager::Mod* _mod) {
	auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
	auto lrSize  = cocos2d::CCSize { 320.0f, 220.0f };

	if (!this->initWithColor({ 0, 0, 0, 75 })) return false;
	this->m_pLayer = cocos2d::CCLayer::create();
	this->addChild(this->m_pLayer);


	auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
	bg->setContentSize(lrSize);
	bg->setPosition(winSize.width / 2, winSize.height / 2);
	this->m_pLayer->addChild(bg);

    this->m_pButtonMenu = cocos2d::CCMenu::create();
    this->m_pLayer->addChild(this->m_pButtonMenu);


	auto title = cocos2d::CCLabelBMFont::create(_mod->name.c_str(), "goldFont.fnt");

	title->setPosition(lrSize.width / 2, lrSize.height - 20);
	title->setScale(.75f);

	bg->addChild(title);


	auto pathTitle = cocos2d::CCLabelBMFont::create("Path:", "bigFont.fnt");
	pathTitle->setPosition(lrSize.width / 2, lrSize.height / 2 + 50);
	pathTitle->setScale(.5);

	auto pathBG = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
	pathBG->setPosition(lrSize.width / 2, lrSize.height / 2 + 20);
	pathBG->setColor({ 0, 0, 0 });
	pathBG->setOpacity(75);
	pathBG->setContentSize({ lrSize.width * 2 - 60, 60 });

	// for some reason the texture is fucked if it's at normal scale
	pathBG->setScale(.5);

	auto pathText = cocos2d::CCLabelBMFont::create(_mod->path.c_str(), "chatFont.fnt");

	pathText->setPosition(
		pathBG->getContentSize() / 2
	);
	pathText->setColor({ 156, 221, 132 });
	pathText->setScale(2);

	pathBG->addChild(pathText);

	bg->addChild(pathBG);
	bg->addChild(pathTitle);


	auto enableToggle = CCMenuItemToggler::createWithScale(
		this->m_pButtonMenu,
		(cocos2d::SEL_MenuHandler)&ModLayer::toggleMod,
		.7f
	);
	enableToggle->toggle(_mod->enabled);
	enableToggle->setPosition(-45, -20);
	enableToggle->setUserData(reinterpret_cast<void*>(_mod));

	this->m_pButtonMenu->addChild(enableToggle);


	auto enableText = cocos2d::CCLabelBMFont::create("Enabled", "bigFont.fnt");

	enableText->setPosition(15, -18);
	enableText->setScale(.6f);

	this->m_pButtonMenu->addChild(enableText);


	auto closeBtn = CCMenuItemSpriteExtraGD::create(
		cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
		this->m_pButtonMenu,
		(cocos2d::SEL_MenuHandler)&ModInfoLayer::onClose
	);
	closeBtn->setUserData(reinterpret_cast<void*>(this));

	this->m_pButtonMenu->addChild(closeBtn);

	closeBtn->setPosition(
		- lrSize.width / 2,
		lrSize.height / 2
	);

	this->setKeypadEnabled(true);
	this->setTouchEnabled(true);

	return true;
}

void ModLdr::ModInfoLayer::onClose(cocos2d::CCObject* pSender) {
	auto layer = reinterpret_cast<ModInfoLayer*>(
		reinterpret_cast<cocos2d::CCNode*>(pSender)->getUserData()
	);

	layer->setKeypadEnabled(false);
	layer->removeFromParentAndCleanup(true);
}

ModLdr::ModInfoLayer* ModLdr::ModInfoLayer::create(Manager::Mod* _mod) {
	ModInfoLayer* pRet = new ModInfoLayer();

	if (pRet && pRet->init(_mod)) {
		pRet->autorelease();
		return pRet;
	}

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

