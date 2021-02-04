#include "hook.hpp"
#include "offsets.hpp"

namespace gd::editor {
    uintptr_t GetEditorUI() {
        uintptr_t gm = *reinterpret_cast<uintptr_t*>(offsets::gamemanager);
        uintptr_t gmm = *reinterpret_cast<uintptr_t*>(gm + 0x168);

        if (!gmm) return 0;

        uintptr_t gmmm = *reinterpret_cast<uintptr_t*>(gmm + 0x380);

        return gmmm;
    }

    std::string getObjectString(void* _obj) {
        std::string str;
        gd::functions::GetObjectString(_obj, &str, &str);
        return str;
    }

    namespace ObjKey {
        static constexpr unsigned int xPos = 0x84;
        static constexpr unsigned int yPos = 0x88;
        static constexpr unsigned int Selected = 0x3DA;
    };

    template <typename T>
    std::vector<void*> findObjs(unsigned int _key, T _val) {
        int count = gh::hooks::ReadChain<int>({ offsets::gamemanager, 0x168, 0x144, 0xB4, 0xB4, 0x3A0 }, 0);
        std::vector<void*> objs = {};

        for (int i = 0; i < count; i++) {
            void* obj = gh::hooks::ReadChain<void*>({ offsets::gamemanager, 0x168, 0x234, 0x20, 0x8, static_cast<uintptr_t>(0x4 * i) });

            if (*(T*)((intptr_t)obj + _key) == _val)
                objs.push_back(obj);
        }

        return objs;
    }
}