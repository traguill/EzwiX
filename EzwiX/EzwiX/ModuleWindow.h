#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "Module.h"

class ModuleWindow : public Module
{
public:
	ModuleWindow(const char* name, bool start_enabled = true);
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	int GetScreenWidth()const;
	int GetScreenHeight()const;

	void GetWindowUpperLeftPosition(int& x, int& y)const;

	bool IsVsyncEnabled()const;
	bool IsFullScreen()const;

private:

	int screen_width = 0;
	int screen_height = 0;

	//TODO: Save this variables in a configuration file
	bool full_screen = false;
	bool vsync_enabled = true;

public:
	int win_pos_x = 0; //Upper left points
	int win_pos_y = 0;

};

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif