


#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include "main.h"
#include "3dmath.h"

class CFrustum 
{
public:
	void CalculateFrustum();
	bool PointInFrustum(float x, float y, float z);
	bool SphereInFrustum(float x, float y, float z, float radius);
	bool CubeInFrustum(float x, float y, float z, float size);
	bool BoxInFrustum(float x, float y, float z, float sizeX, float sizeY, float sizeZ);

private:
	float m_Frustum[6][4];	// This holds the A B C and D values for each side of our frustum.
};

class CDebug 
{
public:
	void AddDebugLine(CVector3 vPoint1, CVector3 vPoint2);
	void AddDebugBox(CVector3 vCenter, float width, float height, float depth);
	void RenderDebugLines();		
	void Clear();					

private:
	vector<CVector3> m_vLines;		
};

extern CFrustum g_frustum;

#endif
