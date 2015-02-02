
#include "entity.h"
#include "3dmath.h"
#include "main.h"
#include "model.h"
#include "frustum.h"

CEntityType g_entityType[ENTITY_TYPES];
CEntity g_entity[ENTITIES];
vector<CSpawn> g_spawn;
CSceneryType g_sceneryT[SCENERY_TYPES];
CScenery g_scenery[SCENERY];

void DrawScenery()
{
	CScenery* s;
	CSceneryType* t;

	for(int i=0; i<SCENERY; i++)
	{
		s = &g_scenery[i];

		if(!s->on)
			continue;

		if(!g_frustum.PointInFrustum(s->pos.x, s->pos.y, s->pos.z))
			continue;

		t = &g_sceneryT[s->type];

		g_model[t->model].Draw(0, s->pos, s->yaw);
	}
}

void Scenery(int i, char* file, CVector3 scale, CVector3 translate)
{
	strcpy(g_sceneryT[i].file, file);
	AssignModel(&g_sceneryT[i].model, file, scale, translate);
}

void Scenery()
{
	Scenery(STREETLIGHT, "models\\streetlight\\streetlight.ms3d", CVector3(1, 1, 1), CVector3(0, 0, 0));
}

int NewEntity()
{
	for(int i=0; i<ENTITIES; i++)
		if(!g_entity[i].on)
			return i;

	return -1;
}

int PlaceEntity(int type, int controller, CVector3 pos, float yaw)
{
	int i = NewEntity();
	if(i < 0)
		return i;

	g_entity[i].on = true;
	g_entity[i].type = type;
	g_entity[i].controller = controller;
	g_entity[i].camera.PositionCamera(pos.x, pos.y, pos.z, pos.x+1, pos.y, pos.z, 0, 1, 0);
	g_entity[i].camera.RotateView(DEGTORAD(yaw), 0, 1, 0);

	return i;
}

int NewScenery()
{
	for(int i=0; i<SCENERY; i++)
		if(!g_scenery[i].on)
			return i;

	return -1;
}

int FindSceneryT(char* file)
{
	for(int i=0; i<SCENERY_TYPES; i++)
		if(stricmp(g_sceneryT[i].file, file) == 0)
			return i;

	return -1;
}

void PlaceScenery(char* file, CVector3 pos, float angle)
{
	int i = NewScenery();
	if(i < 0)
		return;

	int type = FindSceneryT(file);
	if(type < 0)
		return;

	g_scenery[i].on = true;
	g_scenery[i].type = type;
	g_scenery[i].pos = pos;
	g_scenery[i].yaw = angle+90;
}

void ClearScenery()
{
	for(int i=0; i<SCENERY; i++)
		g_scenery[i].on = false;
}

void Entity(int type, CVector3 vMin, CVector3 vMax, float maxStep, float speed, float boost)
{
	CEntityType* t = &g_entityType[type];
	t->vMin = vMin;
	t->vMax = vMax;
	t->maxStep = maxStep;
	t->speed = speed;
	t->boost = boost;
}

void Entities()
{
	Entity(MECH, CVector3(-20, -110, -20), CVector3(20, 10, 20), 30, 300, 11);
}

/*
{
"classname" "worldspawn"
"farplane_cull" "1"
}
{
"classname" "info_player_start"
"origin" "1920 640 -64"
"angle" "360"
}*/

void ReadEntity(string classname, string origin, string angle, string model)
{
	if(stricmp(classname.c_str(), "info_player_start") == 0)
	{
		CVector3 pos;
		sscanf(origin.c_str(), "%f %f %f", &pos.x, &pos.y, &pos.z);
		float temp = pos.z;
		pos.z = -pos.y;
		pos.y = temp;
		float ang;
		sscanf(angle.c_str(), "%f", &ang);
		CSpawn spawn(pos, ang);
		g_spawn.push_back(spawn);
	}
	else if(stricmp(classname.c_str(), "item_enviro") == 0)
	{
		CVector3 pos;
		sscanf(origin.c_str(), "%f %f %f", &pos.x, &pos.y, &pos.z);
		float temp = pos.z;
		pos.z = -pos.y;
		pos.y = temp;
		float ang;
		sscanf(angle.c_str(), "%f", &ang);
		PlaceScenery((char*)model.c_str(), pos, ang);
	}
	//g_log<<"classname = "<<classname<<", origin = "<<origin<<", angle = "<<angle<<", model = "<<model<<endl;
}

void ReadEntities(char* str)
{
	g_spawn.clear();

	string classname;
	string origin;
	string angle;
	string model;
	string var;
	string val;

	int len = strlen(str);

	for(int i=0; i<len; i++)
	{
		classname = "";
		angle = "";
		origin = "";
		model = "";

		for(; i<len; i++)
			if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r')
				break;
		
		for(; i<len; i++)
			if(str[i] == '{')
			{
				i++;
				break;
			}

		for(; i<len; i++)
			if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r')
				break;

		while(str[i] != '}' && i < len)
		{
			var = "";
			val = "";

			for(; i<len; i++)
				if(str[i] == '"')
				{
					i++;
					break;
				}

			for(; i<len; i++)
			{
				if(str[i] == '"')
				{
					i++;
					break;
				}

				var = var + str[i];
			}

			for(; i<len; i++)
				if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r')
					break;

			for(; i<len; i++)
				if(str[i] == '"')
				{
					i++;
					break;
				}

			for(; i<len; i++)
			{
				if(str[i] == '"')
				{
					i++;
					break;
				}

				val = val + str[i];
			}

			if(stricmp(var.c_str(), "classname") == 0)
				classname = val;
			else if(stricmp(var.c_str(), "origin") == 0)
				origin = val;
			else if(stricmp(var.c_str(), "angle") == 0)
				angle = val;
			else if(stricmp(var.c_str(), "model") == 0)
				model = val;
			
			for(; i<len; i++)
				if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r')
					break;
		}

		ReadEntity(classname, origin, angle, model);
	}
}