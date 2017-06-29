#ifndef __TEXTURE_SHADER_CLASS_H__
#define __TEXTURE_SHADER_CLASS_H__

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

class TextureShaderClass
{
public:
	TextureShaderClass();
	~TextureShaderClass();

	bool Init(ID3D11Device* device, HWND hWnd);
	void CleanUp();

	bool Render(ID3D11DeviceContext* device_context, int index_count, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, ID3D11ShaderResourceView* texture);

private: 
	bool InitializeShader(ID3D11Device* device, HWND hWnd, const char* vs_filename, const char* ps_filename);

	void OutputShaderErrorMessage(ID3D10Blob* error_msg, HWND hWnd, const char* shader_filename)const;

	bool SetShaderParameters(ID3D11DeviceContext* device_context, D3DXMATRIX projection, D3DXMATRIX view, D3DXMATRIX model, ID3D11ShaderResourceView* texture);
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
	ID3D11SamplerState* sampler_state = nullptr;
};


#endif // !__TEXTURE_SHADER_CLASS_H__
