



#include "3dmath.h"
#include "main.h"
#include "quake3bsp.h"
#include "physics.h"

CCamera* g_camera;

float clip(float n, float lower, float upper)
{
  return max(lower, min(n, upper));
}

float Magnitude(CVector3 vNormal)
{
	return (float)sqrt( (vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z) );
}

float Magnitude2(CVector3 vNormal)
{
	return (vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z);
}

CVector3 Normalize(CVector3 vNormal)
{
	float magnitude = Magnitude(vNormal);

	vNormal.x /= magnitude;
	vNormal.y /= magnitude;
	vNormal.z /= magnitude;

	return vNormal;
}

CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;
	
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vNormal;
}


CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;

	vVector.x = vPoint1.x - vPoint2.x;
	vVector.y = vPoint1.y - vPoint2.y;
	vVector.z = vPoint1.z - vPoint2.z;

	return vVector;
}

float PlaneDistance(CVector3 Normal, CVector3 Point)
{	
	float distance = 0; // This variable holds the distance from the plane to the origin

	// Use the plane equation to find the distance (Ax + By + Cz + D = 0)  We want to find D.
	// So, we come up with D = -(Ax + By + Cz)
	// Basically, the negated dot product of the normal of the plane and the point.
	distance = - ((Normal.x * Point.x) + (Normal.y * Point.y) + (Normal.z * Point.z));

	return distance;
}

// Clockwise
CVector3 Normal(CVector3 vTriangle[])					
{
	CVector3 vVector1 = Vector(vTriangle[2], vTriangle[0]);
	CVector3 vVector2 = Vector(vTriangle[1], vTriangle[0]);

	CVector3 vNormal = Cross(vVector1, vVector2);

	vNormal = Normalize(vNormal);

	return vNormal;
}

// Counter-clockwise
CVector3 Normal2(CVector3 vTriangle[])					
{
	CVector3 vVector1 = Vector(vTriangle[2], vTriangle[0]);
	CVector3 vVector2 = Vector(vTriangle[1], vTriangle[0]);

	CVector3 vNormal = Cross(vVector2, vVector1);

	vNormal = Normalize(vNormal);

	return vNormal;
}

bool IntersectedPlane(CVector3 vPoly[], CVector3 vLine[], CVector3 &vNormal, float &originDistance)
{
	float distance1=0, distance2=0;						// The distances from the 2 points of the line from the plane
			
	vNormal = Normal(vPoly);

	// Let's find the distance our plane is from the origin.  We can find this value
	// from the normal to the plane (polygon) and any point that lies on that plane (Any vertice)
	originDistance = PlaneDistance(vNormal, vPoly[0]);

	// Get the distance from point1 from the plane using: Ax + By + Cz + D = (The distance from the plane)

	distance1 = ((vNormal.x * vLine[0].x)  +					// Ax +
		         (vNormal.y * vLine[0].y)  +					// Bx +
				 (vNormal.z * vLine[0].z)) + originDistance;	// Cz + D
	
	// Get the distance from point2 from the plane using Ax + By + Cz + D = (The distance from the plane)
	
	distance2 = ((vNormal.x * vLine[1].x)  +					// Ax +
		         (vNormal.y * vLine[1].y)  +					// Bx +
				 (vNormal.z * vLine[1].z)) + originDistance;	// Cz + D

	// Now that we have 2 distances from the plane, if we times them together we either
	// get a positive or negative number.  If it's a negative number, that means we collided!
	// This is because the 2 points must be on either side of the plane (IE. -1 * 1 = -1).

	if(distance1 * distance2 >= 0)			// Check to see if both point's distances are both negative or both positive
	   return false;
					
	return true;
}

float Dot(CVector3 vVector1, CVector3 vVector2) 
{
	return ( (vVector1.x * vVector2.x) + (vVector1.y * vVector2.y) + (vVector1.z * vVector2.z) );
}

double AngleBetweenVectors(CVector3 Vector1, CVector3 Vector2)
{							
	float dotProduct = Dot(Vector1, Vector2);				
	float vectorsMagnitude = Magnitude(Vector1) * Magnitude(Vector2) ;
	double angle = acos( dotProduct / vectorsMagnitude );

	if(_isnan(angle))
		return 0;

	return( angle );
}

CVector3 IntersectionPoint(CVector3 vNormal, CVector3 vLine[], double distance)
{
	CVector3 vPoint;
	CVector3 vLineDir;
	double Numerator = 0.0, Denominator = 0.0, dist = 0.0;

	// Here comes the confusing part.  We need to find the 3D point that is actually
	// on the plane.  Here are some steps to do that:
	
	// 1)  First we need to get the vector of our line, Then normalize it so it's a length of 1
	vLineDir = Vector(vLine[1], vLine[0]);		// Get the Vector of the line
	vLineDir = Normalize(vLineDir);				// Normalize the lines vector


	// 2) Use the plane equation (distance = Ax + By + Cz + D) to find the distance from one of our points to the plane.
	//    Here I just chose a arbitrary point as the point to find that distance.  You notice we negate that
	//    distance.  We negate the distance because we want to eventually go BACKWARDS from our point to the plane.
	//    By doing this is will basically bring us back to the plane to find our intersection point.
	Numerator = - (vNormal.x * vLine[0].x +		// Use the plane equation with the normal and the line
				   vNormal.y * vLine[0].y +
				   vNormal.z * vLine[0].z + distance);

	// 3) If we take the dot product between our line vector and the normal of the polygon,
	//    this will give us the cosine of the angle between the 2 (since they are both normalized - length 1).
	//    We will then divide our Numerator by this value to find the offset towards the plane from our arbitrary point.
	Denominator = Dot(vNormal, vLineDir);		// Get the dot product of the line's vector and the normal of the plane
				  
	// Since we are using division, we need to make sure we don't get a divide by zero error
	// If we do get a 0, that means that there are INFINATE points because the the line is
	// on the plane (the normal is perpendicular to the line - (Normal.Vector = 0)).  
	// In this case, we should just return any point on the line.

	if( Denominator == 0.0)						// Check so we don't divide by zero
		return vLine[0];						// Return an arbitrary point on the line

	// We divide the (distance from the point to the plane) by (the dot product)
	// to get the distance (dist) that we need to move from our arbitrary point.  We need
	// to then times this distance (dist) by our line's vector (direction).  When you times
	// a scalar (single number) by a vector you move along that vector.  That is what we are
	// doing.  We are moving from our arbitrary point we chose from the line BACK to the plane
	// along the lines vector.  It seems logical to just get the numerator, which is the distance
	// from the point to the line, and then just move back that much along the line's vector.
	// Well, the distance from the plane means the SHORTEST distance.  What about in the case that
	// the line is almost parallel with the polygon, but doesn't actually intersect it until half
	// way down the line's length.  The distance from the plane is short, but the distance from
	// the actual intersection point is pretty long.  If we divide the distance by the dot product
	// of our line vector and the normal of the plane, we get the correct length.  Cool huh?

	dist = Numerator / Denominator;				// Divide to get the multiplying (percentage) factor
	
	// Now, like we said above, we times the dist by the vector, then add our arbitrary point.
	// This essentially moves the point along the vector to a certain distance.  This now gives
	// us the intersection point.  Yay!

	vPoint.x = (float)(vLine[0].x + (vLineDir.x * dist));
	vPoint.y = (float)(vLine[0].y + (vLineDir.y * dist));
	vPoint.z = (float)(vLine[0].z + (vLineDir.z * dist));

	return vPoint;								// Return the intersection point
}


bool InsidePolygon(CVector3 vIntersection, CVector3 Poly[], long verticeCount)
{
	const double MATCH_FACTOR = 0.9999;		// Used to cover up the error in floating point
	double Angle = 0.0;						// Initialize the angle
	CVector3 vA, vB;						// Create temp vectors
	
	// Just because we intersected the plane, doesn't mean we were anywhere near the polygon.
	// This functions checks our intersection point to make sure it is inside of the polygon.
	// This is another tough function to grasp at first, but let me try and explain.
	// It's a brilliant method really, what it does is create triangles within the polygon
	// from the intersection point.  It then adds up the inner angle of each of those triangles.
	// If the angles together add up to 360 degrees (or 2 * PI in radians) then we are inside!
	// If the angle is under that value, we must be outside of polygon.  To further
	// understand why this works, take a pencil and draw a perfect triangle.  Draw a dot in
	// the middle of the triangle.  Now, from that dot, draw a line to each of the vertices.
	// Now, we have 3 triangles within that triangle right?  Now, we know that if we add up
	// all of the angles in a triangle we get 180° right?  Well, that is kinda what we are doing,
	// but the inverse of that.  Say your triangle is an equilateral triangle, so add up the angles
	// and you will get 180° degree angles.  60 + 60 + 60 is 360°.

	for (int i = 0; i < verticeCount; i++)		// Go in a circle to each vertex and get the angle between
	{	
		vA = Vector(Poly[i], vIntersection);	// Subtract the intersection point from the current vertex
												// Subtract the point from the next vertex
		vB = Vector(Poly[(i + 1) % verticeCount], vIntersection);
												
		Angle += AngleBetweenVectors(vA, vB);	// Find the angle between the 2 vectors and add them all up as we go along
	}

	// Now that we have the total angles added up, we need to check if they add up to 360 degrees.
	// Since we are using the dot product, we are working in radians, so we check if the angles
	// equals 2*PI.  We defined PI in 3DMath.h.  You will notice that we use a MATCH_FACTOR
	// in conjunction with our desired degree.  This is because of the inaccuracy when working
	// with floating point numbers.  It usually won't always be perfectly 2 * PI, so we need
	// to use a little twiddling.  I use .9999, but you can change this to fit your own desired accuracy.
												
	if(Angle >= (MATCH_FACTOR * (2.0 * M_PI)) )	// If the angle is greater than 2 PI, (360 degrees)
		return true;							// The point is inside of the polygon
		
	return false;								// If you get here, it obviously wasn't inside the polygon, so Return FALSE
}

bool IntersectedPolygon(CVector3 vPoly[], CVector3 vLine[], int verticeCount, CVector3* vIntersection)
{
	CVector3 vNormal;// = {0};
	float originDistance = 0;

	// First we check to see if our line intersected the plane.  If this isn't true
	// there is no need to go on, so return false immediately.
	// We pass in address of vNormal and originDistance so we only calculate it once

									 // Reference   // Reference
	if(!IntersectedPlane(vPoly, vLine,   vNormal,   originDistance))
		return false;

	// Now that we have our normal and distance passed back from IntersectedPlane(), 
	// we can use it to calculate the intersection point.  The intersection point
	// is the point that actually is ON the plane.  It is between the line.  We need
	// this point test next, if we are inside the polygon.  To get the I-Point, we
	// give our function the normal of the plan, the points of the line, and the originDistance.

	CVector3 vTemp = IntersectionPoint(vNormal, vLine, originDistance);

	// Now that we have the intersection point, we need to test if it's inside the polygon.
	// To do this, we pass in :
	// (our intersection point, the polygon, and the number of vertices our polygon has)

	if(InsidePolygon(vTemp, vPoly, verticeCount))
	{
		if(vIntersection != NULL)
			(*vIntersection) = vTemp;

		return true;
	}

	return false;
}

CVector3 Rotate(CVector3 v, float rad, float x, float y, float z)
{
	CVector3 newV;
	float cosTheta = (float)cos(rad);
	float sinTheta = (float)sin(rad);

	newV.x  = (cosTheta + (1 - cosTheta) * x * x)		* v.x;
	newV.x += ((1 - cosTheta) * x * y - z * sinTheta)	* v.y;
	newV.x += ((1 - cosTheta) * x * z + y * sinTheta)	* v.z;

	newV.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* v.x;
	newV.y += (cosTheta + (1 - cosTheta) * y * y)		* v.y;
	newV.y += ((1 - cosTheta) * y * z - x * sinTheta)	* v.z;

	newV.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* v.x;
	newV.z += ((1 - cosTheta) * y * z + x * sinTheta)	* v.y;
	newV.z += (cosTheta + (1 - cosTheta) * z * z)		* v.z;

	return newV;
}

bool PointBehindPlane(CVector3 p, CVector3 normal, float dist)
{
	float result = p.x*normal.x + p.y*normal.y + p.z*normal.z + dist;

	if(result < 0)
		return true;

	return false;
}

float GetYaw(float dx, float dz)
{
	return atan2(dx, dz);
}

void CVector3::Transform( const CMatrix& m )
{
	double vector[4];
	const float *matrix = m.getMatrix();

	vector[0] = x*matrix[0]+y*matrix[4]+z*matrix[8]+matrix[12];
	vector[1] = x*matrix[1]+y*matrix[5]+z*matrix[9]+matrix[13];
	vector[2] = x*matrix[2]+y*matrix[6]+z*matrix[10]+matrix[14];
	vector[3] = x*matrix[3]+y*matrix[7]+z*matrix[11]+matrix[15];

	x = vector[0];
	y = vector[1];
	z = vector[2];
}

void CVector3::Transform3( const CMatrix& m )
{
	double vector[3];
	const float *matrix = m.getMatrix();

	vector[0] = x*matrix[0]+y*matrix[4]+z*matrix[8];
	vector[1] = x*matrix[1]+y*matrix[5]+z*matrix[9];
	vector[2] = x*matrix[2]+y*matrix[6]+z*matrix[10];

	x = vector[0];
	y = vector[1];
	z = vector[2];
}

inline void CMatrix::set( const float *matrix )
{
	memcpy( m_matrix, matrix, sizeof( float )*16 );
}

inline void CMatrix::loadIdentity()
{
	memset( m_matrix, 0, sizeof( float )*16 );
	m_matrix[0] = m_matrix[5] = m_matrix[10] = m_matrix[15] = 1;
}

void CMatrix::inverseRotateVect( float *pVect )
{
	float vec[3];

	vec[0] = pVect[0]*m_matrix[0]+pVect[1]*m_matrix[1]+pVect[2]*m_matrix[2];
	vec[1] = pVect[0]*m_matrix[4]+pVect[1]*m_matrix[5]+pVect[2]*m_matrix[6];
	vec[2] = pVect[0]*m_matrix[8]+pVect[1]*m_matrix[9]+pVect[2]*m_matrix[10];

	memcpy( pVect, vec, sizeof( float )*3 );
}

void CMatrix::inverseTranslateVect( float *pVect )
{
	pVect[0] = pVect[0]-m_matrix[12];
	pVect[1] = pVect[1]-m_matrix[13];
	pVect[2] = pVect[2]-m_matrix[14];
}

inline void CQuaternion::inverse()
{
	m_quat[0] = -m_quat[0];
	m_quat[1] = -m_quat[1];
	m_quat[2] = -m_quat[2];
	m_quat[3] = -m_quat[3];
}

void CQuaternion::fromAngles( const float *angles )
{
	float angle;
	double sr, sp, sy, cr, cp, cy;

	angle = angles[2]*0.5f;
	sy = sin( angle );
	cy = cos( angle );
	angle = angles[1]*0.5f;
	sp = sin( angle );
	cp = cos( angle );
	angle = angles[0]*0.5f;
	sr = sin( angle );
	cr = cos( angle );

	double crcp = cr*cp;
	double srsp = sr*sp;

	m_quat[0] = ( float )( sr*cp*cy-cr*sp*sy );
	m_quat[1] = ( float )( cr*sp*cy+sr*cp*sy );
	m_quat[2] = ( float )( crcp*sy-srsp*cy );
	m_quat[3] = ( float )( crcp*cy+srsp*sy ); 
}

void CQuaternion::slerp( const CQuaternion& q1, CQuaternion& q2, float interp )
{
	// Decide if one of the quaternions is backwards
	int i;
	float a = 0, b = 0;
	for ( i = 0; i < 4; i++ )
	{
		a += ( q1[i]-q2[i] )*( q1[i]-q2[i] );
		b += ( q1[i]+q2[i] )*( q1[i]+q2[i] );
	}
	if ( a > b )
		q2.inverse();

	float cosom = q1[0]*q2[0]+q1[1]*q2[1]+q1[2]*q2[2]+q1[3]*q2[3];
	double sclq1, sclq2;

	if (( 1.0+cosom ) > 0.00000001 )
	{
		if (( 1.0-cosom ) > 0.00000001 )
		{
			double omega = acos( cosom );
			double sinom = sin( omega );
			sclq1 = sin(( 1.0-interp )*omega )/sinom;
			sclq2 = sin( interp*omega )/sinom;
		}
		else
		{
			sclq1 = 1.0-interp;
			sclq2 = interp;
		}
		for ( i = 0; i < 4; i++ )
			m_quat[i] = ( float )( sclq1*q1[i]+sclq2*q2[i] );
	}
	else
	{
		m_quat[0] = -q1[1];
		m_quat[1] = q1[0];
		m_quat[2] = -q1[3];
		m_quat[3] = q1[2];

		sclq1 = sin(( 1.0-interp )*0.5*M_PI );
		sclq2 = sin( interp*0.5*M_PI );
		for ( i = 0; i < 3; i++ )
			m_quat[i] = ( float )( sclq1*q1[i]+sclq2*m_quat[i] );
	}
}

CMatrix::CMatrix()
{
	loadIdentity();
}

CMatrix::~CMatrix()
{
}

void CMatrix::postMultiply( const CMatrix& matrix )
{
	float newMatrix[16];
	const float *m1 = m_matrix, *m2 = matrix.m_matrix;

	newMatrix[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2];
	newMatrix[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2];
	newMatrix[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2];
	newMatrix[3] = 0;

	newMatrix[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6];
	newMatrix[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6];
	newMatrix[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6];
	newMatrix[7] = 0;

	newMatrix[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10];
	newMatrix[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10];
	newMatrix[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10];
	newMatrix[11] = 0;

	newMatrix[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12];
	newMatrix[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13];
	newMatrix[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14];
	newMatrix[15] = 1;

	set( newMatrix );
}

void CMatrix::setTranslation( const float *translation )
{
	m_matrix[12] = translation[0];
	m_matrix[13] = translation[1];
	m_matrix[14] = translation[2];
}

void CMatrix::setInverseTranslation( const float *translation )
{
	m_matrix[12] = -translation[0];
	m_matrix[13] = -translation[1];
	m_matrix[14] = -translation[2];
}

void CMatrix::setRotationDegrees( const float *angles )
{
	float vec[3];
	vec[0] = ( float )( angles[0]*180.0/M_PI );
	vec[1] = ( float )( angles[1]*180.0/M_PI );
	vec[2] = ( float )( angles[2]*180.0/M_PI );
	setRotationRadians( vec );
}

void CMatrix::setInverseRotationDegrees( const float *angles )
{
	float vec[3];
	vec[0] = ( float )( angles[0]*180.0/M_PI );
	vec[1] = ( float )( angles[1]*180.0/M_PI );
	vec[2] = ( float )( angles[2]*180.0/M_PI );
	setInverseRotationRadians( vec );
}

void CMatrix::setRotationRadians( const float *angles )
{
	double cr = cos( angles[0] );
	double sr = sin( angles[0] );
	double cp = cos( angles[1] );
	double sp = sin( angles[1] );
	double cy = cos( angles[2] );
	double sy = sin( angles[2] );

	m_matrix[0] = ( float )( cp*cy );
	m_matrix[1] = ( float )( cp*sy );
	m_matrix[2] = ( float )( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	m_matrix[4] = ( float )( srsp*cy-cr*sy );
	m_matrix[5] = ( float )( srsp*sy+cr*cy );
	m_matrix[6] = ( float )( sr*cp );

	m_matrix[8] = ( float )( crsp*cy+sr*sy );
	m_matrix[9] = ( float )( crsp*sy-sr*cy );
	m_matrix[10] = ( float )( cr*cp );
}

void CMatrix::setInverseRotationRadians( const float *angles )
{
	double cr = cos( angles[0] );
	double sr = sin( angles[0] );
	double cp = cos( angles[1] );
	double sp = sin( angles[1] );
	double cy = cos( angles[2] );
	double sy = sin( angles[2] );

	m_matrix[0] = ( float )( cp*cy );
	m_matrix[4] = ( float )( cp*sy );
	m_matrix[8] = ( float )( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	m_matrix[1] = ( float )( srsp*cy-cr*sy );
	m_matrix[5] = ( float )( srsp*sy+cr*cy );
	m_matrix[9] = ( float )( sr*cp );

	m_matrix[2] = ( float )( crsp*cy+sr*sy );
	m_matrix[6] = ( float )( crsp*sy-sr*cy );
	m_matrix[10] = ( float )( cr*cp );
}

void CMatrix::setRotationQuaternion( const CQuaternion& quat )
{
	m_matrix[0] = ( float )( 1.0 - 2.0*quat[1]*quat[1] - 2.0*quat[2]*quat[2] );
	m_matrix[1] = ( float )( 2.0*quat[0]*quat[1] + 2.0*quat[3]*quat[2] );
	m_matrix[2] = ( float )( 2.0*quat[0]*quat[2] - 2.0*quat[3]*quat[1] );

	m_matrix[4] = ( float )( 2.0*quat[0]*quat[1] - 2.0*quat[3]*quat[2] );
	m_matrix[5] = ( float )( 1.0 - 2.0*quat[0]*quat[0] - 2.0*quat[2]*quat[2] );
	m_matrix[6] = ( float )( 2.0*quat[1]*quat[2] + 2.0*quat[3]*quat[0] );

	m_matrix[8] = ( float )( 2.0*quat[0]*quat[2] + 2.0*quat[3]*quat[1] );
	m_matrix[9] = ( float )( 2.0*quat[1]*quat[2] - 2.0*quat[3]*quat[0] );
	m_matrix[10] = ( float )( 1.0 - 2.0*quat[0]*quat[0] - 2.0*quat[1]*quat[1] );
}

CCamera::CCamera()
{
	CVector3 vZero = CVector3(0.0, 0.0, 0.0);
	CVector3 vView = CVector3(0.0, -1.0, 1.0);
	CVector3 vUp = CVector3(0.0, 1.0, 0.0);

	m_vPosition	= vZero;
	m_vView		= vView;
	m_vUpVector	= vUp;

	m_bGrounded = false;

	m_vOrientation.x = 0;
	m_vOrientation.y = 0;

	CalcStrafe();
}

void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
{
	CVector3 vPosition	= CVector3(positionX, positionY, positionZ);
	CVector3 vView		= CVector3(viewX, viewY, viewZ);
	CVector3 vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);

	m_vPosition = vPosition;
	m_vView     = vView;
	m_vUpVector = vUpVector;

	CalcStrafe();
	CalcYaw();
	CalcPitch();
}

void CCamera::SetViewByMouse()
{
	int middleX = g_width  >> 1;				// This is a binary shift to get half the width
	int middleY = g_height >> 1;				// This is a binary shift to get half the height
	float angleY = 0.0f;
	float angleZ = 0.0f;
		
	MousePosition();
	
	if( (g_mouse.x == middleX) && (g_mouse.y == middleY) ) return;

	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
	angleY = (float)( (middleX - g_mouse.x) ) / 1000.0f;		
	angleZ = (float)( (middleY - g_mouse.y) ) / 1000.0f;	

	CenterMouse();

	static float lastRotX = 0.0f; 
 	lastRotX = m_vOrientation.x; // We store off the currentRotX and will use it in when the angle is capped
	
	// Here we keep track of the current rotation (for up and down) so that
	// we can restrict the camera from doing a full 360 loop.
	m_vOrientation.x += angleZ;
 
	// If the current rotation (in radians) is greater than 1.0, we want to cap it.
	if(m_vOrientation.x > 1.0f)     
	{
		m_vOrientation.x = 1.0f;
		
		// Rotate by remaining angle if there is any
		if(lastRotX != 1.0f) 
		{
			// To find the axis we need to rotate around for up and down
			// movements, we need to get a perpendicular vector from the
			// camera's view vector and up vector.  This will be the axis.
			// Before using the axis, it's a good idea to normalize it first.
			CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);
				
			// rotate the camera by the remaining angle (1.0f - lastRotX)
			RotateView( 1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Check if the rotation is below -1.0, if so we want to make sure it doesn't continue
	else if(m_vOrientation.x < -1.0f)
	{
		m_vOrientation.x = -1.0f;
		
		// Rotate by the remaining angle if there is any
		if(lastRotX != -1.0f)
		{
			// To find the axis we need to rotate around for up and down
			// movements, we need to get a perpendicular vector from the
			// camera's view vector and up vector.  This will be the axis.
			// Before using the axis, it's a good idea to normalize it first.
			CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);
			
			// rotate the camera by ( -1.0f - lastRotX)
			RotateView( -1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Otherwise, we can rotate the view around our position
	else 
	{	
		// To find the axis we need to rotate around for up and down
		// movements, we need to get a perpendicular vector from the
		// camera's view vector and up vector.  This will be the axis.
		// Before using the axis, it's a good idea to normalize it first.
		CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);
	
		// Rotate around our perpendicular axis
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}

	// Always rotate the camera around the y-axis
	RotateView(angleY, 0, 1, 0);
	CalcStrafe();
	CalcYaw();
	//CalcPitch();
}

void CCamera::Grounded(bool bGrounded)
{
	m_bGrounded = bGrounded;

	if(bGrounded && m_vVelocity.y < 0.0f)
		m_vVelocity.y = 0.0f;
}

void CCamera::LimitHVel(float vLimit)
{
	CVector3 hVel = CVector3(m_vVelocity.x, 0, m_vVelocity.z);
	float vVel = Magnitude( hVel );

	if(vVel <= vLimit)
		return;

	hVel = hVel * vLimit / vVel;

	m_vVelocity.x = hVel.x;
	m_vVelocity.z = hVel.z;
}

void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;

	CVector3 vView = m_vView - m_vPosition;		

	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	m_vView = m_vPosition + vNewView;
	CalcYaw();
}

void CCamera::View(CVector3 vView)
{
	m_vView = vView;
	CalcYaw();
}

void CCamera::Strafe(float speed)
{
	m_vVelocity.x += m_vStrafe.x * speed;
	m_vVelocity.z += m_vStrafe.z * speed;
}

void CCamera::Move(float speed)
{
	CVector3 vVector = m_vView - m_vPosition;

	vVector = Normalize(vVector);

	m_vVelocity.x += vVector.x * speed;
	m_vVelocity.z += vVector.z * speed;
}

void CCamera::Friction()
{
	m_vVelocity.x /= FRICTION;
	m_vVelocity.z /= FRICTION;
}

void CCamera::Rise(float speed)
{
	CVector3 vVector = m_vUpVector;

	vVector = Normalize(vVector);

	m_vVelocity.y += vVector.y * speed;
}		

void CCamera::Move(CVector3 delta)
{
	m_vPosition = m_vPosition + delta;
	m_vView = m_vView + delta;
}

void CCamera::MoveTo(CVector3 vPos)
{
	CVector3 delta = vPos - m_vPosition;
	Move(delta);
	//m_vPosition = vPos;
}

void CCamera::Stop()
{
	m_vVelocity = CVector3(0, 0, 0);
}

void CCamera::CalcStrafe() 
{
	CVector3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);
	m_vStrafe = Normalize(vCross);
}

void CCamera::Look()
{
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
			  m_vView.x,	 m_vView.y,     m_vView.z,	
			  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

void CCamera::CalcYaw()
{
	CVector3 d = m_vView - m_vPosition;
	m_vOrientation.y = GetYaw(d.x, d.z);
}

void CCamera::CalcPitch()
{
	CVector3 d = m_vView - m_vPosition;
	float lateral = Magnitude(CVector3(d.x, 0, d.z));
	m_vOrientation.x = RADTODEG( atan2(d.y, lateral) );
}

void CCamera::Step()
{
	CVector3 vNew = m_vPosition + m_vVelocity * g_FrameInterval;
	MoveTo(vNew);
}

CVector3 GetRay(int x, int y)
{
	float halfWidth = g_width / 2.0f;
	float halfHeight = g_height / 2.0f;

	float ratioX = (x - halfWidth) / halfWidth;

	float ratioY = -(y - halfHeight) / halfHeight;

	CVector3 direction = Normalize( g_camera->View() - g_camera->Position() );

	float Hnear = 2 * tan( DEGTORAD(FIELD_OF_VIEW) / 2) * MIN_DISTANCE;
	float Wnear = Hnear * g_width/g_height;

	return ( direction * MIN_DISTANCE + g_camera->Strafe() * ratioX * Wnear/2.0f + g_camera->Up2() * ratioY * Hnear/2.0f );
}