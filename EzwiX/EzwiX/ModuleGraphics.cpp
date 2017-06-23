#include "ModuleGraphics.h"

#include "log.h"

#include "D3DModule.h"

ModuleGraphics::ModuleGraphics(const char * name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleGraphics::~ModuleGraphics()
{
}

bool ModuleGraphics::Init()
{
	LOG("Init Graphics");

	bool ret = true;
	
	d3d = new D3DModule();
	ret = d3d->Init();

	return ret;
}

bool ModuleGraphics::CleanUp()
{
	LOG("Clean Up Graphics");

	d3d->CleanUp();

	delete d3d;

	return true;
}

update_status ModuleGraphics::Update()
{
	
	d3d->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	d3d->EndScene();

	return update_status::UPDATE_CONTINUE;
}

float ModuleGraphics::GetScreenDepth() const
{
	return screen_depth;
}

float ModuleGraphics::GetScreenNear() const
{
	return screen_near;
}

bool ModuleGraphics::IsVsyncEnabled() const
{
	return vsync_enabled;
}
