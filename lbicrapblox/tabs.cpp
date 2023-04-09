#include "ui.h"

#include <vector>
#include <format>
#include <iostream>
#include <sstream>

std::vector<UserInterface::Tabs::LuaTab*> lua_tabs{ new UserInterface::Tabs::LuaTab("print('Hello, World!')") }; /* initial tab */
int currentTab = 0;

void UserInterface::tabLogic()
{
	for (int i = 0; i < lua_tabs.size(); ++i)
		if (ImGui::TabItemButton(std::format("Tab {}", i).c_str()))
		{
			currentTab = i;
			Tabs::loadCurrentTab();
		}

	ImGui::SameLine();

	if (ImGui::Button("+"))
	{
		lua_tabs.push_back(new Tabs::LuaTab(""));
		Tabs::loadTabByIndex(lua_tabs.size()-1); /* load new tab */
	}
}

void UserInterface::Tabs::loadCurrentTab()
{
	if (!lua_tabs.at(currentTab))
		return;
	loadTab(lua_tabs.at(currentTab));
}

void UserInterface::Tabs::setCurrentTabText(std::string txt)
{
	if (!lua_tabs.at(currentTab))
		return; 

	lua_tabs.at(currentTab)->setlng(Tabs::loadLngSrc(txt));
}

void UserInterface::Tabs::loadTabByIndex(int idx)
{
	if (!lua_tabs.at(currentTab))
		return;
	currentTab = idx;
	loadTab(lua_tabs.at(idx));
}