#include "MeshImporter.h"

#include "Application.h"
#include "Random.h"

#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "log.h"
#include "Data.h"

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
		aiNode* root = scene->mRootNode;
		vector<GameObject*> objects_created;
		vector<const aiMesh*> boned_meshes;
		vector<const GameObject*> boned_game_objects;

		MeshImporter::ImportNode(root, scene, nullptr, objects_created);

		//Renaming RootNode
		if (objects_created[0]->name == "RootNode")
		{
			std::string name = App->file_system->GetFileNameFromPath(path);
			unsigned int slashPos;
			if ((slashPos = name.find_last_of("/")) != std::string::npos)
				name = name.substr(slashPos + 1, name.size() - slashPos);

			unsigned int pointPos;
			if ((pointPos = name.find_first_of(".")) != std::string::npos)
				name = name.substr(0, name.size() - (name.size() - pointPos));
			objects_created[0]->name = name;
		}

		string file = "Meshes/" + App->file_system->GetFileNameFromPath(path) + ".inf";
		SaveInfoFile(objects_created, file.data());

		for (vector<GameObject*>::iterator go = objects_created.begin(); go != objects_created.end(); ++go)
			delete (*go);

		objects_created.clear();
		aiReleaseImport(scene);

		ret = true;
	}

	delete[] buff;

	return ret;
}

void MeshImporter::ImportNode(aiNode * node, const aiScene * scene, GameObject* parent, std::vector<GameObject*>& created_go)
{
	//Transformation ------------------------------------------------------------------------------------------------------------------
	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos(translation.x, translation.y, translation.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	float3 scale(scaling.x, scaling.y, scaling.z);

	//Don't load fbx dummies as gameobjects. 
	static const char* dummies[5] =
	{
		"$AssimpFbx$_PreRotation",
		"$AssimpFbx$_Rotation",
		"$AssimpFbx$_PostRotation",
		"$AssimpFbx$_Scaling",
		"$AssimpFbx$_Translation"
	};

	std::string name = node->mName.C_Str();
	for (int i = 0; i < 5; ++i)
	{
		if (name.find(dummies[i]) != string::npos && node->mNumChildren == 1)
		{
			node = node->mChildren[0];
			node->mTransformation.Decompose(scaling, rotation, translation);
			pos += float3(translation.x, translation.y, translation.z);
			scale = float3(scale.x * scaling.x, scale.y * scaling.y, scale.z * scaling.z);
			rot = rot * Quat(rotation.x, rotation.y, rotation.z, rotation.w);
			name = node->mName.C_Str();
			i = -1;
		}
	}

	GameObject* go_root = new GameObject(parent);
	if (parent)
		parent->AddChild(go_root);

	created_go.push_back(go_root);

	go_root->name = name;

	go_root->transform->SetPosition(pos);
	go_root->transform->SetRotation(rot);
	go_root->transform->SetScale(scale);

	go_root->transform->Update(); //Force it to update the matrix

	if (node->mName.length > 0)
		go_root->name = node->mName.C_Str();

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		GameObject* child = nullptr;

		if (node->mNumMeshes > 1)
		{
			child = new GameObject(go_root);
			go_root->AddChild(child);

			created_go.push_back(child);
		}
		else
		{
			child = go_root;
		}

		if (node->mName.length > 0)
			child->name = node->mName.C_Str();

		aiMesh* mesh_to_load = scene->mMeshes[node->mMeshes[i]];

		//Mesh --------------------------------------------------------------------------------------------------------------------------------
		string mesh_path;

		unsigned int msh_uuid = 0;
		bool ret = MeshImporter::ImportMesh(mesh_to_load, mesh_path);
		ComponentMesh* mesh = (ComponentMesh*)child->AddComponent(C_MESH);
		mesh->SetMeshPath(mesh_path.data());
		//Load Textures --------------------------------------------------------------------------------------------------------------------
		//TODO: Textures
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
		MeshImporter::ImportNode(node->mChildren[i], scene, go_root, created_go);
}

bool MeshImporter::ImportMesh(const aiMesh * mesh_to_load, string& mesh_path)
{
	//Mesh --------------------------------------------------------------------------------------------------------------------------------

	Mesh mesh;

	//Vertices ------------------------------------------------------------------------------------------------------
	mesh.num_vertices = mesh_to_load->mNumVertices;
	mesh.vertices = new float[mesh.num_vertices * 3];
	memcpy(mesh.vertices, mesh_to_load->mVertices, sizeof(float3) * mesh.num_vertices);


	//Indices --------------------------------------------------------------------------------------------------------
	if (mesh_to_load->HasFaces())
	{
		mesh.num_indices = mesh_to_load->mNumFaces * 3;
		mesh.indices = new unsigned int[mesh.num_indices];
		for (unsigned int f = 0; f < mesh_to_load->mNumFaces; f++)
		{
			if (mesh_to_load->mFaces[f].mNumIndices != 3)
			{
				LOG("[WARNING] Geometry with face != 3 indices is trying to be loaded");
			}
			else
			{
				memcpy(&mesh.indices[f * 3], mesh_to_load->mFaces[f].mIndices, 3 * sizeof(unsigned int));
			}
		}
	}

	//Load UVs -----------------------------------------------------------------------------------------------------------------------
	if (mesh_to_load->HasTextureCoords(0))
	{
		mesh.num_uvs = mesh_to_load->mNumVertices; //Same size as vertices
		mesh.uvs = new float[mesh.num_uvs * 2];
		for (unsigned int uvs_item = 0; uvs_item < mesh.num_uvs; uvs_item++)
		{
			memcpy(&mesh.uvs[uvs_item * 2], &mesh_to_load->mTextureCoords[0][uvs_item].x, sizeof(float));
			memcpy(&mesh.uvs[(uvs_item * 2) + 1], &mesh_to_load->mTextureCoords[0][uvs_item].y, sizeof(float));
		}
	}

	//Normals ---------------------------------------------------------------------------------------------------------
	if (mesh_to_load->HasNormals())
	{
		mesh.normals = new float[mesh.num_vertices * 3];
		memcpy(mesh.normals, mesh_to_load->mNormals, sizeof(float) * mesh.num_vertices * 3);
	}

	return Save(mesh, mesh_path);
}

bool MeshImporter::Save(Mesh& mesh, string& mesh_path)
{
	bool ret = false;

	//Indices - Vertices - Normals - UVs
	unsigned int header[4] =
	{
		mesh.num_indices,
		mesh.num_vertices,
		(mesh.normals) ? mesh.num_vertices : 0,  
		mesh.num_uvs
	};

	unsigned size = sizeof(header) + sizeof(unsigned) * header[0] + sizeof(float) * header[1] * 3;
	if (header[2] != 0) size += sizeof(float) * header[2] * 3;
	size += sizeof(float) * header[3] * 2;

	char* data = new char[size];
	char* cursor = data;

	//Header
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	//Indices
	bytes = sizeof(unsigned int) * header[0];
	memcpy(cursor, mesh.indices, bytes);

	cursor += bytes;

	//Vertices
	bytes = sizeof(float3) * header[1];
	memcpy(cursor, mesh.vertices, bytes);

	cursor += bytes;

	//Note: don't update bytes because for normals is the same size as vertices

	//Normals
	if (header[2] != 0)
	{
		memcpy(cursor, mesh.normals, bytes);
		cursor += bytes;
	}

	//Uvs
	bytes = sizeof(float) * header[3] * 2;
	memcpy(cursor, mesh.uvs, bytes);
	cursor += bytes;

	//Generate random UUID for the name
	unsigned int msh_uuid = (unsigned int)App->rnd->RandomInt();
	string name = "Meshes/" + std::to_string(msh_uuid) + ".msh";
	ret = App->file_system->Save(name.data(), data, size);

	mesh_path = name;

	delete[] data;
	data = nullptr;

	return ret;
}

Mesh * MeshImporter::Load(const char * path)
{
	char* buffer = nullptr;
	unsigned int file_size = App->file_system->Load(path, &buffer);

	if (file_size == 0)
	{
		if (buffer)
			delete[] buffer;
		LOG("Error while opening the file of the mesh %s", path);
		return nullptr;
	}

	Mesh* mesh = new Mesh();

	char* cursor = buffer;

	unsigned int header[4];
	unsigned int bytes = sizeof(header);
	memcpy(header, cursor, bytes);

	mesh->num_indices = header[0];
	mesh->num_vertices = header[1];
	mesh->num_uvs = header[3];

	//Indices
	cursor += bytes;
	bytes = sizeof(unsigned int) * mesh->num_indices;
	mesh->indices = new unsigned int[mesh->num_indices];
	memcpy(mesh->indices, cursor, bytes);

	//Vertices
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_vertices * 3;
	mesh->vertices = new float[mesh->num_vertices * 3];
	memcpy(mesh->vertices, cursor, bytes);

	//Normals
	cursor += bytes;
	if (header[2] != 0)
	{
		bytes = sizeof(float) * mesh->num_vertices * 3;
		mesh->normals = new float[mesh->num_vertices * 3];
		memcpy(mesh->normals, cursor, bytes);

		cursor += bytes;
	}

	//Uvs
	bytes = sizeof(float) * mesh->num_uvs * 2;
	mesh->uvs = new float[mesh->num_uvs * 2];
	memcpy(mesh->uvs, cursor, bytes);

	delete[] buffer;
	buffer = nullptr;

	return mesh;
}

void MeshImporter::SaveInfoFile(std::vector<GameObject*> vector, const char* file)
{
	Data data;
	data.AppendArray("GameObjects");
	for (unsigned int i = 0; i < vector.size(); i++)
	{
		SaveGameObjectInfo(vector[i], data);
	}

	char* buf;
	size_t size = data.Serialize(&buf);

	App->file_system->Save(file, buf, size);

	delete[] buf;
}

void MeshImporter::SaveGameObjectInfo(GameObject* gameObject, Data& data)
{
	Data go_data;
	go_data.AppendString("name", gameObject->name.data());
	go_data.AppendUInt("UUID", gameObject->GetUUID());
	go_data.AppendUInt("parent", gameObject->GetParent() == nullptr ? 0 : gameObject->GetParent()->GetUUID());
	go_data.AppendArray("components");

	std::vector<Component*>::const_iterator component = gameObject->GetComponents()->begin();
	while (component != gameObject->GetComponents()->end())
	{
		Data component_data;
		if ((*component)->GetType() != C_TRANSFORM)
		{
			component_data.AppendInt("type", (*component)->GetType());
			component_data.AppendUInt("UUID", (unsigned int)App->rnd->RandomInt());
			component_data.AppendBool("active", true);

			switch ((*component)->GetType())
			{
			case (C_MESH):
				component_data.AppendString("path", ((ComponentMesh*)*component)->GetMeshPath().c_str());
				break;
			}
			go_data.AppendArrayValue(component_data);
		}
		else
			(*component)->Save(go_data);

		component++;
	}

	data.AppendArrayValue(go_data);
}
