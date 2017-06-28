#include "ModuleEditor.h"

#include "Console.h"
#include "Window.h"

#include "ImGui\imgui.h"

ModuleEditor::ModuleEditor(const char * name, bool start_enabled) : Module(name, start_enabled)
{
	windows.push_back(console = new Console());
}

ModuleEditor::~ModuleEditor()
{
}

update_status ModuleEditor::Update()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Console"))
				console->SetActive(true);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	for (vector<Window*>::iterator win = windows.begin(); win != windows.end(); ++win)
		if((*win)->IsActive())
			(*win)->Draw();

	return update_status::UPDATE_CONTINUE;
}
