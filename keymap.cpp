


#include "main.h"
#include "keymap.h"
#include "3dmath.h"
#include "gui.h"
#include "player.h"

void Escape()
{
	if(g_mode != PLAY)
		return;
	
	OpenAnotherView("quit");
}

void Forward()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].forward = true;
}

void Unforward()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].forward = false;
}

void Back()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].backward = true;
}

void Unback()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].backward = false;
}

void Left()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].left = true;
}

void Unleft()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].left = false;
}

void Right()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].right = true;
}

void Unright()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].right = false;
}

void Jump()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].jump = true;
}

void Unjump()
{
	if(g_mode != PLAY)
		return;

	g_player[g_localP].jump = false;
}

void MouseLeftButtonDown()
{
	if(g_mode != PLAY)
		return;
}

void MouseLeftButtonUp()
{
	if(g_mode != PLAY)
		return;
}

void MouseWheel(int delta)
{
	if(g_mode != PLAY)
		return;
}

void MouseMove()
{
	if(g_mode != PLAY)
		return;

	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.dwHoverTime = 100;
	tme.hwndTrack = g_hWnd;
	TrackMouseEvent(&tme);
}

void MapKeys()
{
	AssignKey(VK_ESCAPE, &Escape, NULL);
	AssignKey('W', &Forward, &Unforward);
	AssignKey('A', &Left, &Unleft);
	AssignKey('D', &Right, &Unright);
	AssignKey('S', &Back, &Unback);
	AssignKey(VK_SPACE, &Jump, &Unjump);
	AssignMouseWheel(&MouseWheel);
	AssignMouseMove(&MouseMove);
	AssignLButton(&MouseLeftButtonDown, &MouseLeftButtonUp);
}