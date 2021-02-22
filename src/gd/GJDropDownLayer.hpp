#pragma once

#pragma warning( push, 0 )
#include <cocos2d.h>
#pragma warning( pop )

#include "../offsets.hpp"

class GJDropDownLayer : public cocos2d::CCLayer {
	public:
		static GJDropDownLayer* create(const char* title) {
			GJDropDownLayer* pRet = reinterpret_cast<decltype(pRet)>(new char[0x1C0]());
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
			return reinterpret_cast<bool(__thiscall*)(GJDropDownLayer*, const char*)>(
				ModLdr::offsets::DropDownLayerInit
			)(this, title);
		}
		void showLayer() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				ModLdr::offsets::DropDownLayerShow
			)(this, false);
		}
		void hideLayer() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				ModLdr::offsets::DropDownLayerHide
			)(this, false);
		}
};

