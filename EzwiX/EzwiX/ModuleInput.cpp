#include "ModuleInput.h"

#include "log.h"

ModuleInput::ModuleInput(const char * name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleInput::~ModuleInput()
{
}

bool ModuleInput::Init()
{
	LOG("Init Input\n");

	for (int i = 0; i < 256; ++i)
		keys[i] = false;

	return true;
}

bool ModuleInput::CleanUp()
{
	LOG("Clean Up Input\n");
	return true;
}

void ModuleInput::KeyDown(unsigned int id)
{
	keys[id] = true;
}

void ModuleInput::KeyUp(unsigned int id)
{
	keys[id] = false;
}

bool ModuleInput::IsKeyDown(unsigned int id)
{
	return keys[id];
}
