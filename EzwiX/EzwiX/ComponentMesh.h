#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"

#include <d3d11.h>
#include <d3dx10math.h>

#include <string>

struct Mesh
{
	unsigned int id_vertices = 0;
	unsigned int num_vertices = 0;
	float* vertices = nullptr;

	unsigned int id_indices = 0;
	unsigned int num_indices = 0;
	unsigned int* indices = nullptr;

	unsigned int id_uvs = 0;
	unsigned int num_uvs = 0 ;
	float* uvs = nullptr;

	unsigned int id_normals = 0;
	float* normals = nullptr;
};

class ComponentMesh : public Component
{
public:

	ComponentMesh(ComponentType type, GameObject* game_object);
	~ComponentMesh();

	void Update();

	void OnInspector(bool debug);
	void OnTransformModified();

	void Save(Data& file) const;
	void Load(Data& conf);

	void Render();

	unsigned int GetIndexCount()const;

	void SetMeshPath(const char* path);
	std::string GetMeshPath()const;

private:
	void InitializeBuffers(ID3D11Device* device);
	void CleanUpBuffers();

	

private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	ID3D11Buffer* vertex_buffer = nullptr; 
	ID3D11Buffer* index_buffer = nullptr;

	int vertex_count = 0; 
	int index_count = 0;

	Mesh* mesh = nullptr;
	std::string mesh_path;

public:
};


#endif // !__COMPONENTMESH_H__