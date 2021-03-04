#pragma once

#include "../offsets.hpp"
#include "../gd/FLAlertLayer.hpp"
#include "../ModManager.hpp"

namespace ModLdr {
    class ModInfoLayer : public FLAlertLayer {
        protected:
            virtual bool init(Manager::Mod*);

            void onClose(cocos2d::CCObject*);

            std::function<void()> callback_;
            std::function<void(bool)> checkbox_cb_;

        public:
            static ModInfoLayer* create(
                Manager::Mod*,
                std::function<void()> = nullptr,
                std::function<void(bool)> = nullptr
            );
    };
}

