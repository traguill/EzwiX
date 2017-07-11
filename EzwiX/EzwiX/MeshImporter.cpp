#include "MeshImporter.h"

#include "ModuleFileSystem.h"

bool MeshImporter::Import(const char * path)
{
	char* buff;
	App->file_system->Load(path, &buff);

	return true;
}
