#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#include "Module.h"

class ModuleInput : public Module
{
public:
	ModuleInput(const char* name, bool start_enabled = true);
	~ModuleInput();

	bool Init();
	bool CleanUp();

	void KeyDown(unsigned int id);
	void KeyUp(unsigned int id);

	bool IsKeyDown(unsigned int id);

private:
	bool keys[256]; //TODO: Keys with state (UP, DOWN, REPEAT)

};

#endif // !__MODULE_INPUT_H_
