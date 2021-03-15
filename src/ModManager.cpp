#include "ModManager.hpp"
#include <filesystem>
#include <fstream>

static bool saveFileText(std::string const& _path, std::string const& _cont) {
    std::ofstream file;
    file.open(_path);
    if (file.is_open()) {
        file << _cont;
        file.close();

        return true;
    }
    file.close();
    return false;
}

static ModLdr::Mod::LoadTime loadTimeForString(const std::string & _time) {
    if (!_time.size())
        return ModLdr::Mod::LoadTime::Undefined;

    switch (h$(_time.c_str())) {
        case h$("Splash"):      return ModLdr::Mod::LoadTime::Splash;
        case h$("Immediate"):   return ModLdr::Mod::LoadTime::Immediate;
        default:                return ModLdr::Mod::LoadTime::Undefined;
    }
}

void ModLdr::Manager::scheduleError(ModLdr::Notification _err) {
    _err.sender = "ModLdr";
    _err.time = std::chrono::system_clock::now();

    this->notifications_.push_back(_err);
    this->unreadNotifications_.push_back(_err);
}

void ModLdr::Manager::scheduleForSplash(ModLdr::Mod* _mod) {
    this->mods_.push_back(_mod);
}

ModLdr::Manager* ModLdr::Manager::initShared() {
    shared_ = new Manager(modFile, true);

    return shared_;
}

ModLdr::Manager* ModLdr::Manager::getShared() {
    return shared_;
}

ModLdr::Manager::Manager(const char* _file, bool _make_if_not) {
    if (std::filesystem::exists(_file) &&
        std::filesystem::is_regular_file(_file)
    ) {
        auto ds = new DS_Dictionary();

        if (ds->loadRootSubDictFromFile(_file)) {
            this->data_ = ds;

            // check version
            int ver = ds->getIntegerForKey("Version");

            if (!ver) {
                // if version is 0, aka has not been set
                // (0 is not an allowed version number)

                this->scheduleError({
                    Notification::Type::Notable, 
                    "ModLdr plist version not set, assuming latest (" + std::to_string(plistVersion) + ")"
                });

                ver = plistVersion;
            } else if (ver > plistVersion) {
                // if version is higher, it may have some
                // fuckerydoo future Fod came up with that
                // silly, naive past Fod can't support

                this->scheduleError({
                    Notification::Type::Fatal,
                    "ModLdr plist version is higher than supported (" + std::to_string(plistVersion) + ")"
                });  
            } else {
                if (ds->stepIntoSubDictWithKey("Mods"))
                    for (const auto modkey : ds->getAllKeys())
                        if (ds->stepIntoSubDictWithKey(modkey.c_str())) {
                            auto name = ds->getStringForKey("Name");
                            auto path = ds->getStringForKey("Path");
                            auto time = ds->getStringForKey("LoadTime");
                            auto able = ds->getBoolForKey("Enabled");

                            Mod::LoadTime t = loadTimeForString(time);

                            auto mod = new Mod(name, path, able);

                            if (t == Mod::LoadTime::Undefined)
                                t = mod->wantsToLoadWhen();

                            switch (t) {
                                case Mod::LoadTime::Splash:
                                    if (able)
                                        this->scheduleForSplash(mod);
                                    break;

                                case Mod::LoadTime::Immediate:
                                default:
                                    if (able)
                                        if (!mod->load())
                                            this->scheduleError({
                                                Notification::Type::Critical,
                                                "Unable to load " + name
                                            });
                                    break;
                            }

                            this->mods_.push_back(mod);

                            ds->stepOutOfSubDict();
                        } else {
                            this->scheduleError({
                                Notification::Type::Critical,
                                "Unable to read " + modkey + " subdict!"
                            });
                        }

                // if not, no mods have been installed :)
            }
        } else
            this->scheduleError({ Notification::Type::Fatal, "Unable to read ModLdr.plist!" });
    } else {
        if (_make_if_not) {
            auto ds = new DS_Dictionary();

            this->data_ = ds;

            saveFileText(_file,
            "<plist version=\"1.0\">\n"
            "    <dict>\n"
            "        <k>Version</k>\n"
            "        <i>" + std::to_string(plistVersion) + "</i>\n"
            "        <k>Mods</k>\n"
            "        <d/>\n"
            "    </dict>\n"
            "</plist>\n"
            );
        }
    }
}

ModLdr::Manager::~Manager() {
    for (auto mod : this->mods_) {
        mod->unload();
        delete mod;
    }
}

std::vector<ModLdr::Notification> ModLdr::Manager::getNotifications() {
    return this->notifications_;
}

unsigned int ModLdr::Manager::getUnreadNotificationCount() {
    return this->unreadNotifications_.size();
}

void ModLdr::Manager::markNotificationsAsRead() {
    this->unreadNotifications_.clear();
}

std::vector<ModLdr::Mod*> ModLdr::Manager::getScheduledForSplash() {
    return this->scheduledForSplashMods_;
}

ModLdr::Manager* ModLdr::Manager::shared_ = nullptr;
