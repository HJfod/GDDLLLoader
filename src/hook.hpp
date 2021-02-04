#include <stdio.h>
#define NOMINMAX
#include <Windows.h>
#include <tlhelp32.h>
#include "MinHook.h"
#include "../app.hpp"
#include <functional>
#include <limits>
#include <thread>
#include "offsets.hpp"

namespace gh {
    struct handle_data {
        unsigned long process_id;
        HWND window_handle;
    };

    BOOL is_main_window(HWND handle) {   
        return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
    }

    BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam) {
        handle_data& data = *(handle_data*)lParam;
        unsigned long process_id = 0;
        GetWindowThreadProcessId(handle, &process_id);
        if (data.process_id != process_id || !is_main_window(handle))
            return TRUE;
        data.window_handle = handle;
        return FALSE;   
    }

    HWND find_main_window(unsigned long process_id) {
        handle_data data;
        data.process_id = process_id;
        data.window_handle = 0;
        EnumWindows(enum_windows_callback, (LPARAM)&data);
        return data.window_handle;
    }

    namespace hooks {
        static constexpr const unsigned char H_KEYBOARD = 0b0001;
        static constexpr const unsigned char H_MOUSE = 0b0010;

        template <typename ret>
        ret ReadChain(std::vector<uintptr_t>_offs, ret _if_fucky = ret()) {
            uintptr_t read = *((uintptr_t*)_offs[0]);
            bool not_valid = false;

            for (int i = 1; i < _offs.size() - 1; i++)
                if (read == 0) {
                    not_valid = true; break;
                } else
                    read = *(uintptr_t*)(read + _offs[i]);
            
            if (!not_valid)
                return *(ret*)(read + _offs[_offs.size() - 1]);
            else return _if_fucky;
        }

        struct aKeyHook {
            int key;
            std::function<void (void)> func;

            aKeyHook(int _k, std::function<void (void)> _f) {
                this->key = _k;
                this->func = _f;
            }
        };

        struct aMouseHook {
            int button;
            std::function<void (LPMOUSEHOOKSTRUCT, bool*)> func;

            aMouseHook(int _k, std::function<void (LPMOUSEHOOKSTRUCT, bool*)> _f) {
                this->button = _k;
                this->func = _f;
            }
        };

        std::vector<aKeyHook*> keyhooks;
        std::vector<aMouseHook*> mousehooks;

        LRESULT WINAPI keyHookW(int msg, WPARAM wp, LPARAM lp) {
            for (auto k : keyhooks)
                if (GetAsyncKeyState(k->key))
                    k->func();

            return CallNextHookEx(NULL, msg, wp, lp);
        }

        LRESULT WINAPI mouseHook(int msg, WPARAM wp, LPARAM lp) {
            bool swallow = false;
            for (auto k : mousehooks)
                if (wp == k->button)
                    k->func((LPMOUSEHOOKSTRUCT)lp, &swallow);

            if (!swallow)
                return CallNextHookEx(NULL, msg, wp, lp);
            return 0;
        }

        bool InitHooker(HWND* wnd, unsigned char _flags = 0) {
            // Initialize hooker
            MH_STATUS ini = MH_Initialize();
            if (ini != MH_OK)
                return false;

            *wnd = find_main_window(_getpid());

            DWORD gdThId = (DWORD)GetWindowThreadProcessId(*wnd, NULL);

            if (_flags & H_KEYBOARD)
                SetWindowsHookExA(
                    WH_KEYBOARD,
                    keyHookW,
                    NULL,
                    gdThId
                );
            
            if (_flags & H_MOUSE)
                SetWindowsHookExA(
                    WH_MOUSE,
                    mouseHook,
                    NULL,
                    gdThId
                );

            return true;
        }

        bool AttachConsole() {
            // create console
            if (AllocConsole() == 0)
                return false;
            
            // redirect console output
            freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
            freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
            freopen_s(reinterpret_cast<FILE**>(stdout), "CONERR$", "w", stderr);

            // disable closing console from X button
            HWND hwnd = ::GetConsoleWindow();
            if (hwnd != NULL) {
                HMENU hMenu = ::GetSystemMenu(hwnd, FALSE);
                if (hMenu != NULL) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
            }

            return true;
        }

        void AddKeyHook(int nCode, std::function<void (void)> f) {
            keyhooks.push_back(new aKeyHook(nCode, f));
        }

        void AddMouseHook(int mCode, std::function<void (LPMOUSEHOOKSTRUCT, bool*)> f) {
            mousehooks.push_back(new aMouseHook(mCode, f));
        }
    }
}