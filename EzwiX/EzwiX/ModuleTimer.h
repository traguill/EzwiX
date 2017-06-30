#ifndef __MODULE_TIMER_H__
#define __MODULE_TIMER_H__

#include "Module.h"

class ModuleTimer : public Module
{
public:
	ModuleTimer(const char* name, bool start_enabled = true);
	~ModuleTimer();

	bool Init();
	update_status PreUpdate();

	float GetDeltaTime()const;

private:
	INT64 frequency;
	float ticks_per_ms;
	INT64 start_time;
	float delta_time;
};

#endif // !__MODULE_TIMER_H__
