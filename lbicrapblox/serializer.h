#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lobject.h"
#include "lstring.h"
#include "lfunc.h"
#include "ldo.h"
#include "lopcodes.h"
}

#include "util/xxhash.h"

#include "../lz4/lz4.h"
#include "../lz4/lz4hc.h"

static const char kBytecodeMagic[] = "RSB1";
static const unsigned int kBytecodeHashSeed = 42;
static const unsigned int kBytecodeHashMultiplier = 41;

typedef unsigned int (*RbxOpEncoder)(unsigned int i, int pc, unsigned key);


enum BytecodeConstantType
{
    Constant_Nil,
    Constant_False,
    Constant_True,
    Constant_Number,
    Constant_String,
};

class LuaSerializer {
public:
    std::string serialize(lua_State* L, const std::string& source, RbxOpEncoder encode, int ckey);
private:
    struct LoadS
    {
        const char* s;
        size_t size;
    };
    static const char* getS(lua_State* L, void* ud, size_t* size)
    {
        LoadS* ls = (LoadS*)ud;
        (void)L;
        if (ls->size == 0) return NULL;
        *size = ls->size;
        ls->size = 0;
        return ls->s;
    }
    /* writing */
    static void writeByte(std::ostringstream& ss, unsigned char value)
    {
        //std::cout << "[#log]: writing byte(" << (int)value << "), at " << ss.tellp() << "\n";
        ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static void writeInt(std::ostringstream& ss, int value)
    {
        //std::cout << "[#log]: writing int(" << value << "), at " << ss.tellp() << "\n";
        ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static void writeDouble(std::ostringstream& ss, double value)
    {
        //std::cout << "[#log]: writing dbl(" << value << "), at " << ss.tellp() << "\n";
        ss.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static void writeString(std::ostringstream& ss, std::map<const TString*, unsigned int>& strings, const TString* string) {
        if (string) {
            unsigned int& index = strings[string];
            if (index == 0)
                index = strings.size();
            writeInt(ss, index);
        }
        else {
            writeInt(ss, 0);
        }
    }

    void writeRoot(std::ostringstream& ss, Proto* p, RbxOpEncoder encode, unsigned int ckey);
    void writeProto(std::ostringstream& ss, std::map<const TString*, unsigned int>& strings, RbxOpEncoder encode, Proto* p, unsigned int ckey);
};