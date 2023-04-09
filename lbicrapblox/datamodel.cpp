#include "execution.h"
#include "datamodelFuncs.h"

void* oldIndex = 0;

int RBX::datamodelNewIndex(std::uintptr_t rL)
{
    const char* ff = rbx_tolstring(rL, 2, 0);

    Execution::Executor* e = Execution::executorSingleton();
    Execution::DatamodelHandler* d = e->getDatamodelHandler();

    if (d->pushDatamodelEvent(rL, ff)) /*   Check for field in our functions, return 1 if there.   */
        return 1;                      /*            If not return ROBLOX's function               */

    Execution::setOldIndexAtTop(rL, (std::uintptr_t)oldIndex);

    rbx_pushvalue(rL, 1);
    rbx_pushvalue(rL, 2);

    int r = rbx_pcall(rL, 2, 1, 0);

    if (r)
    {
        const char* err = rbx_tolstring(rL, -1, 0);
        rbx_settop(rL, -2);
        return rbx_luaL_error(rL, err);
    }

    return 1;
}

void RBX::Execution::Executor::registerGameIndex()
{
    std::uintptr_t tL = getExploitThread();

    std::cout << "[::registerGameIndex()]: registering new game index";

    /* Set new __index, old __index as old index */

    rbx_getfield(tL, -10002, "game");
    rbx_getmetatable(tL, -1);

    rbx_getfield(tL, -1, "__index");

    oldIndex = rbx_topointer(tL, -1);
    rbx_settop(tL, -2);

    pushCFunction((int)datamodelNewIndex, tL); // gone from stack 

    rbx_setreadonly(tL, -2, 0);
    rbx_setfield(tL, -2, "__index");
    rbx_setreadonly(tL, -2, 1);

    rbx_settop(tL, 0);

    /* Push DatamodelHandler functions */

    DatamodelHandler* handler = getDatamodelHandler();

    handler->newDatamodelEvent((int)CustomFuncs::r_httpGet, "HttpGet");
    handler->newDatamodelEvent((int)CustomFuncs::r_getObjects, "GetObjects");

    printf(" [OK]\n");
}

void RBX::Execution::Executor::fixLoadAsset()
{

}