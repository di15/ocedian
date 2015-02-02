

#include "debug.h"
#include "main.h"
#include "gui.h"

CProfile g_profile[PROFILES];
ofstream g_profF;

void LastNum(char* l)
{
	return;

	ofstream last;
	last.open("last.txt", ios_base::out);
	last<<l;
	last.flush();
}

void StartProfile(int id)
{
	if(g_mode != PLAY)
		return;

	g_profile[id].starttick = GetTickCount();
}

void EndProfile(int id)
{
	if(g_mode != PLAY)
		return;

	if(id == FRAME || g_profile[id].lastframe < g_profile[FRAME].lastframe)
	{
		//g_profile[id].averagems = ( g_profile[id].lastframeaverage + g_profile[id].averagems*g_profile[id].frames ) / (g_profile[id].frames+1);
		g_profile[id].averagems = ( g_profile[id].lastframetotal + g_profile[id].averagems*g_profile[id].frames ) / (g_profile[id].frames+1);
		g_profile[id].frames+=1.0;
		//g_profile[id].timescountedperframe = 0;
		g_profile[id].lastframetotal = 0;
		g_profile[id].lastframe = g_profile[FRAME].lastframe;

		//g_log<<g_profile[id].name<<" "<<g_profile[id].averagems<<"ms"<<endl;
	}
	if(id == FRAME)
		g_profile[id].lastframe++;
	
	double elapsed = GetTickCount() - g_profile[id].starttick;
	g_profile[id].lastframetotal += elapsed;
	//g_profile[id].lastframeaverage = ( elapsed + g_profile[id].lastframeaverage*g_profile[id].timescountedperframe ) / (g_profile[id].timescountedperframe+1);
	//g_profile[id].timescountedperframe+=1.0f;
}

void WriteProfiles(int in, int layer)
{
	if(in == -1)
		g_profF.open("profiles.txt", ios_base::out);

	double totalms = 0;
	double percentage;

	for(int j=0; j<PROFILES; j++)
	{
		if(g_profile[j].inside != in)
			continue;

		totalms += g_profile[j].averagems;
	}

	for(int j=0; j<PROFILES; j++)
	{
		if(g_profile[j].inside != in)
			continue;

		percentage = 100.0 * g_profile[j].averagems / totalms;

		for(int k=0; k<layer; k++)
			g_profF<<"\t";

		g_profF<<g_profile[j].name<<" "<<g_profile[j].averagems<<"ms, "<<percentage<<"%"<<endl;

		WriteProfiles(j, layer+1);
	}

	if(in == -1)
		g_profF.flush();
}

void Profile(int id, int inside, char* name)
{
	g_profile[id].inside = inside;
	strcpy(g_profile[id].name, name);
}

void InitProfiles()
{
	Profile(FRAME, -1, "Frame");
	Profile(UPDATE, FRAME, "Update();");
	Profile(DRAW, FRAME, "Draw();");
	Profile(UPDATEUNITS, UPDATE, "UpdateUnits();");
	Profile(UPDATEBUILDINGS, UPDATE, "UpdateBuildings();");
	Profile(DRAWBUILDINGS, DRAW, "DrawBuildings();");
	Profile(DRAWUNITS, DRAW, "DrawUnits();");
	Profile(SORTPARTICLES, DRAW, "SortParticles();");
	Profile(DRAWPARTICLES, DRAW, "DrawParticles();");
	Profile(DRAWMAP, DRAW, "DrawMap();");
	Profile(SHADOWS, DRAW, "Shadows");
	Profile(DRAWSKY, DRAW, "DrawSky();");
	Profile(DRAWPOWERLINES, DRAW, "DrawPowerlines();");
	Profile(DRAWROADS, DRAW, "DrawRoads();");
	Profile(DRAWMODEL1, DRAWBUILDINGS, "Draw model 1");
	Profile(DRAWMODEL2, DRAWBUILDINGS, "Draw model 2");
	Profile(DRAWMODEL3, DRAWBUILDINGS, "Draw model 3");
}

void UpdateFPS()
{
	char msg[128];
	sprintf(msg, "FPS: %f, %fms", 1000.0f / (float)g_profile[FRAME].lastframetotal, (float)g_profile[FRAME].lastframetotal);
	g_GUI.getview("play")->gettext("fps")->text = msg;
}