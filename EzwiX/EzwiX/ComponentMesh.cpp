#include "ComponentMesh.h"

#include "Application.h"
#include "Data.h"
#include "D3DModule.h"
#include "log.h"
#include "Globals.h"

#include "ModuleGraphics.h"

ComponentMesh::ComponentMesh(ComponentType type, GameObject* game_object) : Component(type, game_object)
{
	InitializeBuffers(App->graphics->d3d->GetDevice());
}

ComponentMesh::~ComponentMesh()
{
	CleanUpBuffers();
}

void ComponentMesh::Update()
{
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

	//TODO: Load specific mesh info
}

void ComponentMesh::InitializeBuffers(ID3D11Device * device)
{
	//For now we draw a quad
	
	HRESULT result;

	vertex_count = 4;
	index_count = 6;

	VertexType* vertices = new VertexType[vertex_count];
	unsigned long* indices = new unsigned long[index_count];

	//Vertices
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); //Left - bot
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f); //left - top
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f); //Right - top
	vertices[2].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[3].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f); //Right - bot
	vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);

	//Indices
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 3;
	indices[3] = 3;
	indices[4] = 1;
	indices[5] = 2;

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
