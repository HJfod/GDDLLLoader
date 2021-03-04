#pragma once

#include "../offsets.hpp"

// thanks pie

class ButtonSprite : public cocos2d::CCMenuItem {
    public:
        inline static ButtonSprite* create(
            cocos2d::CCSprite* spr,
            cocos2d::CCMenu* targ,
            cocos2d::SEL_MenuHandler click
        ) {
            auto pRet = reinterpret_cast<ButtonSprite*(__thiscall*)(
                cocos2d::CCSprite*,
                cocos2d::CCMenu*,
                cocos2d::SEL_MenuHandler
            )>
                (ModLdr::offsets::ButtonCreate)
                (spr, targ, click);

            __asm add esp, 0x8
            return pRet;
        }
};

class ButtonSpriteSpr : public cocos2d::CCSprite {
	public:
		/*PARAMS:
		* caption - the button's caption.
		* width - the width of the button. only used if absolute is TRUE.
		* absolute - whether or not to use provided width. if FALSE, game will auto-adjust.
		* font - font file for caption.
		* texture - texture file for button background.
		* height - height of button. put 0 for auto.
		* scale - scale of the caption.
		*/
		inline static ButtonSpriteSpr* create(
            const char* caption,
            int width,
            bool absolute,
            const char* font,
            const char* texture,
            float height,
            float scale
        ) {
			//scale is passed in lower 4 bytes of xmm3
			__asm movss xmm3, scale

			//arg 3 is always 0. dunno why it's not optimized out as a param
			auto pRet = reinterpret_cast<ButtonSpriteSpr* (__fastcall*)(
                const char*,
                int,
                int,
                bool,
                const char*,
                const char*,
                float
            )>
                (ModLdr::offsets::ButtonSpriteCreate)
                (caption, width, 0, absolute, font, texture, height);
            
			//clean stack before returning
			__asm add esp, 0x14
			return pRet;
		}
};


