#include "serializer.h"
#include "r_opcodes.h"

std::string LuaSerializer::serialize(lua_State* L, const std::string& source, RbxOpEncoder encode, int ckey)
{
	LoadS ls = { source.c_str(), source.size() };
	std::ostringstream ss;

	// write basic data

	int error = lua_load(L, getS, &ls, "=");
	writeByte(ss, error);

	if (error != 0) {

		const char* err = lua_tostring(L, -1);
		size_t length = err ? strlen(err) : 0;

		writeInt(ss, length);
		ss.write(err, length);

		lua_pop(L, 1);
	}
	else {
		const LClosure* cl = static_cast<const LClosure*>(lua_topointer(L, -1));
		
		writeRoot(ss, cl->p, encode, ckey);
		lua_pop(L, 1);
	}

	std::string ssdata = ss.str();

	// compress

	int dataSize = ssdata.size();
	int maxSize = LZ4_compressBound(dataSize);

	std::vector<char> compressed(maxSize);
	int compressedSize = LZ4_compress(ssdata.c_str(), &compressed[0], dataSize);

	// prepend magic

	std::string result = kBytecodeMagic;
	result.append(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
	result.append(&compressed[0], compressedSize);

	// compute xxhash

	unsigned int hash = XXH32(&result[0], result.size(), kBytecodeHashSeed);

	// xor data with the hash

	unsigned char hashbytes[4];
	memcpy(hashbytes, &hash, sizeof(hash));

	for (size_t i = 0; i < result.size(); ++i)
		result[i] ^= hashbytes[i % 4] + i * kBytecodeHashMultiplier;

	return result;
}

void LuaSerializer::writeRoot(std::ostringstream& ss, Proto* p, RbxOpEncoder encode, unsigned int ckey)
{
	unsigned int streamStart = ss.tellp();

	// marker for string table offset - will be patched later
	writeInt(ss, 0);

	std::map<const TString*, unsigned int> strings;

	// write prototype and fill string table
	writeProto(ss, strings, encode, p, ckey);

	// write string table
	unsigned int stringTableOffset = ss.tellp();

	std::vector<const TString*> stringTable(strings.size());
	
	for (std::map<const TString*, unsigned int>::const_iterator it = strings.begin(); it != strings.end(); ++it)
		stringTable[it->second - 1] = it->first;

	writeInt(ss, strings.size());

	for (size_t i = 0; i < stringTable.size(); ++i)
		writeInt(ss, stringTable[i]->tsv.len);

	for (size_t i = 0; i < stringTable.size(); ++i)
		ss.write(getstr(stringTable[i]), stringTable[i]->tsv.len);

	ss.seekp(streamStart);
	writeInt(ss, stringTableOffset - streamStart);
}

void LuaSerializer::writeProto(std::ostringstream& ss, std::map<const TString*, unsigned int>& strings, RbxOpEncoder encode, Proto* p, unsigned int ckey)
{

	writeInt(ss, p->sizep);
	writeInt(ss, p->sizek);
	writeInt(ss, p->sizecode);
	writeInt(ss, p->sizelocvars);
	writeInt(ss, p->sizelineinfo);
	writeInt(ss, p->sizeupvalues);

	writeByte(ss, p->maxstacksize);
	writeByte(ss, p->is_vararg);
	writeByte(ss, p->nups);
	writeByte(ss, p->numparams);

	for (int i = 0; i < p->sizek; ++i) {
		const TValue* o = &p->k[i];

		switch (ttype(o)) {
		case LUA_TNIL: 
			//printf("[#log]: writing nil\n");
			writeByte(ss, Constant_Nil);
			break;
		case LUA_TBOOLEAN:
			//printf("[#log]: writing true or false\n");
			writeByte(ss, bvalue(o) ? Constant_True : Constant_False); 
			break;
		case LUA_TNUMBER:
			//printf("[#log]: writing number\n");
			writeByte(ss, Constant_Number);
			writeDouble(ss, nvalue(o));
			break;
		case LUA_TSTRING:
			writeByte(ss, Constant_String);
			writeString(ss, strings, rawtsvalue(o));
			break;
		default:
			lua_assert(0);
			break;
		}
	}

	int lastLine = 0;

	for (int i = 0; i < p->sizelineinfo; ++i) 
	{
		// the lines were previously encoded.
		writeInt(ss, p->lineinfo[i] - lastLine);

		lastLine = p->lineinfo[i];
	}

	for (int i = 0; i < p->sizelocvars; ++i)
	{
		const struct LocVar* l = &p->locvars[i];

		writeInt(ss, l->startpc);
		writeInt(ss, l->endpc);	
		writeString(ss, strings, l->varname);
	}

	for (int i = 0; i < p->sizeupvalues; ++i)
		writeString(ss, strings, p->upvalues[i]);
	
	for (int i = 0; i < p->sizecode; ++i) 
	{
		writeInt(ss, encode(p->code[i], i, ckey));
	}

	for (int i = 0; i < p->sizep; ++i)
		writeProto(ss, strings, encode, p->p[i], ckey);

}
