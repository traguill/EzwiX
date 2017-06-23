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

	bool IsVsyncEnabled()const;
	bool IsFullScreen()const;

private:

	int screen_width = 0;
	int screen_height = 0;

	//TODO: Save this variables in a configuration file
	bool full_screen = false;
	bool vsync_enabled = true;

};

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif