


#include "main.h"
#include "3dmath.h"
#include "gui.h"
#include "image.h"
#include "menu.h"
#include "entity.h"
#include "player.h"

void Click_OpenJoin()
{
}

void Click_Join()
{
}

void Click_JoinIP()
{
}

void Click_OpenHost()
{
	Click_Host();
}

void Click_Host()
{
	//g_camera = &g_entity[0].camera;
	CSpawn spawn = g_spawn[0];
	int e = PlaceEntity(MECH, g_localP, spawn.pos, spawn.angle);
	g_camera = &g_entity[e].camera;
	g_mode = PLAY;
	RedoGUI();
}

void Click_BackToGame()
{
	CloseView("quit");
}

void Click_Settings()
{
	OpenSoleView("settings");
}

void Click_Credits()
{
	OpenSoleView("credits");
}

void Click_Quit()
{
	PostQuitMessage(0);
}

void Click_BackToMain()
{
	OpenSoleView("main");
}

void Change_Fullscreen()
{
	int selected = g_GUI.getview("settings")->getdropdown("fullscreen")->selected;

	if(g_fullscreen == (bool)selected)
		return;

	DestroyWindow();
	g_fullscreen = selected;
	WriteConfig();
	MakeWindow();
	ReloadTextures();
	RedoGUI();
}

void Change_Resolution()
{
	int selected = g_GUI.getview("settings")->getdropdown("resolution")->selected;

	if(g_selectedRes.width == g_resolution[selected].width && g_selectedRes.height == g_resolution[selected].height)
		return;

	g_selectedRes = g_resolution[selected];
	WriteConfig();
	
	if(g_fullscreen)
	{	
		DestroyWindow();
		MakeWindow();
	}
	else
	{
		int startx = GetSystemMetrics(SM_CXSCREEN)/2 - g_selectedRes.width/2;
		int starty = GetSystemMetrics(SM_CYSCREEN)/2 - g_selectedRes.height/2;

		MoveWindow(g_hWnd, startx, starty, g_selectedRes.width, g_selectedRes.height, false);
	}
}

void Change_BPP()
{
	int selected = g_GUI.getview("settings")->getdropdown("bpp")->selected;

	if(g_bpp == g_bpps[selected])
		return;

	g_bpp = g_bpps[selected];
	WriteConfig();
	DestroyWindow();
	MakeWindow();
	ReloadTextures();
	RedoGUI();
}