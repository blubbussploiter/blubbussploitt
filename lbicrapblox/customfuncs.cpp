
#include "execution.h"
#include "customfuncs.h"

#include "newcclosure.h"
#include "hooking.h"

#include "content.h"
#include "reflection.h"

#include "ui.h"
#include "saveinstance_addresses.h"

#include <fstream>

#define rbx_getInstance(rL, idx) (*(std::uintptr_t*)(Lua::touserdata(rL, idx)))

int RBX::CustomFuncs::r_getgenv(std::uintptr_t rL)
{
    rbx_pushvalue(rL, -10002);
    return 1;
}

int RBX::CustomFuncs::r_getreg(std::uintptr_t rL)
{
    rbx_pushvalue(rL, -10000);
    return 1;
}

int RBX::CustomFuncs::r_getienv(std::uintptr_t rL)
{
    rbx_pushvalue(rL, -10001);
    return 1;
}

int RBX::CustomFuncs::r_setreadonly(std::uintptr_t rL)
{
    CHECKARGTYPE(rL, 1, R_LUA_TTABLE);
    CHECKARGTYPE(rL, 2, R_LUA_TBOOLEAN);
    rbx_setreadonly(rL, 1, Lua::toboolean(rL, 2));
    return 0;
}

int RBX::CustomFuncs::r_dofile(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 1, R_LUA_TSTRING);

    const char* fname = rbx_tolstring(rL, 1, 0);
    ContentId c(fname);

    std::string buffer = c.resolve();

    if(buffer == "")
        return rbx_luaL_error(rL, "dofile('%s') invalid path for argument 1", fname);

    Execution::Executor* e = Execution::executorSingleton();
    e->runScript(buffer);

    return 0;
}

int RBX::CustomFuncs::r_readfile(std::uintptr_t rL)
{
    return 0;
}

int RBX::CustomFuncs::r_writefile(std::uintptr_t rL)
{
    return 0;
}

int RBX::CustomFuncs::r_loadstring(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 1, R_LUA_TSTRING);

    const char* script = rbx_tolstring(rL, 1, 0);

    std::string chunkname = "=loadstring";
    if (rbx_tolstring(rL, 2, 0))
        chunkname = rbx_tolstring(rL, 2, 0);

    std::string _script = Execution::executorSingleton()->rawSerialize(rL, script);
    int r = rbx_luaVM_load(rL, ProtectedString(_script), chunkname.c_str(), 1);

    if (r == 0) /* OK? */
        return 1;
    else
    {
        rbx_pushnil(rL);
        rbx_insert(rL, -2); /* put before error message */
        return 2; /* return nil plus error message */
    }

    return 0;
}

int RBX::CustomFuncs::r_setrawmetatable(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TTABLE);
    
    rbx_settop(rL, 2);
    rbx_setmetatable(rL, 1);

    return 1; 
}

int RBX::CustomFuncs::r_setsimulationradius(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 1, R_LUA_TNUMBER);

    double sr = Lua::tonumber(rL, 1);
    double msr = Lua::tonumber(rL, 2);

    rbx_getfield(rL, -10002, "game");
    rbx_getfield(rL, -1, "Players");
    rbx_getfield(rL, -1, "LocalPlayer");

    if (!rbx_type(rL, -1)) 
        BASICERR(rL, "localplayer is nil");

    Lua::pushnumber(rL, sr);
    rbx_setfield(rL, -2, "SimulationRadius");

    if (msr)
    {
        Lua::pushnumber(rL, msr);
        rbx_setfield(rL, -2, "MaximumSimulationRadius");
    }

    rbx_settop(rL, -2); /* pop localplayer */
    return 0;
}

int RBX::CustomFuncs::r_getscriptsource(std::uintptr_t rL)
{
    CHECKARGTYPE(rL, 1, R_LUA_TUSERDATA);
    std::uintptr_t script = rbx_getInstance(rL, 1);

    std::uintptr_t src = rbx_getScriptSource(script);
    std::cout << src << "\n";

    return 0;
}

int RBX::CustomFuncs::r_getcurrentidentity(std::uintptr_t rL)
{
    Lua::pushnumber(rL, Execution::executorSingleton()->getidentity(rL));
    return 1;
}

int RBX::CustomFuncs::r_getrawmetatable(std::uintptr_t rL)
{
    CHECKARGS(rL, 1);
    if (!rbx_getmetatable(rL, 1))
    {
        rbx_pushnil(rL);
        return 1; /* no metatable */
    }
    return 1; /* return metatable */
}

int RBX::CustomFuncs::r_getcallingscript(std::uintptr_t rL)
{
    rbx_getfield(rL, -10002, "script");
    return 1;
}

int RBX::CustomFuncs::r_hookmetamethod(std::uintptr_t rL)
{
    CHECKARGTYPE(rL, 1, R_LUA_TUSERDATA);
    CHECKARGTYPE(rL, 2, R_LUA_TSTRING);
    CHECKARGTYPE(rL, 3, R_LUA_TFUNCTION);

    /* Get metatable of userdata */

    if (!rbx_getmetatable(rL, 1))
        BASICERR(rL, "invalid or nil metatable for argument 1");

    /* Get metamethod */

    const char* method = rbx_tolstring(rL, 2, 0);
    rbx_getfield(rL, -1, method); /* Get original metamethod */

    RClosure* f = reinterpret_cast<RClosure*>(rbx_topointer(rL, -1));
    rbx_settop(rL, -2); /* Pop method, we got it in memory already (as f) */

    rbx_setreadonly(rL, -1, 0); /* unprotect metatable */
    rbx_pushvalue(rL, 3);
    rbx_setfield(rL, -2, method); /* mt.__method = function */
    rbx_setreadonly(rL, -1, 1); /* reprotect metatable */

    Lua::pushclosure(rL, f); /* push original function, is / could be a upvalue */
    return 1;
}

int RBX::CustomFuncs::r_fireclickdetector(std::uintptr_t rL)
{
    CHECKARGTYPE(rL, 1, R_LUA_TUSERDATA);
    CHECKARGTYPE(rL, 2, R_LUA_TNUMBER);

    DWORD cd;
    DWORD plr;

    cd = rbx_getInstance(rL, 1);
    
    double f = Lua::tonumber(rL, 2);

    rbx_getfield(rL, -10002, "game");
    rbx_getfield(rL, -1, "Players");
    rbx_getfield(rL, -1, "LocalPlayer");

    plr = rbx_getInstance(rL, -1);

    rbx_settop(rL, -2);

    if (!cd || !plr)
        BASICERR(rL, "invalid clickdetector or player");

    rbx_fireclickdetector(cd, f, plr);

    return 0;
}

int RBX::CustomFuncs::r_checkcaller(std::uintptr_t rL)
{
    bool b = (rbx_index2adr(rL, -10002) == rbx_index2adr(Execution::executorSingleton()->getExploitThread(), -10002));
    Lua::pushboolean(rL, b);
    return 1;
}

int RBX::CustomFuncs::r_getcaller(std::uintptr_t rL)
{
    rbx_pushthread(rL);
    return 1;
}

int RBX::CustomFuncs::r_newcclosure(std::uintptr_t rL)
{
    CHECKARGS(rL, 1);

    Execution::executorSingleton()->pushCFunction((int)NewCclosure::r_newcclosurehandler, rL, 1);

    return 1;
}