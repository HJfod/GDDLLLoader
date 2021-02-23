#pragma once

#include "../offsets.hpp"

#define PAD(size) char pad[size] = {};

// thanks pie <3

class CustomListView;

class GJDropDownLayer : public cocos2d::CCLayerColor {
	protected:
		cocos2d::CCPoint m_obEndPosition;
		cocos2d::CCPoint m_obStartPosition;
		cocos2d::CCMenu* m_pButtonMenu;
		CustomListView* m_pListLayer;
		bool m_bControllerEnabled; //?
		cocos2d::CCLayer* m_pLayer;
		bool m_bHidden; //?
		PAD(4);

	public:
		virtual void customSetup() {};
		virtual void enterLayer() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x16970
			)(this);
		}
		virtual void exitLayer() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x113980
			)(this);
		}
		virtual void showLayer(bool noTransition) {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				ModLdr::offsets::DropDownLayerShow
			)(this, noTransition);
		}
		virtual void hideLayer(bool noTransition) {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				ModLdr::offsets::DropDownLayerHide
			)(this, noTransition);
		}
		virtual void layerVisible() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x16A40
			)(this);
		}
		virtual void layerHidden() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x113860
			)(this);
		}
		virtual void enterAnimFinished() {}
		virtual void disableUI() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x113920
			)(this);
		}
		virtual void enableUI() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x113940
			)(this);
		}

		static GJDropDownLayer* create(const char* title) {
			GJDropDownLayer* pRet = new GJDropDownLayer();
			if (pRet) {
				pRet->constructor();
				if (pRet->init(title))
					pRet->autorelease();
				//no idea if this works correctly, destructor calls vtable function with stack cookie or something
				else pRet->destructor();
			}
			return pRet;
		}
		void constructor() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x038470
			)(this);
		}
		void destructor() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				ModLdr::base + 0x038560
			)(this, true);
		}
		bool init(const char* title) {
			// same size as settingslayer
			float x = 220.0;

			// set height (it's stored in xmm2)
			__asm { movss xmm2, [x] }

			return reinterpret_cast<bool(__thiscall*)(GJDropDownLayer*, const char*)>(
				ModLdr::offsets::DropDownLayerInit
			)(this, title);
		}
		void showLayerB() {
			return this->showLayer(false);
		}
};

