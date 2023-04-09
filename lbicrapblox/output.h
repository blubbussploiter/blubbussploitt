#pragma once

/* 'borrowed' from headhunter */

#include <string>

void CreateConsole(const char* Name);

class output {
public:

	enum class color
	{
		red,
		green,
		blue,
		cyan,
		white,
		pink
	};

	output(std::string title);

	const output& operator<<(const std::string& str) const;
	const output& operator<<(color col) const;
};