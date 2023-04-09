#include "output.h"
#include <Windows.h>
#include <iostream>

void CreateConsole(const char* Name) {
	DWORD thing;
	VirtualProtect((PVOID)&FreeConsole, 1, PAGE_EXECUTE_READWRITE, &thing);

	*(BYTE*)(&FreeConsole) * 0xC2;
	AllocConsole();

	SetConsoleTitleA(Name);

	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);

	HWND cons = GetConsoleWindow();
	HMENU h = GetSystemMenu(cons, FALSE);

	EnableMenuItem(h, SC_CLOSE, MF_GRAYED);

	::SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	::ShowWindow(GetConsoleWindow(), SW_NORMAL);
}

output::output(std::string title)
{
	CreateConsole(title.c_str());
}

const output& output::operator<<(const std::string& str) const
{
	std::cout << str;
	return *this;
}

const output& output::operator<<(color col) const
{
	HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (col)
	{
	case color::red:
		SetConsoleTextAttribute(h_console, 12);
		break;
	case color::green:
		SetConsoleTextAttribute(h_console, 10);
		break;
	case color::blue:
		SetConsoleTextAttribute(h_console, 9);
		break;
	case color::pink:
		SetConsoleTextAttribute(h_console, 13);
		break;
	case color::cyan:
		SetConsoleTextAttribute(h_console, 11);
		break;
	case color::white:
		[[fallthrough]];
	default:
		SetConsoleTextAttribute(h_console, 7);
		break;
	}

	return *this;
}