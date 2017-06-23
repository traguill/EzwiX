#ifndef __MODULE_GRAPHICS_H__
#define __MODULE_GRAPHICS_H__

#include "Module.h"

class D3DModule;

class ModuleGraphics : public Module
{
public:
	ModuleGraphics(const char* name, bool start_enabled = true);
	~ModuleGraphics();

	bool Init();
	bool CleanUp();

	update_status Update();

	float GetScreenDepth()const;
	float GetScreenNear()const;
	bool IsVsyncEnabled()const;

private:
	D3DModule* d3d = nullptr;

	float screen_depth = 1000.0f;
	float screen_near = 0.1f;
	bool vsync_enabled = true;
};

#endif 
