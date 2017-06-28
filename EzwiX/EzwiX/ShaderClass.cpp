#include "ShaderClass.h"

#include "log.h"
#include "Globals.h"

ShaderClass::ShaderClass()
{
}

ShaderClass::~ShaderClass()
{
}

bool ShaderClass::Init(ID3D11Device* device, HWND hWnd)
{
	bool ret = true;

	ret = InitializeShader(device, hWnd, "Color.vs", "Color.ps");
	if (!ret)
	{
		LOG("Shader ERROR: Could not initialize Color shader");
	}

	return ret;
}

void ShaderClass::CleanUp()
{
	CleanUpShader();
}

bool ShaderClass::Render(ID3D11DeviceContext * device_context, int index_count, D3DXMATRIX projection, D3DXMATRIX view, D3DXMATRIX model)
{
	bool ret = true;

	ret = SetShaderParameters(device_context, projection, view, model);
	if (!ret)
		return ret;

	RenderShader(device_context, index_count);

	return ret;
}

bool ShaderClass::InitializeShader(ID3D11Device * device, HWND hWnd, const char * v_shader, const char * p_shader)
{
	HRESULT result;
	ID3D10Blob* error_message;

	//Compile the vertex shader
	ID3D10Blob* vertex_shader_buffer = nullptr;
	result = D3DX11CompileFromFile(v_shader, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertex_shader_buffer, &error_message, NULL);
	if (FAILED(result))
	{
		if (error_message)
			OutputShaderErrorMessage(error_message, hWnd, v_shader);
		else
			MessageBox(hWnd, v_shader, "Missing shader file", MB_OK);

		return false;
	}

	//Compile the pixel shader
	ID3D10Blob* pixel_shader_buffer = nullptr;
	result = D3DX11CompileFromFile(p_shader, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixel_shader_buffer, &error_message, NULL);
	if (FAILED(result))
	{
		if (error_message)
			OutputShaderErrorMessage(error_message, hWnd, p_shader);
		else
			MessageBox(hWnd, p_shader, "Missing shader file", MB_OK);

		return false;
	}

	//Create the vertex shader from the buffer
	result = device->CreateVertexShader(vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), NULL, &vertex_shader);
	if (FAILED(result))
	{
		LOG("Shader ERROR: Could not create the vertex shader from the buffer. Vertex Shader: %s", v_shader);
		return false;
	}

	//Create the pixel shader from the buffer
	result = device->CreatePixelShader(pixel_shader_buffer->GetBufferPointer(), pixel_shader_buffer->GetBufferSize(), NULL, &pixel_shader);
	if (FAILED(result))
	{
		LOG("Shader ERROR: Could not create the pixel shader from the buffer. Pixel Shader: %s", p_shader);
		return false;
	}

	//Set up the layout of the data of the shader
	D3D11_INPUT_ELEMENT_DESC polygon_layout[2];

	polygon_layout[0].SemanticName = "POSITION";
	polygon_layout[0].SemanticIndex = 0;
	polygon_layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygon_layout[0].InputSlot = 0;
	polygon_layout[0].AlignedByteOffset = 0;
	polygon_layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygon_layout[0].InstanceDataStepRate = 0;

	polygon_layout[1].SemanticName = "COLOR";
	polygon_layout[1].SemanticIndex = 0;
	polygon_layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygon_layout[1].InputSlot = 0;
	polygon_layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygon_layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygon_layout[1].InstanceDataStepRate = 0;

	//Get the number of elements in the layout
	unsigned int num_elements = sizeof(polygon_layout) / sizeof(polygon_layout[0]);

	result = device->CreateInputLayout(polygon_layout, num_elements, vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		LOG("Shader ERROR while creating the layout of the shader %s", v_shader);
		return false;
	}

	RELEASE(vertex_shader_buffer);
	RELEASE(pixel_shader_buffer);

	//Constant buffer in the vertex shader
	D3D11_BUFFER_DESC matrix_buff_desc;
	matrix_buff_desc.Usage = D3D11_USAGE_DYNAMIC;
	matrix_buff_desc.ByteWidth = sizeof(MatrixBufferType);
	matrix_buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrix_buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrix_buff_desc.MiscFlags = 0;
	matrix_buff_desc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrix_buff_desc, NULL, &matrix_buffer);
	if (FAILED(result))
	{
		LOG("Shader ERROR while creating the matrix buffer");
		return false;
	}

	return true;
}

void ShaderClass::CleanUpShader()
{
	RELEASE(matrix_buffer);
	RELEASE(layout);
	RELEASE(pixel_shader);
	RELEASE(vertex_shader);
}

void ShaderClass::OutputShaderErrorMessage(ID3D10Blob * error_msg, HWND hWnd, const char * shader_filename) const
{
	char* errors = (char*)(error_msg->GetBufferPointer());

	LOG("Shader %s ERROR: %s", shader_filename, errors);

	error_msg->Release();
	error_msg = nullptr;

	MessageBox(hWnd, "Error compiling shader. Check output for more info", shader_filename, MB_OK);
}

bool ShaderClass::SetShaderParameters(ID3D11DeviceContext * device_context, D3DXMATRIX projection, D3DXMATRIX view, D3DXMATRIX model)
{
	HRESULT result;

	//Transpose the matrices before passing to the shader
	D3DXMatrixTranspose(&model, &model);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);

	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	result = device_context->Map(matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	if (FAILED(result))
	{
		LOG("ERROR while locking the constant buffer");
		return false;
	}

	//Get the pointer to the data in the constant buffer
	MatrixBufferType* data_ptr = (MatrixBufferType*)mapped_resource.pData;

	//Copy the matrices into the constant buffer
	data_ptr->world = model;
	data_ptr->view = view;
	data_ptr->projection = projection;

	//Unlock the constant buffer
	device_context->Unmap(matrix_buffer, 0);

	//Set the position of the constant buffer in the vertex shader
	unsigned int buff_number = 0;

	//Set the constant buffer in the vertex shader with the updated values
	device_context->VSSetConstantBuffers(buff_number, 1, &matrix_buffer);

	return true;
}

void ShaderClass::RenderShader(ID3D11DeviceContext * device_context, int index_count)
{
	//Set the vertex input layout
	device_context->IASetInputLayout(layout);

	//Set the vertex and pixel shaders
	device_context->VSSetShader(vertex_shader, NULL, 0);
	device_context->PSSetShader(pixel_shader, NULL, 0);

	//Actual render
	device_context->DrawIndexed(index_count, 0, 0);
}
