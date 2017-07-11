#include "ModuleFileSystem.h"

ModuleFileSystem::ModuleFileSystem(const char* name, bool start_enabled) : Module(name, start_enabled)
{}

ModuleFileSystem::~ModuleFileSystem()
{}

bool ModuleFileSystem::Load(const char * path, char ** buffer) const
{
	bool ret = false;

	FILE* file;
	fopen_s(&file, path, "rb");
	
	if (file)
	{
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		rewind(file);

		*buffer = new char[(unsigned int)size];
		if (*buffer)
		{
			size_t result;

			result = fread(*buffer, 1, size, file);

			if (result == size)
				ret = true;
		}
		fclose(file);
	}

	return ret;
}
