
#include "ms3d.h"
#include "image.h"
#include "main.h"
#include "model.h"

MS3DModel::MS3DModel()
{
	m_numMeshes = 0;
	m_pMeshes = NULL;
	m_numMaterials = 0;
	m_pMaterials = NULL;
	m_numTriangles = 0;
	m_pTriangles = NULL;
	m_numVertices = 0;
	m_pVertices = NULL;
	m_numJoints = 0;
	m_pJoints = NULL;
}

MS3DModel::~MS3DModel()
{
	int i;
	for ( i = 0; i < m_numMeshes; i++ )
		delete[] m_pMeshes[i].m_pTriangleIndices;
	for ( i = 0; i < m_numMaterials; i++ )
		delete[] m_pMaterials[i].m_pTextureFilename;

	m_numMeshes = 0;
	if ( m_pMeshes != NULL )
	{
		delete[] m_pMeshes;
		m_pMeshes = NULL;
	}

	m_numMaterials = 0;
	if ( m_pMaterials != NULL )
	{
		delete[] m_pMaterials;
		m_pMaterials = NULL;
	}

	m_numTriangles = 0;
	if ( m_pTriangles != NULL )
	{
		delete[] m_pTriangles;
		m_pTriangles = NULL;
	}

	m_numVertices = 0;
	if ( m_pVertices != NULL )
	{
		delete[] m_pVertices;
		m_pVertices = NULL;
	}

	m_numJoints = 0;
	if(m_pJoints != NULL)
	{
		delete [] m_pJoints;
		m_pJoints = NULL;
	}
}

void MS3DModel::reloadTextures()
{
	for ( int i = 0; i < m_numMaterials; i++ )
		if ( strlen( m_pMaterials[i].m_pTextureFilename ) > 0 )
		{
			char filepath[MAX_PATH+1];
			sprintf(filepath, "%s%s", path, m_pMaterials[i].m_pTextureFilename);
			CreateTexture(m_pMaterials[i].m_texture, filepath);
		}
		else
			m_pMaterials[i].m_texture = 0;
}


bool MS3DModel::loadModelData( const char *filename )
{
	g_log<<filename<<"\n\r";

	ifstream inputFile( filename, ios::in | ios::binary );
	if ( inputFile.fail()) 
	{
		g_log << "Couldn't open the model file "<< filename << endl;
		return false;
	}

	char pathTemp[MAX_PATH+1];
	int pathLength;
	for ( pathLength = strlen( filename ); --pathLength; ) 
	{
		if ( filename[pathLength] == '/' || filename[pathLength] == '\\' ) 
			break;
	}
	strncpy( pathTemp, filename, pathLength );
	
	int i;
	if ( pathLength > 0 ) 
	{
		pathTemp[pathLength++] = '/';
	}
	
	strncpy( path, filename, pathLength );

	inputFile.seekg( 0, ios::end );
	long fileSize = inputFile.tellg();
	inputFile.seekg( 0, ios::beg );

	char *pBuffer = new char[fileSize];
	inputFile.read( pBuffer, fileSize );
	inputFile.close();

	const char *pPtr = pBuffer;
	MS3DHeader *pHeader = ( MS3DHeader* )pPtr;
	pPtr += sizeof( MS3DHeader );

	if ( strncmp( pHeader->m_ID, "MS3D000000", 10 ) != 0 ) 
	{
		g_log << "Not an MS3D file "<< filename << endl;
		return false;
    }

	if ( pHeader->m_version < 3 ) 
	{
		g_log << "I know nothing about MS3D v1.2" << endl;
		return false;
	}

	int nVertices = *( word* )pPtr; 
	m_numVertices = nVertices;
	m_pVertices = new Vertex[nVertices];
	pPtr += sizeof( word );

	for ( i = 0; i < nVertices; i++ )
	{
		MS3DVertex *pVertex = ( MS3DVertex* )pPtr;
		m_pVertices[i].m_boneID = pVertex->m_boneID;
		memcpy( m_pVertices[i].m_location, pVertex->m_vertex, sizeof( float )*3 );
		pPtr += sizeof( MS3DVertex );
	}

	int nTriangles = *( word* )pPtr;
	m_numTriangles = nTriangles;
	m_pTriangles = new Triangle[nTriangles];
	pPtr += sizeof( word );

	for ( i = 0; i < nTriangles; i++ )
	{
		MS3DTriangle *pTriangle = ( MS3DTriangle* )pPtr;
		int vertexIndices[3] = { pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2] };
		float t[3] = { 1.0f-pTriangle->m_t[0], 1.0f-pTriangle->m_t[1], 1.0f-pTriangle->m_t[2] };
		memcpy( m_pTriangles[i].m_vertexNormals, pTriangle->m_vertexNormals, sizeof( float )*3*3 );
		memcpy( m_pTriangles[i].m_s, pTriangle->m_s, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_t, t, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_vertexIndices, vertexIndices, sizeof( int )*3 );
		pPtr += sizeof( MS3DTriangle );
	}

	int nGroups = *( word* )pPtr;
	m_numMeshes = nGroups;
	m_pMeshes = new Mesh[nGroups];
	pPtr += sizeof( word );
	for ( i = 0; i < nGroups; i++ )
	{
		pPtr += sizeof( byte );	// flags
		pPtr += 32;				// name

		word nTriangles = *( word* )pPtr;
		pPtr += sizeof( word );
		int *pTriangleIndices = new int[nTriangles];
		for ( int j = 0; j < nTriangles; j++ )
		{
			pTriangleIndices[j] = *( word* )pPtr;
			pPtr += sizeof( word );
		}

		char materialIndex = *( char* )pPtr;
		pPtr += sizeof( char );
	
		m_pMeshes[i].m_materialIndex = materialIndex;
		m_pMeshes[i].m_numTriangles = nTriangles;
		m_pMeshes[i].m_pTriangleIndices = pTriangleIndices;
	}

	int nMaterials = *( word* )pPtr;
	m_numMaterials = nMaterials;
	m_pMaterials = new Material[nMaterials];
	pPtr += sizeof( word );
	for ( i = 0; i < nMaterials; i++ )
	{
		MS3DMaterial *pMaterial = ( MS3DMaterial* )pPtr;
		memcpy( m_pMaterials[i].m_ambient, pMaterial->m_ambient, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_diffuse, pMaterial->m_diffuse, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_specular, pMaterial->m_specular, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_emissive, pMaterial->m_emissive, sizeof( float )*4 );
		m_pMaterials[i].m_shininess = pMaterial->m_shininess;
		if ( strncmp( pMaterial->m_texture, ".\\", 2 ) == 0 ) {
			// MS3D 1.5.x relative path
			strcpy( pathTemp + pathLength, pMaterial->m_texture + 2 );
			m_pMaterials[i].m_pTextureFilename = new char[strlen( pathTemp )+1];
			strcpy( m_pMaterials[i].m_pTextureFilename, pathTemp );
		}
		else {
			// MS3D 1.4.x or earlier - absolute path
			m_pMaterials[i].m_pTextureFilename = new char[strlen( pMaterial->m_texture )+1];
			strcpy( m_pMaterials[i].m_pTextureFilename, pMaterial->m_texture );
		}
		PathStripPath(m_pMaterials[i].m_pTextureFilename);
		pPtr += sizeof( MS3DMaterial );
	}

	reloadTextures();

	float animFPS = *( float* )pPtr;
	pPtr += sizeof( float );

	// skip currentTime
	pPtr += sizeof( float );

	m_totalFrames = *( int* )pPtr;
	pPtr += sizeof( int );

	m_totalTime = m_totalFrames*1000.0/animFPS;

	m_numJoints = *( word* )pPtr;
	pPtr += sizeof( word );

	m_pJoints = new Joint[m_numJoints];

	struct JointNameListRec
	{
		int m_jointIndex;
		const char *m_pName;
	};

	const char *pTempPtr = pPtr;

	JointNameListRec *pNameList = new JointNameListRec[m_numJoints];
	for ( i = 0; i < m_numJoints; i++ )
	{
		MS3DJoint *pJoint = ( MS3DJoint* )pTempPtr;
		pTempPtr += sizeof( MS3DJoint );
		pTempPtr += sizeof( MS3DKeyframe )*( pJoint->m_numRotationKeyframes+pJoint->m_numTranslationKeyframes );

		pNameList[i].m_jointIndex = i;
		pNameList[i].m_pName = pJoint->m_name;
	}

	for ( i = 0; i < m_numJoints; i++ )
	{
		MS3DJoint *pJoint = ( MS3DJoint* )pPtr;
		pPtr += sizeof( MS3DJoint );

		int j, parentIndex = -1;
		if ( strlen( pJoint->m_parentName ) > 0 )
		{
			for ( j = 0; j < m_numJoints; j++ )
			{
				if ( stricmp( pNameList[j].m_pName, pJoint->m_parentName ) == 0 )
				{
					parentIndex = pNameList[j].m_jointIndex;
					break;
				}
			}
			if ( parentIndex == -1 ) {
				g_log << "Unable to find parent bone in MS3D file" << endl;
				return false;
			}
		}

		memcpy( m_pJoints[i].m_localRotation, pJoint->m_rotation, sizeof( float )*3 );
		memcpy( m_pJoints[i].m_localTranslation, pJoint->m_translation, sizeof( float )*3 );
		m_pJoints[i].m_parent = parentIndex;
		m_pJoints[i].m_numRotationKeyframes = pJoint->m_numRotationKeyframes;
		m_pJoints[i].m_pRotationKeyframes = new Keyframe[pJoint->m_numRotationKeyframes];
		m_pJoints[i].m_numTranslationKeyframes = pJoint->m_numTranslationKeyframes;
		m_pJoints[i].m_pTranslationKeyframes = new Keyframe[pJoint->m_numTranslationKeyframes];

		for ( j = 0; j < pJoint->m_numRotationKeyframes; j++ )
		{
			MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
			pPtr += sizeof( MS3DKeyframe );

			setJointKeyframe( i, j, pKeyframe->m_time*1000.0f, pKeyframe->m_parameter, true );
		}

		for ( j = 0; j < pJoint->m_numTranslationKeyframes; j++ )
		{
			MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
			pPtr += sizeof( MS3DKeyframe );

			setJointKeyframe( i, j, pKeyframe->m_time*1000.0f, pKeyframe->m_parameter, false );
		}
	}
	delete[] pNameList;

	setupJoints();

	delete[] pBuffer;

	restart();

	return true;
}

void MS3DModel::GenerateVertexArrays(CVertexArray** vertexArrays, CVector3 scale, CVector3 translate)
{
	(*vertexArrays) = new CVertexArray[ m_totalFrames ];

	CVector3* vertices;
	CVector2* texcoords;
	CVector3* normals;
	CVertexArray* va;
	int numverts;
	int vert;
	CGroup* group;

	for(int f = 0; f < m_totalFrames; f++)
	{
		advanceAnimation();

		va = vertexArrays[f];
		
		va->groups = new CGroup[ m_numMeshes ];
  
		for(int i = 0; i < m_numMeshes; i++)
		{	
			group = & va->groups[i];
			numverts = 3 * m_pMeshes[i].m_numTriangles;
			
			group->numverts = numverts;
			group->vertices = new CVector3[ numverts ];
			group->texcoords = new CVector2[ numverts ];
			group->normals = new CVector3[ numverts ];
			
			vertices = group->vertices;
			texcoords = group->texcoords;
			normals = group->normals;
	
			vert = 0;

			for(int j = 0; j < m_pMeshes[i].m_numTriangles; j++)
			{
				int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
				const Triangle* pTri = &m_pTriangles[triangleIndex];

				for(int k = 0; k < 3; k++)
				{
					int index = pTri->m_vertexIndices[k];
				
					if(m_pVertices[index].m_boneID == -1)
					{
						texcoords[vert].x = pTri->m_s[k];
						texcoords[vert].y = 1.0f - pTri->m_t[k];
				
						normals[vert].x = pTri->m_vertexNormals[k][0];
						normals[vert].y = pTri->m_vertexNormals[k][1];
						normals[vert].z = pTri->m_vertexNormals[k][2];
				
						vertices[vert].x = m_pVertices[index].m_location[0] * scale.x + translate.x;
						vertices[vert].y = m_pVertices[index].m_location[1] * scale.y + translate.y;
						vertices[vert].z = m_pVertices[index].m_location[2] * scale.z + translate.z;
					}
					else
					{
						// rotate according to transformation matrix
						const CMatrix& final = m_pJoints[m_pVertices[index].m_boneID].m_final;
					
						texcoords[vert].x = pTri->m_s[k];
						texcoords[vert].y = 1.0f - pTri->m_t[k];

						CVector3 newNormal(pTri->m_vertexNormals[k]);
						newNormal.Transform3(final);
						newNormal = Normalize(newNormal);
					
						normals[vert] = newNormal;

						CVector3 newVertex(m_pVertices[index].m_location);
						newVertex.Transform(final);
					
						vertices[vert].x = newVertex.x * scale.x + translate.x;
						vertices[vert].y = newVertex.y * scale.y + translate.y;
						vertices[vert].z = newVertex.z * scale.z + translate.z;
					}

					vert ++;
				}
			}
		}
	}
}

void MS3DModel::setJointKeyframe( int jointIndex, int keyframeIndex, float time, float *parameter, bool isRotation )
{
	Keyframe& keyframe = isRotation ? m_pJoints[jointIndex].m_pRotationKeyframes[keyframeIndex] :
		m_pJoints[jointIndex].m_pTranslationKeyframes[keyframeIndex];

	keyframe.m_jointIndex = jointIndex;
	keyframe.m_time = time;
	memcpy( keyframe.m_parameter, parameter, sizeof( float )*3 );
}

void MS3DModel::setupJoints()
{
	int i;
	for ( i = 0; i < m_numJoints; i++ )
	{
		Joint& joint = m_pJoints[i];

		joint.m_relative.setRotationRadians( joint.m_localRotation );
		joint.m_relative.setTranslation( joint.m_localTranslation );
		if ( joint.m_parent != -1 )
		{
			joint.m_absolute.set( m_pJoints[joint.m_parent].m_absolute.getMatrix());
			joint.m_absolute.postMultiply( joint.m_relative );
		}
		else
			joint.m_absolute.set( joint.m_relative.getMatrix());
	}

	for ( i = 0; i < m_numVertices; i++ )
	{
		Vertex& vertex = m_pVertices[i];

		if ( vertex.m_boneID != -1 )
		{
			CMatrix& matrix = m_pJoints[vertex.m_boneID].m_absolute;

			matrix.inverseTranslateVect( vertex.m_location );
			matrix.inverseRotateVect( vertex.m_location );
		}
	}

	for ( i = 0; i < m_numTriangles; i++ ) {
		Triangle& triangle = m_pTriangles[i];
		for ( int j = 0; j < 3; j++ ) {
			const Vertex& vertex = m_pVertices[triangle.m_vertexIndices[j]];
			if ( vertex.m_boneID != -1 ) {
				CMatrix& matrix = m_pJoints[vertex.m_boneID].m_absolute;
				matrix.inverseRotateVect( triangle.m_vertexNormals[j] );
			}
		}
	}
}

void MS3DModel::restart()
{
	for ( int i = 0; i < m_numJoints; i++ )
	{
		m_pJoints[i].m_currentRotationKeyframe = m_pJoints[i].m_currentTranslationKeyframe = 0;
		m_pJoints[i].m_final.set( m_pJoints[i].m_absolute.getMatrix());
	}
}

void MS3DModel::advanceAnimation()
{
	static int frame = 0;

	double time = m_totalTime * (double)frame/(double)m_totalFrames;

	frame++;

	if(time > m_totalTime)
	{
		restart();
		time = 0;
	}

	for(int i = 0; i < m_numJoints; i++)
	{
		float transVec[3];
		CMatrix transform;
		int frame;
		Joint *pJoint = &m_pJoints[i];

		if(pJoint->m_numRotationKeyframes == 0 && pJoint->m_numTranslationKeyframes == 0)
		{
			pJoint->m_final.set( pJoint->m_absolute.getMatrix());
			continue;
		}

		frame = pJoint->m_currentTranslationKeyframe;
		while(frame < pJoint->m_numTranslationKeyframes && pJoint->m_pTranslationKeyframes[frame].m_time < time)
		{
			frame++;
		}
		pJoint->m_currentTranslationKeyframe = frame;

		if(frame == 0)
			memcpy( transVec, pJoint->m_pTranslationKeyframes[0].m_parameter, sizeof ( float )*3 );
		else if(frame == pJoint->m_numTranslationKeyframes)
			memcpy( transVec, pJoint->m_pTranslationKeyframes[frame-1].m_parameter, sizeof ( float )*3 );
		else
		{
			//assert( frame > 0 && frame < pJoint->m_numTranslationKeyframes );

			const MS3DModel::Keyframe& curFrame = pJoint->m_pTranslationKeyframes[frame];
			const MS3DModel::Keyframe& prevFrame = pJoint->m_pTranslationKeyframes[frame-1];

			float timeDelta = curFrame.m_time-prevFrame.m_time;
			float interpValue = ( float )(( time-prevFrame.m_time )/timeDelta );

			transVec[0] = prevFrame.m_parameter[0]+( curFrame.m_parameter[0]-prevFrame.m_parameter[0] )*interpValue;
			transVec[1] = prevFrame.m_parameter[1]+( curFrame.m_parameter[1]-prevFrame.m_parameter[1] )*interpValue;
			transVec[2] = prevFrame.m_parameter[2]+( curFrame.m_parameter[2]-prevFrame.m_parameter[2] )*interpValue; 
		}

		frame = pJoint->m_currentRotationKeyframe;
		while(frame < pJoint->m_numRotationKeyframes && pJoint->m_pRotationKeyframes[frame].m_time < time)
		{
			frame++;
		}
		pJoint->m_currentRotationKeyframe = frame;

		if(frame == 0)
			transform.setRotationRadians( pJoint->m_pRotationKeyframes[0].m_parameter );
		else if(frame == pJoint->m_numRotationKeyframes)
			transform.setRotationRadians( pJoint->m_pRotationKeyframes[frame-1].m_parameter );
		else
		{
			const MS3DModel::Keyframe& curFrame = pJoint->m_pRotationKeyframes[frame];
			const MS3DModel::Keyframe& prevFrame = pJoint->m_pRotationKeyframes[frame-1];
			
			float timeDelta = curFrame.m_time-prevFrame.m_time;
			float interpValue = (float)(( time-prevFrame.m_time )/timeDelta );

#if 0
			Quaternion qPrev( prevFrame.m_parameter );
			Quaternion qCur( curFrame.m_parameter );
			Quaternion qFinal( qPrev, qCur, interpValue );
			transform.setRotationQuaternion( qFinal );
#else
			float rotVec[3];

			rotVec[0] = prevFrame.m_parameter[0]+( curFrame.m_parameter[0]-prevFrame.m_parameter[0] )*interpValue;
			rotVec[1] = prevFrame.m_parameter[1]+( curFrame.m_parameter[1]-prevFrame.m_parameter[1] )*interpValue;
			rotVec[2] = prevFrame.m_parameter[2]+( curFrame.m_parameter[2]-prevFrame.m_parameter[2] )*interpValue;

			transform.setRotationRadians( rotVec );
#endif
		}

		transform.setTranslation( transVec );
		CMatrix relativeFinal( pJoint->m_relative );
		relativeFinal.postMultiply( transform );

		if ( pJoint->m_parent == -1 )
			pJoint->m_final.set( relativeFinal.getMatrix());
		else
		{
			pJoint->m_final.set( m_pJoints[pJoint->m_parent].m_final.getMatrix());
			pJoint->m_final.postMultiply( relativeFinal );
		}
	}
}