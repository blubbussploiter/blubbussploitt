#include <Windows.h>

#include "execution.h"
#include "output.h"

#include "customfuncs.h"
#include "auth.h"

#include "ui.h"
#include "sigscan.h"

#include <iostream>
#include <thread>

bool RBX::isExploitReady = false;

int main()
{
    VMProtectBeginMutation(NULL);

    std::uintptr_t global = 0;
    UserInterface::init();

    output("hi");

    UserInterface::setMainTabSelected(OUTPUT_TAB_CURRENT);

    UserInterface::Lua::Out(UserInterface::Lua::print, "Injected!");
    UserInterface::Lua::Out(UserInterface::Lua::print, "Authorizing..");

    UserInterface::Lua::Out(UserInterface::Lua::print, "Scanning...");

    do
    {
        global = RBX::GET_STATE();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    } while (global == 0);

    UserInterface::Lua::Out(UserInterface::Lua::print, "Pushing...");

    std::uintptr_t exploitThread = rbx_newthread(global);
    rbx_sandboxThread(exploitThread);

    RBX::Execution::Executor* e = RBX::Execution::executorSingleton(exploitThread);
    LuaSecureDouble::initDouble();

    e->registerCf((int)RBX::CustomFuncs::r_dofile, "dofile"); // done
    e->registerCf((int)RBX::CustomFuncs::r_loadstring, "loadstring"); // done 

    e->registerCf((int)RBX::CustomFuncs::r_setreadonly, "setreadonly"); // done
    e->registerCf((int)RBX::CustomFuncs::r_setrawmetatable, "setrawmetatable"); // done

    e->registerCf((int)RBX::CustomFuncs::r_setsimulationradius, "setsimulationradius"); // done

    e->registerCf((int)RBX::CustomFuncs::r_hookmetamethod, "hookmetamethod"); // done
    e->registerCf((int)RBX::CustomFuncs::r_newcclosure, "newcclosure"); // done 

    e->registerCf((int)RBX::CustomFuncs::r_checkcaller, "checkcaller");

    e->registerCf((int)RBX::CustomFuncs::r_getcallingscript, "getcallingscript"); // done
    e->registerCf((int)RBX::CustomFuncs::r_getrawmetatable, "getrawmetatable"); // done 

    e->registerCf((int)RBX::CustomFuncs::r_getcaller, "getcaller");

    e->registerCf((int)RBX::CustomFuncs::r_getgenv, "getgenv"); // done 
    e->registerCf((int)RBX::CustomFuncs::r_getienv, "getienv"); // done
    e->registerCf((int)RBX::CustomFuncs::r_getgenv, "getrenv"); // done 
    e->registerCf((int)RBX::CustomFuncs::r_getreg, "getreg"); // done 

    e->registerCf((int)RBX::CustomFuncs::r_getcurrentidentity, "getcurrentidentity"); // done
    e->registerCf((int)RBX::CustomFuncs::r_fireclickdetector, "fireclickdetector"); // not done 

    e->registerCf((int)RBX::CustomFuncs::r_getscriptsource, "getscriptsource"); // not done 

    UserInterface::Lua::Out(UserInterface::Lua::print, "Opening libraries...");

    RBX::CustomLibs::r_open_crypt_lib(exploitThread);
    RBX::CustomLibs::r_open_bit_lib(exploitThread);
    RBX::CustomLibs::r_open_debug_lib(exploitThread);
    
    e->registerGameIndex();
    RBX::Execution::setup(); /* 
                                we need to hook wsj step so theres no inconsistency within execution times
                             (say roblox thread executed 2 seconds ago, ours executed now, scheduler (steps per 1 second or so) cannot keep up with our thread!)\
                                so setup this hook to load our script ON TIME with the scheduler so no crashes
                             */
    RBX::isExploitReady = true;

    UserInterface::Lua::Out(UserInterface::Lua::print, "Ready! (press F2 to hide GUI)");
    UserInterface::setMainTabSelected(EXECUTOR_TAB_CURRENT);

    return 0;
    VMProtectEnd();
}

BOOL APIENTRY DllMain(HMODULE hModule,
    unsigned long  ul_reason_for_call,
    LPVOID lpReserved
)
{

    dll = hModule;
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call != DLL_PROCESS_ATTACH) return 1;
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&main, NULL, NULL, NULL);

    return 1;
}

