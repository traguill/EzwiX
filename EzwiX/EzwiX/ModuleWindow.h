#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "Module.h"

#define FULL_SCREEN false

class ModuleWindow : public Module
{
public:
	ModuleWindow(const char* name, bool start_enabled = true);
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

private:

	int screen_width = 0;
	int screen_height = 0;

};

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif