#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#include "Module.h"

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum MOUSE_BUTTON_ID
{
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_RIGHT
};

class ModuleInput : public Module
{
public:
	ModuleInput(const char* name, bool start_enabled = true);
	~ModuleInput();

	bool Init();
	bool CleanUp();

	update_status PreUpdate();
	update_status PostUpdate();

	void GetMousePosition(int& x, int& y)const;
	KEY_STATE GetKey(int key_id)const;
	int GetMouseWheel()const;
	void GetMouseMotion(int& x, int& y)const;
	KEY_STATE GetMouseButton(MOUSE_BUTTON_ID id)const;

	//Messages
	void MSGKeyDown(unsigned int id);
	void MSGKeyUp(unsigned int id);
	void MSGMouseZ(int value);
	void MSGMouseButton(MOUSE_BUTTON_ID button_id, bool down);

private:

	bool ReadKeyboard();
	bool ReadMouse();

	void ProcessMouseInput();

private:
	bool keys[256];
	bool mouse_input[3];

	int mouse_state_x;
	int mouse_state_y;

	int screen_width;
	int screen_height;

	int mouse_x;
	int mouse_y;
	int mouse_z;

	int mouse_motion_x;
	int mouse_motion_y;

	KEY_STATE* keyboard_keys = nullptr;
	KEY_STATE* mouse_buttons = nullptr;

};

//Defines for all the keys. TODO: I'm sure there is a better way to do this. Search for more info.

#define KEY_ESC 0x1B
#define KEY_SPACEBAR 0x20

#define KEY_ARROW_LEFT 0x25
#define KEY_ARROW_UP 0x26
#define KEY_ARROW_RIGHT 0x27
#define KEY_ARROW_DOWN 0x28

#define KEY_0 0x30
#define KEY_1 0x31
#define KEY_2 0x32
#define KEY_3 0x33
#define KEY_4 0x34
#define KEY_5 0x35
#define KEY_6 0x36
#define KEY_7 0x37
#define KEY_8 0x38
#define KEY_9 0x39




//TODO: Do the same with the other keys. Too lazy to do it now...
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx


#endif // !__MODULE_INPUT_H_
