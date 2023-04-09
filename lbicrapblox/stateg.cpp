#include "stateg.h"

std::uintptr_t RBX::GET_STATE(std::uintptr_t sc)
{
    if(!sc)
        return rbx_GetGlobalState(GET_SCRIPTCONTEXT(), 0);
    return rbx_GetGlobalState(sc, 0);
}

std::uintptr_t RBX::GET_SCRIPTCONTEXT()
{
    return Scan((char*)&ScriptContextVFTable_Address);
}