#include "ModInfoLayer.hpp"
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "../gd/ButtonSprite.hpp"

bool ModLdr::ModInfoLayer::init(Manager::Mod*) {
	auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

	if (!this->initWithColor({ 0, 0, 0, 75 })) return false;
	this->m_pLayer = cocos2d::CCLayer::create();
	this->addChild(this->m_pLayer);

	auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
	bg->setContentSize({ 360.0f, 260.0f });
	bg->setPosition(winSize.width / 2, winSize.height / 2);
	this->m_pLayer->addChild(bg);

    this->m_pButtonMenu = cocos2d::CCMenu::create();
    this->m_pLayer->addChild(this->m_pButtonMenu);

	auto closeBtn = CCMenuItemSpriteExtraGD::create(
		cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
		this->m_pButtonMenu,
		(cocos2d::SEL_MenuHandler)&ModInfoLayer::onClose
	);

	this->m_pButtonMenu->addChild(closeBtn);

	closeBtn->setPosition(
		closeBtn->convertToNodeSpace({ (winSize.width / 2) - 198.0f, (winSize.height / 2) + 102.0f })
	);
    closeBtn->setSizeMult(1.5f);

	this->setKeypadEnabled(true);
	this->setTouchEnabled(true);
	return true;
}

void ModLdr::ModInfoLayer::onClose(cocos2d::CCObject*) {
	this->setKeypadEnabled(false);
	this->removeFromParentAndCleanup(true);
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

