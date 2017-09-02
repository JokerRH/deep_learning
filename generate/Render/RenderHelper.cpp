#include "RenderHelper.h"
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

CMatrix<3, 3> CRenderHelper::GetRotationMatrix( double dX, double dY, double dZ )
{
	return GetRotationMatrixRad( dX * M_PI / 180, dY * M_PI / 180, dZ * M_PI / 180 );
}

CMatrix<3, 3> CRenderHelper::GetRotationMatrixRad( double dX, double dY, double dZ )
{
	//Roll
	double dCos = cos( dX );
	double dSin = sin( dX );
	CMatrix<3, 3> matX(
	{
		1, 0, 0,
		0, dCos, -dSin,
		0, dSin, dCos
	} );

	//Pitch
	dCos = cos( dY );
	dSin = sin( dY );
	CMatrix<3, 3> matY(
	{
		dCos, 0, dSin,
		0, 1, 0,
		-dSin, 0, dCos
	} );

	//Yaw
	dCos = cos( dZ );
	dSin = sin( dZ );
	CMatrix<3, 3> matZ(
	{
		dCos, -dSin, 0,
		dSin, dCos, 0,
		0, 0, 1
	} );

	return matX * matY * matZ;
}

CMatrix<3, 3> CRenderHelper::GetTransformationMatrix( CVector<3> vec3X, CVector<3> vec3Y, CVector<3> vec3Z )
{
	vec3X.Normalize( );
	vec3Y.Normalize( );
	vec3Z.Normalize( );

	return CMatrix<3, 3>(
	{
		vec3X[ 0 ], vec3Y[ 0 ], vec3Z[ 0 ],
		vec3X[ 1 ], vec3Y[ 1 ], vec3Z[ 1 ],
		vec3X[ 2 ], vec3Y[ 2 ], vec3Z[ 2 ]
	} );
}