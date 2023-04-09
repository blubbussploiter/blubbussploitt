#include "addresses.h"

RBX_ALIGN(16) int LuaSecureDouble::luaXorMask[4];

// For lua, create a random bitstring to use for doubles.
// Mainly care about the exponent bits.  This ensures at least one of the top 7 exp
// bits gets changed.
void LuaSecureDouble::initDouble()
{
    uint32_t luaRandMask = *reinterpret_cast<uint32_t*>(rbx_xorMask);

    for (int i = 0; i < sizeof(LuaSecureDouble::luaXorMask) / sizeof(int); ++i)
    {
        LuaSecureDouble::luaXorMask[i] = luaRandMask;
    }
}