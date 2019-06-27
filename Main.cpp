#include <Windows.h>

int width = 1000;
int height = 600;
bool running = false;
HWND mainWindowHandle = nullptr;
int* pixelBuffer = nullptr;
BITMAPINFO pixelbufferInfo;

//TestCode:
int xPos = 100;
int yPos = 100;

void PutPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char p) {
	int index = 0;
	int colour = 0;
	
	//calculate colour:
	colour = p;
	colour <<= 8;
	colour |= r;
	colour <<= 8;
	colour |= g;
	colour <<= 8;
	colour |= b;

	//calculate index:
	index = (width*y) + x;

	//Write to pixelbuffer:
	pixelBuffer[index] = colour;
}

void DrawTestRectangle(int x, int y, int width, int height) {
	for (int i = x; i < x + width; i++) {
		for (int j = y; j < y+height; j++) {
			PutPixel(i, j, 255, 255, 255, 255);
		}
	}
}
//TestCode end.


void Update() {
	xPos++;
	if (xPos > 400) {
		xPos = 100;
	}
	yPos++;
	if (yPos > 400) {
		yPos = 100;
	}
}
void ClearPixelbuffer() {
	for (int i = 0; i < height*width; i++) {
		//colour is represented as: 0xPPRRGGBB (P=padding, R=red, G=green, B=blue) (padding will later be used as alpha)
		pixelBuffer[i] = 0x000000;
	}
}
void Draw() {
	DrawTestRectangle(xPos, yPos, 100, 100);
}
void PresentPixelbufferToWindow() 
{
	HDC currentDeviceContext = GetDC(mainWindowHandle);
	StretchDIBits(currentDeviceContext, 0, 0, width, height, 0, 0, width, height, pixelBuffer, &pixelbufferInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(mainWindowHandle, currentDeviceContext);
}

void UpdateMaster() {
	Update();
}
void DrawMaster() {
	ClearPixelbuffer();
	Draw();
	PresentPixelbufferToWindow();
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
		//Create pixelbuffer:
		pixelbufferInfo.bmiHeader.biSize = sizeof(pixelbufferInfo.bmiHeader);
		pixelbufferInfo.bmiHeader.biWidth = width;
		pixelbufferInfo.bmiHeader.biHeight = height;
		pixelbufferInfo.bmiHeader.biPlanes = 1;
		pixelbufferInfo.bmiHeader.biBitCount = sizeof(int) * 8;
		pixelbufferInfo.bmiHeader.biCompression = BI_RGB;

		pixelBuffer = new int[width*height];
		ClearPixelbuffer();
	}

	//Main loop:
	while (running) 
	{
		{
			//Handle window messages:
			MSG message = {};
			while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		{
			//Update:
			UpdateMaster();
		}
		{
			//render:
			DrawMaster();
		}
	}

	//clean up:
	delete[] pixelBuffer;
	return EXIT_SUCCESS;
}
