
/* State grabbing header */

#include "memory.h"
#include "addresses.h"

#include <cstdint>

#define shift(x) (x - 0x400000 + reinterpret_cast<unsigned int>(GetModuleHandle(0)))

namespace RBX {

	static std::uintptr_t ScriptContextVFTable_Address = shift(0x00F7F6A4); //
	static std::uintptr_t GetGlobalState_Address = shift(0x5614A0); //

	typedef std::uintptr_t(__thiscall* r_getglobalstate)(std::uintptr_t sc, int identity);
	static r_getglobalstate rbx_GetGlobalState = (r_getglobalstate)GetGlobalState_Address;

	/* Get lua_State* from ScriptContext (from global states) */
	extern std::uintptr_t GET_STATE(std::uintptr_t sc=0);

	/* Get ScriptContext singleton by VFTable scanning */
	extern std::uintptr_t GET_SCRIPTCONTEXT();
}