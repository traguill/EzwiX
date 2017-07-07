#include "ModuleInput.h"

#include "log.h"
#include "Globals.h"

#include "ModuleWindow.h"

ModuleInput::ModuleInput(const char * name, bool start_enabled) : Module(name, start_enabled)
{
	keyboard_keys = new KEY_STATE[256];
	ZeroMemory(keyboard_keys, sizeof(keyboard_keys));
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

	mouse_x = mouse_y = 0;

	result = DirectInput8Create(App->hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&direct_input, NULL);
	if (FAILED(result))
	{
		LOG("Input ERROR while creating the main direct input interface");
		return false;
	}

	//Keyboard -----------------------------------------------------------------------------------------------------
	result = direct_input->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	if (FAILED(result))
	{
		LOG("Input ERROR while creating the keyboard direct input interface");
		return false;
	}

	//Set data format. Use the predefined data format for the keyboard
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		LOG("Input ERROR while setting the data format for the keyboard");
		return false;
	}

	//Set the cooperative level of the keyboard to not share with other programs. To share it with other programs set it to: DISCL_NONEXCLUSIVE
	result = keyboard->SetCooperativeLevel(App->hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		LOG("Input ERROR while setting the cooperative level of the keyboard");
		return false;
	}

	//Acquire the keyboard to use it
	result = keyboard->Acquire();
	if (FAILED(result))
	{
		LOG("Input ERROR while acquiring the keyboard.");
		return false;
	}

	//Mouse -------------------------------------------------------------------------------------------------------------------------------
	result = direct_input->CreateDevice(GUID_SysMouse, &mouse, NULL);
	if (FAILED(result))
	{
		LOG("Input ERROR while creating the direct input interface for the mouse.");
		return false;
	}

	//Predefined data format for the mouse
	result = mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		LOG("Input ERROR while setting the data format for the mouse.");
		return false;
	}
	
	//Cooperative level of the mouse shared with other programs
	result = mouse->SetCooperativeLevel(App->hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		LOG("Input ERROR while setting the cooperative level for the mouse");
		return false;
	}

	//Acquire the mouse
	result = mouse->Acquire();
	if (FAILED(result))
	{
		LOG("Input ERROR while acquiring the mouse.");
		return false;
	}

	return true;
}

bool ModuleInput::CleanUp()
{
	LOG("Clean Up Input\n");

	RELEASE(mouse);
	RELEASE(keyboard);
	RELEASE(direct_input);

	return true;
}

update_status ModuleInput::PreUpdate()
{
	ReadKeyboard();
	
	bool ret_mouse = ReadMouse();
	if (ret_mouse)
		ProcessMouseInput();

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

bool ModuleInput::ReadKeyboard()
{
	HRESULT result;

	result = keyboard->GetDeviceState(sizeof(keyboard_state), (LPVOID)&keyboard_state);
	if (FAILED(result))
	{
		//Check if the keyboard lost focus or was not acquired. Try to get control back
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			keyboard->Acquire();
		}
		else
		{
			LOG("Input ERROR: could not get the keyboard state");
			return false;
		}
	}
	else
	{
		for (int i = 0; i < 256; ++i)
		{
			if (keyboard_state[i] != 0) //Pressed
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
	}


	return true;
}

bool ModuleInput::ReadMouse()
{
	POINT point;
	bool ret = GetCursorPos(&point);

	if (ret)
	{
		mouse_state_x = point.x;
		mouse_state_y = point.y;
	}

	return true;
}

void ModuleInput::ProcessMouseInput()
{
	int win_x, win_y;
	App->window->GetWindowUpperLeftPosition(win_x, win_y);

	mouse_x = mouse_state_x - win_x;
	mouse_y = mouse_state_y - win_y;

	//Check boundaries
	if (mouse_x < 0) mouse_x = 0;
	if (mouse_y < 0) mouse_y = 0;
	if (mouse_x > screen_width) mouse_x = screen_width;
	if (mouse_y > screen_height) mouse_y = screen_height;
}
