#include "auth.h"
#include "interwebs.h"

#include "files.h"
#include "hashing/sha256.h"

#include <format>
#include <iostream>
#include <filesystem>

HMODULE dll;

void Auth::Authorizer::authorize()
{
	VMProtectBeginMutation(NULL);

	std::string _key = gethash();

	std::string formatted = std::format("{}", _key);
	std::string e = DownloadURL(formatted.c_str());

	try
	{
		if (std::stoi(e) != 200)
		{
			MessageBox(NULL, "Authorization failed!", "Access denied", NULL);
			reinterpret_cast<int(__cdecl*)(int)>(0x54CDB0)(0);
			return;
		}
		else
			return;
		exit(0);
	}
	catch (std::exception)
	{
		MessageBox(NULL, "Authorization failed!", "Access denied", NULL);
		reinterpret_cast<int(__cdecl*)(int)>(0x54CDB0)(0);
		return;
	}

	VMProtectEnd();
}

std::string Auth::Authorizer::gethwid()
{
	HW_PROFILE_INFO hwProfileInfo;

	if (GetCurrentHwProfile(&hwProfileInfo) != NULL)
		return hwProfileInfo.szHwProfileGuid;
}

std::string Auth::Authorizer::gethash()
{
	SHA256 sha;
	return sha(gethwid());
}
