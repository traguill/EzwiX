#include "HierarchyWindow.h"

#include "Application.h"

#include "ModuleGameObjectManager.h"

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

		if ((*object)->ChildCount() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		ImGui::PushID((*object)->GetUUID());
		bool open = ImGui::TreeNodeEx((*object)->name.data(), flags);

		bool hasChilds = (*object)->ChildCount() > 0;
		ImGui::PopID();

		if (hasChilds && open)
		{
			DisplayGameObjects((*object)->GetChilds());
			ImGui::TreePop();
		}
	}
}

