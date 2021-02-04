#include <string>
#include <ShlObj.h>
#include <windows.h>
#include <vector>
#include <fstream>
#include <regex>
#include <algorithm>
#include <stdio.h>
#include <chrono>
#include <commdlg.h>
#include <iostream>
#include "methods.hpp"
#include "../ext/ZlibHelper.hpp"
#include "../ext/Base64.hpp"
#include "../ext/dirent.h"
#include "../ext/json.hpp"

namespace user {
    nlohmann::json userdata;

    bool LoadUserdata() {
        std::string path = "user.gdt";
        int max_tries = 5;

        while (!methods::fexists(path))
            if (max_tries <= 0)
                return false;
            else {
                path = "..\\" + path;
                max_tries--;
            }
        
        std::string data = methods::fread(path);

        if (data._Starts_with("::GDTools User::"))
            userdata = nlohmann::json::parse(
                methods::fread(
                    path.substr(
                        path.find("{")
                    )
                )
            );
        return true;
    }
}

namespace gd {
    enum err {
        SUCCESS,
        FILE_DOESNT_EXIST,
        LEVEL_NOT_FOUND,
        FILE_DIALOG,
        UNSUPPORTED_COMPRESSION,
        CANT_CREATE_ZIP,
        LEVEL_SOMEHOW_DOESNT_HAVE_A_SONG_WTF,
        GD_EXE_PATH_NOT_SET
    };
    
    const char* GetErrorText(int _err) {
        switch (_err) {
        case SUCCESS:
            return "SUCCESS";
        case FILE_DOESNT_EXIST:
            return "FILE_DOESNT_EXIST";
        case LEVEL_NOT_FOUND:
            return "LEVEL_NOT_FOUND";
        case FILE_DIALOG:
            return "FILE_DIALOG";
        case UNSUPPORTED_COMPRESSION:
            return "UNSUPPORTED_COMPRESSION";
        case CANT_CREATE_ZIP:
            return "CANT_CREATE_ZIP";
        case LEVEL_SOMEHOW_DOESNT_HAVE_A_SONG_WTF:
            return "LEVEL_SOMEHOW_DOESNT_HAVE_A_SONG_WTF";
        case GD_EXE_PATH_NOT_SET:
            return "GD_EXE_PATH_NOT_SET";
        default:
            return "UNKNOWN";
        }
    }

    std::string export_dir = "gdshare_exports";

    namespace decode {
        rapidxml::xml_document<> decoded_data;
        std::vector<rapidxml::xml_node<>*> levels;

        std::string GetCCFolder() {
            wchar_t* localAppData = 0;
            SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppData);

            std::wstring CCW (localAppData);

            std::string RESULT ( CCW.begin(), CCW.end() );
            RESULT += "\\GeometryDash";

            CoTaskMemFree(static_cast<void*>(localAppData));
            
            return RESULT;
        }

        std::string GetCCPath(std::string WHICH) {
            return GetCCFolder() + "\\CC" + WHICH + ".dat";
        }

        std::vector<uint8_t> readf(std::string const& path) {
            std::vector<uint8_t> buffer;
            std::ifstream file(path, std::ios::ate, std::ios::binary);

            if (file.is_open()) {
                buffer.resize(file.tellg());
                file.seekg(0, std::ios::beg);

                file.read(
                    reinterpret_cast<char*>(buffer.data()),
                    buffer.size());
            }

            return buffer;
        }
        
        void DecodeXOR(std::vector<uint8_t>& BYTES, int KEY) {
            for (auto& b : BYTES)
                b ^= KEY;
        }

        std::vector<uint8_t> DecodeBase64(const std::string& str) {
            gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);
            return b64.decode(str);
        }

        std::string DecompressGZip(const std::vector<uint8_t> str) {
            auto buffer = gdcrypto::zlib::inflateBuffer(str);
            return std::string(buffer.data(), buffer.data() + buffer.size());
        }

        std::string DecodeCCFile(const char* _name, void (*_prog)(unsigned short, LPCSTR) = NULL) {
            methods::perf::start();

            if (_prog != NULL) _prog(0, "Reading file...");

            std::string CCPATH = decode::GetCCPath(_name);
            std::vector<uint8_t> CCCONTENTS = decode::readf(CCPATH);

            std::string c = methods::fread(CCPATH);
            if (c._Starts_with("<?xml version=\"1.0\"?>")) {
                if (_prog != NULL) _prog(100, "Finished!");

                return c;
            }

            if (_prog != NULL) _prog(25, "Decoding XOR...");

            DecodeXOR(CCCONTENTS, 11);

            if (_prog != NULL) _prog(50, "Decoding Base64...");

            auto XOR = std::string(CCCONTENTS.begin(), CCCONTENTS.end());
            std::vector<uint8_t> B64 = DecodeBase64(XOR);

            if (_prog != NULL) _prog(75, "Decompressing GZip...");

            std::string ZLIB = DecompressGZip(B64);

            if (_prog != NULL) _prog(100, ("Finished in " + std::to_string(methods::perf::end()) + " !").c_str());

            return ZLIB;
        }

        bool SaveCCLocalLevels() {
            methods::fsave(decode::GetCCPath("LocalLevels"), "<?xml version=\"1.0\"?>\n" + methods::xts(&decode::decoded_data));

            return true;
        }

        rapidxml::xml_document<>* GetCCFileAsXML(const char* _which) {
            decoded_data.parse<0>(methods::stc(DecodeCCFile(_which)));
            return &decoded_data;
        }

        std::string DecodeLevelData(std::string _data) {
            return DecompressGZip(DecodeBase64(_data));
        }
    }

    namespace levels {
        enum export_types {
            gmd2 = 0, gmd, lvl
        };

        std::string ReplaceOfficialSong(int ID) {
            std::string arr[35] = {
                "Stereo Madness",
                "Back on Track",
                "Polargeist",
                "Dry Out",
                "Base After Base",
                "Cant Let Go",
                "Jumper",
                "Time Machine",
                "Cycles",
                "xStep",
                "Clutterfunk",
                "Theory of Everything",
                "Electronman Adventures",
                "Clubstep",
                "Electrodynamix",
                "Hexagon Force",
                "Blast Processing",
                "Theory of Everything 2",
                "Geometrical Dominator",
                "Deadlocked",
                "Fingerdash",
                "The Seven Seas",
                "Viking Arena",
                "Airborne Robots",
                "Secret (The Challenge)",
                "Payload",
                "Beast Mode",
                "Machina",
                "Years",
                "Frontlines",
                "Space Pirates",
                "Striker",
                "Embers",
                "Round 1",
                "Monster Dance Off"
            };
            return ID <= arr->length()-1 && ID >= 0 ? arr[ID] : "Unknown";
        }

        std::string GetKey_X(rapidxml::xml_node<>* _lvl, const char* _key) {
            for (rapidxml::xml_node<>* child = _lvl->first_node(); child; child = child->next_sibling())
                if (std::strcmp(child->name(), "k") == 0)
                    if (std::strcmp(child->value(), _key) == 0)
                        return child->next_sibling()->value();
            return "";
        }

        void GetSongFilePath(std::string _id, bool* _bro_you_didnt_set_gd_path, std::string* _ok_heres_the_path) {
            std::string ccf = gd::decode::GetCCFolder();

            if (methods::fexists(ccf + "\\" + _id + ".mp3")) {
                std::cout << ccf + "\\" + _id + ".mp3" << std::endl;

                *_ok_heres_the_path = ccf + "\\" + _id + ".mp3";
                *_bro_you_didnt_set_gd_path = false;
            } else {
                if (!user::LoadUserdata())
                    *_bro_you_didnt_set_gd_path = true;
                else {
                    if (!user::userdata["GDPath"].is_null()) {
                        std::cout << user::userdata["GDPath"].get<std::string>() + "\\" + _id + ".mp3" << std::endl;

                        if (methods::fexists(user::userdata["GDPath"].get<std::string>() + "\\" + _id + ".mp3")) {
                            *_ok_heres_the_path = user::userdata["GDPath"].get<std::string>() + "\\" + _id + ".mp3";
                            *_bro_you_didnt_set_gd_path = false;
                        } else {
                            *_ok_heres_the_path = "";
                            *_bro_you_didnt_set_gd_path = false;
                        }                        
                    } else
                        *_bro_you_didnt_set_gd_path = true;
                }
            }
        }

        void LoadLevels(std::string* _names = NULL) {
            if (decode::levels.size() == 0) {
                gd::decode::GetCCFileAsXML("LocalLevels");

                rapidxml::xml_node<>* d = decode::decoded_data.first_node("plist")->first_node("dict")->first_node("d");
                rapidxml::xml_node<>* fs = NULL;
                
                std::vector<rapidxml::xml_node<>*> LIST = {};
                for (rapidxml::xml_node<>* child = d->first_node(); child; child = child->next_sibling())
                    if (std::strcmp(child->name(), "d") == 0) {
                        LIST.push_back(child);
                        if (_names != NULL)
                            *_names += GetKey_X(child, "k2") + ";";
                    }

                decode::levels = LIST;
            }
        }

        bool SetKey_X(rapidxml::xml_node<>* _lvl, const char* _key, const char* _val, const char* _type = "s") {
            for (rapidxml::xml_node<>* child = _lvl->first_node(); child; child = child->next_sibling())
                if (std::strcmp(child->name(), "k") == 0)
                    if (std::strcmp(child->value(), _key) == 0) {
                        child->next_sibling()->first_node()->value(_val);
                        return true;
                    }
            std::string n_k ("<k>" + std::string (_key) + "</k><" + std::string (_type) + ">" + std::string (_val) + "</" + std::string (_type) + ">");
            rapidxml::xml_document<> n;
            n.parse<0>(methods::stc(n_k));

            rapidxml::xml_node<>* $k = _lvl->document()->clone_node(n.first_node("k"));
            rapidxml::xml_node<>* $t = _lvl->document()->clone_node(n.first_node(_type));
            _lvl->append_node($k);
            _lvl->append_node($t);
            return true;
        }

        int ImportLevel_X(std::string _path, std::string _lvl = "", std::string _name = "") {
            if (!methods::fexists(_path))
                return err::FILE_DOESNT_EXIST;

            std::string lvl;
            if (_lvl == "")
                lvl = methods::fread(_path);
            else lvl = _lvl;

            gd::decode::GetCCFileAsXML("LocalLevels");

            rapidxml::xml_node<>* d = decode::decoded_data.first_node("plist")->first_node("dict")->first_node("d");
            rapidxml::xml_node<>* fs = NULL;
            for (rapidxml::xml_node<>* child = d->first_node(); child; child = child->next_sibling()) {
                if (std::strcmp(child->name(), "k") == 0)
                    if (std::string(child->value()).find("k_") != std::string::npos) {
                        child->first_node()->value(methods::stc("k_" + std::to_string(std::stoi(std::string(child->value()).substr(2)) + 1)));
                        if (fs == NULL) fs = child;
                    }
            }

            rapidxml::xml_document<> lv;
            lv.parse<0>(methods::stc(lvl));
            SetKey_X(lv.first_node(), "k2", _name.c_str());

            rapidxml::xml_node<>* ln = lv.first_node();
            rapidxml::xml_node<>* lnt = decode::decoded_data.clone_node(ln);

            rapidxml::xml_document<> k_ix;
            k_ix.parse<0>(methods::stc("<k>k_0</k>"));
            rapidxml::xml_node<>* lk_ix = decode::decoded_data.clone_node(k_ix.first_node());

            d->insert_node(fs, lk_ix);
            d->insert_node(fs, lnt);

            gd::decode::SaveCCLocalLevels();

            return err::SUCCESS;
        }
    }
}