
#include "customfuncs.h"
#include "execution.h"

#include "base64.h"

#include "hashing\crc32.h"
#include "hashing\sha1.h"
#include "hashing\sha3.h"
#include "hashing\sha256.h"
#include "hashing\keccak.h"
#include "hashing\md5.h"

#include <map>

static std::map<const char*, int> algorithms = 
{
	{"crc32", 1},
	{"sha1", 2},
	{"sha3", 3},
	{"sha256", 4},
	{"keccak", 5},
	{"md5", 6},
};

int RBX::CustomLibs::Crypt::r_crypt_b64encode(std::uintptr_t rL)
{
	CHECKARGSTYPE(rL, 1, R_LUA_TSTRING);

	const char* toEncode = rbx_tolstring(rL, 1, 0);
	Lua::pushstring(rL, macaron::Base64::Encode(toEncode).c_str());

	return 1;
}

int RBX::CustomLibs::Crypt::r_crypt_b64decode(std::uintptr_t rL)
{
	CHECKARGSTYPE(rL, 1, R_LUA_TSTRING);

	const char* toDecode = rbx_tolstring(rL, 1, 0);

	std::string out;
	macaron::Base64::Decode(toDecode, out);

	Lua::pushstring(rL, out.c_str());

	return 1;
}

int RBX::CustomLibs::Crypt::r_crypt_hash(std::uintptr_t rL)
{
	CHECKARGSTYPE(rL, 1, R_LUA_TSTRING);
	CHECKARGSTYPE(rL, 2, R_LUA_TSTRING);

	const char* algorithm = rbx_tolstring(rL, 1, 0);
	const char* to_hash = rbx_tolstring(rL, 2, 0);

	switch (algorithms.at(algorithm))
	{
		case 1: 
		{
			CRC32 f;
			Lua::pushstring(rL, f(to_hash).c_str());
			return 1;
		}
		case 2:
		{
			SHA1 f;
			Lua::pushstring(rL, f(to_hash).c_str());
			return 1;
		}
		case 3:
		{
			SHA3 f;
			Lua::pushstring(rL, f(to_hash).c_str());
			return 1;
		}
		case 4:
		{
			SHA256 f;
			Lua::pushstring(rL, f(to_hash).c_str());
			return 1;
		}
		case 5:
		{
			Keccak f;
			Lua::pushstring(rL, f(to_hash).c_str());
			return 1;
		}
		case 6:
		{
			MD5 f;
			Lua::pushstring(rL, f(to_hash).c_str());
			return 1;
		}
		default: rbx_pushnil(rL);
	}

	return 1;
}

void RBX::CustomLibs::r_open_crypt_lib(std::uintptr_t rL)
{
	rbx_createtable(rL, 0, 0); /* Create baselib */

	/* Populate library */

	RBX::Execution::executorSingleton()->pushCFunction((int)Crypt::r_crypt_b64encode, rL);
	rbx_setfield(rL, -2, "b64encode");

	RBX::Execution::executorSingleton()->pushCFunction((int)Crypt::r_crypt_b64decode, rL);
	rbx_setfield(rL, -2, "b64decode");

	RBX::Execution::executorSingleton()->pushCFunction((int)Crypt::r_crypt_hash, rL);
	rbx_setfield(rL, -2, "hash");

	rbx_setreadonly(rL, -1, 1); /* Protect it */
	rbx_setfield(rL, -10002, "crypt");
}