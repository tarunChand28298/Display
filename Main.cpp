#include <Windows.h>

int width = 1000;
int height = 600;
bool running = false;
HWND mainWindowHandle = nullptr;
int* backbuffer = nullptr;
BITMAPINFO backbufferInfo;

void PresentBackbufferToWindow() 
{
	HDC currentDeviceContext = GetDC(mainWindowHandle);
	StretchDIBits(currentDeviceContext, 0, 0, width, height, 0, 0, width, height, backbuffer, &backbufferInfo, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK MainWindowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam) 
{
	switch (message)
	{
		case WM_CREATE: {
			running = true;
			break;
		}
		case WM_CLOSE: {
			DestroyWindow(windowHandle);
			break;
		}
		case WM_DESTROY: {
			running = false;
			break;
		}
		default: {
			break;
		}
	}

	return DefWindowProc(windowHandle, message, wparam, lparam);
}

int main() 
{
	//Setup:
	{
		//Create window:
		WNDCLASS wc = {};
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = MainWindowProcedure;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(0);
		wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = 0;
		wc.lpszClassName = (LPCWSTR)L"MainWindowClassName";
		RegisterClass(&wc);

		mainWindowHandle = CreateWindow((LPCWSTR)L"MainWindowClassName", (LPCWSTR)"", WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, GetModuleHandle(0), nullptr);
		ShowWindow(mainWindowHandle, SW_SHOW);
	}
	{
		//Create backbuffer:
		backbufferInfo.bmiHeader.biSize = sizeof(backbufferInfo.bmiHeader);
		backbufferInfo.bmiHeader.biWidth = width;
		backbufferInfo.bmiHeader.biHeight = height;
		backbufferInfo.bmiHeader.biPlanes = 1;
		backbufferInfo.bmiHeader.biBitCount = sizeof(int) * 8;
		backbufferInfo.bmiHeader.biCompression = BI_RGB;

		backbuffer = new int[width*height];
		for (int i = 0; i < height*width; i++) {
			//in little endian architectures, colour is represented as: 0xPPRRGGBB (P=padding, R=red, G=green, B=blue) (padding will later be used as alpha)
			backbuffer[i] = 0x000000;
		}
	}

	//Main loop:
	while (running) 
	{
		{
			//Handle window messages:
			MSG message = {};
			while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		{
			//render from backbuffer:
			PresentBackbufferToWindow();
		}
	}

	//clean up:
	delete[] backbuffer;
	return 0;
}
