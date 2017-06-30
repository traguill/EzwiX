#ifndef __APLICATION_H__
#define __APLICATION_H__

#include <windows.h>
#include <vector>

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleGraphics;
class ModuleCamera;
class ModuleEditor;
class ModuleFPS;
class ModuleCPU;
class ModuleTimer;

class Random;

using namespace std;

#define WIN32_LEAN_AND_MEAN

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

class Application
{
public:
	Application();
	~Application();

	bool Init();
	update_status Update();
	update_status UpdateFrame();
	bool CleanUp();

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:


public:
	LPCSTR app_name;
	HINSTANCE hInstance;
	HWND hWnd;

	//Modules
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleGraphics* graphics = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleFPS* fps = nullptr;
	ModuleCPU* m_cpu = nullptr;
	ModuleTimer* timer = nullptr;

	//Other
	Random* rnd = nullptr;

private:
	MSG msg;

	vector<Module*> list_modules;
};

extern Application* App;

#endif // !__APLICATION_H__
