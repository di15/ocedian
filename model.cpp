



#include "3dmath.h"
#include "model.h"
#include "image.h"
#include "main.h"
#include "gui.h"
#include "shader.h"

CModel g_model[MODELS];
vector<tModelToLoad> g_modelsToLoad;

int NewModel()
{
	for(int i=0; i<MODELS; i++)
		if(!g_model[i].used)
			return i;

	return -1;
}

void AssignModel(int* id, char* filepath, CVector3 scale, CVector3 translate)
{
	tModelToLoad toLoad;
	toLoad.id = id;
	strcpy(toLoad.filepath, filepath);
	toLoad.scale = scale;
	toLoad.translate = translate;

	g_modelsToLoad.push_back(toLoad);
}

int FindModel(char* filepath)
{
	for(int i=0; i<MODELS; i++)
	{
		if(!g_model[i].used)
			continue;

		if(stricmp(g_model[i].filepath, filepath) == 0)
			return i;
	}

	return -1;
}

int LoadModel(char* filepath, CVector3 scale, CVector3 translate)
{
	int id = FindModel(filepath);
	if(id >= 0)
		return id;

	id = NewModel();
	g_model[id].used = true;
	g_model[id].Load(filepath, scale, translate);
	return id;
}

bool Load1Model()
{
	static int last = -1;

	if(last+1 < g_modelsToLoad.size())
		Status(g_modelsToLoad[last+1].filepath);

	if(last >= 0)
		(*g_modelsToLoad[last].id) = LoadModel(g_modelsToLoad[last].filepath, g_modelsToLoad[last].scale, g_modelsToLoad[last].translate);

	last ++;

	if(last >= g_modelsToLoad.size())
	{
		g_modelsToLoad.clear();
		return true;	// Done loading all models
	}

	return false;	// Not finished loading models
}

void CModel::Draw(int frame, CVector3 pos, float yaw)
{
	glPushMatrix();

	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(yaw, 0, 1, 0);

	CVertexArray* va = &vertexArrays[frame];
	CGroup* group;
	unsigned int tex;
	unsigned int texIndex;
	
	//glActiveTextureARB(GL_TEXTURE3_ARB);
	//glBindTexture(GL_TEXTURE_2D, spectex);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	for(int i=0; i<ms3d.m_numMeshes; i++)
	{
		texIndex = ms3d.m_pMeshes[i].m_materialIndex;
		tex = ms3d.m_pMaterials[ texIndex ].m_texture;
		glBindTexture(GL_TEXTURE_2D, tex);

		group = &va->groups[i];
	
		glVertexPointer(3, GL_FLOAT, 0, group->vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, group->texcoords);
		glNormalPointer(GL_FLOAT, 0, group->normals);
		
		glDrawArrays(GL_TRIANGLES, 0, group->numverts);
	}

	glPopMatrix();
}

void CModel::Load(char* file, CVector3 scale, CVector3 translate)
{
	strcpy(filepath, file);

	ms3d.loadModelData(file);
		
	specfile = new string[ ms3d.m_numMaterials ];
	spectex = new unsigned int[ ms3d.m_numMaterials ];

	for(int i=0; i<ms3d.m_numMaterials; i++)
	{
		char specfilepath[MAX_PATH+1];
		sprintf(specfilepath, "%s%s", ms3d.path, ms3d.m_pMaterials[i].m_pTextureFilename);
		int lastindex = string(specfilepath).find_last_of("."); 
		string rawname = string(specfilepath).substr(0, lastindex);
		string spec = rawname + "spec.jpg";
		specfile[i] = spec;
		CreateTexture(spectex[i], (LPSTR)specfile[i].c_str());
	}

	ms3d.GenerateVertexArrays(&vertexArrays, scale, translate);
	
	CorrectNormals();
}

void CModel::CorrectNormals()
{
	CVertexArray* va;
	CGroup* group;
	int numverts;
	CVector3* vertices;
	CVector3* normals;
	CVector2* texcoords;
	CVector3 normal;
	CVector3 tri[3];

	for(int frame=0; frame<ms3d.m_totalFrames; frame++)
	{
		va = &vertexArrays[frame];

		for(int g=0; g<ms3d.m_numMeshes; g++)
		{
			group = &va->groups[g];
			numverts = group->numverts;
			vertices = group->vertices;
			normals = group->normals;
			texcoords = group->texcoords;

			for(int i=0; i<numverts; i+=3)
			{
				tri[0] = vertices[i];
				tri[1] = vertices[i+1];
				tri[2] = vertices[i+2];
				normal = Normal2(tri);
				normals[i] = normal;
				normals[i+1] = normal;
				normals[i+2] = normal;
			}
		}
	}
}

void CModel::ReloadTexture()
{
	ms3d.reloadTextures();
	for(int i=0; i<ms3d.m_numMaterials; i++)
		CreateTexture(spectex[i], (LPSTR)specfile[i].c_str());
}

void BeginVertexArrays()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
}

void EndVertexArrays()
{
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glActiveTextureARB(GL_TEXTURE0_ARB);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}