#include "datamodelFuncs.h"

#include "execution.h"
#include "reflection.h"

#include "content.h"
#include "getobjects.h"

#include <format>
#include <thread>
#include <future>

#define popArguments(rL, args) (rbx_settop(rL, -(args)-1))

int RBX::CustomFuncs::r_httpGet(std::uintptr_t rL)
{
    CHECKARGS(rL, 2);

    const char* url = rbx_tolstring(rL, 2, 0);
    popArguments(rL, 2);

    std::string downloaded = DownloadURL(url);

    Lua::pushstring(rL, downloaded.c_str());

    return 1;
}

/* Fix me, scripts arent loading */

int RBX::CustomFuncs::r_getObjects(std::uintptr_t rL)
{
    CHECKARGS(rL, 2);

    std::string url;
    switch (rbx_type(rL, 2))
    {
        case R_LUA_TSTRING: { url = rbx_tolstring(rL, 2, 0); break; }
        case R_LUA_TNUMBER: { url = std::string("rbxassetid://" + std::to_string(Lua::tonumber(rL, 2))); break; }
        default: BASICERR(rL, "");
    }

    std::string got = resolveUrl(url);

    std::shared_ptr<Instances> instances(new Instances());
    std::istringstream stream = std::istringstream(got);

    try
    {
        rbx_LoadInstances((int)url.c_str(), stream, *instances);
    }
    catch (std::exception e)
    {
        BASICERR(rL, e.what());
    }
    catch (...)
    {
        BASICERR(rL, "unknown exception thrown");
    }

    rbx_createtable(rL, instances->size(), 0);

    for (int i = 0; i < instances->size(); i++)
    {
        std::cout << "instance = 0x" << std::hex << instances->at(i) << "\n";
        GetObjects::loadScripts(rL, instances->at(i));
        rbx_bridgepush(rL, instances->at(i));
        rbx_rawseti(rL, -2, i + 1);
    }

    return 1;
}