
#ifndef _DEBUG_H
#define _DEBUG_H

#include <Windows.h>
#include <fstream>

using namespace std;


enum PROFILE{FRAME, UPDATE, DRAW, UPDATEUNITS, UPDATEBUILDINGS, DRAWBUILDINGS, DRAWUNITS, SORTPARTICLES, DRAWPARTICLES, DRAWMAP, SHADOWS, DRAWSKY, DRAWPOWERLINES, DRAWROADS, DRAWMODEL1, DRAWMODEL2, DRAWMODEL3, PROFILES};

class CProfile
{
public:
	char name[64];
	double averagems;
	long lastframe;
	double frames;
	//double framems;
	long starttick;
	//double timescountedperframe;
	//double lastframeaverage;
	double lastframetotal;
	int inside;

	CProfile()
	{
		lastframe = 0;
		frames = 0;
		inside = -1;
	}
};

extern CProfile g_profile[PROFILES];

void StartProfile(int id);
void EndProfile(int id);
void WriteProfiles(int in, int layer);
void InitProfiles();
void UpdateFPS();

void LastNum(char* l);

#endif