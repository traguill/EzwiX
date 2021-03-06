#include "ModuleWindow.h"

#include "Application.h"
#include "log.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx11.h"

ModuleWindow::ModuleWindow(const char * name, bool start_enabled) : Module(name, start_enabled)
{
}

ModuleWindow::~ModuleWindow()
{
}

bool ModuleWindow::Init()
{
	LOG("Init Window\n");

	WNDCLASSEX wc;
	DEVMODE dm_screen_settings;
	int pos_x, pos_y;

	// Get an external pointer to this object. (I think I don't need this...)
	//ApplicationHandle = this;

	App->hInstance = GetModuleHandle(NULL);
	App->app_name = "EzwiX";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = App->hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = App->app_name;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screen_width = GetSystemMetrics(SM_CXSCREEN);
	screen_height = GetSystemMetrics(SM_CYSCREEN);

	if (full_screen)
	{
		memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth = (unsigned long)screen_width;
		dm_screen_settings.dmPelsHeight = (unsigned long)screen_height;
		dm_screen_settings.dmBitsPerPel = 32;
		dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);

		pos_x = pos_y = 0;

		App->hWnd = CreateWindowEx(WS_EX_APPWINDOW, App->app_name, App->app_name, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, pos_x, pos_y, screen_width, screen_height, NULL, NULL, App->hInstance, NULL);
	}
	else
	{
		//Set windowed default resolution (for now it's this)
		screen_width = 1280;
		screen_height = 720;

		pos_x = (GetSystemMetrics(SM_CXSCREEN) - screen_width) / 2;
		pos_y = (GetSystemMetrics(SM_CYSCREEN) - screen_height) / 2;

		RECT wr = { 0, 0, screen_width, screen_height };
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

		App->hWnd = CreateWindowEx(WS_EX_APPWINDOW, App->app_name, App->app_name, WS_OVERLAPPEDWINDOW, pos_x, pos_y, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, App->hInstance, NULL);
	}

	//Set the window upper left position in the screen coordinates
	RECT wnd_rect;
	bool client_rect_ret = GetClientRect(App->hWnd, &wnd_rect);
	if (client_rect_ret == false)
	{
		LOG("Window Init ERROR while getting the client rect");
		return false;
	}

	win_pos_x = pos_x;
	win_pos_y = pos_y;

	ShowWindow(App->hWnd, SW_SHOW);
	SetForegroundWindow(App->hWnd);
	SetFocus(App->hWnd);

	//If necessary (hide the cursor)
	//ShowCursor(false);

	return true;
}

bool ModuleWindow::CleanUp()
{
	LOG("Destroying Window\n");

	ShowCursor(true); //In case we hide it before

	if (full_screen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(App->hWnd);
	App->hWnd = NULL;

	UnregisterClass(App->app_name, App->hInstance);
	App->hInstance = NULL;

	return true;
}

int ModuleWindow::GetScreenWidth() const
{
	return screen_width;
}

int ModuleWindow::GetScreenHeight() const
{
	return screen_height;
}

void ModuleWindow::GetWindowUpperLeftPosition(int & x, int & y) const
{
	x = win_pos_x;
	y = win_pos_y;
}

bool ModuleWindow::IsVsyncEnabled() const
{
	return vsync_enabled;
}

bool ModuleWindow::IsFullScreen() const
{
	return full_screen;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{

	ImGui_ImplDX11_WndProcHandler(hwnd, umessage, wparam, lparam);

	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_MOVE:
	{
		App->window->win_pos_x = (int)(short)LOWORD(lparam);
		App->window->win_pos_y = (int)(short)HIWORD(lparam);
		return true;
	}
	default:
	{
		return App->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
