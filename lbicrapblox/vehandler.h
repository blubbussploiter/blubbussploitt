
#include <Windows.h>
#include <vector>

struct VEHInfo {
	int b;
	int cclosure;
};

class VEHandler {
public:
	std::vector<VEHInfo*> breakpoints;

	int push(int cclosure);
	void start();

	DWORD find_int3();
};

extern VEHandler* globalelle;
extern void init_veh(VEHandler* ng);