#ifndef __MODULE_H__
#define __MODULE_H__

#include "Application.h"

#include <string>

#define NAME_MODULE_LENGTH 32

class Module
{

private:

	bool enabled;
	std::string name;

public:

	Module(const char* name, bool start_enabled = true) : name(name)
	{}

	virtual ~Module()
	{}

	virtual bool Init()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	const char* GetName()const
	{
		return name.data();
	}

};

#endif // !__MOUDLE_H__