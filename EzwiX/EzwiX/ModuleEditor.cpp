#include "ModuleEditor.h"

#include "Console.h"
#include "Window.h"

#include "ModuleFPS.h"
#include "ModuleWindow.h"
#include "ModuleCPU.h"

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

	if (stats_enabled)
		DrawStatistics();

	return update_status::UPDATE_CONTINUE;
}

void ModuleEditor::DrawStatistics() const
{
	int screen_width = App->window->GetScreenWidth();
	ImGui::SetNextWindowPos(ImVec2(screen_width - 120, 30));
	bool open = true;
	ImGui::Begin("##Statistics", &open, ImVec2(100, 50), 0.6f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::Text("FPS: %i", App->fps->GetFPS());
	ImGui::Text("CPU: %i %%", App->m_cpu->GetCPUUsage());
	ImGui::End();
}
