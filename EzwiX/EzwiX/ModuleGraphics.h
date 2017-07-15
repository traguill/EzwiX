#ifndef __MODULE_GRAPHICS_H__
#define __MODULE_GRAPHICS_H__

#include "Module.h"

class D3DModule;
class ComponentMesh; 
class ComponentTexture; //TODO: remove. It's here for testing
class ShaderClass;
class TextureShaderClass;

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

	void AddToDraw(ComponentMesh* mesh); //Testing

public:

	D3DModule* d3d = nullptr;

private:
	float screen_depth = 1000.0f;
	float screen_near = 0.1f;
	bool vsync_enabled = true;

	ComponentTexture* texture = nullptr; //Testing
	//ShaderClass* shader_class = nullptr;
	TextureShaderClass* texture_shader = nullptr;

	std::vector<ComponentMesh*> models; //Testing
};

#endif 
