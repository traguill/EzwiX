#include "D3DModule.h"

#include "log.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleGraphics.h"

D3DModule::D3DModule()
{
}

D3DModule::~D3DModule()
{
}

bool D3DModule::Init()
{
	HRESULT result;
	vsync_enabled = App->graphics->IsVsyncEnabled();

	//Create a DirectX graphics interface factory
	IDXGIFactory* factory = nullptr;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not create a factory");
		return false;
	}

	//Create an adapter for the primary graphics interface, video card.
	IDXGIAdapter* adapter = nullptr;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not create an adapter");
		return false;
	}

	//Enumerate the primary adpater output, monitor.
	IDXGIOutput* adapter_output = nullptr;
	result = adapter->EnumOutputs(0, &adapter_output);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not enumerate the primary adapter output");
		return false;
	}

	//Get the number of modes of this type for the adapter output
	unsigned int num_modes;
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not get the display mode list");
		return false;
	}

	//Create a list to hold all the possible display modes for the monitor-video card combination
	DXGI_MODE_DESC* display_mode_list = new DXGI_MODE_DESC[num_modes];

	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, display_mode_list);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not get the display mode list");
		return false;
	}

	//Search the display mode that matches the screen width and height and store the numerator and denominator of the refresh rate for the monitor
	int numerator, denominator;
	for (int i = 0; i < num_modes; ++i)
	{
		if (display_mode_list[i].Width == (unsigned int)App->window->GetScreenWidth() && display_mode_list[i].Height == (unsigned int)App->window->GetScreenHeight())
		{
			numerator = display_mode_list[i].RefreshRate.Numerator;
			denominator = display_mode_list[i].RefreshRate.Denominator;
		}
	}

	//Get de video card description
	DXGI_ADAPTER_DESC adapter_desc;
	result = adapter->GetDesc(&adapter_desc);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not get the video card description");
		return false;
	}

	//Store the dedicated video card memory in MB
	video_card_memory = (int)(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

	//Get the name of the video card
	int string_length;
	int desc_error = wcstombs_s((size_t*)&string_length, video_card_description, 128, adapter_desc.Description, 128);
	if (desc_error != 0)
	{
		LOG("D3D Init exits with ERROR: Could not get the name of the video card");
		return false;
	}

	LOG("Video card description: %s", video_card_description);
	LOG("Video card memory %i MB", video_card_memory);

	delete[] display_mode_list;
	display_mode_list = nullptr;

	adapter_output->Release();
	adapter_output = nullptr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;

	//Initialize the swap chain description
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain));

	//Single back buffer
	swap_chain_desc.BufferCount = 1;

	swap_chain_desc.BufferDesc.Width = App->window->GetScreenWidth();
	swap_chain_desc.BufferDesc.Height = App->window->GetScreenHeight();

	//Set 32-bit surface for the back buffer
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//Set refresh rate of the back buffer
	if (App->window->IsVsyncEnabled())
	{
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = App->hWnd;

	//Turn multisampling off
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;

	//Set to full screen or windowed
	swap_chain_desc.Windowed = !App->window->IsFullScreen();

	//Set scan line ordering and scaling to unspecified
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Discard the back buffer content after present
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swap_chain_desc.Flags = 0;

	//Set to DirectX 11
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	//Create the swap chain, the Direct3D device and the Direct3D device context
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, NULL, &device_context);
	if (FAILED(result))
	{
		LOG("D3D exits with ERROR: Could not create the swapchain/Direct3D device/Direct3D device context. The primary video card may not be compatible with DirectX 11");
		return false;
	}

	//Get the pointer to the back buffer
	ID3D11Texture2D* back_buffer_ptr;
	result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr);
	if (FAILED(result))
	{
		LOG("D3D exits with ERROR: Could not get the pointer to the back buffer");
		return false;
	}

	//Create a render target view
	result = device->CreateRenderTargetView(back_buffer_ptr, NULL, &render_target_view);
	if (FAILED(result))
	{
		LOG("D3D exits with ERROR: Could not create a render target view");
		return false;
	}

	back_buffer_ptr->Release();
	back_buffer_ptr = nullptr;

	D3D11_TEXTURE2D_DESC depth_buffer_desc;
	ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));

	depth_buffer_desc.Width = App->window->GetScreenWidth();
	depth_buffer_desc.Height = App->window->GetScreenHeight();
	depth_buffer_desc.MipLevels = 1;
	depth_buffer_desc.ArraySize = 1;
	depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_buffer_desc.SampleDesc.Count = 1;
	depth_buffer_desc.SampleDesc.Quality = 0;
	depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_buffer_desc.CPUAccessFlags = 0;
	depth_buffer_desc.MiscFlags = 0;

	//Create a texture for the depth buffer
	result = device->CreateTexture2D(&depth_buffer_desc, NULL, &depth_stencil_buffer);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not create the texture for the depth buffer");
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));

	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	depth_stencil_desc.StencilEnable = true;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;

	//Stencil operations for front-facing pixels
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations for back-facing pixels
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Create the depth stencil states
	result = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not create the depth stencil state");
		return false;
	}

	//Set the depth stencil state
	device_context->OMSetDepthStencilState(depth_stencil_state, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stencil_view);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not create the depth stencil view");
		return false;
	}

	//Bind the render target view and the depth stencil buffer to the output render pipeline
	device_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

	//Set up the rasterizer description
	D3D11_RASTERIZER_DESC raster_desc;
	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = D3D11_CULL_BACK;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = D3D11_FILL_SOLID;
	raster_desc.FrontCounterClockwise = false;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = false;
	raster_desc.SlopeScaledDepthBias = 0.0f;

	result = device->CreateRasterizerState(&raster_desc, &rasterizer_state);
	if (FAILED(result))
	{
		LOG("D3D Init exits with ERROR: Could not create the rasterizer state");
		return false;
	}

	//Set the rasterizer state
	device_context->RSSetState(rasterizer_state);

	//Set up the viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)App->window->GetScreenWidth();
	viewport.Height = (float)App->window->GetScreenHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//Create the viewport
	device_context->RSSetViewports(1, &viewport);

	float fov = (float)D3DX_PI / 4.0f; //45º FOV. Change to 60?
	float screen_aspect = (float)App->window->GetScreenWidth() / (float)App->window->GetScreenHeight();

	//Create the projecion matrix
	D3DXMatrixPerspectiveFovLH(&projection_matrix, fov, screen_aspect, App->graphics->GetScreenNear(), App->graphics->GetScreenDepth());

	//Init the world matrix as identity matrix
	D3DXMatrixIdentity(&world_matrix);

	//Create an orthographic projection matrix for 2D rendering
	D3DXMatrixOrthoLH(&ortho_matrix, (float)App->window->GetScreenWidth(), (float)App->window->GetScreenHeight(), App->graphics->GetScreenNear(), App->graphics->GetScreenDepth());

	return true;
}

bool D3DModule::CleanUp()
{
	//Set to window mode before clean up to avoid exceptions
	if (swap_chain)
		swap_chain->SetFullscreenState(false, NULL);

	if (rasterizer_state)
	{
		rasterizer_state->Release();
		rasterizer_state = nullptr;
	}

	if (depth_stencil_view)
	{
		depth_stencil_view->Release();
		depth_stencil_view = nullptr;
	}

	if (depth_stencil_state)
	{
		depth_stencil_state->Release();
		depth_stencil_state = nullptr;
	}

	if (depth_stencil_buffer)
	{
		depth_stencil_buffer->Release();
		depth_stencil_buffer = nullptr;
	}

	if (render_target_view)
	{
		render_target_view->Release();
		render_target_view = nullptr;
	}

	if (device_context)
	{
		device_context->Release();
		device_context = nullptr;
	}

	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (swap_chain)
	{
		swap_chain->Release();
		swap_chain = nullptr;
	}

	return true;
}

void D3DModule::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	//Color to clear the buffer TODO: change it to make it editable
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	device_context->ClearRenderTargetView(render_target_view, color);

	device_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DModule::EndScene()
{
	if (vsync_enabled)
	{
		swap_chain->Present(1, 0);
	}
	else
	{
		swap_chain->Present(0, 0);
	}
}

ID3D11Device * D3DModule::GetDevice() const
{
	return device;
}

ID3D11DeviceContext * D3DModule::GetDeviceContext() const
{
	return device_context;
}

void D3DModule::GetProjectionMatrix(D3DXMATRIX & proj)
{
	proj = projection_matrix;
}

void D3DModule::GetWorldMatrix(D3DXMATRIX & world)
{
	world = world_matrix;
}

void D3DModule::GetOrthoMatrix(D3DXMATRIX & ortho)
{
	ortho = ortho_matrix;
}

void D3DModule::GetVideoCardInfo(char * card_name, int & memory)
{
	strcpy_s(card_name, 128, video_card_description);
	memory = video_card_memory;
}
