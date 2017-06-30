#include "ModuleTimer.h"

#include "log.h"

ModuleTimer::ModuleTimer(const char * name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleTimer::~ModuleTimer()
{
}

bool ModuleTimer::Init()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	if (frequency == 0)
	{
		LOG("INFO: High perfomance timers are not supported");
		return true;
	}

	ticks_per_ms = (float)(frequency / 1000);
	QueryPerformanceCounter((LARGE_INTEGER*)&start_time);

	return true;
}

update_status ModuleTimer::PreUpdate()
{
	INT64 current_time;
	QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
	
	float difference = (float)(current_time - start_time);
	delta_time = difference / ticks_per_ms;

	start_time = current_time;

	return update_status::UPDATE_CONTINUE;
}

float ModuleTimer::GetDeltaTime() const
{
	return delta_time;
}
