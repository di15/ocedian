

#include "main.h"
#include "gui.h"
#include "keymap.h"
#include "shader.h"
#include "res.h"
#include "font.h"
#include "image.h"
#include "model.h"
#include "quake3bsp.h"
#include "frustum.h"
#include "physics.h"
#include "entity.h"

ofstream g_log;
int g_width = WIDTH;
int g_height = HEIGHT;
int g_bpp = BPP;
bool g_quit = false;
POINT g_mouse;
POINT g_mousestart;
HDC g_hDC = NULL;
HGLRC g_hRC = NULL;
HWND g_hWnd = NULL;
HINSTANCE g_hInstance;
bool g_keys[256];
bool g_mousekeys[3];
bool g_active = true;
bool g_fullscreen = false;
bool g_mouseout = false;
GAMEMODE g_mode = LOADING;
bool g_host = false;
string g_username;
double g_FrameInterval = 0.0f;

tResolution g_selectedRes;
vector<tResolution> g_resolution;
vector<int> g_bpps;

void MousePosition()
{
	GetCursorPos(&g_mouse);
	ScreenToClient(g_hWnd, &g_mouse);
}

void CenterMouse()
{
	g_mouse.x = g_width/2;
	g_mouse.y = g_height/2;
	POINT temp = g_mouse;
	ClientToScreen(g_hWnd, &temp);
	SetCursorPos(temp.x, temp.y);
}

void Resize(int width, int height)
{
	if(height == 0)
		height = 1;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, (float)width/(float)height, MIN_DISTANCE, MAX_DISTANCE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	g_width = width;
	g_height = height;

	if(g_fullscreen)
		ReloadTextures();
	RedoGUI();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_ACTIVATE:
		{
			if (!HIWORD(wParam)) // Check minimization state
				g_active = true;
			else
				g_active = false;

			return 0;
		}

		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE: // Screensaver trying to start?
				case SC_MONITORPOWER: // Monitor trying to enter powersave?
				return 0; // Prevent from happening
			}
			break;
		}
			
		case WM_CREATE:
		{
			if(g_fullscreen)
				CenterMouse();
		}break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_MOUSELEAVE:
		{
			g_mouseout = true;
		}break;

		case WM_MOUSEHOVER:
		{
			g_mouseout = false;
		}break;

		case WM_LBUTTONDOWN:
		{
			g_mousekeys[0] = true;
			g_GUI.lbuttondown();
		}break;
			
		case WM_LBUTTONUP:
		{
			g_mousekeys[0] = false;
			g_GUI.lbuttonup();
		}break;
		
		case WM_RBUTTONDOWN:
		{
			g_mousekeys[2] = true;
			g_GUI.rbuttondown();
		}break;
			
		case WM_RBUTTONUP:
		{
			g_mousekeys[2] = false;
			g_GUI.rbuttonup();
		}break;
		
		case WM_MBUTTONDOWN:
		{
			g_mousekeys[1] = true;
			g_GUI.mbuttondown();
		}break;
			
		case WM_MBUTTONUP:
		{
			g_mousekeys[1] = false;
			g_GUI.mbuttonup();
		}break;

		case WM_MOUSEMOVE:
		{
			MousePosition();
			g_GUI.mousemove();
		}break;

		case WM_MOUSEWHEEL:
		{
			g_GUI.mousewheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		}break;

		case WM_KEYDOWN:
		{
			g_keys[wParam] = true;
			g_GUI.keydown(wParam);
			return 0;
		}

		case WM_KEYUP:
		{
			g_keys[wParam] = false;	
			g_GUI.keyup(wParam);
			return 0;
		}

		case WM_SIZE:
		{
			Resize(LOWORD(lParam), HIWORD(lParam)); 
			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool InitWindow()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
    glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");

	if(!glActiveTextureARB || !glMultiTexCoord2fARB || !glClientActiveTextureARB)
	{
		MessageBox(g_hWnd, "Your current setup does not support multitexturing", "Error", MB_OK);
		return false;
	}

	InitGLSL();
	CreateFonts();
	RedoGUI();

	return true;
}

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f; // Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime; // Get the slice of time
	float desiredFPS = 1.0f / desiredFrameRate; // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		elapsedTime -= desiredFPS; // Adjust the elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}

void CalculateFrameRate()
{
	static double framesPerSecond   = 0.0f;		// This will store our fps
    static double lastTime			= 0.0f;		// This will hold the time from the last frame
	static char strFrameRate[50] = {0};			// We will store the string here for the window title

	static double frameTime = 0.0f;				// This stores the last frame's time

	// Get the current time in seconds
    double currentTime = timeGetTime() * 0.001f;				


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// We added a small value to the frame interval to account for some video
	// cards (Radeon's) with fast computers falling through the floor without it.

	// Here we store the elapsed time between the current and last frame,
	// then keep the current frame in our static variable for the next frame.
 	g_FrameInterval = currentTime - frameTime + 0.005f;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	frameTime = currentTime;

	// Increase the frame counter
    ++framesPerSecond;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
    if( currentTime - lastTime > 1.0f )
    {
		// Here we set the lastTime to the currentTime
	    lastTime = currentTime;
		
		// Copy the frames per second into a string to display in the window title bar
		//sprintf(strFrameRate, "Current Frames Per Second: %d", int(framesPerSecond));

		// Set the window title bar to our string
		//SetWindowText(g_hWnd, strFrameRate);

		// Reset the frames per second
        framesPerSecond = 0;
    }
}

void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();	

	if(g_mode == PLAY)
	{
		g_camera->Look();

	//	RenderToShadowMap();
	//	RenderShadowedScene();
		g_frustum.CalculateFrustum();
		
		glCullFace(GL_FRONT);
		g_map.RenderSky();
		glCullFace(GL_BACK);

		DrawBackdrop();
		
		glCullFace(GL_FRONT);
		g_map.RenderLevel(g_camera->Position());
		glCullFace(GL_BACK);

		BeginVertexArrays();
		DrawScenery();
		EndVertexArrays();
	}

	g_GUI.draw();

	SwapBuffers(g_hDC);
}

void DestroyWindow()
{
	FreeTextures();
	DestroyFonts();
	ReleaseShaders();

	if(g_fullscreen)
		ChangeDisplaySettings(NULL, 0);

	if(g_hRC)
	{
		if(!wglMakeCurrent(NULL,NULL))
			MessageBox(NULL, "Release of DC and RC failed", "Error", MB_OK | MB_ICONINFORMATION);

		if(!wglDeleteContext(g_hRC))
			MessageBox(NULL, "Release of rendering context failed", "Error", MB_OK | MB_ICONINFORMATION);

		g_hRC = NULL;
	}

	if(g_hDC && !ReleaseDC(g_hWnd, g_hDC))
	{
		MessageBox(NULL, "Release device context failed", "Error", MB_OK | MB_ICONINFORMATION);
		g_hDC = NULL;
	}

	if(g_hWnd && !DestroyWindow(g_hWnd))
	{
		MessageBox(NULL, "Could not release hWnd", "Error", MB_OK | MB_ICONINFORMATION);
		g_hWnd = NULL;
	}

	if(!UnregisterClass((TITLE),g_hInstance))
	{
		MessageBox(NULL, "Could not unregister class", "Error", MB_OK | MB_ICONINFORMATION);
		g_hInstance = NULL;
	}
}

bool MakeWindow()
{
	unsigned int PixelFormat;
	WNDCLASS wc;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)g_selectedRes.width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)g_selectedRes.height;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_TRIGEAR));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TITLE;

	if(!RegisterClass(&wc))
	{
		MessageBox(NULL, "Failed to register the window class", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if(g_fullscreen)
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= g_selectedRes.width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= g_selectedRes.height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= g_bpp;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			char msg[128];
			sprintf(msg, "The requested fullscreen mode %dx%dx%d is not supported by\nyour video card. Use windowed mode instead?", g_width, g_height, g_bpp);

			if(MessageBox(NULL, msg, TITLE, MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				g_fullscreen = false;
			else
			{
				MessageBox(NULL, "Program will now close", "Error", MB_OK | MB_ICONSTOP);
				g_quit = true;
				return FALSE;
			}
		}
	}

	int startx = 0;
	int starty = 0;

	if(g_fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
		//startx = CW_USEDEFAULT;
		//starty = CW_USEDEFAULT
		startx = GetSystemMetrics(SM_CXSCREEN)/2 - g_selectedRes.width/2;
		starty = GetSystemMetrics(SM_CYSCREEN)/2 - g_selectedRes.height/2;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!(g_hWnd = CreateWindowEx(dwExStyle, TITLE, TITLE, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								startx, starty,	WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top,
								NULL, NULL,	g_hInstance, NULL)))
	{
		DestroyWindow();
		MessageBox(NULL, "Window creation error", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	static PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		g_bpp,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(g_hDC = GetDC(g_hWnd)))	
	{
		DestroyWindow();
		MessageBox(NULL, "Can't create a GL device context", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!(PixelFormat = ChoosePixelFormat(g_hDC, &pfd)))
	{
		DestroyWindow();
		MessageBox(NULL, "Can't find a suitable pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(!SetPixelFormat(g_hDC, PixelFormat, &pfd))
	{
		DestroyWindow();
		MessageBox(NULL, "Can't set the pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!(g_hRC = wglCreateContext(g_hDC)))
	{
		DestroyWindow();
		MessageBox(NULL, "Can't create a GL rendering context", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(!wglMakeCurrent(g_hDC, g_hRC))
	{
		DestroyWindow();
		MessageBox(NULL, "Can't activate the GL rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);
	Resize(g_width, g_height);

	if(!InitWindow())
	{
		DestroyWindow();
		MessageBox(NULL, "Initialization failed", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

void UpdateLoading()
{
	static int stage = 0;

	switch(stage)
	{
	case 0: if(Load1Model()) stage++; break;
	case 1: Status("Loading map..."); stage++; break;
	case 2: g_map.LoadBSP("town"); stage++; break;
	case 3: g_mode = MENU; RedoGUI(); stage++; break;
	}
}

void UpdateGameState()
{
	if(g_mode != PLAY)
		return;

	CalculateFrameRate();

	g_camera->SetViewByMouse();

	Physics();
}

void Update()
{
	if(g_mode == LOADING)
		UpdateLoading();
	else if(g_mode == PLAY)
		UpdateGameState();
}

void LoadConfig()
{
	ifstream config("config.ini");

	getline(config, g_username);

	int fulls;
	config>>fulls;
	
	if(fulls)
		g_fullscreen = true;
	else
		g_fullscreen = false;

	config>>g_selectedRes.width>>g_selectedRes.height;
	config>>g_bpp;

	g_width = g_selectedRes.width;
	g_height = g_selectedRes.height;
}

void WriteConfig()
{
	ofstream config;
	config.open("config.ini", ios_base::out);

	config<<g_username<<endl;
	
	int fulls;
	if(g_fullscreen)
		fulls = 1;
	else
		fulls = 0;

	config<<fulls<<endl;
	config<<g_selectedRes.width<<" "<<g_selectedRes.height<<endl;
	config<<g_bpp;
}

void EnumerateDisplay()
{
	DEVMODE dm;
	int index=0;
	while(0 != EnumDisplaySettings(NULL, index++, &dm))
	{
		tResolution r;
		r.width = dm.dmPelsWidth;
		r.height = dm.dmPelsHeight;

		bool found = false;

		for(int i=0; i<g_resolution.size(); i++)
		{
			if(g_resolution[i].width == r.width && g_resolution[i].height == r.height)
			{
				found = true;
				break;
			}
		}

		if(!found)
			g_resolution.push_back(r);

		found = false;

		int bpp = dm.dmBitsPerPel;
		
		for(int i=0; i<g_bpps.size(); i++)
		{
			if(g_bpps[i] == bpp)
			{
				found = true;
				break;
			}
		}
		
		if(!found)
			g_bpps.push_back(bpp);
	}
}

void Init()
{
	g_log.open("log.txt", ios_base::out);
	g_log<<"Version "<<VERSION<<endl<<endl;
	g_log.flush();

	srand(GetTickCount());
	
	LoadConfig();
	EnumerateDisplay();
	MapKeys();
	Entities();
	Scenery();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	
	MSG msg;
	g_hInstance = hInstance;

	Init();

	if(!MakeWindow())
		return 0;

	while(!g_quit)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				g_quit = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (g_active && AnimateNextFrame(FRAME_RATE))
			{
				Update();
				Draw();
			}
			else
				Sleep(1);
		}
	}

	DestroyWindow();

	return msg.wParam;
}