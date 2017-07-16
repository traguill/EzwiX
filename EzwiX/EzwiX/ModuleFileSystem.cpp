#include "ModuleFileSystem.h"

#include "log.h"

ModuleFileSystem::ModuleFileSystem(const char* name, bool start_enabled) : Module(name, start_enabled)
{}

ModuleFileSystem::~ModuleFileSystem()
{}

bool ModuleFileSystem::Init()
{
	//Get the current directory

	char directory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, directory);

	current_directory = string(directory);
	current_directory.append("\\");

	library_directory = current_directory + "Library\\";
	CreateDirectory(library_directory.data(), NULL);

	meshes_directory = library_directory + "Meshes\\";
	CreateDirectory(meshes_directory.data(), NULL);

	textures_directory = library_directory + "Textures\\";
	CreateDirectory(textures_directory.data(), NULL);

	LOG("Current directory: %s", current_directory.data());

	return true;
}

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

unsigned int ModuleFileSystem::Save(const char * path, const void * buffer, unsigned int size) const
{
	size_t ret = 0;
	FILE* file;
	fopen_s(&file, path, "wb");

	if(file)
	{
		ret = fwrite(buffer, sizeof(char), size, file);
		fclose(file);
	}

	return ret;
}

std::string ModuleFileSystem::GetDirectory() const
{
	return current_directory;
}

std::string ModuleFileSystem::GetLibraryDirectory() const
{
	return library_directory;
}

std::string ModuleFileSystem::GetMeshesDirectory() const
{
	return meshes_directory;
}

std::string ModuleFileSystem::GetTexturesDirectory() const
{
	return textures_directory;
}

std::string ModuleFileSystem::GetFileNameFromPath(const std::string & path) const
{
	size_t begin = path.find_last_of("\\/") + 1;
	size_t end = path.find_last_of(".");
	return path.substr(begin, end - begin);
}

std::string ModuleFileSystem::GetDirectoryFromPath(const std::string & path) const
{
	return path.substr(0, path.find_last_of("\\/") + 1);
}

unsigned int ModuleFileSystem::GetAllFilesInDirectory(const char * path, vector<string>& filenames) const
{
	WIN32_FIND_DATA data;
	HANDLE h_find = FindFirstFile(path, &data);

	filenames.clear();

	if (h_find != INVALID_HANDLE_VALUE)
	{
		do
		{
			filenames.push_back(data.cFileName);
		} 
		while (FindNextFile(h_find, &data));
		FindClose(h_find);
	}

	return filenames.size();
}

unsigned int ModuleFileSystem::FilterFiles(const vector<string>& filenames, vector<string>& filtered, const string & filter) const
{
	filtered.clear();

	string extension;
	for (vector<string>::const_iterator file = filenames.begin(); file != filenames.end(); ++file)
	{
		extension = (*file).substr((*file).find_last_of("."), (*file).size());
		if (extension.compare(filter.data()) == 0)
		{
			filtered.push_back((*file));
		}
	}

	return filtered.size();
}
