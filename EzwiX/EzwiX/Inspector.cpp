#include "Inspector.h"

#include "GameObject.h"
#include "Component.h"

#include "Application.h"

#include "ModuleEditor.h"

Inspector::Inspector()
{
	//Testing
	active = true;
}

Inspector::~Inspector()
{
}

void Inspector::Draw()
{
	if (!active) return;

	ImGui::Begin("Inspector", &active);

	ImGui::Text("Debug: "); ImGui::SameLine(); ImGui::Checkbox("##debug_inspector", &debug);
	ImGui::Separator();

	GameObject* selected_GO = App->editor->selected_gameobject;

	if (selected_GO)
	{
		//Active
		bool is_active = selected_GO->IsActive();
		if (ImGui::Checkbox("", &is_active))
			selected_GO->SetActive(is_active);

		//Name
		ImGui::SameLine();
		ImGui::InputText("###goname", selected_GO->name._Myptr(), selected_GO->name.capacity());

		ImGui::Separator();
		//App->go_manager->layer_system->DisplayLayerSelector(selected_GO->layer); TODO: layers

		if (debug)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 0, 255));
			ImGui::Text("UUID: %u", (int)selected_GO->GetUUID());
			//ImGui::Text("Local UUID: %u", (int)selected_GO->local_uuid);
			//ImGui::Text("Layer id: %i", selected_GO->layer);
			ImGui::PopStyleColor();
		}

		//Components
		const std::vector<Component*>* components = selected_GO->GetComponents();
		for (std::vector<Component*>::const_iterator component = (*components).begin(); component != (*components).end(); ++component)
		{
			(*component)->OnInspector(debug);
		}

		ImGui::NewLine();
		if (ImGui::Button("Create component"))
		{
			ImGui::OpenPopup("InspectorOptions");
		}

		if (ImGui::BeginPopup("InspectorOptions"))
		{
			if (ImGui::Selectable("Add Mesh"))				selected_GO->AddComponent(C_MESH);
			if (ImGui::Selectable("Add Texture"))			selected_GO->AddComponent(C_TEXTURE);
			ImGui::EndPopup();
		}
	}

	ImGui::End();
}
