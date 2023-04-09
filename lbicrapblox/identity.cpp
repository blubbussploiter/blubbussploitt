#include "execution.h"

void RBX::Execution::Executor::setidentity(std::uintptr_t L, int i)
{
    /* Set ExtraSpace identity as i */

    DWORD* loc1 = reinterpret_cast<DWORD*>(L - 20);
    *loc1 ^= (i ^ static_cast<unsigned __int8>(*loc1)) & 0x1F;
}

DWORD RBX::Execution::Executor::getidentity(std::uintptr_t L)
{
    DWORD* loc1 = reinterpret_cast<DWORD*>(L - 20);
    return static_cast<unsigned __int8>(*loc1) & 0x1F;
}