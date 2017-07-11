#include "ModuleFileSystem.h"

ModuleFileSystem::ModuleFileSystem(const char* name, bool start_enabled) : Module(name, start_enabled)
{}

ModuleFileSystem::~ModuleFileSystem()
{}

unsigned int ModuleFileSystem::Load(const char * path, char ** buffer) const
{
	size_t result = 0;
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
			result = fread(*buffer, 1, size, file);
		}
		fclose(file);
	}

	return result;
}
