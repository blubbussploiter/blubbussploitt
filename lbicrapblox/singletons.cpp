#include "execution.h"

RBX::Execution::Executor* executor = nullptr;

/* Singletons */

RBX::Execution::Executor* RBX::Execution::executorSingleton(std::uintptr_t t)
{
    if (!executor)
        if(!t)
            executor = new RBX::Execution::Executor(RBX::GET_STATE());
        else
            executor = new RBX::Execution::Executor(t);
    return executor;
}