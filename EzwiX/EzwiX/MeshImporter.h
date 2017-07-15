#ifndef __MESH_IMPORTER_H__
#define __MESH_IMPORTER_H__

struct aiMesh;
struct aiNode;
struct aiScene;
struct Mesh;

class Data;
class GameObject;

#include <vector>

namespace MeshImporter
{
	bool Import(const char* path);
	void ImportNode(aiNode * node, const aiScene * scene, GameObject* parent, std::vector<GameObject*>& created_go);
	bool ImportMesh(const aiMesh* mesh, std::string& mesh_path);
	bool Save(Mesh& mesh, std::string& mesh_path);
	Mesh* Load(const char* path);

	void SaveInfoFile(std::vector<GameObject*> vector, const char* file);
	void SaveGameObjectInfo(GameObject* gameObject, Data& data);
}

#endif // !__MESH_IMPORTER_H__
