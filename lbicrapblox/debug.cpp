#include "customfuncs.h"
#include "execution.h"

#define checkK(constant) (!constant || constant->tt > 9)  /* hacky method, since pointer doesnt ALWAYS return as NULL or nullptr if invalid, just check the tagged type and see if its larger than usual tagged types */

void qkS_PshSet(std::uintptr_t rL, const char* v, const char* k)
{
    RBX::Lua::pushstring(rL, v);
    rbx_setfield(rL, -2, k);
}

void qkN_PshSet(std::uintptr_t rL, double v, const char* k)
{
    RBX::Lua::pushnumber(rL, v);
    rbx_setfield(rL, -2, k);
}

void qkNl_PshSet(std::uintptr_t rL, const char* k)
{
    rbx_pushnil(rL);
    rbx_setfield(rL, -2, k);
}

int RBX::CustomLibs::Debug::r_debug_getconstant(std::uintptr_t rL)
{
    CHECKARGTYPE(rL, 1, R_LUA_TFUNCTION);
    CHECKARGTYPE(rL, 2, R_LUA_TNUMBER);

    TValue* f = reinterpret_cast<TValue*>(rbx_index2adr(rL, 1));
    int idx = Lua::tointeger(rL, 2);

    if (idx <= 0)
        BASICERR(rL, "index cannot be negative or zero");

    RClosure* cl = (RClosure*)(f->value.gc);

    if (cl->c.isC)
        BASICERR(rL, "argument 1 must be lua function");

    RProto* p = cl->l.p;
    TValue* constant = &p->k[idx-1];

    if (idx - 1 > p->sizek)
        BASICERR(rL, "constant index out of range");

    if (checkK(constant))
    { 
        rbx_pushnil(rL);
        return 1;
    }

    Lua::pushraw(rL, constant);
    return 1;
}

int RBX::CustomLibs::Debug::r_debug_getconstants(std::uintptr_t rL)
{
    CHECKARGTYPE(rL, 1, R_LUA_TFUNCTION);

    TValue* f = reinterpret_cast<TValue*>(rbx_index2adr(rL, 1));
    RClosure* cl = (RClosure*)(f->value.gc);

    if (cl->c.isC)
        BASICERR(rL, "argument 1 must be lua function");

    RProto* p = cl->l.p;

    rbx_createtable(rL, p->sizek, 0);
    for (int i = 0; i < p->sizek; i++)
    {
        Lua::pushraw(rL, &p->k[i]);
        rbx_rawseti(rL, -2, i + 1);
    }

    return 1;
}

int RBX::CustomLibs::Debug::r_debug_getinfo(std::uintptr_t rL)
{
    CHECKARGTYPE(rL, 1, R_LUA_TFUNCTION);

    TValue* f = reinterpret_cast<TValue*>(rbx_index2adr(rL, 1));
    RClosure* cl = (RClosure*)(f->value.gc);

    RLClosure l = cl->l;
    RCClosure c = cl->c;

    bool isC = c.isC;
    std::string src = isC ? "C" : getstr(l.p->source);

    rbx_createtable(rL, 0, 0); /* create a new table, 11 since thats all we're writing */

    qkS_PshSet(rL, "source", src.c_str());
    qkS_PshSet(rL, "short_src", src.c_str()); /* same thing */

    isC ? qkS_PshSet(rL, "what", "C") : qkS_PshSet(rL, "what", "Lua"); /* if C, push C, if lua push lua... */

    isC ? qkN_PshSet(rL, c.nupvalues, "nups") : qkN_PshSet(rL, l.nupvalues, "nups");

    return 1;
}

void RBX::CustomLibs::r_open_debug_lib(std::uintptr_t rL)
{
    rbx_createtable(rL, 0, 0); /* Create baselib */

    /* Populate library */

    RBX::Execution::executorSingleton()->pushCFunction((int)Debug::r_debug_getconstant, rL);
    rbx_setfield(rL, -2, "getconstant");

    RBX::Execution::executorSingleton()->pushCFunction((int)Debug::r_debug_getconstants, rL);
    rbx_setfield(rL, -2, "getconstants");

    RBX::Execution::executorSingleton()->pushCFunction((int)Debug::r_debug_getinfo, rL);
    rbx_setfield(rL, -2, "getinfo");

    rbx_setreadonly(rL, -1, 1); /* Protect it */
    rbx_setfield(rL, -10002, "debug");
}