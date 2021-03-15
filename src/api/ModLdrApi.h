// by AndreNIH - thank you!!

#pragma once
#include <Windows.h>

#ifdef  MODLDRAPI_EXPORTS
#define MODLDR_DLL __declspec(dllexport)
#else
#define MODLDR_DLL __declspec(dllimport)
#endif

typedef void(__stdcall* ModCallback)();
typedef  unsigned int   ModHandle;

enum ModLdrStatus {
	ML_OK,
	ML_INVALID_HANDLE_ERR,
	ML_ALREADY_LOADED_ERR,
	ML_UNSPECIFIED_ERR
};

struct MODLDR_MOD_ENTRY {
	const char* displayName;
	const char* modName;
	ModCallback initializeMod   = 0;
	ModCallback uninitializeMod = 0;
};
//Public API

#ifdef __cplusplus
extern "C" {
#endif

	MODLDR_DLL ModHandle mod_register_dll(HMODULE dllModule);

	MODLDR_DLL void mod_set_title(ModHandle handle, const char* displayName);
	MODLDR_DLL void mod_add_enable(ModHandle  handle, ModCallback callback);
	MODLDR_DLL void mod_add_disable(ModHandle handle, ModCallback callback);


	MODLDR_DLL void mod_set_error(ModLdrStatus code);
	MODLDR_DLL ModLdrStatus mod_get_error();


	//Functions reserved for internal usage
	//DO NOT CALL THEM UNLESS YOU KNOW WHAT YOU ARE DOING

	MODLDR_DLL const MODLDR_MOD_ENTRY* devmod_get_mod_info(ModHandle handle);
	MODLDR_DLL ModHandle  devmod_fetch_id_with_name(const char* modName);
	MODLDR_DLL void devmod_remove_mod(ModHandle ID);

#ifdef __cplusplus
	}
#endif