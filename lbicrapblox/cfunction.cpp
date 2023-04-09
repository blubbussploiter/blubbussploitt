#include "execution.h"

/* Misc */

void RBX::Execution::Executor::pushCFunction(int f, std::uintptr_t tL, int n)
{
    if (!tL)
        tL = getExploitThread();
    Lua::pushcfunction(tL, (r_luaC_function)f, n);
}

void RBX::Execution::Executor::registerCf(int f, const char* g)
{
    std::uintptr_t tL = getExploitThread();

    /* Have actual function as an upvalue */

    pushCFunction(f, tL);
    rbx_setfield(tL, -10002, g);

}