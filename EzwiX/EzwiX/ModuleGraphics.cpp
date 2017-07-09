#include "ModuleGraphics.h"

#include "log.h"

#include "Application.h"
#include "D3DModule.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ModuleCamera.h"

#include "ShaderClass.h"
#include "TextureShaderClass.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx11.h"

ModuleGraphics::ModuleGraphics(const char * name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleGraphics::~ModuleGraphics()
{
}

bool ModuleGraphics::Init()
{
	LOG("Init Graphics");

	bool ret = true;
	
	d3d = new D3DModule();
	ret = d3d->Init();
	if (!ret)
	{
		MessageBox(App->hWnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	model = new ComponentMesh(ComponentType::C_MESH, nullptr); //Testing
	texture = new ComponentTexture(ComponentType::C_TEXTURE, nullptr);//Testing
	texture->Initialize(d3d->GetDevice(), "img.dds");
	
	//shader_class = new ShaderClass();
	texture_shader = new TextureShaderClass();

	ret = texture_shader->Init(d3d->GetDevice(), App->hWnd);
	if (!ret)
	{
		LOG("Could not initialzie Shader Class");
		return false;
	}

	ImGui_ImplDX11_Init(App->hWnd, d3d->GetDevice(), d3d->GetDeviceContext());

	return ret;
}

bool ModuleGraphics::CleanUp()
{
	LOG("Clean Up Graphics");

	ImGui_ImplDX11_Shutdown();

	texture_shader->CleanUp();
	delete texture_shader;
	texture_shader = nullptr;

	delete model;
	model = nullptr;

	d3d->CleanUp();
	delete d3d;

	return true;
}

update_status ModuleGraphics::PreUpdate()
{
	d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	ImGui_ImplDX11_NewFrame();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleGraphics::PostUpdate()
{
	D3DXMATRIX view_matrix, projection_matrix, world_matrix;

	App->camera->GetViewMatrix(view_matrix);
	d3d->GetWorldMatrix(world_matrix);
	App->camera->GetProjectionMatrix(projection_matrix);

	//Render all the objects. For now only a triangle
	model->Render();
	texture_shader->Render(d3d->GetDeviceContext(), model->GetIndexCount(), world_matrix, view_matrix, projection_matrix, texture->GetTexture());

	ImGui::Render();
	d3d->EndScene();

	return update_status::UPDATE_CONTINUE;
}

float ModuleGraphics::GetScreenDepth() const
{
	return screen_depth;
}

float ModuleGraphics::GetScreenNear() const
{
	return screen_near;
}

bool ModuleGraphics::IsVsyncEnabled() const
{
	return vsync_enabled;
}
