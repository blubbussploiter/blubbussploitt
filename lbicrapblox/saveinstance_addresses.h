#include "addresses.h"

/* just to keep it nice & tidy */

static unsigned int r_serializerV2_newRootElement = shift(0x714760); /* "http://www.w3.org/2005/05/xmlmime" */
static unsigned int r_instance_writeValue = shift(0x574060); /* 0 file: v8tree\\Instance.cpp line: */

static unsigned int r_getRootAncestor = shift(0x561B60); /* "script->getRootAncestor() == getRootAncestor()" */

typedef unsigned int(__cdecl* r_SerializerV2_newRootElement)(const std::string& type);
typedef int(__thiscall* r_Instance_writeValue)(std::uintptr_t _this, int a2, int a3);

r_SerializerV2_newRootElement rbx_SerializerV2_newRootElement = (r_SerializerV2_newRootElement)r_serializerV2_newRootElement;
r_Instance_writeValue rbx_Instance_writeXml = (r_Instance_writeValue)r_instance_writeValue;