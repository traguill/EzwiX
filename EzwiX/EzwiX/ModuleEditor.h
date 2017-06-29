#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"

#include <vector>

class Window;

class ModuleEditor : public Module
{
public:
	ModuleEditor(const char* name, bool start_enabled = true);
	~ModuleEditor();

	update_status Update();

private:

	void DrawStatistics()const;

private:

	std::vector<Window*> windows;

	bool stats_enabled = true;
};

#endif // !__MODULE_EDITOR_H__
