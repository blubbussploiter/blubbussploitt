#include "serializer.h"
#include "stateg.h"

#include "encoding.h"

#include "rproto.h"

#include <map>

#define CURRENT_THREAD_USES_MAX 8

#define CHECKARGS(rL, args) if(rbx_gettop(rL) < args) return rbx_luaL_error(rL, "%s does not take %d arguments", __FUNCTION__, rbx_gettop(rL))
#define CHECKARGSTYPE(rL, args, t) {CHECKARGS(rL, args); for(int i = 1; i <= args; ++i) if(rbx_type(rL, i) != t) return rbx_luaL_error(rL, "%s does not take argument of type %s at %d", __FUNCTION__, r_luaT_typenames.at(rbx_type(rL, i)), i);}
#define CHECKARGTYPE(rL, arg, t) {CHECKARGS(rL, arg); if(rbx_type(rL, arg) != t) return rbx_luaL_error(rL, "%s does not take argument of type %s at %d", __FUNCTION__, r_luaT_typenames.at(rbx_type(rL, arg)), arg);}
#define BASICERR(rL, err, ...) return rbx_luaL_error(rL, "%s %s", __FUNCTION__, err, __VA_ARGS__);

namespace RBX
{

	extern bool isExploitReady;

	extern int datamodelNewIndex(std::uintptr_t rL);

	namespace Lua
	{

		static void pushstring(std::uintptr_t rL, const char* str)
		{
			TValue*& topstack = *reinterpret_cast<TValue**>(rL + 16);

			topstack->tt = R_LUA_TSTRING;
			topstack->value.gc = (GCObject*)rbx_luaS_newlstr(rL, str, strlen(str));

			(topstack)++;
		}

		static void pushnumber(std::uintptr_t rL, double n)
		{
			TValue*& topstack = *reinterpret_cast<TValue**>(rL + 16);

			topstack->tt = R_LUA_TNUMBER;
			topstack->value.n = RBXNumber(n);

			(topstack)++;
		}

		static void pushboolean(std::uintptr_t rL, bool b)
		{
			TValue*& topstack = *reinterpret_cast<TValue**>(rL + 16);

			topstack->tt = R_LUA_TBOOLEAN;
			topstack->value.b = b;

			(topstack)++;
		}

		static void pushclosure(std::uintptr_t rL, RClosure* cl)
		{
			TValue*& topstack = *reinterpret_cast<TValue**>(rL + 16);

			topstack->tt = R_LUA_TFUNCTION;
			topstack->value.gc = (GCObject*)cl;
		
			(topstack++);
		}


		static void pushcfunction(std::uintptr_t exploitThread, r_luaC_function cl, int n)
		{
			RClosure* rcl = reinterpret_cast<RClosure * (__cdecl*)(std::uintptr_t, int, std::uintptr_t)>(rbx_newcclosure)(exploitThread, n, rbx_index2adr(exploitThread, -10002));

			rcl->c.f = cl;
			*(DWORD*)(exploitThread + 16) -= sizeof(TValue) * n; /* decrement by n (upvalues)*/

			while (n--)
				setobj2s(0, &rcl->c.upvalue[n], *(TValue**)(exploitThread + 16) + n); /* set in upvalues */

			RBX::Lua::pushclosure(exploitThread, rcl);
		}

		static void pushraw(std::uintptr_t rL, TValue* o)
		{
			TValue*& topstack = *reinterpret_cast<TValue**>(rL + 16);
			topstack->tt = o->tt;
			topstack->value = o->value;
			(topstack)++;
		}
		
		static double tonumber(std::uintptr_t rL, int idx)
		{
			TValue* o = (TValue*)rbx_index2adr(rL, idx);
			if (o->tt != R_LUA_TNUMBER) return 0;
			return RBXNumber(o->value.n);
		}

		static int tointeger(std::uintptr_t rL, int idx)
		{
			return int(tonumber(rL, idx));
		}

		static bool toboolean(std::uintptr_t rL, int idx)
		{
			TValue* o = (TValue*)rbx_index2adr(rL, idx);
			if (o->tt != R_LUA_TBOOLEAN) return 0;
			return o->value.b;
		}

		static void* touserdata(std::uintptr_t rL, int idx)
		{
			TValue* o = (TValue*)rbx_index2adr(rL, idx);
			switch (o->tt)
			{
			case R_LUA_TUSERDATA: return (rawuvalue(o) + 1);
			case R_LUA_TLIGHTUSERDATA: return pvalue(o);
			default: return NULL;
			}
		}

	}

	namespace Execution
	{

		class DatamodelHandler;

		extern void setup();

		static void setOldIndexAtTop(std::uintptr_t rL, std::uintptr_t oldIndex)
		{
			RBX::Lua::pushclosure(rL, (RClosure*)oldIndex);
		}

		class DatamodelHandler
		{
		private:

			/* Cclosure, event name */

			std::map<std::uintptr_t, std::string> datamodelEvents;

		public:

			/* Pushes datamodel event */

			bool pushDatamodelEvent(std::uintptr_t rL, std::string name)
			{
				for (auto it : datamodelEvents)
				{
					std::uintptr_t f = it.first;
					std::string n = it.second;
					
					if (n == name)
					{
						Lua::pushcfunction(rL, (r_luaC_function)f, 0);
						return true;
					}
				}
				return false;
			}

			/* Creates new datamodel event for name */

			void newDatamodelEvent(int f, std::string name)
			{
				datamodelEvents.insert(std::pair<std::uintptr_t, std::string>(f, name));
			}

		};

		class Executor
		{
		private:

			LuaSerializer* s;

			std::uintptr_t rL;

			std::uintptr_t scriptContext;
			DatamodelHandler* datamodeler;

			int globalState_uses = 0;

			/* Pseudo LocalScript (for script global) */
			TValue* PseudoLocalScript;

		public:

			lua_State* L;

			/* Runs Script on new thread (RunningThread) */
			void runScript(std::string script);
			/* Pushes a cfunction onto stack */
			void pushCFunction(int f, unsigned int tL = 0, int n = 0);
			/* Registers a cfunction (on handler) as a global */
			void registerCf(int f, const char* g);
			/* Sets thread identity */
			void setidentity(std::uintptr_t L, int i);
			/* Registers new __index for game, (for httpget, getobjects) */
			void registerGameIndex();
			/* Fixes loadasset for execution */
			void fixLoadAsset();
			/* Get identity */
			DWORD getidentity(std::uintptr_t L);

			std::string generateBytecodeHash(std::string bytecode)
			{
				unsigned int hash = XXH32(&bytecode[0], bytecode.size(), kBytecodeHashSeed);
				return std::to_string(hash);
			}

			std::uintptr_t getExploitThread() { return rL; }

			/* Gets datamodel handler or creates it */

			DatamodelHandler* getDatamodelHandler()
			{
				if (!datamodeler)
					datamodeler = new DatamodelHandler();
				return datamodeler;
			}

			/* Returns serialized script */
			
			std::string rawSerialize(std::uintptr_t tL, std::string script)
			{
				std::string _script = s->serialize(L, script, RBX::Encoding::encode, RBX::getEncode(tL));
				return _script;
			}

			Executor(std::uintptr_t eT)
			{
				rL = eT;
				L = lua_open();

				s = new LuaSerializer();

				scriptContext = GET_SCRIPTCONTEXT();
			}
		};
		
		extern Executor* executorSingleton(std::uintptr_t t=0);
	}
}