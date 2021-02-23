#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <direct.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <conio.h>
#include <WtsApi32.h>
#include <ctype.h>
#include <regex>
#include <sstream>
#include <filesystem>

#define METH_COPY_FROM_DOESNT_EXIST 0
#define METH_SUCCESS 1

namespace methods {
    std::string replace(std::string const& original, std::string const& from, std::string const& to) {
        std::string results;
        std::string::const_iterator end = original.end();
        std::string::const_iterator current = original.begin();
        std::string::const_iterator next = std::search( current, end, from.begin(), from.end() );
        while ( next != end ) {
            results.append( current, next );
            results.append( to );
            current = next + from.size();
            next = std::search( current, end, from.begin(), from.end() );
        }
        results.append( current, next );
        return results;
    }

    std::string lower(std::string const& _s) {
        std::string s = _s;
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c){ return std::tolower(c); });
        return s;
    }

    std::string workdir() {
        char buff[FILENAME_MAX];
        _getcwd(buff, FILENAME_MAX);
        std::string current_working_dir(buff);
        return current_working_dir;
    }

    void fsave (std::string _path, std::string _cont) {
        std::ofstream file;
        file.open(_path);
        file << _cont;
        file.close();
    }

    void fsaveb (std::string _path, const char* _bytes) {
        std::ofstream file;
        file.open(_path, std::ios::binary);
        file << _bytes;
        file.close();
    }

    std::string fread (std::string _path) {
        std::ifstream in(_path, std::ios::in | std::ios::binary);
        if (in) {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize((size_t)in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return(contents);
        } throw(errno);
    }

    int fcopy(std::string from, std::string to) {
        if (!std::filesystem::exists(from))
            return METH_COPY_FROM_DOESNT_EXIST;
        std::ifstream src(from, std::ios::binary);
        std::ofstream dst(to,   std::ios::binary);
        dst << src.rdbuf();

        return METH_SUCCESS;
    }

    bool ewith (std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }

    std::string sanitize (std::string _str) {
        std::string ret = _str;
        while (ret._Starts_with("\""))
            ret = ret.substr(1);
        while (ewith(ret, "\""))
            ret = ret.substr(0, ret.length() - 1);
        return ret;
    }

    std::vector<std::string> split (std::string _str, std::string _split) {
        size_t pos = 0;
        std::string token;
        std::vector<std::string> res = {};
        while ((pos = _str.find(_split)) != std::string::npos) {
            token = _str.substr(0, pos);
            res.push_back(token);
            _str.erase(0, pos + _split.length());
        }
        res.push_back(_str);
        return res;
    }

    std::wstring conv (std::string _str) {
        return std::wstring(_str.begin(), _str.end());
    }

    int count (std::string _s, char _c) {
        int count = 0;
        for (unsigned int i = 0; i < _s.size(); i++)
            if (_s[i] == _c) count++;
        return count;
    }

    std::string remove(std::string _str, std::string _sub, bool all = false) {
        if (all) {
            int s;
            while ((s = _str.find(_sub)) != std::string::npos)
                _str = _str.erase(s, _sub.length());
            return _str;
        }
        int s = _str.find(_sub);
        if (s != std::string::npos)
            return _str.erase(s, _sub.length());
        else return _str;
    }

    char* stc (std::string _str) {
        int sz = _str.size() + 1;
        char* cstr = new char[sz];
        int err = strcpy_s(cstr, sz, _str.c_str());
        return cstr;
    }

    bool proc_running(const char* _proc, DWORD* _pid = NULL) {
        WTS_PROCESS_INFO* pWPIs = NULL;
        DWORD dwProcCount = 0;
        bool found = false;
        if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
            for (DWORD i = 0; i < dwProcCount; i++)
                if (strcmp((LPSTR)pWPIs[i].pProcessName, _proc) == 0) {
                    found = true;
                    if (_pid != NULL)
                        *_pid = pWPIs[i].ProcessId;
                }

        if (pWPIs) {
            WTSFreeMemory(pWPIs);
            pWPIs = NULL;
        }

        return found;
    }

    std::string unspace(std::string _str) {
        return std::regex_replace(_str, std::regex("^ +| +$|( ) +"), "$1");
    }

    std::string hexstr(int _i) {
        std::stringstream ss;
        ss << std::hex << _i << std::dec;
        return ss.str();
    }

    namespace perf {
        std::chrono::time_point<std::chrono::high_resolution_clock> perf_;

        void start() {
            perf_ = std::chrono::high_resolution_clock().now();
        }

        int end() {
            auto end_ = std::chrono::high_resolution_clock().now();
            return (int)std::chrono::duration_cast<std::chrono::milliseconds>(end_ - perf_).count();
        }

        void log(std::string _text) {
            std::cout << _text << "\t " << end() << "ms" << std::endl;
        }
    }
}
