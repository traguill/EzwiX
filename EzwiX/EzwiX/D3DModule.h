#ifndef __D3D_H__
#define __D3D_H__

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX10math.h>

class D3DModule
{
public:
	D3DModule();
	~D3DModule();

	bool Init();
	bool CleanUp();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice()const;
	ID3D11DeviceContext* GetDeviceContext()const;
	void GetProjectionMatrix(D3DXMATRIX& proj);
	void GetWorldMatrix(D3DXMATRIX& world);
	void GetOrthoMatrix(D3DXMATRIX& ortho);
	void GetVideoCardInfo(char* card_name, int& memory);

private:
	bool vsync_enabled = true;
	int video_card_memory;
	char video_card_description[128];
	IDXGISwapChain* swap_chain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* device_context = nullptr;
	ID3D11RenderTargetView* render_target_view = nullptr;
	ID3D11Texture2D* depth_stencil_buffer = nullptr;
	ID3D11DepthStencilState* depth_stencil_state = nullptr;
	ID3D11DepthStencilView* depth_stencil_view = nullptr;
	ID3D11RasterizerState* rasterizer_state = nullptr;

	//TODO: Change this in the future for our own matrix
	D3DXMATRIX projection_matrix;
	D3DXMATRIX world_matrix;
	D3DXMATRIX ortho_matrix;
};
#endif // !__D3D_H__
