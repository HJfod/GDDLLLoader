#include <cocos2d.h>

namespace textureldr {
    namespace gates {
		inline void(__thiscall* loadingFinished)(cocos2d::CCScene*);
		inline void(__thiscall* dataLoaded)(cocos2d::CCObject*, void*);
		inline void(__thiscall* trySaveGame)(cocos2d::CCObject*);
		inline void(__thiscall* addSearchPath)(cocos2d::CCFileUtils*, const char*);
	}

	namespace hooks {
		void __fastcall loadingFinished(cocos2d::CCScene* LoadingLayer);
		void __fastcall dataLoaded(cocos2d::CCObject* GameManager, void*, void* DS_Dictionary);
		void __fastcall trySaveGame(cocos2d::CCObject* AppDelegate);
		void __fastcall addSearchPath(cocos2d::CCFileUtils* _this, void*, const char* path);
	}

	void __fastcall enterScene(cocos2d::CCObject*);
	bool init();
}