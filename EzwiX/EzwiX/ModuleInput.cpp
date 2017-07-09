#include "ModuleInput.h"

#include "log.h"
#include "Globals.h"

#include "ModuleWindow.h"

ModuleInput::ModuleInput(const char * name, bool start_enabled) : Module(name, start_enabled)
{
	keyboard_keys = new KEY_STATE[256];
	ZeroMemory(keyboard_keys, sizeof(keyboard_keys));

	mouse_buttons = new KEY_STATE[3];
	ZeroMemory(mouse_buttons, sizeof(mouse_buttons));

	memset(&keys, false, sizeof(keys));
	memset(&mouse_input, false, sizeof(mouse_input));
}

ModuleInput::~ModuleInput()
{
	delete[] keyboard_keys;
}

bool ModuleInput::Init()
{
	LOG("Init Input\n");

	HRESULT result;

	screen_height = App->window->GetScreenHeight();
	screen_width = App->window->GetScreenWidth();

	mouse_x = mouse_y = mouse_z = 0;
	mouse_motion_x = mouse_motion_y = 0;

	return true;
}

bool ModuleInput::CleanUp()
{
	LOG("Clean Up Input\n");

	return true;
}

update_status ModuleInput::PreUpdate()
{
	ReadKeyboard();
	
	bool ret_mouse = ReadMouse();
	if (ret_mouse)
		ProcessMouseInput();

	//Quit
	if (keyboard_keys[KEY_ESC] == KEY_UP)
		return update_status::UPDATE_STOP;

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleInput::PostUpdate()
{
	mouse_z = 0;
	mouse_motion_x = mouse_motion_y = 0;

	return update_status::UPDATE_CONTINUE;
}

void ModuleInput::GetMousePosition(int & x, int & y) const
{
	x = mouse_x;
	y = mouse_y;
}

KEY_STATE ModuleInput::GetKey(int key_id) const
{
	return keyboard_keys[key_id];
}

int ModuleInput::GetMouseWheel() const
{
	return mouse_z;
}

void ModuleInput::GetMouseMotion(int & x, int & y) const
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}

KEY_STATE ModuleInput::GetMouseButton(MOUSE_BUTTON_ID id) const
{
	return mouse_buttons[id];
}

void ModuleInput::MSGKeyDown(unsigned int id)
{
	keys[id] = true;
}

void ModuleInput::MSGKeyUp(unsigned int id)
{
	keys[id] = false;
}

void ModuleInput::MSGMouseZ(int value)
{
	mouse_z = value / 120; //Uses multiples of 120 to track the wheel movement. Positive->Forward (away from the user) / Negative->Backward (toward the user)
}

void ModuleInput::MSGMouseButton(MOUSE_BUTTON_ID button_id, bool down)
{
	mouse_input[button_id] = down;
}

bool ModuleInput::ReadKeyboard()
{
	for (int i = 0; i < 256; ++i)
	{
		if (keys[i] == true) //Pressed
		{
			if (keyboard_keys[i] == KEY_IDLE)
				keyboard_keys[i] = KEY_DOWN;
			else
				keyboard_keys[i] = KEY_REPEAT;
		}
		else //Not pressed
		{
			if (keyboard_keys[i] == KEY_REPEAT || keyboard_keys[i] == KEY_DOWN)
				keyboard_keys[i] = KEY_UP;
			else
				keyboard_keys[i] = KEY_IDLE;
		}
	}

	return true;
}

bool ModuleInput::ReadMouse()
{
	//Windows input. For mouse position
	POINT point;
	bool ret = GetCursorPos(&point);

	if (ret)
	{
		mouse_state_x = point.x;
		mouse_state_y = point.y;
	}

	for (int i = 0; i < 3; ++i)
	{
		if (mouse_input[i] == true) //Pressed
		{
			if (mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else //Not pressed
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	return ret;
}

void ModuleInput::ProcessMouseInput()
{
	//Mouse Position
	int win_x, win_y;
	App->window->GetWindowUpperLeftPosition(win_x, win_y); //OPTIMIZATION: Save this as a variable and only updated when receive the moving window message

	int tmp_mouse_x, tmp_mouse_y;

	tmp_mouse_x = mouse_state_x - win_x;
	tmp_mouse_y = mouse_state_y - win_y;

	mouse_motion_x = tmp_mouse_x - mouse_x;
	mouse_motion_y = tmp_mouse_y - mouse_y;

	mouse_x = tmp_mouse_x;
	mouse_y = tmp_mouse_y;

	//Check boundaries
	if (mouse_x < 0) mouse_x = 0;
	if (mouse_y < 0) mouse_y = 0;
	if (mouse_x > screen_width) mouse_x = screen_width;
	if (mouse_y > screen_height) mouse_y = screen_height;
}
