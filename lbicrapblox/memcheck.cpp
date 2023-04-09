#include "memcheck.h"
#include "mem.h"
#include "VMProtectSDK.h"
#include <Windows.h>
#include <vector>

// Memory Checker Bypass 

const int gNopCount = 5;
int gHookLocation = 0x1469D6;
int gHookReturn = 0x146A41;

int gSegmentBase;
int gSegmentEnd;
int gSegmentCopy;

int GetCopyDifference(int base) {
	VMProtectBeginMutation("mem");
	if (base > gSegmentBase && base + 16 < gSegmentEnd)
		return gSegmentCopy - gSegmentBase;

	VMProtectEnd();
	return 0;
}

int gSCopy; /* esp-4 copy */
int gSPCopy; /* stack pointer copy */

void __declspec(naked) Hook() {
	__asm {
		sub		esp, 4
		pop		gSCopy

		push	ecx
		push	edx

		push	esi
		call	GetCopyDifference
		add		esp, 4

		pop		edx
		pop		ecx

		mov		gSPCopy, esp
		mov		esp, eax

		mov     eax, [esp + esi]
		add     eax, esi
		imul    eax, 1594FE2Dh
		add     edi, eax
		lea     eax, [esi + 4]
		sub     eax, [esp + esi + 4]
		rol     edi, 13h
		imul    eax, 344B5409h
		imul    edi, 0CBB4ABF7h
		add     eax, [ebp - 14h]
		add     esi, 8
		rol     eax, 11h
		imul    eax, 1594FE2Dh
		mov[ebp - 14h], eax
		mov     eax, [esp + esi]
		xor eax, esi
		imul    eax, 1594FE2Dh
		add     eax, [ebp - 10h]
		add     esi, 4
		rol     eax, 0Dh
		imul    eax, 0CBB4ABF7h
		mov[ebp - 10h], eax
		mov     eax, [esp + esi]
		sub     eax, esi
		imul    eax, 344B5409h
		add     ebx, eax
		rol     ebx, 0Fh
		add     esi, 4
		imul    ebx, 1594FE2Dh

		mov		esp, gSPCopy

		push	gSCopy
		add		esp, 4

		cmp     esi, ecx
		jb      Hook

		jmp		gHookReturn
	}
}

void BypassMemoryChecker() {
	gHookReturn = memutil->offset(gHookReturn);
	gHookLocation = memutil->offset(gHookLocation);

	ImageSectionInfo segment;
	if (!memutil->GetSegmentInformation(GetModuleHandle(NULL), ".text", &segment))
		throw std::exception("failed to fetch segment information");

	gSegmentCopy = (int)VirtualAlloc(0, segment.Size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!gSegmentCopy)
		throw std::exception("failed to allocate memory");

	gSegmentBase = (int)segment.Address;
	gSegmentEnd = gSegmentBase + segment.Size;

	memcpy((void*)gSegmentCopy, (void*)gSegmentBase, segment.Size);

	memutil->CodeInjection(gHookLocation, 0, Hook, 0, false, JUMPHOOK);

	// hash code is around 100 bytes
	char junk[80];

	for (int i = 0; i < sizeof(junk); i++)
		junk[i] = rand();

	memutil->WriteMemory(gHookLocation + 5, junk, sizeof(junk), 0, true);
}