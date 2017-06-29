#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"

#include <d3d11.h>
#include <d3dx10math.h>

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

public:
};


#endif // !__COMPONENTMESH_H__