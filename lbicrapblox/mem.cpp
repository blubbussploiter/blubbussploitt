#include "mem.h"
#include <limits.h>
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

#define WRITABLE (PAGE_READWRITE | PAGE_WRITECOPY |PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)

// Memory Utility

int MemoryUtility::WriteNOPs(int dAddress, byte dSize) {
	if (dSize > 0) {
		byte patch[UCHAR_MAX];
		memset(patch, 0x90, dSize);
		return write_memory(dAddress, patch, dSize);
	}
	return 1;
}

int MemoryUtility::CodeInjection(int destAddress, byte nopCount, void(*func)(void), int funcLength, byte saveInjection, CODEINJ_t type, PatchInformation** pPatchInfo) {
	int offset;
	if (saveInjection) {
		void* allocf = VirtualAlloc(NULL, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		if (!funcLength) {
			byte* iter = (byte*)func;

			while (*iter != 0xC3)
				++iter;

			++iter;
			memcpy(allocf, func, iter - (byte*)func);
		}
		else {
			memcpy(allocf, func, funcLength);
		}

		offset = (PtrToUlong(allocf) - destAddress) - 5;
	}
	else {
		offset = (PtrToUlong(func) - destAddress) - 5;
	}

	byte patch[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 }; // E8 = call, E9 = jmp 64 bit relative
	patch[0] = type == JUMPHOOK ? 0xE9 : 0xE8;
	memcpy(patch + 1, &offset, sizeof(int));
	return WriteMemory(destAddress, patch, 5, nopCount, !saveInjection, pPatchInfo);
}

int MemoryUtility::WriteMemory(int dAddress, const void* patch, int dSize, byte nopCount, byte doDiscard, PatchInformation** pPatchInfo) {
	int totalSize = dSize + nopCount;

	if (doDiscard) {
		PatchInformation patchInfo;
		byte* originalMemory = new byte[totalSize];

		memcpy(originalMemory, (void*)dAddress, totalSize);
		patchInfo.Address = (void*)dAddress;
		patchInfo.Memory = originalMemory;
		patchInfo.Size = totalSize;

		patch_list->push_back(patchInfo);

		if (pPatchInfo)
			*pPatchInfo = &patch_list->back();
	}

	if (write_memory(dAddress, patch, dSize)) {
		return WriteNOPs(dAddress + dSize, nopCount);
	}

	return 0;
}

int MemoryUtility::write_memory(int dAddress, const void* patch, int dSize) {
	int oldProtect;
	if (VirtualProtect((LPVOID)dAddress, dSize, PAGE_EXECUTE_READWRITE, (PDWORD)&oldProtect)) {
		for (int i = 0; i < dSize; i++)
			((unsigned char*)dAddress)[i] = ((unsigned char*)patch)[i];
		return VirtualProtect((LPVOID)dAddress, dSize, oldProtect, (PDWORD)&oldProtect);
	}
	return 0;
}

/* creates a detour and returns the trampoline function */
void* MemoryUtility::CreateDetour(void* targetf, void* detourf, int nopCount) {
	if (!targetf || !detourf)
		return nullptr;

	/* store old bytes */
	int bytecount = 5 + nopCount;
	byte* oldbytes = new byte[bytecount];
	memcpy(oldbytes, targetf, bytecount);

	/* create jmp to detour patch */
	int offset = PtrToUlong(detourf) - PtrToUlong(targetf) - 5;
	byte patch[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
	memcpy(patch + 1, &offset, sizeof(int));

	/* create trampoline function */
	void* trampolinef = VirtualAlloc(0, bytecount + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!trampolinef)
		return nullptr;

	/* write trampoline function */
	memcpy(trampolinef, oldbytes, bytecount);
	int trampoffset = PtrToUlong(targetf) - (PtrToUlong(trampolinef) + 5) - 5 + 5 - nopCount;
	byte tramppatch[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
	memcpy(tramppatch + 1, &trampoffset, sizeof(int));
	memcpy((void*)(PtrToUlong(trampolinef) + 5 + nopCount), tramppatch, 5);

	/* hook target function */
	write_memory(PtrToUlong(targetf), patch, 5);

	/* write nops (if necessary) */
	WriteNOPs(PtrToUlong(targetf) + 5, nopCount);

	/* create detour def and add to list */
	DetourDef def;
	def.targetf = targetf;
	def.detourf = detourf;
	def.trampolinef = trampolinef;
	def.oldbytes = oldbytes;
	def.bytecount = bytecount;

	detour_list->push_back(def);

	/* return trampoline */
	return trampolinef;
}

void MemoryUtility::destroy_detour(DetourDef* def) {
	/* free allocated trampoline function */
	VirtualFree(def->trampolinef, def->bytecount + 5, MEM_RELEASE);

	/* restore target function */
	write_memory((int)def->targetf, def->oldbytes, def->bytecount);

	/* free allocated old bytes */
	delete[] def->oldbytes;
}

void MemoryUtility::DestroyDetour(void* targetf) {
	for (std::vector<DetourDef>::iterator it = detour_list->begin(); it != detour_list->end(); it++) {
		if (it->targetf == targetf) {
			/* destroy */
			destroy_detour(&*it);

			/* erase detourdef */
			detour_list->erase(it);
			break;
		}
	}
}

int MemoryUtility::GetSegmentInformation(HMODULE hModule, const char* s_name, ImageSectionInfo* pSectionInfo) {
	memset(pSectionInfo, 0, sizeof(ImageSectionInfo));

	//store the base address the loaded Module
	int dllImageBase = (int)hModule; //suppose hModule is the handle to the loaded Module (.exe or .dll)

	//get the address of NT Header
	IMAGE_NT_HEADERS* pNtHdr = ImageNtHeader(hModule);

	//after Nt headers comes the table of section, so get the addess of section table
	IMAGE_SECTION_HEADER* pSectionHdr = (IMAGE_SECTION_HEADER*)(pNtHdr + 1);

	//iterate through the list of all sections, and check the section name in the if conditon. etc
	for (int i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++)
	{
		char* name = (char*)pSectionHdr->Name;
		if (strcmp(name, s_name) == 0)
		{
			pSectionInfo->Address = (void*)(dllImageBase + pSectionHdr->VirtualAddress);
			pSectionInfo->Size = pSectionHdr->Misc.VirtualSize;
			strcpy(pSectionInfo->Name, name);
			return 1;
		}
		pSectionHdr++;
	}
	return 0;
}

void* MemoryUtility::CopySegment(ImageSectionInfo& seg_info, int page_protection) {
	void* seg_cpy = VirtualAlloc(0, seg_info.Size, MEM_COMMIT | MEM_RESERVE, page_protection);

	if (seg_cpy) {
		memcpy(seg_cpy, seg_info.Address, seg_info.Size);

		return seg_cpy;
	}

	return 0;
}

MemoryUtility::MemoryUtility() {
	base_module = GetModuleHandle(NULL);
	patch_list = new std::vector < PatchInformation >;
	detour_list = new std::vector < DetourDef >;

	//AddVectoredExceptionHandler(true, VectoredHandler);
}

MemoryUtility::~MemoryUtility() {
	/* clean patch_list */
	for (int i = 0; i < patch_list->size(); i++) {
		PatchInformation patch_info = patch_list->at(i);
		WriteMemory((int)patch_info.Address, patch_info.Memory, patch_info.Size, 0, false);
		delete[] patch_info.Memory;
	}

	/* clean detour_list */
	for (std::vector<DetourDef>::iterator it = detour_list->begin(); it != detour_list->end(); it++) {
		destroy_detour(&*it);
	}

	delete patch_list;
	delete detour_list;
}

MemoryUtility lmemutil;
MemoryUtility* memutil = &lmemutil;