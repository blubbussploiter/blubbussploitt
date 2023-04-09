#include "addresses.h"
#include "hooking.h"

static DWORD* getJobByName(std::string name)
{
	DWORD* ts = taskScheduler();
	std::vector<DWORD*> jobs{};

	getJobsByName(ts, name, jobs);
	return jobs.at(0);
}