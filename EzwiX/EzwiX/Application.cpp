#include "Application.h"
#include "log.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleGraphics.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleFPS.h"
#include "ModuleCPU.h"
#include "ModuleTimer.h"

#include "Random.h"

Application::Application()
{
	rnd = new Random();

	timer = new ModuleTimer("timer");
	fps = new ModuleFPS("fps");
	m_cpu = new ModuleCPU("cpu");

	window = new ModuleWindow("window");
	input = new ModuleInput("input");
	camera = new ModuleCamera("camera");
	editor = new ModuleEditor("editor");
	graphics = new ModuleGraphics("graphics");
	
	list_modules.push_back(timer);
	list_modules.push_back(fps);
	list_modules.push_back(m_cpu);

	list_modules.push_back(window);
	list_modules.push_back(input);
	list_modules.push_back(camera);
	list_modules.push_back(editor);
	list_modules.push_back(graphics);
}

Application::~Application()
{
	vector<Module*>::reverse_iterator i = list_modules.rbegin();

	for (i; i != list_modules.rend(); ++i)
		delete (*i);

	delete rnd;
}

bool Application::Init()
{
	LOG("-------------------- Application Init ------------------");

	bool ret = true;

	ZeroMemory(&msg, sizeof(MSG));

	vector<Module*>::iterator module = list_modules.begin();

	for (module; module != list_modules.end(); ++module)
	{
		ret = (*module)->Init();
		if (ret == false)
			break;
	}

	return ret;
}

update_status Application::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message != WM_QUIT)
	{
		ret = UpdateFrame();
	}
	else
	{
		return update_status::UPDATE_STOP;
	}

	return ret;
}

update_status Application::UpdateFrame()
{
	//Check escape
	/*if (App->input->IsKeyDown(VK_ESCAPE))
	{
		return update_status::UPDATE_STOP;
	}*/
	
	update_status ret = update_status::UPDATE_CONTINUE;

	vector<Module*>::iterator module = list_modules.begin();

	//PreUpdate
	for (module; module != list_modules.end(); ++module)
	{
		ret = (*module)->PreUpdate();
		if (ret != update_status::UPDATE_CONTINUE)
			return ret;
	}

	//Update
	for (module = list_modules.begin(); module != list_modules.end(); ++module)
	{
		ret = (*module)->Update();
		if (ret != update_status::UPDATE_CONTINUE)
			return ret;
	}

	//PostUpdate
	for (module = list_modules.begin(); module != list_modules.end(); ++module)
	{
		ret = (*module)->PostUpdate();
		if (ret != update_status::UPDATE_CONTINUE)
			return ret;
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	
	vector<Module*>::reverse_iterator module = list_modules.rbegin();

	for (module; module != list_modules.rend(); ++module)
	{
		ret = (*module)->CleanUp();
		if (ret == false)
			break;
	}

	return ret;
}

LRESULT Application::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return LRESULT();
}
