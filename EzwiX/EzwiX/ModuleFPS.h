#ifndef __MODULE_FPS_H__
#define __MODULE_FPS_H__

#include "Module.h"

class ModuleFPS : public Module
{
public:
	ModuleFPS(const char* name, bool start_enabled = true);
	~ModuleFPS();

	bool Init();

	update_status PreUpdate();
	
	int GetFPS()const;

private:
	int fps = 0;
	int count = 0;
	unsigned long start_time = 0;

	//TODO: Cap FPS

};
#endif // !__MODULE_FPS_H__
