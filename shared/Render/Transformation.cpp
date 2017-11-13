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
	//Pitch
	double dCos = cos( dPitch );
	double dSin = sin( dPitch );
	CMatrix<3, 3> matX(
	{
		1, 0, 0,
		0, dCos, dSin,
		0, -dSin, dCos
	} );

	//Yaw
	dCos = cos( dYaw );
	dSin = sin( dYaw );
	CMatrix<3, 3> matY(
	{
		dCos, 0, dSin,
		0, 1, 0,
		-dSin, 0, dCos
	} );

	//Roll
	dCos = cos( dRoll );
	dSin = sin( dRoll );
	CMatrix<3, 3> matZ(
	{
		dCos, -dSin, 0,
		dSin, dCos, 0,
		0, 0, 1
	} );

	CMatrix<3, 3> mat( matX * matY * matZ );
	return CTransformation( {
		mat.m_adValues[ 0 ], mat.m_adValues[ 1 ], mat.m_adValues[ 2 ], 0,
		mat.m_adValues[ 3 ], mat.m_adValues[ 4 ], mat.m_adValues[ 5 ], 0,
		mat.m_adValues[ 6 ], mat.m_adValues[ 7 ], mat.m_adValues[ 8 ], 0,
		0, 0, 0, 1
	} );
}

CTransformation CTransformation::GetTRSMatrix( const CVector<3> &vec3Translation, const CMatrix<3, 3> matRS )
{
	return CTransformation( {
		matRS.m_adValues[ 0 ], matRS.m_adValues[ 1 ], matRS.m_adValues[ 2 ], vec3Translation[ 0 ],
		matRS.m_adValues[ 3 ], matRS.m_adValues[ 4 ], matRS.m_adValues[ 5 ], vec3Translation[ 1 ],
		matRS.m_adValues[ 6 ], matRS.m_adValues[ 7 ], matRS.m_adValues[ 8 ], vec3Translation[ 2 ],
		0, 0, 0, 1
	} );
}

CVector<3> CTransformation::operator*( const CVector<3> &vec3 ) const
{
	return CVector<3>( {
		m_adValues[ 0 ] * vec3[ 0 ] + m_adValues[ 1 ] * vec3[ 1 ] + m_adValues[ 2 ] * vec3[ 2 ] + m_adValues[ 3 ],
		m_adValues[ 4 ] * vec3[ 0 ] + m_adValues[ 5 ] * vec3[ 1 ] + m_adValues[ 6 ] * vec3[ 2 ] + m_adValues[ 7 ],
		m_adValues[ 8 ] * vec3[ 0 ] + m_adValues[ 9 ] * vec3[ 1 ] + m_adValues[ 10 ] * vec3[ 2 ] + m_adValues[ 11 ]
	} );
}

CTransformation CTransformation::Inverse( void ) const
{
	CMatrix<3, 3> matRSInv( {
		m_adValues[ 0 ], m_adValues[ 4 ] , m_adValues[ 8 ],
		m_adValues[ 1 ], m_adValues[ 5 ] , m_adValues[ 9 ],
		m_adValues[ 2 ], m_adValues[ 6 ] , m_adValues[ 10 ],
	} );
	CVector<3> vec3Translate( { m_adValues[ 3 ], m_adValues[ 7 ], m_adValues[ 11 ] } );

	return CTransformation::GetTRSMatrix( -( matRSInv * vec3Translate ), matRSInv );
}