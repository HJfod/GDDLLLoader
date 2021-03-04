#pragma once

#include "../offsets.hpp"
#include "../gd/FLAlertLayer.hpp"
#include "../ModManager.hpp"

namespace ModLdr {
    class ModInfoLayer : public FLAlertLayer {
        protected:
            virtual bool init(Manager::Mod*);

            void onClose(cocos2d::CCObject*);

        public:
            static ModInfoLayer* create(Manager::Mod*);
    };
}

