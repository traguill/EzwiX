#include "ModuleFPS.h"

#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>

ModuleFPS::ModuleFPS(const char * name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleFPS::~ModuleFPS()
{
}

bool ModuleFPS::Init()
{
	start_time = timeGetTime();

	return true;
}

update_status ModuleFPS::PreUpdate()
{
	++count;

	if (timeGetTime() >= (start_time + 1000))
	{
		fps = count;
		count = 0;

		start_time = timeGetTime();
	}

	return update_status::UPDATE_CONTINUE;
}

int ModuleFPS::GetFPS() const
{
	return fps;
}
