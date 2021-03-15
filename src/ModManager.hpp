#pragma once

#include "offsets.hpp"
#include <string>
#include <vector>

namespace ModLdr {
    class Mod {
        public:
            enum Type {
                DLL,
                ModLdr
            };

            enum LoadTime {
                Immediate,
                Splash,
                Undefined
            };

        protected:
            std::string path;
            std::string name;

            bool loaded;
            bool enabled;

            Type type;
            LoadTime loadtime;

            HMODULE module;

        public:
            Mod(std::string const&, std::string const&, bool const&);

            bool load();
            bool unload();
            void enable(bool);

            Type getType();
            LoadTime wantsToLoadWhen();
    };

    struct Notification {
        enum Type {
            // for errors that break the whole modloader
            Fatal,

            // for errors that break individual mods
            Critical,

            // for errors that could pose an issue
            Notable,

            // for errors that likely don't pose an issue
            Trivial,

            // for information
            Info
        };

        Type type;
        std::string info;
        std::string sender;
        std::chrono::time_point<std::chrono::system_clock> time;

        inline Notification() {
            this->type = Type::Info;
            this->info = "";
            this->sender = "ModLdr";
            this->time = std::chrono::system_clock::now();
        }

        inline Notification(Type _type, std::string _info) {
            this->type = _type;
            this->info = _info;
            this->sender = "ModLdr";
            this->time = std::chrono::system_clock::now();
        };

        inline Notification(Type _type, std::string _info, std::string _send) {
            this->type = _type;
            this->info = _info;
            this->sender = _send;
            this->time = std::chrono::system_clock::now();
        }
    };

    class Manager {
        static constexpr const char* modFolder = "mods";
        static constexpr const char* modFile = "ModLdr.plist";

        static Manager* shared_;
        DS_Dictionary* data_;

        std::vector<Notification> notifications_;
        std::vector<Notification> unreadNotifications_;
        std::vector<Mod*> scheduledForSplashMods_;
        std::vector<Mod*> mods_;

        void scheduleError(Notification);
        void scheduleForSplash(Mod*);
        
        public:
            Manager(const char*, bool);
            ~Manager();

            static Manager* initShared();
            static Manager* getShared();

            std::vector<Notification> getNotifications();
            unsigned int getUnreadNotificationCount();
            void markNotificationsAsRead();

            std::vector<Mod*> getScheduledForSplash();
    };
}

