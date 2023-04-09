/* FUCK JAMIE */

#include "memcheck.h"

#include <cstdint>
#include "scheduler.h"

extern void detourDatamodelJobStep(const char* name, void* newStep, std::uintptr_t& oldStep);