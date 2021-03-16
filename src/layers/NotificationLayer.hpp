#pragma once

#include "../offsets.hpp"
#include "../ModManager.hpp"

class NotificationCell : public gd::TableViewCell {
    ModLdr::Notification m_notif;

    public:
        NotificationCell(ModLdr::Notification, float);

        static NotificationCell* create(ModLdr::Notification, float);

        ModLdr::Notification getNotification();
};

class CustomListViewHook {
    public:
        static constexpr const int NotificationType = 0x600;

    protected:

        inline static cocos2d::CCNode* (__thiscall* getListCell)(gd::CustomListView*, const char*);
        inline static void (__thiscall* loadCell)(gd::CustomListView*, gd::TableViewCell*, int);
        //inline static void (__fastcall* setupList)(gd::CustomListView*);

        static cocos2d::CCNode* __fastcall getListCellHook(gd::CustomListView*, uintptr_t, const char*);
        static void __fastcall loadCellHook(gd::CustomListView*, uintptr_t, gd::TableViewCell*, int);
        //static void __fastcall setupListHook(gd::CustomListView*);

        static int getType(gd::CustomListView*);
    
    public:
        static bool load() {
            makeHook(
                gd::base + 0x585c0,
                CustomListViewHook::loadCellHook,
                &CustomListViewHook::loadCell
            );

            makeHook(
                gd::base + 0x58050,
                CustomListViewHook::getListCellHook,
                &CustomListViewHook::getListCell
            );

            //makeHook(
            //    gd::base + 0x58870,
            //    CustomListViewHook::setupListHook,
            //    &CustomListViewHook::setupList
            //);

            return true;
        };
};

class NotificationLayer : public gd::GJDropDownLayer {
    protected:
        virtual void customSetup();

    public:
        static NotificationLayer* create();
};
