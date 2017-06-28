#ifndef __MODULE_GRAPHICS_H__
#define __MODULE_GRAPHICS_H__

#include "Module.h"

class D3DModule;
class ComponentMesh; //TODO: remove. It's here for testing
class ShaderClass;

class ModuleGraphics : public Module
{
public:
	ModuleGraphics(const char* name, bool start_enabled = true);
	~ModuleGraphics();

	bool Init();
	bool CleanUp();

	update_status PreUpdate();
	update_status PostUpdate();

	float GetScreenDepth()const;
	float GetScreenNear()const;
	bool IsVsyncEnabled()const;

public:

	D3DModule* d3d = nullptr;

private:
	float screen_depth = 1000.0f;
	float screen_near = 0.1f;
	bool vsync_enabled = true;

	ComponentMesh* model = nullptr;
	ShaderClass* shader_class = nullptr;
};

#endif 
