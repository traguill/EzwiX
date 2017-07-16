#include "ModuleEditor.h"

#include "Console.h"
#include "Window.h"

#include "ModuleFPS.h"
#include "ModuleWindow.h"
#include "ModuleCPU.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleFileSystem.h"
#include "ModuleGameObjectManager.h"

#include "HierarchyWindow.h"
#include "Inspector.h"

#include "GameObject.h"

#include "ComponentTransform.h"

#include "log.h"
#include "ImGui\imgui.h"
#include "MathGeoLib\include\MathGeoLib.h"

ModuleEditor::ModuleEditor(const char * name, bool start_enabled) : Module(name, start_enabled)
{
	windows.push_back(console = new Console());
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	layout = EDITOR_LAYOUT::MINIMAL; //TODO: Dislay different layout configurations for the windows. I'll do it another day...

	windows.push_back(hierarchy = new HierarchyWindow());
	windows.push_back(inspector = new Inspector());

	return true;
}

update_status ModuleEditor::Update()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Load 3D model"))
			{
				Load3DModel();
				ImGui::EndMenu();
			}
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

	DisplayGuizmos();

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

void ModuleEditor::DisplayGuizmos()
{
	//Selection keys
	if (App->input->GetKey(KEY_W) == KEY_DOWN)
		guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
	if (App->input->GetKey(KEY_E) == KEY_DOWN)
		guizmo_operation = ImGuizmo::OPERATION::ROTATE;
	if (App->input->GetKey(KEY_R) == KEY_DOWN)
		guizmo_operation = ImGuizmo::OPERATION::SCALE;

	ImGuizmo::BeginFrame();

	if (selected_gameobject != nullptr)
	{
		GameObject* go = selected_gameobject;

		ImGuizmo::Enable(guizmo_enabled);

		float4x4 matrix = go->transform->GetLocalTransformMatrix().Transposed();

		ComponentTransform* transform = go->transform;

		ComponentTransform* parent_transform = nullptr;

		if (go->GetParent())
		{
			parent_transform = (ComponentTransform*)go->GetParent()->GetComponent(C_TRANSFORM);
			assert(parent_transform);

			matrix = parent_transform->GetGlobalMatrix() * transform->GetLocalTransformMatrix();
			matrix = matrix.Transposed();
		}

		float4x4 view_matrix = float4x4::identity;
		float4x4 projection_matrix = float4x4::identity;
		App->camera->GetViewMatrix(view_matrix);
		App->camera->GetProjectionMatrix(projection_matrix);

		//view_matrix = view_matrix.Transposed();
		//projection_matrix = projection_matrix.Transposed();

		if (guizmo_operation == ImGuizmo::OPERATION::SCALE)
			ImGuizmo::Manipulate(view_matrix.ptr(), projection_matrix.ptr(), (ImGuizmo::OPERATION)guizmo_operation, ImGuizmo::LOCAL, matrix.ptr());
		else
			ImGuizmo::Manipulate(view_matrix.ptr(), projection_matrix.ptr(), (ImGuizmo::OPERATION)guizmo_operation, (ImGuizmo::MODE)guizmo_mode, matrix.ptr());

		if (ImGuizmo::IsUsing())
		{
			matrix.Transpose();

			if (go->GetParent())
			{
				matrix = parent_transform->GetGlobalMatrix().Inverted() * matrix;
			}

			float3 position, scale;
			Quat rotation;
			matrix.Decompose(position, rotation, scale);

			go->transform->SetPosition(position);
			go->transform->SetRotation(rotation);
			go->transform->SetScale(scale);
		}
	}
}

void ModuleEditor::Load3DModel()
{
	//Collect all files with the extension .inf. TODO: Don't do this every frame.
	vector<string> files, models;

	string meshes_folder = App->file_system->GetMeshesDirectory() + "*.*";
	App->file_system->GetAllFilesInDirectory(meshes_folder.data(), files);

	App->file_system->FilterFiles(files, models, ".inf");

	for (vector<string>::iterator item = models.begin(); item != models.end(); ++item)
	{
		if (ImGui::MenuItem((*item).data()))
		{
			string file_name = App->file_system->GetMeshesDirectory() + (*item).data();
			App->gameobject_manager->Load3DModelFile(file_name.data());
		}
	}
}
