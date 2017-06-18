#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Handle for the window
	HWND hWnd;
	
	//Information for the window class
	WNDCLASSEX wc;

	//Clear out the window
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "WindowClass1";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(
		NULL,
		"WindowClass1", //name of the window class
		"EzwiX Window", //title of the window
		WS_OVERLAPPEDWINDOW, //style of the window
		300, //x-position
		300, //y-position
		500, //width
		400, //height
		NULL, //parent window
		NULL, //for menus
		hInstance, //application handle
		NULL //for multiple windows
	);

	ShowWindow(hWnd, nCmdShow);

	//Handles Windows event messages
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		//send the mesage to the WindowProc function
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}