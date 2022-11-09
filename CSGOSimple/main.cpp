#define NOMINMAX

#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"
#include "core/features/events.h"
#include "core/features/game_chams.h"
#include "core/hooks/hooks.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "render.hpp"
#include "valve_sdk/misc/weaponids.h"
#include "core/features/buy_bot.h"
#include <thread>

void cfg_auto_save()
{
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        if (g_cheat.config_autosave)
        {
            if (!g_cheat.cfg_name.empty())
            {
               g_Config->save(g_cheat.cfg_name);
            }
        }
    }
}

DWORD WINAPI OnDllAttach(LPVOID base)
{
	while (!GetModuleHandleA("serverbrowser.dll"))
		Sleep(1000);

#if _DEBUG
    Utils::AttachConsole();
#endif
    try 
    {
#if _DEBUG
        Utils::ConsolePrint("Initializing...\n");
#endif
        Interfaces::Initialize();
        Interfaces::Dump();

       // weaponids.on_init();
        NetvarSys::Get().Initialize();
        InputSys::Get().Initialize();
		Render::Get().Initialize();
        Menu::Get().Initialize();

        Hooks::Initialize();
        HookedEvents->RegisterSelf();
        g_Chams->CreateMaterials();
     
        InputSys::Get().RegisterHotkey(VK_DELETE, [base]() {
            g_Unload = true;
        });

        InputSys::Get().RegisterHotkey(VK_INSERT, [base]() {
            Menu::Get().Toggle();
        });

        Utils::ConsolePrint("\n");
        std::thread use_config_auto_save(cfg_auto_save);
        Utils::ConsolePrint("Config Autosave Thread Init Successfull\n");
    	use_config_auto_save.detach();
        Utils::ConsolePrint("Config Autosave Thread Detach Successfull\n");
        Utils::ConsolePrint("\n");
        Utils::ConsolePrint("Finished.\n");
		Utils::ConsolePrint("Built on: %s %s\n", __DATE__, __TIME__);

        while(!g_Unload)
            Sleep(1000);

        g_CVar->FindVar("crosshair")->SetValue(true);

        FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);

    } 
    catch(const std::exception& ex) 
    {
#if _DEBUG
        Utils::ConsolePrint("An error occured during initialization:\n");
        Utils::ConsolePrint("%s\n", ex.what());
        Utils::ConsolePrint("Press any key to exit.\n");
        Utils::ConsoleReadKey();
        Utils::DetachConsole();
#endif
        FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
    }
}

BOOL WINAPI OnDllDetach()
{
    Utils::DetachConsole();
    Hooks::Shutdown();
    Menu::Get().Shutdown();

    return TRUE;
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    switch(fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDll);
            CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
            return TRUE;
        case DLL_PROCESS_DETACH:
            if(lpvReserved == nullptr)
                return OnDllDetach();
            return TRUE;
        default:
            return TRUE;
    }
}
