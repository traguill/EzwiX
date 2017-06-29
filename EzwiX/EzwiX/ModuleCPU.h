#ifndef __MODULE_CPU_H__
#define __MODULE_CPU_H__

#include "Module.h"

#pragma comment(lib, "pdh.lib")

#include <pdh.h>

class ModuleCPU : public Module
{
public:
	ModuleCPU(const char* name, bool start_enabled = true);
	~ModuleCPU();

	bool Init();
	bool CleanUp();

	update_status PreUpdate();

	int GetCPUUsage();

private:

	bool can_read_cpu = true; //Sometimes the CPU cannot be readed.In that case, this will set to false.
	HQUERY query_handle;
	HCOUNTER counter_handle;
	unsigned long last_sample_time = 0;
	long cpu_usage = 0; 
};

#endif // !__MODULE_CPU_H__
