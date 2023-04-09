
#include "addresses.h"
#include <cstdint>

struct InstructionV
{
	lu_byte v;
};

#define LUAVM_SHUFFLE2(sep,a0,a1) a1 sep a0
#define LUAVM_SHUFFLE3(sep,a0,a1,a2) a1 sep a2 sep a0
#define LUAVM_SHUFFLE4(sep,a0,a1,a2,a3) a3 sep a1 sep a0 sep a2
#define LUAVM_SHUFFLE5(sep,a0,a1,a2,a3,a4) a4 sep a0 sep a2 sep a1 sep a3
#define LUAVM_SHUFFLE6(sep,a0,a1,a2,a3,a4,a5) a3 sep a5 sep a2 sep a0 sep a1 sep a4
#define LUAVM_SHUFFLE7(sep,a0,a1,a2,a3,a4,a5,a6) a2 sep a3 sep a0 sep a4 sep a6 sep a1 sep a5
#define LUAVM_SHUFFLE8(sep,a0,a1,a2,a3,a4,a5,a6,a7) a7 sep a0 sep a5 sep a6 sep a3 sep a1 sep a2 sep a4
#define LUAVM_SHUFFLE9(sep,a0,a1,a2,a3,a4,a5,a6,a7,a8) a2 sep a6 sep a4 sep a7 sep a1 sep a8 sep a0 sep a3 sep a5

typedef struct RProto {
	CommonHeader;

	LUAVM_SHUFFLE9(; ,
		LuaVMValue<TValue*> k,  /* constants used by the function */
		LuaVMValue<InstructionV*> code,
		LuaVMValue<struct Proto**> p,  /* functions defined inside the function */
		LuaVMValue<int*> lineinfo,  /* map from opcodes to source lines */
		LuaVMValue<int*> locvars,  /* information about local variables */
		LuaVMValue<int**> upvalues,  /* upvalue names */
		LuaVMValue<int*> source,
		int sizeupvalues,
		int sizek);  /* size of `k' */

	LUAVM_SHUFFLE7(; ,
		int sizecode,
		int sizelineinfo,
		int sizep,  /* size of `p' */
		int sizelocvars,
		int linedefined,
		int lastlinedefined,
		GCObject* gclist);

	LUAVM_SHUFFLE4(; ,
		lu_byte nups,  /* number of upvalues */
		lu_byte numparams,
		lu_byte is_vararg,
		lu_byte maxstacksize);
} RProto;


#define RClosureHeader \
	CommonHeader; lu_byte isC; lu_byte nupvalues; GCObject *gclist; \
	struct Table *env

typedef struct RCClosure {
	RClosureHeader;
	LuaVMValue<r_luaC_function> f;
	TValue upvalue[1];
} RCClosure;


typedef struct RLClosure {
	RClosureHeader;
	LuaVMValue<struct RProto*> p;
	UpVal* upvals[1];
} RLClosure;


typedef union RClosure {
	RCClosure c;
	RLClosure l;
} RClosure;