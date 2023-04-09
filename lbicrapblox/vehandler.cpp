#include "vehandler.h"
#include <iostream>

VEHandler* globalelle;

void init_veh(VEHandler* ng) {
    globalelle = ng;
}

VEHInfo* get(DWORD eip)
{

    if (globalelle == NULL)
        return 0;

    for (VEHInfo* inf : globalelle->breakpoints)
        if (inf->b == eip)
            return inf;

    return 0;
}

LONG __stdcall handler(PEXCEPTION_POINTERS exception)
{
	switch (exception->ExceptionRecord->ExceptionCode)
	{
	case (DWORD)0x80000003L:
	{
        VEHInfo* got = get(exception->ContextRecord->Eip);

        if (got != nullptr) {
            exception->ContextRecord->Eip = (DWORD)(got->cclosure);
            return EXCEPTION_CONTINUE_EXECUTION;
        }

        return -1;
	}
    default: { return -1; }
	}
	return 0;
}

int VEHandler::push(int cclosure)
{
    VEHInfo* inf = new VEHInfo();
    DWORD int3 = find_int3();

    inf->b = int3;
    inf->cclosure = cclosure;
    
    breakpoints.push_back(inf);

    return int3;
}

DWORD VEHandler::find_int3()
{

    DWORD base = (0x400000 - 0x400000 + reinterpret_cast<unsigned int>(GetModuleHandleA(0)));
    const char i3_8opcode[8] = { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };
    
    for (int i = 0; i < INT_MAX; i++) {

        if (memcmp((void*)(base + i), i3_8opcode, sizeof(i3_8opcode)) == 0) {
            return (base + i);
        }

    }

    return 0;

}

void VEHandler::start()
{
    printf("starting handler..\n");
    //AddVectoredExceptionHandler(0, &handler);
    printf("Started handler\n");
}