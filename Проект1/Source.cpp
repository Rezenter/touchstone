#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <d3d9.h>
#include <ctime>
#include <io.h>
#include "ObjReader.h"
#include "Log.h"

#pragma comment (lib, "d3d9.lib")
using namespace std;

LPDIRECT3D9 d3d;    
LPDIRECT3DDEVICE9 d3ddev;    
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;    

int HEIGHT;
int WIGHT;
string OBJ_PATH = "C:\\Users\\res\\Desktop\\";
DWORD LastFrameTime = 0;
DWORD FPSLimit;
DWORD currentTime;

void initD3D(HWND hWnd);
void render_frame(void);
void cleanD3D(void);
void init_graphics(void);

struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)


LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

class Resolution {
public:
	int h;
	int w;
	Resolution(int wight, int height) : h(height), w(wight) {}
	~Resolution() {};
};

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Log log("log.txt");
	ObjReader a(OBJ_PATH + "probe" + ".obj");
	a.createModel();
	int resolution = 0;
	ifstream file;
	if (_access("config.txt", 0) == -1) {
		log.write("config not found");
		return 0;
	}
	file.open("config.txt");
	log.write("config opened");
	string line;
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	Resolution availableResolutions[11] = {
	Resolution(800, 600),
	Resolution(1024, 768),
	Resolution(1152, 864),
	Resolution(1280, 800),
	Resolution(1280, 1024),
	Resolution(1600, 1200),
	Resolution(1440, 900),
	Resolution(1680, 1050),
	Resolution(1920, 1080),
	Resolution(1920, 1200),
	Resolution(desktop.right, desktop.bottom)
	};
	while (getline(file, line)) {
		istringstream iss(line);
		if (iss.str() == "resolutionValue") {
			getline(file, line);
			istringstream iss(line);
			resolution = stoi(iss.str());
			string resol = to_string(availableResolutions[resolution].h) + "x"
				+ to_string(availableResolutions[resolution].w);
			log.write("resolution = " + resol);
		}
		else if(iss.str() == "FPS"){
			getline(file, line);
			istringstream iss(line);
			FPSLimit = stoi(iss.str());
			log.write("fps limit = " + to_string(FPSLimit));
		}
	}
	file.close();
	HEIGHT = availableResolutions[resolution].h;
	WIGHT = availableResolutions[resolution].w;

	// the handle for the window, filled by a function
	HWND hWnd;
	// this struct holds information for the window class
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	// register the window class
	RegisterClassEx(&wc);

	// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		L"WindowClass1",    // name of the window class
		L"!!!",   // title of the window
		WS_EX_TOPMOST | WS_POPUP,    // fullscreen values
		0,    // x-position of the window
		0,    // y-position of the window
		WIGHT,    // width of the window
		HEIGHT,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

				  // display the window on the screen
	ShowWindow(hWnd, nCmdShow);
	initD3D(hWnd);
	// enter the main loop:

	// this struct holds Windows event messages
	MSG msg;

	

	while (TRUE)
	{
		//*****************************************************************************************************************
		msg.message = WM_QUIT;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		render_frame();
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
	
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	} break;
	}
	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}
// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = FALSE;   
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;    // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = WIGHT;    // set the width of the buffer
	d3dpp.BackBufferHeight = HEIGHT;    // set the height of the buffer

								   // create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);
	init_graphics();    // call the function to initialize the triangle
}


// this is the function used to render a single frame
void render_frame(void)
{
	currentTime = time(nullptr);
	if ((currentTime - LastFrameTime) < (1000 / FPSLimit))
	{
		Sleep(currentTime - LastFrameTime);
	}
	LastFrameTime = currentTime;
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 200) , 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

							 // do 3D rendering on the back buffer here

							 // select which vertex format we are using
	d3ddev->SetFVF(CUSTOMFVF);

	// select the vertex buffer to display
	d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

	// copy the vertex buffer to the back buffer
	d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	v_buffer->Release();    // close and release the vertex buffer
	d3ddev->Release();    // close and release the 3D device
	d3d->Release();    // close and release Direct3D
}

// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
	// create the vertices using the CUSTOMVERTEX struct
	CUSTOMVERTEX vertices[] =
	{
		{ 400.0f, 62.5f, 0.5f, 1.0f, D3DCOLOR_ARGB(255, 0, 0, 100), },
		{ 650.0f, 500.0f, 0.5f, 1.0f, D3DCOLOR_ARGB(255, 0, 100, 0), },
		{ 150.0f, 500.0f, 0.5f, 1.0f, D3DCOLOR_ARGB(255, 100, 0, 0), },
	};

	// create a vertex buffer interface called v_buffer
	d3ddev->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	VOID* pVoid;    // a void pointer

					// lock v_buffer and load the vertices into it
	v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	v_buffer->Unlock();
}
