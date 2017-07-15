#ifndef __MODULE_FILESYSTEM_H__
#define __MODULE_FILESYSTEM_H__

#include "Module.h"

#include <vector>

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(const char* name, bool start_enabled = true);
	~ModuleFileSystem();

	bool Init();

	unsigned int Load(const char* path, char** buffer)const;
	unsigned int Save(const char* path, const void* buffer, unsigned int size)const;

	std::string GetDirectory()const;

	std::string GetFileNameFromPath(const std::string& path)const;

	unsigned int GetAllFilesInDirectory(const char* path, vector<string>& filenames)const; //Returns the number of files found
	unsigned int FilterFiles(const vector<string>& filenames, vector<string>& filtered, const string& filter)const;

private:
	std::string current_directory;
};
#endif // !__MODULE_FILESYSTEM_H__
