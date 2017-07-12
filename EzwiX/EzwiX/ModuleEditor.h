#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"

#include "ImGuizmo\ImGuizmo.h"

#include <vector>

class Window;
class HierarchyWindow;
class Inspector;
class GameObject;

enum EDITOR_LAYOUT
{
	MINIMAL,
	EDITOR
};

class ModuleEditor : public Module
{
public:
	ModuleEditor(const char* name, bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	update_status Update();

private:

	void DrawStatistics()const;
	void DisplayGuizmos();

private:

	std::vector<Window*> windows;

	//Windows
	HierarchyWindow* hierarchy = nullptr;
	Inspector* inspector = nullptr;

	bool stats_enabled = true;

	EDITOR_LAYOUT layout;

public:

	//Guizmos
	ImGuizmo::OPERATION guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE guizmo_mode = ImGuizmo::MODE::LOCAL;
	bool guizmo_enabled = true;

	GameObject* selected_gameobject = nullptr;
};

#endif // !__MODULE_EDITOR_H__
