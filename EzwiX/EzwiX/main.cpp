#include <windows.h>
#include <windowsx.h>

#include "Application.h"
#include "log.h"

/*#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")*/

enum main_state
{
	MAIN_INIT,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LOG("Starting EzwiX");

	main_state state = MAIN_INIT;
	App = new Application();

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
			case MAIN_INIT:
				if (App->Init() == false)
				{
					LOG("Application Init exits with Error");
					state = MAIN_EXIT;
				}
				else
				{
					state = MAIN_UPDATE;
					LOG("-------------- Application Update -------------------");
				}
				break;
			case MAIN_UPDATE:
				{	int update_ret = App->Update();
				if (update_ret == UPDATE_ERROR)
				{
					LOG("Application Update exits with Error");
					state = MAIN_EXIT;
				}

				if (update_ret == UPDATE_STOP)
					state = MAIN_FINISH;
				break;
				}
			case MAIN_FINISH:
				LOG("------------------ Application CleanUp ----------------------");
				if (App->CleanUp() == false)
				{
					LOG("Application CleanUp exits with Error");
				}
				
				state = MAIN_EXIT;

				break;
		}
	}

	delete App;

	return 0;
}
