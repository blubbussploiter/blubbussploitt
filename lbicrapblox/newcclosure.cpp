#include "newcclosure.h"

#include "execution.h"
#include "customfuncs.h"

int RBX::NewCclosure::r_newcclosurehandler(std::uintptr_t rL)
{

    TValue* f = (TValue*)(rbx_index2adr(rL, lua_upvalueindex(1))); /* lol no more veh shit */
    RClosure* cl = (RClosure*)(f->value.p);

    int nargs = rbx_gettop(rL);

    RBX::Lua::pushclosure(rL, cl);
    for (int i = 1; i <= nargs; ++i) /* repush args */
        rbx_pushvalue(rL, i);
    rbx_pcall(rL, nargs, -1, 0);

    return rbx_gettop(rL);
}