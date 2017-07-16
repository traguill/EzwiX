#include "ModuleGraphics.h"

#include "log.h"

#include "Application.h"
#include "D3DModule.h"
#include "ModuleCamera.h"

#include "ShaderClass.h"
#include "TextureShaderClass.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx11.h"

#include "Devil/include/il.h"
#include "Devil/include/ilut.h"
#pragma comment ( lib, "Devil/libx86/DevIL.lib" )
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )


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

	//Init ImGui
	ImGui_ImplDX11_Init(App->hWnd, d3d->GetDevice(), d3d->GetDeviceContext());

	//Init Devil
	ilInit();
	iluInit();

	return ret;
}

bool ModuleGraphics::CleanUp()
{
	LOG("Clean Up Graphics");

	models.clear();

	ImGui_ImplDX11_Shutdown();

	texture_shader->CleanUp();
	delete texture_shader;
	texture_shader = nullptr;

	d3d->CleanUp();
	delete d3d;

	ilShutDown();

	return true;
}

update_status ModuleGraphics::PreUpdate()
{
	d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	ImGui_ImplDX11_NewFrame();

	models.clear();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleGraphics::PostUpdate()
{
	D3DXMATRIX view_matrix, projection_matrix, world_matrix;

	App->camera->GetViewMatrix(view_matrix);
	d3d->GetWorldMatrix(world_matrix);
	App->camera->GetProjectionMatrix(projection_matrix);

	//Render all the objects.
	ComponentTexture* tex = nullptr;
	ComponentTexture* model_texture = nullptr;
	for (vector<ComponentMesh*>::iterator model = models.begin(); model != models.end(); ++model)
	{
		(*model)->Render();

		//TODO: Improve this approach
		model_texture = texture; //Texture by default
		tex = (ComponentTexture*)(*model)->GetGameObject()->GetComponent(C_TEXTURE);
		if (tex)
		{
			if (tex->GetTexturePath().size() > 0)
				model_texture = tex;
		}
		texture_shader->Render(d3d->GetDeviceContext(), (*model)->GetIndexCount(), D3DXMATRIX((*model)->GetGameObject()->transform->GetGlobalMatrixTransposed().ptr()), view_matrix, projection_matrix, model_texture->GetTexture());
	}
	

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

void ModuleGraphics::AddToDraw(ComponentMesh * mesh)
{
	if (mesh)
	{
		models.push_back(mesh);
	}
}
