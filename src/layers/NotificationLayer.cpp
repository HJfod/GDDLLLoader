#include "NotificationLayer.hpp"
#include "../ModManager.hpp"

int CustomListViewHook::getType(gd::CustomListView* _list) {
    return *as<int*>(
        as<uintptr_t>(_list) + 300
    );
}

void __fastcall CustomListViewHook::loadCellHook(
    gd::CustomListView* _this,
    uintptr_t,
    gd::TableViewCell* _node,
    int _ix
) {
    switch (getType(_this)) {
        case NotificationType: {
            std::cout << "fuck " << _node->getChildrenCount() << "\n";

            auto n = dynamic_cast<NotificationCell*>(_node);

            std::cout << n << "\n";
        } break;
        default:
            loadCell(_this, _node, _ix);
    }
}

// void __fastcall CustomListViewHook::setupListHook(gd::CustomListView* _this) {
    // setupList(_this);
// };

cocos2d::CCNode* __fastcall CustomListViewHook::getListCellHook(
    gd::CustomListView* _this,
    uintptr_t,
    const char* _name
) {
    std::cout << "getListCell " << _name << "\n";
    std::cout << _this->getListWidth() << "\n";

    switch (getType(_this)) {
        case NotificationType: {
            std::cout << "NotificationType\n";

            return new NotificationCell(
                {
                    ModLdr::Notification::Info,
                    _name
                },
                _this->getListWidth()
            );
        } break;

        default:
            return getListCell(_this, _name);
    }
}

NotificationCell::NotificationCell(ModLdr::Notification _notif, float _width)
    : gd::TableViewCell(_notif.info.c_str(), _width, 69.0f)
{
    this->m_notif = _notif;
}

NotificationCell* NotificationCell::create(ModLdr::Notification _notif, float _width) {
    auto pRet = new NotificationCell(_notif, _width);

    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

ModLdr::Notification NotificationCell::getNotification() {
    return this->m_notif;
}


void NotificationLayer::customSetup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto lrSize  = this->m_pListLayer->getScaledContentSize();

    auto notifs = ModLdr::Manager::getShared()->getNotifications();

    if (notifs.size()) {
        auto a = new cocos2d::CCArray();

        a->addObject(NotificationCell::create({
            ModLdr::Notification::Fatal,
            "This is a test error",
            "ModLdr"
        }, lrSize.width));

        a->addObject(NotificationCell::create({
            ModLdr::Notification::Trivial,
            "This is a test notification",
            "eat my shit"
        }, lrSize.width));

        a->addObject(NotificationCell::create({
            ModLdr::Notification::Info,
            "Cummy wummy!",
            "dick bro"
        }, lrSize.width));

        auto listview = gd::CustomListView::create(
            a,
            lrSize.width,
            lrSize.height,
            CustomListViewHook::NotificationType
        );
    } else {
        // make this use TextArea when you figure it out
        
        auto noLabel = cocos2d::CCLabelBMFont::create(
            "You have no\nnotifications!", "bigFont.fnt"
        );

        noLabel->setScale(.7f);
        noLabel->setPosition(lrSize / 2);

        this->m_pListLayer->addChild(noLabel);
    }
}

NotificationLayer* NotificationLayer::create() {
    auto pRet = new NotificationLayer();

    if (pRet && pRet->init("Notifications", 220.0f)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}
