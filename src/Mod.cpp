#include "ModManager.hpp"

ModLdr::Mod::Mod(std::string const& _name, std::string const& _path, bool const& _enabled) {
    this->name = _name;
    this->path = _path;

    this->loaded = false;
    this->enabled = _enabled;

    this->type = Type::DLL;
    this->loadtime = LoadTime::Undefined;
}

ModLdr::Mod::Type ModLdr::Mod::getType() {
    return this->type;
}

ModLdr::Mod::LoadTime ModLdr::Mod::wantsToLoadWhen() {
    return this->loadtime;
}

bool ModLdr::Mod::load() {
    if (!this->loaded) {
        auto l = LoadLibraryA(this->path.c_str());

        if (l) {
            this->loaded = true;
            this->enabled = true;
            this->module = l;

            return true;
        }

        return false;
    }
    return true;
}

bool ModLdr::Mod::unload() {
    if (this->type == Type::DLL)
        return false;
    
    return false;
}

void ModLdr::Mod::enable(bool _able) {
    // python looking ass
    if (_able)
        if (!this->loaded)
            this->load();
        else
            this->enabled = true;
    else
        if (this->loaded)
            this->unload();
        else
            this->enabled = false;
}

