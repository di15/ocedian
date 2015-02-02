

#ifndef _MAIN_H
#define _MAIN_H

#pragma warning(disable: 4996)
#pragma warning(disable: 4018)
#pragma warning(disable: 4160)
#pragma warning(disable: 4244)
#pragma warning(disable: 4800)

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <mmsystem.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <glaux.h>
#include <glext.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <commdlg.h>
#include <string>
#include <jpeglib.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "jpeg.lib")

using namespace std;

extern ofstream g_log;

#define VERSION				1.0f
#define TITLE				"Ocedian"
#define WIDTH				640
#define HEIGHT				480
#define BPP					32
#define FRAME_RATE			30
#define MIN_DISTANCE		1.0f
#define MAX_DISTANCE		9000.0f
#define FIELD_OF_VIEW		45.0f

extern int g_width;
extern int g_height;
extern int g_bpp;
extern bool g_quit;
extern POINT g_mouse;
extern POINT g_mousestart;
extern HDC g_hDC;
extern HGLRC g_hRC;
extern HWND g_hWnd;
extern HINSTANCE g_hInstance;
extern bool g_keys[256];
extern bool g_mousekeys[3];
extern bool g_active;
extern bool g_fullscreen;
extern bool g_mouseout;
extern double g_FrameInterval;

struct tResolution
{
	int width;
	int height;
};

extern tResolution g_selectedRes;
extern vector<tResolution> g_resolution;
extern vector<int> g_bpps;

enum GAMEMODE{LOADING, MENU, HOSTLOBBY, JOINLOBBY, PLAY};
extern GAMEMODE g_mode;
extern bool g_host;
extern string g_username;

void MousePosition();
void CenterMouse();
void WriteConfig();
void DestroyWindow();
bool MakeWindow();

#endif
