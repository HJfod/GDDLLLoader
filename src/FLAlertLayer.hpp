#include "GJGameLevel.hpp"

class FLAlertLayer;

class FLAlertLayerProtocol {
    public:
        virtual void FLAlert_Clicked( FLAlertLayer* layer, bool clicked );
    };

        // FLAlertLayer.h
class FLAlertLayer : public cocos2d::CCLayerColor {
    public:
        cocos2d::CCMenu* btnMenu_; // 0x198
        int controllersConnected_; // 0x19C
        FLAlertLayerProtocol* PParent_; // 0x1A0
        cocos2d::CCScene* targetScene_; // 0x1A4
        bool reverseKeyBack_; // 0x1A8
        uint8_t _pad1[ 0x3 ]; // 0x1A9
        cocos2d::CCLayer* internalLayer_; // 0x1AC
        int ZValue_; // 0x1B0
        bool showInstant_; // 0x1B4
        uint8_t _pad2[ 0x3 ]; // 0x1B5
        void* btn1_; // 0x1B8       ButtonSprite
        void* btn2_; // 0x1BC       ButtonSprite
        void* scrollLayer_; // 0x1C0        ScrollLayer
        uint32_t _pad3; // 0x1C4
        bool containsActions_; // 0x1C8 ?
        bool noBtnAction_; // 0x1C9

    public:
        FLAlertLayer( );
        virtual ~FLAlertLayer( );

        static FLAlertLayer* (__fastcall * create)( FLAlertLayerProtocol* parent, const char* title, std::string caption, const char* btn1, const char* btn2 );

        virtual void onEnter( );
        virtual void registerWithTouchDispatcher( );
        virtual void show( ); // 0x1FC
        // ...
};