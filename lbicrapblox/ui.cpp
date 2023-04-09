#include "ui.h"
#include "execution.h"

#include "extra/TextEditor.h"

#include <vector>
#include <format>

std::string currentScript;

TextEditor* editor;
ImVec2 winSize = ImVec2(640, 480);

bool firstStart = 1;
int mainTabCurrent = 0;

void UserInterface::Tabs::loadTab(UserInterface::Tabs::LuaTab* t)
{
	editor->SetTextLines(t->lng_src);
}

void UserInterface::setMainTabSelected(int t)
{
	mainTabCurrent = t;
}

void UserInterface::renderUi()
{
	ImGui::SetNextWindowSize(winSize, ImGuiCond_FirstUseEver);
	ImGui::Begin("Crapblox - 'pizza party'", 0);

	if (!editor)
	{
		editor = new TextEditor();
		editor->SetLanguageDefinition(TextEditor::LanguageDefinition::PizzaParty());
		UserInterface::Tabs::loadTabByIndex(0);
	}

	/* base->main */

	ImGui::BeginTabBar("Main");

	ImGui::TabItemButton("Executor") == 1 ? mainTabCurrent = EXECUTOR_TAB_CURRENT : mainTabCurrent;
	ImGui::TabItemButton("Startup log") == 1 ? mainTabCurrent = OUTPUT_TAB_CURRENT : mainTabCurrent;

	/* render current maintab */

	switch (mainTabCurrent)
	{
		case EXECUTOR_TAB_CURRENT:
		{
			renderExecutor();
			break;
		}
		case OUTPUT_TAB_CURRENT:
		{
			renderOutput();
			break;
		}
		default:
		{
			break;
		}
	}

	ImGui::EndTabBar();

	/* base->main (end) */

	ImGui::End();
}

void UserInterface::renderOutput()
{
	ImGui::BeginGroup();

	if (RBX::isExploitReady)
		return;

	for (int i = 0; i < Lua::logged_output.size(); i++)
	{
		Lua::Output* out = Lua::logged_output.at(i);
		std::time_t at = out->at;

		std::tm* now = std::localtime(&at);
		ImVec4 color;

		switch (out->type)
		{
			case Lua::OutputTypes::print:
			{
				color = ImVec4(1, 1, 1, 1);
				break;
			}
			case Lua::OutputTypes::warn:
			{
				color = ImVec4(1, 1, 0, 1);
				break;
			}
			case Lua::OutputTypes::error:
			{
				color = ImVec4(0.8, 0, 0, 1);
				break;
			}
		}

		ImGui::TextColored(color, "[%d:%d:%d]: %s", now->tm_hour, now->tm_min, now->tm_sec, out->out);
	}

	ImGui::EndGroup();
}

void UserInterface::renderExecutor()
{
	ImVec2 sz_mod = ImGui::GetWindowSize();
	int y_sz = sz_mod.y - (sz_mod.y / 4);

	auto cpos = editor->GetCursorPosition();

	firstStart = false;
	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s ", cpos.mLine + 1, cpos.mColumn + 1, editor->GetTotalLines(), editor->IsOverwrite() ? "Ovr" : "Ins", editor->CanUndo() ? "*" : " ", editor->GetLanguageDefinition().mName.c_str());

	ImGui::BeginTabBar("ScriptsTabs"); /* base->executorTab->scriptsTab */

	tabLogic();
	editor->Render("ScriptBox", ImVec2(0, y_sz), true);

	ImGui::EndTabBar();

	currentScript = editor->GetText();

	if (currentScript != "") /* on edit */
		UserInterface::Tabs::setCurrentTabText(currentScript);

	if (ImGui::Button("Execute") && currentScript != "")
		RBX::Execution::executorSingleton()->runScript(std::string(currentScript));

}