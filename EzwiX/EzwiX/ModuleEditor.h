#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"

#include <vector>

class Window;
class HierarchyWindow;

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

private:

	std::vector<Window*> windows;

	//Windows
	HierarchyWindow* hierarchy = nullptr;

	bool stats_enabled = true;

	EDITOR_LAYOUT layout;
};

#endif // !__MODULE_EDITOR_H__
