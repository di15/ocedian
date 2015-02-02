



#ifndef _MODEL_H
#define _MODEL_H

#include <Windows.h>
#include <string>
#include "ms3d.h"
#include "3dmath.h"

using namespace std;

class CGroup
{
public:
	int numverts;
	CVector3* vertices;
	CVector2* texcoords;
	CVector3* normals;

	CGroup()
	{
		int numverts = 0;
	}

	void free()
	{
		if(numverts <= 0)
			return;

		delete [] vertices;
		delete [] texcoords;
		delete [] normals;
		numverts = 0;
	}

	~CGroup()
	{
		free();
	}
};

class CVertexArray
{
public:
	CGroup* groups;

	CVertexArray()
	{
		groups = NULL;
	}

	void free()
	{
		if(groups != NULL)
		{
			delete [] groups;
			groups = NULL;
		}
	}

	~CVertexArray()
	{
		free();
	}
};

class MS3DModel;

class CModel
{
public:
	bool used;
	MS3DModel ms3d;
	CVertexArray* vertexArrays;
	string* specfile;
	unsigned int* spectex;
	char filepath[128];

	CModel()
	{
		vertexArrays = NULL;
		used = false;
		specfile = NULL;
		spectex = NULL;
	}

	~CModel()
	{

		if(vertexArrays != NULL)
		{
			delete [] vertexArrays;
			vertexArrays = NULL;
		}

		if(specfile != NULL)
		{
			delete [] specfile;
			specfile = NULL;
		}

		if(spectex != NULL)
		{
			delete [] spectex;
			spectex = NULL;
		}
	}
	
	void CorrectNormals();

	void Load(char* file, CVector3 scale, CVector3 translate);
	void ReloadTexture();
	void Draw(int frame, CVector3 pos, float yaw);
};

#define MODELS	128
extern CModel g_model[MODELS];

int NewModel();
void AssignModel(int* id, char* filepath, CVector3 scale, CVector3 translate);
int LoadModel(char* filepath, CVector3 scale, CVector3 translate);
bool Load1Model();

struct tModelToLoad
{
	int* id;
	char filepath[MAX_PATH];
	CVector3 scale;
	CVector3 translate;
};

void BeginVertexArrays();
void EndVertexArrays();

#endif