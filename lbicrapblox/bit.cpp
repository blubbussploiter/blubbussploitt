
#include "customfuncs.h"
#include "execution.h"

int RBX::CustomLibs::Bit::r_bit_bdiv(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, int(Lua::tointeger(rL, 1) / Lua::tointeger(rL, 2)));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_bmul(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, Lua::tointeger(rL, 1) * Lua::tointeger(rL, 2));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_badd(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, Lua::tointeger(rL, 1) + Lua::tointeger(rL, 2));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_bsub(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, Lua::tointeger(rL, 1) - Lua::tointeger(rL, 2));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_band(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, Lua::tointeger(rL, 1) & Lua::tointeger(rL, 2));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_bor(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, Lua::tointeger(rL, 1) | Lua::tointeger(rL, 2));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_bxor(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, Lua::tointeger(rL, 1) ^ Lua::tointeger(rL, 2));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_bnot(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 1, R_LUA_TNUMBER);
    Lua::pushnumber(rL, ~Lua::tointeger(rL, 1));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_ror(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, _rotr(Lua::tointeger(rL, 1), Lua::tointeger(rL, 2)));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_rol(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, _rotl(Lua::tointeger(rL, 1), Lua::tointeger(rL, 2)));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_tohex(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 1, R_LUA_TNUMBER);
    int n = Lua::tointeger(rL, 1);

    std::stringstream stream;
    stream << std::hex << n;

    Lua::pushstring(rL, stream.str().c_str());
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_tobit(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 1, R_LUA_TNUMBER);
    Lua::pushnumber(rL, Lua::tointeger(rL, 1));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_lshift(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, int(Lua::tointeger(rL, 1) << Lua::tointeger(rL, 2)));
    return 1;
}

int RBX::CustomLibs::Bit::r_bit_rshift(std::uintptr_t rL)
{
    CHECKARGSTYPE(rL, 2, R_LUA_TNUMBER);
    Lua::pushnumber(rL, int(Lua::tointeger(rL, 1) >> Lua::tointeger(rL, 2)));
    return 1;
}


void RBX::CustomLibs::r_open_bit_lib(std::uintptr_t rL)
{
	rbx_createtable(rL, 0, 0); /* Create baselib */

	/* Populate library */

	RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_badd, rL);
    rbx_setfield(rL, -2, "badd");

	RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_band, rL);
	rbx_setfield(rL, -2, "band");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_bdiv, rL);
    rbx_setfield(rL, -2, "bdiv");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_bmul, rL);
    rbx_setfield(rL, -2, "bmul");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_bnot, rL);
    rbx_setfield(rL, -2, "bnot");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_bor, rL);
    rbx_setfield(rL, -2, "bor");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_bsub, rL);
    rbx_setfield(rL, -2, "bsub");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_bxor, rL);
    rbx_setfield(rL, -2, "bxor");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_lshift, rL);
    rbx_setfield(rL, -2, "lshift");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_rol, rL);
    rbx_setfield(rL, -2, "rol");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_ror, rL);
    rbx_setfield(rL, -2, "ror");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_rshift, rL);
    rbx_setfield(rL, -2, "rshift");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_tobit, rL);
    rbx_setfield(rL, -2, "tobit");

    RBX::Execution::executorSingleton()->pushCFunction((int)Bit::r_bit_tohex, rL);
    rbx_setfield(rL, -2, "tohex");

	rbx_setreadonly(rL, -1, 1); /* Protect it */
	rbx_setfield(rL, -10002, "bit");
}