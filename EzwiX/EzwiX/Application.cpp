#include "Application.h"
#include "log.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

Application::Application()
{
	window = new ModuleWindow("window");
	input = new ModuleInput("input");

	list_modules.push_back(window);
	list_modules.push_back(input);
}

Application::~Application()
{
	vector<Module*>::reverse_iterator i = list_modules.rbegin();

	for (i; i != list_modules.rend(); ++i)
		delete (*i);
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
	if (App->input->IsKeyDown(VK_ESCAPE))
	{
		return update_status::UPDATE_STOP;
	}
	//Update graphics frame

	return update_status::UPDATE_CONTINUE;
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
		case WM_KEYDOWN:
		{
			App->input->KeyDown((unsigned int)wParam);
			return 0;
		}
		case WM_KEYUP:
		{
			App->input->KeyUp((unsigned int)wParam);
			return 0;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return LRESULT();
}
