#ifndef ADDRESSES_H
#define ADDRESSES_H

#include <Windows.h>
#include <cstdint>
#include <string>
#include "retcheck.h"

#include "vmprotectsdk.h"

#include <intrin.h>
#include <vector>
#include <memory>
#include <functional>

extern "C"
{
	#include "lobject.h"
}


extern unsigned int L;

#define shift(x) (x - 0x400000 + reinterpret_cast<unsigned int>(GetModuleHandle(0)))
#define unretcheck(x) retcheckBypass(x)

#define RBX_ALIGN(s) _declspec(align(s)) 
typedef std::vector<std::shared_ptr<std::uintptr_t>> Instances;

static unsigned int rbx_getfield_address = unretcheck(shift(0x582830)); //
static unsigned int rbx_setfield_address = unretcheck(shift(0x583AF0)); //

static unsigned int rbx_setmetatable_address = unretcheck(shift(0x583BB0));  //
static unsigned int rbx_getmetatable_address = unretcheck(shift(0x5828D0)); //

static unsigned int rbx_settop_address = unretcheck(shift(0x583D80)); //
static unsigned int rbx_gettop_address = shift(0x5829D0);  //

static unsigned int rbx_newthread_address = unretcheck(shift(0x582CE0)); //
static unsigned int rbx_setreadonly_address = unretcheck(shift(0x583CB0)); //

static unsigned int rbx_spawn_address = shift(0x56CDD0); //
static unsigned int rbx_luaVM_load_address = shift(0x484DF0); //

static unsigned int rbx_bridgePush_address = shift(0x567E20); //

static unsigned int rbx_tolstring_address = unretcheck(shift(0x583F60)); //

static unsigned int rbx_index2adr_address = shift(0x582050);  //
static unsigned int rbx_insert_address = unretcheck(shift(0x582A80)); //

static unsigned int rbx_pcall_address = unretcheck(shift(0x582F70)); //
static unsigned int rbx_pushcclosure_address = unretcheck(shift(0x584A50));  

static unsigned int rbx_pushnil_address = unretcheck(shift(0x583360)); //
static unsigned int rbx_pushvalue_address = unretcheck(shift(0x583510));  //

static unsigned int rbx_pushthread_address = unretcheck(shift(0x5834A0)); //

static unsigned int rbx_rawgeti_address = unretcheck(shift(0x5836C0)); //
static unsigned int rbx_rawseti_address = unretcheck(shift(0x5837F0)); //

static unsigned int rbx_loadinstances_address = shift(0x7141E0); //

static unsigned int rbx_topointer_address = shift(0x584210); //
static unsigned int rbx_createtable_address = unretcheck(shift(0x582520)); //

static unsigned int rbx_Lerror_address = shift(0x775860); //

static unsigned int S_newlstr_address = shift(0x584AE0); //
static unsigned int sandboxThread_address = shift(0x56C1B0);   //

static unsigned int taskScheduler_a = shift(0x525110); //
static unsigned int getJobsByName_a = shift(0x525FA0); //

static unsigned int rbx_xorMask = shift(0x160BD60); //
static unsigned int rbx_newcclosure = shift(0x5850E0); //

static unsigned int rbx_fireclickdetector_address = shift(0x8E1D40); /* 'MouseClick', dword_XXXXXX, find xrefs, few last */
static unsigned int rbx_getinstance_address = shift(0x7870D0); /* Expected ':' not '.' calling member function %s */

static unsigned int rbx_findPropertyDescriptor_Address = shift(0xAF4C20); /* !instance || descriptor.isMemberOf(instance) */
static unsigned int rbx_getScriptSource_Address = shift(0x65A900); /* "getCachedRemoteSourceLoadState() != NotAttemptedToLoad file: script\\Script.cpp line: 206" */

typedef std::uintptr_t(__thiscall* r_getScriptSource)(DWORD);
static r_getScriptSource rbx_getScriptSource = (r_getScriptSource)rbx_getScriptSource_Address;

class ProtectedString {
public:
	std::string source;
	std::string bytecode;
	ProtectedString(std::string str)
	{
		bytecode = str;
	}
	ProtectedString() { bytecode = ""; source = ""; }
};

class LuaSecureDouble
{
private:
	double storage;
public:
	static RBX_ALIGN(16) int luaXorMask[4];

	operator const double() const
	{
		return storage;
	}

	void operator=(const double& value)
	{
		__m128d xmmKey = _mm_load_pd((double*)(luaXorMask));
		__m128d xmmData = _mm_load_sd(&value);
		__m128d xmmResult = _mm_xor_pd(xmmData, xmmKey);
		storage = _mm_cvtsd_f64(xmmResult);
	}

	static void initDouble();

};

static double RBXNumber(double n)
{
	LuaSecureDouble d;
	d = n;
	return d;
}

// Utility class for obfuscating fields of primitive types
// WARNING: this will give incorrect results if T = float.
template <typename T> class LuaVMValue
{
public:
	operator const T() const
	{
		return (T)((uintptr_t)storage + reinterpret_cast<uintptr_t>(this));
	}

	void operator=(const T& value)
	{
		storage = (T)((uintptr_t)value - reinterpret_cast<uintptr_t>(this));
	}

	const T operator->() const
	{
		return operator const T();
	}

private:
	T storage;
};

typedef int(*r_luaC_function)(std::uintptr_t rL);

#define R_LUA_TLIGHTUSERDATA 1
#define R_LUA_TNUMBER 2
#define R_LUA_TBOOLEAN 3
#define R_LUA_TSTRING 4
#define R_LUA_TTHREAD 5
#define R_LUA_TFUNCTION 6
#define R_LUA_TTABLE 7
#define R_LUA_TUSERDATA 8
#define R_LUA_TPROTO 9
#define R_LUA_TUPVAL 10

static std::vector<const char*> r_luaT_typenames = { "nil", "pointer", "number", "bool", "string", "thread", "function", "table", "userdata", "proto", "upval" };

typedef DWORD* (__cdecl* r_S_newlstr)(std::uintptr_t L, const char* s, size_t len);
typedef std::uintptr_t(__cdecl* r_pushnil)(std::uintptr_t L);
typedef std::uintptr_t(__cdecl* r_sandbox)(std::uintptr_t L);
typedef std::uintptr_t(__cdecl* r_gettop)(std::uintptr_t L);
typedef std::uintptr_t(__cdecl* r_settop)(std::uintptr_t L, int idx);
typedef std::uintptr_t(__cdecl* r_setreadonly)(std::uintptr_t L, int idx, bool readonly);
typedef std::uintptr_t(__cdecl* r_spawn)(std::uintptr_t L);
typedef std::uintptr_t(__cdecl* r_newthread)(std::uintptr_t L);
typedef std::uintptr_t(__cdecl* r_getmetatable)(std::uintptr_t L, int idx);
typedef std::uintptr_t(__cdecl* r_setmetatable)(std::uintptr_t L, int idx);
typedef std::uintptr_t(__cdecl* r_getfield)(std::uintptr_t L, int idx, const char* field);
typedef std::uintptr_t(__cdecl* r_setfield)(std::uintptr_t L, int idx, const char* field);
typedef std::uintptr_t(__thiscall* r_getglobalstate)(std::uintptr_t sc, int identity);
typedef void(__cdecl* r_bridgepush)(std::uintptr_t L, std::shared_ptr<std::uintptr_t> instance);
typedef void* (__cdecl* r_topointer)(std::uintptr_t L, int idx);
typedef int(__cdecl* r_rawgeti)(std::uintptr_t L, int idx, int i);
typedef int(__cdecl* r_rawseti)(std::uintptr_t L, int idx, int i);
typedef const char* (__cdecl* r_tolstring)(std::uintptr_t L, int idx, size_t len);
typedef void(__cdecl* r_createtable)(std::uintptr_t L, int narr, int nrec);
typedef int(__cdecl* r_pcall)(std::uintptr_t state, int nargs, int nresults, int nerrfunc);
typedef int(__cdecl* r_pushthread)(std::uintptr_t thread);
typedef std::uintptr_t(__cdecl* r_pushcclosure)(std::uintptr_t L, int f, int n);
typedef std::uintptr_t(__cdecl* r_pushvalue)(std::uintptr_t L, int idx);
typedef int(__cdecl* r_luaVM_load)(std::uintptr_t state, const ProtectedString& source, const char* chunkname, unsigned int modkey);
typedef int(__cdecl* r_index2adr)(std::uintptr_t state, int idx);
typedef int(__cdecl* r_insert)(std::uintptr_t state, int idx);
typedef int(__cdecl* r_remove)(std::uintptr_t state, int idx);
typedef int(__cdecl* r_pushnewobject)(std::uintptr_t state, std::shared_ptr<std::uintptr_t> t);
typedef void(__thiscall* r_loadInstances)(int, std::istream& stream, Instances& result);
typedef void(__thiscall* r_ffireclickdetector)(std::uintptr_t cd, float dist, std::uintptr_t plr);
typedef bool(__cdecl* r_getInstance)(std::uintptr_t L, unsigned int index, std::uintptr_t& value);
typedef std::uintptr_t*(__thiscall* r_getPropDescriptor)(std::uintptr_t _this, const char* name);
typedef int(__cdecl* r_Lerror)(std::uintptr_t L, const char* fmt, ...);

typedef DWORD* (__cdecl* r_taskScheduler)();
typedef void(__thiscall* r_getJobsByName)(DWORD* ts, const std::string& name, std::vector<DWORD*>& result);

static r_taskScheduler taskScheduler = (r_taskScheduler)taskScheduler_a;
static r_getJobsByName getJobsByName = (r_getJobsByName)getJobsByName_a;
static r_gettop rbx_gettop = (r_gettop)rbx_gettop_address;
static r_settop rbx_settop = (r_settop)rbx_settop_address;
static r_rawgeti rbx_rawgeti = (r_rawgeti)rbx_rawgeti_address;
static r_rawseti rbx_rawseti = (r_rawseti)rbx_rawseti_address;
static r_createtable rbx_createtable = (r_createtable)rbx_createtable_address;
static r_getfield rbx_getfield = (r_getfield)rbx_getfield_address;
static r_getmetatable rbx_getmetatable = (r_getmetatable)rbx_getmetatable_address;
static r_setmetatable rbx_setmetatable = (r_setmetatable)rbx_setmetatable_address;
static r_setfield rbx_setfield = (r_setfield)rbx_setfield_address;
static r_newthread rbx_newthread = (r_newthread)rbx_newthread_address;
static r_spawn rbx_spawn = (r_spawn)rbx_spawn_address;
static r_pcall rbx_pcall = (r_pcall)rbx_pcall_address;
static r_pushthread rbx_pushthread = (r_pushthread)rbx_pushthread_address;
static r_pushnil rbx_pushnil = (r_pushnil)rbx_pushnil_address;
static r_topointer rbx_topointer = (r_topointer)rbx_topointer_address;
static r_tolstring rbx_tolstring = (r_tolstring)rbx_tolstring_address;
static r_luaVM_load rbx_luaVM_load = (r_luaVM_load)rbx_luaVM_load_address;
static r_bridgepush rbx_bridgepush = (r_bridgepush)rbx_bridgePush_address;
static r_loadInstances rbx_LoadInstances = (r_loadInstances)rbx_loadinstances_address;
static r_pushcclosure rbx_pushcclosure = (r_pushcclosure)rbx_pushcclosure_address;
static r_pushvalue rbx_pushvalue = (r_pushvalue)rbx_pushvalue_address;
static r_setreadonly rbx_setreadonly = (r_setreadonly)rbx_setreadonly_address;
static r_Lerror rbx_luaL_error = (r_Lerror)rbx_Lerror_address;
static r_index2adr rbx_index2adr = (r_index2adr)rbx_index2adr_address;
static r_insert rbx_insert = (r_insert)rbx_insert_address;
static r_S_newlstr rbx_luaS_newlstr = (r_S_newlstr)S_newlstr_address;
static r_sandbox rbx_sandboxThread = (r_sandbox)sandboxThread_address;
static r_ffireclickdetector rbx_fireclickdetector = (r_ffireclickdetector)rbx_fireclickdetector_address;
static r_getInstance rbx_getInstance = (r_getInstance)rbx_getinstance_address;
static r_getPropDescriptor rbx_findPropertyDescriptor = (r_getPropDescriptor)rbx_findPropertyDescriptor_Address;

static int rbx_type(std::uintptr_t rL, int idx)
{
	int t;
	TValue* r = (TValue*)(rbx_index2adr(rL, idx));

	if (r)
		t = r->tt;

	if (t == -1)
		t = 0;

	return t;
}

#endif