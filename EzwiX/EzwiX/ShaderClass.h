#ifndef __SHADERCLASS_H__
#define __SHADERCLASS_H__

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

//NOTE: For now this class only has one shader that paints the geometry in one color.

class ShaderClass
{
public:
	ShaderClass();
	~ShaderClass();

	bool Init(ID3D11Device* device, HWND hWnd);
	void CleanUp();

	bool Render(ID3D11DeviceContext* device_context, int index_count, D3DXMATRIX projection, D3DXMATRIX view, D3DXMATRIX model);

private:

	bool InitializeShader(ID3D11Device* device, HWND hWnd, const char* v_shader, const char* p_shader);
	void CleanUpShader();

	void OutputShaderErrorMessage(ID3D10Blob* error_msg, HWND hWnd, const char* shader_filename)const;
	
	bool SetShaderParameters(ID3D11DeviceContext* device_context, D3DXMATRIX projection, D3DXMATRIX view, D3DXMATRIX model);
	void RenderShader(ID3D11DeviceContext* device_context, int index_count);

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	ID3D11VertexShader* vertex_shader = nullptr;
	ID3D11PixelShader* pixel_shader = nullptr;
	ID3D11InputLayout* layout = nullptr;
	ID3D11Buffer* matrix_buffer = nullptr;
};
#endif // !__SHADERCLASS_H__
