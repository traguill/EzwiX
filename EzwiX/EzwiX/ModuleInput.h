#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

#include "Module.h"

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	ModuleInput(const char* name, bool start_enabled = true);
	~ModuleInput();

	bool Init();
	bool CleanUp();

	update_status PreUpdate();

	void GetMousePosition(int& x, int& y)const;
	KEY_STATE GetKey(int key_id)const;

private:

	bool ReadKeyboard();
	bool ReadMouse();

	void ProcessMouseInput();

private:

	IDirectInput8* direct_input = nullptr;
	IDirectInputDevice8* keyboard = nullptr;
	IDirectInputDevice8* mouse = nullptr;

	unsigned char keyboard_state[256];
	int mouse_state_x;
	int mouse_state_y;

	int screen_width;
	int screen_height;
	int mouse_x;
	int mouse_y;

	KEY_STATE* keyboard_keys = nullptr;

};

#endif // !__MODULE_INPUT_H_
