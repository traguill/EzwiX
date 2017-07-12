#include "HierarchyWindow.h"

#include "Application.h"

#include "ModuleGameObjectManager.h"
#include "ModuleEditor.h"

#include "GameObject.h"

HierarchyWindow::HierarchyWindow()
{
	//testing
	active = true;
}

HierarchyWindow::~HierarchyWindow()
{}

void HierarchyWindow::Draw()
{
	ImGui::Begin("Hierarchy", &active);

	DisplayGameObjects(App->gameobject_manager->GetRoot()->GetChilds());

	ImGui::End();
}

void HierarchyWindow::DisplayGameObjects(const std::vector<GameObject*>* childs) const
{
	for (std::vector<GameObject*>::const_iterator object = (*childs).begin(); object != (*childs).end(); ++object)
	{
		unsigned int flags = ImGuiTreeNodeFlags_OpenOnArrow;

		if(App->editor->selected_gameobject == (*object))
			flags |= ImGuiTreeNodeFlags_Selected;

		if ((*object)->ChildCount() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		ImGui::PushID((*object)->GetUUID());
		bool open = ImGui::TreeNodeEx((*object)->name.data(), flags);

		bool hasChilds = (*object)->ChildCount() > 0;
		ImGui::PopID();

		//Select GameObject when is clicked (left-button). TODO: Improve this to accept multiple selection
		if (ImGui::IsItemClicked(0))
		{
			App->editor->selected_gameobject = (*object);
		}

		if (hasChilds && open)
		{
			DisplayGameObjects((*object)->GetChilds());
			ImGui::TreePop();
		}
	}
}

