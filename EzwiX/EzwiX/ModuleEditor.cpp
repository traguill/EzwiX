#include "ModuleEditor.h"

#include "Console.h"
#include "Window.h"

#include "ModuleFPS.h"
#include "ModuleWindow.h"
#include "ModuleCPU.h"

//Testing
#include "ModuleInput.h"

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
	ImGui::Begin("##Statistics", &open, ImVec2(200, 100), 0.6f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::Text("FPS: %i", App->fps->GetFPS());
	ImGui::Text("CPU: %i %%", App->m_cpu->GetCPUUsage());
	//Testing
	int mouse_x, mouse_y;
	mouse_x = mouse_y = 0;
	App->input->GetMousePosition(mouse_x, mouse_y);
	ImGui::Text("Mouse X: %i", mouse_x);
	ImGui::Text("Mouse Y: %i", mouse_y);
	ImGui::End();
}
