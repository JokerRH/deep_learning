#include "Transformation.h"
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

CTransformation CTransformation::GetRotationMatrix( double dPitch, double dYaw, double dRoll )
{
	return GetRotationMatrixRad( dPitch * M_PI / 180, dYaw * M_PI / 180, dRoll * M_PI / 180 );
}

CTransformation CTransformation::GetRotationMatrixRad( double dPitch, double dYaw, double dRoll )
{
	//Roll
	double dCos = cos( dPitch );
	double dSin = sin( dPitch );
	CMatrix<3, 3> matX(
	{
		1, 0, 0,
		0, dCos, -dSin,
		0, dSin, dCos
	} );

	//Pitch
	dCos = cos( dYaw );
	dSin = sin( dYaw );
	CMatrix<3, 3> matY(
	{
		dCos, 0, dSin,
		0, 1, 0,
		-dSin, 0, dCos
	} );

	//Yaw
	dCos = cos( dRoll );
	dSin = sin( dRoll );
	CMatrix<3, 3> matZ(
	{
		dCos, -dSin, 0,
		dSin, dCos, 0,
		0, 0, 1
	} );

	CMatrix<3, 3> mat( matX * matY * matZ );
	CTransformation matRotate( { 0 } );
	std::copy( mat.m_adValues.begin( ), mat.m_adValues.end( ), matRotate.m_adValues.begin( ) );
	matRotate.m_adValues[ 15 ] = 1;
	return matRotate;
}

CVector<3> CTransformation::operator*( const CVector<3> &vec3 ) const
{
	return CVector<3>( {
		m_adValues[ 0 ] * vec3[ 0 ] + m_adValues[ 1 ] * vec3[ 1 ] + m_adValues[ 2 ] * vec3[ 2 ] + m_adValues[ 3 ],
		m_adValues[ 4 ] * vec3[ 0 ] + m_adValues[ 5 ] * vec3[ 1 ] + m_adValues[ 6 ] * vec3[ 2 ] + m_adValues[ 7 ],
		m_adValues[ 8 ] * vec3[ 0 ] + m_adValues[ 9 ] * vec3[ 1 ] + m_adValues[ 10 ] * vec3[ 2 ] + m_adValues[ 11 ]
	} );
}