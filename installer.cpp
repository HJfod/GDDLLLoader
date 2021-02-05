#include <iostream>
#include <string>
#include <Windows.h>
#include <WinUser.h>
#include "src/methods.hpp"
#include "WtsApi32.h"
#include <ctype.h>
#include <tlhelp32.h>
#include <Shlwapi.h>
#include <process.h>
#include <filesystem>

const char* AppName = "ModLdr Installer";
const char* DataFile = ".gmdpath";

// #define NOMSGBOX

#define INJECT_SUCCESS 0x3F
#define INJECT_TARGET_OPEN_FAIL 0x30
#define INJECT_TARGET_MALLOC_FAIL 0x31
#define INJECT_TARGET_CANT_WRITE 0x32
#define INJECT_TARGET_CANT_CREATE_THREAD 0x33
#define INJECT_ERROR_UNKNOWN 0x34

bool findGDPath(std::string* _res) {
    char value[255];
    DWORD BufferSize = 8192;

    LSTATUS key = RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Valve\\Steam", "InstallPath", RRF_RT_ANY, NULL, (PVOID)&value, &BufferSize);

    if (key != ERROR_SUCCESS) {
        return false;
    } else {
        std::string steamPath = value;
        std::string testPath = steamPath + std::string("\\steamapps\\common\\Geometry Dash\\GeometryDash.exe");

        if (std::filesystem::exists(testPath))
            if (std::filesystem::is_regular_file(testPath)) {
                *_res = testPath;
                return true;
            }

        // if we're here, then that means steamPath did not contain GD

        std::string line;
        std::string configPath = steamPath + "\\config\\config.vdf";

        std::ifstream f (configPath);

        while (getline(f, line)) {
            if (line.find("BaseInstallFolder_") != std::string::npos) {
                std::string val = line.substr(0, line.find_last_of("\""));
                val = val.substr(val.find_last_of("\"") + 1);

                testPath = val + "\\steamapps\\common\\Geometry Dash\\GeometryDash.exe";

                testPath = methods::replace(testPath, "\\\\", "\\");

                if (std::filesystem::exists(testPath))
                    if (std::filesystem::is_regular_file(testPath)) {
                        *_res = testPath;
                        f.close();
                        return true;
                    }
            }
        }

        return false;
    }

    return false;
}

enum err {
    PROG_SUCCESS = 0,
    FILE_NOT_FOUND,
    GD_PATH_NOT_VALID,
    INJECT_ERROR,
    CANT_OPEN_GD,
    STOPPED,
    GD_RUNNING,
    CANT_BACKUP,
    CANT_UNINSTALL
};

const char* req_files[5] = {
    "ModLdr.dll",
    "MinHook.x86.dll",
    "libcurl.dll",
    "DL_mods.png|resources",
    "DL_folder.png|resources"
};

int throwErr(std::string _msg, int _err) {
    std::cout << " Failed" << std::endl;

    #ifndef NOMSGBOX
        MessageBoxA(NULL, _msg.c_str(), AppName, MB_OK);
    #endif

    exit(_err);
}

int InjectDLL(const int &pid, const std::string &DLL_Path) {
    // adapted from https://github.com/saeedirha/DLL-Injector
    
    long dll_size = DLL_Path.length() + 1;
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (hProc == NULL)
        return INJECT_TARGET_OPEN_FAIL;

    LPVOID MyAlloc = VirtualAllocEx(hProc, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (MyAlloc == NULL)
        return INJECT_TARGET_MALLOC_FAIL;

    int IsWriteOK = WriteProcessMemory(hProc , MyAlloc, DLL_Path.c_str() , dll_size, 0);
    if (IsWriteOK == 0)
        return INJECT_TARGET_CANT_WRITE;

    DWORD dWord;
    LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("kernel32"), "LoadLibraryA");
    HANDLE ThreadReturn = CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, MyAlloc, 0, &dWord);
    if (ThreadReturn == NULL)
        return INJECT_TARGET_CANT_CREATE_THREAD;

    if ((hProc != NULL) && (MyAlloc != NULL) && (IsWriteOK != ERROR_INVALID_HANDLE) && (ThreadReturn != NULL))
        return INJECT_SUCCESS;

    return INJECT_ERROR_UNKNOWN;
}

int main(int argc, char* argv[]) {
    #ifdef UNINSTALL
        SetConsoleTitleA("Uninstaller");
    #else
        SetConsoleTitleA("Installer");
    #endif

    std::cout << "MsgBox: " <<
    #ifdef NOMSGBOX
        0
    #else
        1
    #endif
    << std::endl;
    std::cout << "PID: " << _getpid() << std::endl;

    ////////////////////////////

    std::string GDDataPath = "";

    if (std::filesystem::exists(DataFile))
        GDDataPath = methods::fread(DataFile);
    else {
        std::cout << "Detecting GD path...";

        if (findGDPath(&GDDataPath)) {
            std::cout << " Success" << std::endl;

            std::cout << "Detected path: " << GDDataPath << std::endl;
        } else {
            std::cout << " Failed" << std::endl;
            
            std::cout << "Type the path to GeometryDash.exe" << std::endl;
            getline(std::cin, GDDataPath);
        }
    }

    ////////////////////////////

    std::cout << "Checking data path...";

    bool gddpcs = false;
    if (methods::ewith(GDDataPath, "GeometryDash.exe"))
        if (methods::fexists(GDDataPath))
            gddpcs = true;
    if (!gddpcs)
        throwErr("GeometryDash.exe path does not appear to be correct.", err::GD_PATH_NOT_VALID);

    GDDataPath = methods::replace(GDDataPath, "/", "\\");
    methods::fsave(DataFile, GDDataPath);

    std::cout << " Success" << std::endl;

    ////////////////////////////

    #ifndef UNINSTALL

    std::cout << "Checking required files...";

    for (std::string f : req_files) {
        if (!methods::fexists(f.find("|") != std::string::npos ? f.substr(0, f.find_first_of("|")) : f))
            throwErr("Unable to load: " + f + " not found! (Did you forget to unzip?)", err::FILE_NOT_FOUND);
    }
    
    std::cout << " Success" << std::endl;

    #endif
    
    ////////////////////////////

    std::cout << "Checking GD status...";

    if (!methods::proc_running("GeometryDash.exe"))
        std::cout << " Success" << std::endl;
    else {
        #ifdef UNINSTALL

            std::cout << " Failed" << std::endl;

            throwErr("Please close GD to uninstall.", err::GD_RUNNING);

        #else

            std::cout << " Failed" << std::endl;

            throwErr("Please close GD to install.", err::GD_RUNNING);

        #endif
    }
    
    ////////////////////////////
    
    std::cout 
        << "\n"
        #ifdef UNINSTALL

        << "Path: " << GDDataPath
        << "\n\n"
        << "Proceed with uninstallation? (Y/N) ";

        #else

        << "Installation path: " << GDDataPath
        << "\n"
        << "If you'd like to change the installation path, quit the installer and change the path in the .gmdpath file"
        << "\n\n"
        << "Proceed with installation? (Y/N) ";

        #endif

    std::string proceed;
    std::cin >> proceed;

    if (methods::lower(proceed) != "y") {
        std::cout << "Aborting..." << std::endl;
        return err::STOPPED;
    }

    std::string GDDataFolder = GDDataPath.substr(0, GDDataPath.find_last_of("\\"));

    ////////////////////////////

    #ifndef UNINSTALL

    std::cout << "Checking MegaHack v6...";

    if (std::filesystem::exists(GDDataFolder + "\\absolutedlls")) {
        std::cout << " Found" << std::endl;

        std::cout << "Adding DLL...";

        std::string data = methods::fread(GDDataFolder + "\\absolutedlls");

        data += "\nModLdr.dll";

        methods::fsave(GDDataFolder + "\\absolutedlls", data);

        std::cout << " Success" << std::endl;
    } else {
        std::cout << " Not found" << std::endl;
        std::cout << "Backing up files...";

        if (methods::fcopy(GDDataFolder + "\\libcurl.dll", GDDataFolder + "\\libcurl.dll.bak") != METH_SUCCESS) {
            std::cout << " Failed" << std::endl;
            throwErr("Unable to back up libcurl.dll!", err::CANT_BACKUP);
        }

        std::cout << " Success" << std::endl;
    }

    #endif

    ////////////////////////////

    #ifdef UNINSTALL

    std::cout << "Removing files...";

    if (!std::filesystem::exists(GDDataFolder + "\\libcurl.dll.bak")) {
        std::cout << " Failed" << std::endl;
        throwErr("Backup of libcurl.dll not found! (Unable to uninstall)", err::CANT_UNINSTALL);
    }

    methods::fcopy(GDDataFolder + "\\libcurl.dll.bak", GDDataFolder + "\\libcurl.dll");
    
    if (remove((GDDataFolder + "\\ModLdr.dll").c_str()) != 0) {
        std::cout << " Failed" << std::endl;
        throwErr("Unable to delete ModLdr.dll!", err::CANT_UNINSTALL);
    }

    if (remove((GDDataFolder + "\\Resources\\DL_mods.png").c_str()) != 0) {
        std::cout << " Failed" << std::endl;
        throwErr("Unable to delete DL_mods.png!", err::CANT_UNINSTALL);
    }

    if (remove((GDDataFolder + "\\Resources\\DL_folder.png").c_str()) != 0) {
        std::cout << " Failed" << std::endl;
        throwErr("Unable to delete DL_folder.png!", err::CANT_UNINSTALL);
    }

    std::cout << " Success" << std::endl;

    std::cout
        << "\nWarning: Did not remove MinHook.x86.dll due to other mods possibly depending on it."
        << "\nFor complete uninstallation, you will have to manually remove it."
        << std::endl;

    #else

    std::cout << "Moving files...";

    for (int i = 0; i < (sizeof(req_files)/sizeof(*req_files)); i++) {
        std::string act = req_files[i];
        if (std::string(req_files[i]).find("|") != std::string::npos)
            act = act.substr(act.find_first_of("|") + 1) + "\\" + act.substr(0, act.find_first_of("|"));
        
        std::string nMHpath = GDDataFolder + "\\" + act;
        if (!methods::fexists(nMHpath))
            if (methods::fcopy(act == req_files[i] ? act : act.substr(act.find_first_of("\\") + 1), nMHpath) != METH_SUCCESS)
                throwErr("There was an error copying files (METH_COPY_FROM_DOESNT_EXIST, probably)", err::FILE_NOT_FOUND);
    }

    methods::fcopy("libcurl.dll", GDDataFolder + "\\libcurl.dll");

    std::cout << " Success" << std::endl;

    #endif
    
    ////////////////////////////
    
    #ifndef NOMSGBOX
        #ifdef UNINSTALL

            MessageBoxA(nullptr, "Succesfully uninstalled! :)", AppName, MB_OK);

        #else
            
            MessageBoxA(nullptr, "Succesfully installed! :)", AppName, MB_OK);

        #endif
    #endif

    return PROG_SUCCESS;
}