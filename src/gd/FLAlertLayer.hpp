#pragma once

#include "../offsets.hpp"

// thanks pie & cos8o

#pragma runtime_checks("s", off)
class FLAlertLayer : public cocos2d::CCLayer {
	public:
		/*FLAlertLayer supports colors of text for the caption. wrap desired text in "<cx></c>"
		* where x is the color desired. colors are:
		* r - red
		* l - lime
		* g - green
		* y - yellow
		* o - orange?
		* and more that i'm too lazy to find.
		*/
		static FLAlertLayer* create(
            cocos2d::CCObject* target,
            const char* title,
			const char* btn1,
            const char* btn2,
            float layerh,
            intptr_t l1,
            intptr_t l2,
            std::string caption
        ) {
			auto pRet = reinterpret_cast<FLAlertLayer* (__fastcall*)(
                cocos2d::CCObject*,
                const char*,
                const char*,
                const char*,
                float,
                intptr_t,
                intptr_t,
                std::string
            )>
                (ModLdr::offsets::FLAlertLayerCreate2)
                (target, title, btn1, btn2, layerh, l1, l2, caption);

			//clean stack
			__asm add esp, 0x2C

            return pRet;
		}

		void show() {
			return reinterpret_cast<void(__thiscall*)(FLAlertLayer*)>(
				ModLdr::offsets::FLAlertLayerShow
				)(this);
		}
};
#pragma runtime_checks("s", restore)

