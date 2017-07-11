#ifndef __MODULE_FILESYSTEM_H__
#define __MODULE_FILESYSTEM_H__

#include "Module.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(const char* name, bool start_enabled = true);
	~ModuleFileSystem();

	unsigned int Load(const char* path, char** buffer)const;

private:

};
#endif // !__MODULE_FILESYSTEM_H__
