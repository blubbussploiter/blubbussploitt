
#include <string>
#include <sstream>
#include <vector>

#include "vmprotectsdk.h"

#define DLL_NAME "lbicrapblox.dll"

extern HMODULE dll;

namespace Auth
{
	class Authorizer
	{
	public:

		std::string gethwid();
		std::string gethash();
		void authorize();
	};
}