
#include <vector>
#include "3dmath.h"

using namespace std;

class CEntityType
{
public:
	CVector3 vMin, vMax;
	float maxStep;
	float speed;
	float boost;
};

enum ENTITY{MECH, ENTITY_TYPES};
extern CEntityType g_entityType[ENTITY_TYPES];

class CEntity
{
public:
	bool on;
	int type;
	int controller;
	CCamera camera;

	CEntity()
	{
		on = false;
		controller = -1;
	}
};

#define ENTITIES	128
extern CEntity g_entity[ENTITIES];

class CSpawn
{
public:
	CVector3 pos;
	float angle;

	CSpawn(CVector3 p, float a)
	{
		pos = p;
		angle = a;
	}
};

extern vector<CSpawn> g_spawn;

class CSceneryType
{
public:
	char file[128];
	int model;
};

enum SCENERYTYPE{STREETLIGHT, SCENERY_TYPES};
extern CSceneryType g_sceneryT[SCENERY_TYPES];

class CScenery
{
public:
	bool on;
	int type;
	CVector3 pos;
	float yaw;

	CScenery()
	{
		on = false;
	}
};

#define SCENERY		128
extern CScenery g_scenery[SCENERY];

void DrawScenery();
void Scenery();
void ClearScenery();
int PlaceEntity(int type, int controller, CVector3 pos, float yaw);
void Entities();
void ReadEntities(char* str);