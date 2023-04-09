#include "jhook.h"

void detourDatamodelJobStep(const char* name, void* newStep, std::uintptr_t& oldStep)
{
	VMProtectBegin("jobStepDetour");
	DWORD* job = getJobByName(name);

	void** vtable = new void* [8]; /* size of datamodeljob vtable */
	size_t vt_sz = 8;

	memcpy(vtable, *reinterpret_cast<void**>(job), sizeof(void*) * vt_sz);
	if(!oldStep)
		oldStep = reinterpret_cast<std::uintptr_t>(vtable[7]); /* last func, datamodeljobstep */

	vtable[7] = newStep;
	*reinterpret_cast<void***>(job) = vtable;

	printf("[log]: %s::Step hooked\n", name);
	VMProtectEnd();
}