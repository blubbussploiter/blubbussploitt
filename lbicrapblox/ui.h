
#include <Windows.h>
#define WIN_NAME L"Crapblox"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"

#include <d3d11.h>
#include <cstdint>

#include <string>
#include <vector>
#include <sstream>

#include <D3Dcompiler.h>

#pragma comment(lib, "D3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")

#define EXECUTOR_TAB_CURRENT 0
#define OUTPUT_TAB_CURRENT 1

namespace UserInterface
{

	extern bool shouldRender;

	namespace Tabs
	{

		static std::vector<std::string> loadLngSrc(std::string txt)
		{
			std::istringstream iss(txt);
			std::vector<std::string> fixed;

			for (std::string line; std::getline(iss, line); )
			{
				fixed.push_back(line);
			}
			return fixed;
		}

		class LuaTab
		{
		public:
			std::vector<std::string> lng_src;
			void setlng(std::vector<std::string> lng) { lng_src = lng; }
			LuaTab(std::string _Src) { lng_src = loadLngSrc(_Src); }
		};

		extern void loadTab(LuaTab* t);
		extern void loadCurrentTab();
		extern void setCurrentTabText(std::string txt);
		extern void loadTabByIndex(int idx);
	}

	namespace Lua
	{
		class Output
		{
		public:

			std::time_t at;

			int type;
			const char* out;

			Output(const char* o, int t) 
			{ 
				type = t; out = o; 
				at = std::time(0);
			}
		};

		enum OutputTypes
		{
			print,
			warn,
			error
		};

		static Output* makePrint(const char* o) { return new Output(o, OutputTypes::print); }
		static Output* makeError(const char* o) { return new Output(o, OutputTypes::error); }
		static Output* makeWarn(const char* o) { return new Output(o, OutputTypes::warn); }

		extern std::vector<Output*> logged_output;
		extern void Out(OutputTypes o, const char* fmt, ...);
	}

	extern void init();
	extern void renderUi();
	extern void renderOutput();
	extern void renderExecutor();
	extern void setMainTabSelected(int t);
	extern void tabLogic();
}