#include "ModuleCPU.h"

#include "log.h"



ModuleCPU::ModuleCPU(const char * name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleCPU::~ModuleCPU()
{
}

bool ModuleCPU::Init()
{
	PDH_STATUS status = PdhOpenQuery(NULL, 0, &query_handle);
	if (status != ERROR_SUCCESS)
	{
		can_read_cpu = false;
		LOG("INFO: CPU usage cannot be readed");
	}

	//NOTE: In my computer, this fails here. TODO: Test it in another computer to see if the method works.
	status = PdhAddCounter(query_handle, TEXT("\\Processor(_Total)\\% processor time"), 0, &counter_handle);
	if (status != ERROR_SUCCESS)
	{
		can_read_cpu = false;
		LOG("INFO: CPU usage cannot be readed");
	}

	last_sample_time = GetTickCount();

	return true;
}

bool ModuleCPU::CleanUp()
{
	if (can_read_cpu)
		PdhCloseQuery(query_handle);

	return true;
}

update_status ModuleCPU::PreUpdate()
{
	if (can_read_cpu)
	{
		if (last_sample_time + 1000 < GetTickCount())
		{
			last_sample_time = GetTickCount();
			PdhCollectQueryData(query_handle);
			PDH_FMT_COUNTERVALUE value;
			PdhGetFormattedCounterValue(counter_handle, PDH_FMT_LONG, NULL, &value);
			cpu_usage = value.longValue;
		}
	}

	return update_status::UPDATE_CONTINUE;
}

int ModuleCPU::GetCPUUsage()
{
	return (can_read_cpu) ? (int)cpu_usage : 0;
}
