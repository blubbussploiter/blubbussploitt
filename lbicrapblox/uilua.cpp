#include "ui.h"
#include "execution.h"
#include <cstdarg>

using namespace UserInterface::Lua;
std::vector<Output*> UserInterface::Lua::logged_output{};

int _log(const char* o, OutputTypes t) 
{
	logged_output.push_back(new Output(o, t));
	return 0;
}

/* just printing out (not for lua stuff tho) */

void UserInterface::Lua::Out(OutputTypes o, const char* fmt, ...)
{
	const char* f = luaO_pushfstring(RBX::Execution::executorSingleton()->L, fmt);
	_log(f, o);
}