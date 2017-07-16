#include "ComponentMesh.h"

#include "Application.h"
#include "Data.h"
#include "D3DModule.h"
#include "log.h"
#include "Globals.h"

#include "MeshImporter.h"

#include "ModuleGraphics.h"
#include "ModuleFileSystem.h"

ComponentMesh::ComponentMesh(ComponentType type, GameObject* game_object) : Component(type, game_object)
{
}

ComponentMesh::~ComponentMesh()
{
	CleanUpBuffers();
}

void ComponentMesh::Update()
{
	if (mesh)
	{
		App->graphics->AddToDraw(this);
	}
}

void ComponentMesh::OnInspector(bool debug)
{

}

void ComponentMesh::OnTransformModified()
{
	
}


void ComponentMesh::Save(Data & file)const
{
	Data data;
	data.AppendInt("type", type);
	data.AppendUInt("UUID", uuid);
	data.AppendBool("active", active);
	
	//TODO: Save specific mesh info

	file.AppendArrayValue(data);
}

void ComponentMesh::Load(Data & conf)
{
	uuid = conf.GetUInt("UUID");
	active = conf.GetBool("active");

	mesh_path = conf.GetString("path");

	InitializeBuffers(App->graphics->d3d->GetDevice());
}

void ComponentMesh::InitializeBuffers(ID3D11Device * device)
{
	//Load the mesh file
	mesh = MeshImporter::Load(mesh_path.data());

	if (mesh == nullptr)
		return;

	HRESULT result;

	vertex_count = mesh->num_vertices;
	index_count = mesh->num_indices;

	VertexType* vertices = new VertexType[vertex_count];
	unsigned int* indices = new unsigned int[index_count];
	//TODO: Improve this method. TOO SLOW!!!
	//Vertices
	for (int v = 0; v < vertex_count; ++v)
	{
		vertices[v].position = D3DXVECTOR3(mesh->vertices[v * 3], mesh->vertices[v * 3 + 1], mesh->vertices[v * 3 + 2]);
		vertices[v].texture = D3DXVECTOR2(mesh->uvs[v * 2], mesh->uvs[v * 2 + 1]);
	}

	for (int i = 0; i < index_count; ++i)
	{
		indices[i] = mesh->indices[(index_count - 1) - i];
	}

	//Set up the description of the vertex buffer
	D3D11_BUFFER_DESC v_buffer_desc;
	v_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	v_buffer_desc.ByteWidth = sizeof(VertexType) * vertex_count;
	v_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	v_buffer_desc.CPUAccessFlags = 0;
	v_buffer_desc.MiscFlags = 0;
	v_buffer_desc.StructureByteStride = 0;

	//Set up the subresource structure
	D3D11_SUBRESOURCE_DATA vertex_data;
	vertex_data.pSysMem = vertices;
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&v_buffer_desc, &vertex_data, &vertex_buffer);
	if (FAILED(result))
	{
		LOG("ComponentMesh ERROR while initializing the buffers: could not create the vertex buffer");
		return;
	}

	//Set up the description of the index buffer
	D3D11_BUFFER_DESC i_buffer_desc;
	i_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	i_buffer_desc.ByteWidth = sizeof(unsigned long) * index_count;
	i_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	i_buffer_desc.CPUAccessFlags = 0;
	i_buffer_desc.MiscFlags = 0;
	i_buffer_desc.StructureByteStride = 0;

	//Set up the subresource structure
	D3D11_SUBRESOURCE_DATA index_data;
	index_data.pSysMem = indices;
	index_data.SysMemPitch = 0;
	index_data.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&i_buffer_desc, &index_data, &index_buffer);
	if (FAILED(result))
	{
		LOG("ComponentMesh ERROR while initializing the buffers: could not create the indices buffer");
		return;
	}

	//Clean up
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;
}

void ComponentMesh::CleanUpBuffers()
{
	RELEASE(index_buffer);
	RELEASE(vertex_buffer);
}

void ComponentMesh::Render()
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	ID3D11DeviceContext* device_contex = App->graphics->d3d->GetDeviceContext();
	
	//Set the vertex buffer
	device_contex->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	//Set the index buffer
	device_contex->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive to be rendered (triangles in this case)
	device_contex->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

unsigned int ComponentMesh::GetIndexCount() const
{
	return index_count;
}

void ComponentMesh::SetMeshPath(const char * path)
{
	mesh_path = path;
}

std::string ComponentMesh::GetMeshPath() const
{
	return mesh_path;
}
