#include "MeshImporter.h"

#include "ModuleFileSystem.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "log.h"

bool MeshImporter::Import(const char * path)
{
	bool ret = false;

	//TODO: Check first if the file to import is a valid mesh file (fbx, obj, ...)

	char* buff;
	unsigned int size = App->file_system->Load(path, &buff);

	if (size == 0)
	{
		LOG("Importer ERROR while opening the file %s", path);
		return ret;
	}

	const aiScene* scene = aiImportFileFromMemory(buff, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene != nullptr && scene->HasMeshes())
	{
		LOG("All good");
		//TODO: I need a GameObject hierarchy first...
	}

	delete[] buff;

	return true;
}
