
#include <memory>
#include "addresses.h"

#include <istream>
#include <sstream>

namespace RBX
{
	namespace Reflection
	{
		void pushObject(std::uintptr_t rL, std::shared_ptr<std::uintptr_t> obj);
	}
}