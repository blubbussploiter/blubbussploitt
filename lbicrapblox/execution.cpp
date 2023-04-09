#include "execution.h"
#include "datamodelFuncs.h"

#include "ui.h"
#include "jhook.h"

#include <stack>

std::stack<std::string> script_queue;
std::uintptr_t oldStep = 0;

void dummyRun(std::uintptr_t rL, std::string _script)
{
    int r = rbx_luaVM_load(rL, ProtectedString(_script), "@pizzaparty", 1);

    if (r)
    {

        const char* err = rbx_tolstring(rL, -2, 0);
        if (err)
            UserInterface::Lua::Out(UserInterface::Lua::OutputTypes::error, err);
        rbx_settop(rL, -2);
        return;
    }

    rbx_spawn(rL);
    rbx_settop(rL, -2);
}

/* Actual execution */

int __fastcall newStep(std::uintptr_t _this, int fakearg, int stats)
{

    if (!script_queue.empty())
    {
        std::uintptr_t rL = RBX::Execution::executorSingleton()->getExploitThread();

        const std::string script = script_queue.top();
        script_queue.pop();

        dummyRun(rL, script);
    }

    return reinterpret_cast<int(__thiscall*)(std::uintptr_t, int)>(oldStep)(_this, stats);
}

void RBX::Execution::Executor::runScript(std::string script)
{
    if (script == "" || !isExploitReady)
        return;

    setidentity(rL, 6);

    std::string _script = s->serialize(L, script, RBX::Encoding::encode, RBX::getEncode(rL));
    script_queue.push(_script);
}

void RBX::Execution::setup()
{
    detourDatamodelJobStep("LuaResumeWaitingScripts", newStep, oldStep);
}