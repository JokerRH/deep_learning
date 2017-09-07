#pragma once

#include "Matrix.h"
#include "Vector.h"

class CTransformation : public CMatrix<4, 4>
{
public:
	static CTransformation GetTranslationMatrix( const CVector<3> &vec3Translation );
	static CTransformation GetRotationMatrix( const CVector<3> &vec3X, const CVector<3> &vec3Y, const CVector<3> &vec3Z );
	static CTransformation GetRotationMatrixRad( double dPitch, double dYaw, double dRoll );
	static CTransformation GetRotationMatrix( double dPitch, double dYaw, double dRoll );
	static CTransformation GetScaleMatrix( double dScale );
	static CTransformation GetTRSMatrix( const CVector<3> &vec3Translation, const CVector<3> &vec3X, const CVector<3> &vec3Y, const CVector<3> &vec3Z, double dScale );

	CTransformation( const std::array<double, 4 * 4> &adValues );
	CTransformation( const CMatrix<4, 4> &mat );
	CTransformation( CMatrix<4, 4> &&mat );
	CTransformation( const CTransformation &other ) = default;
	CTransformation &operator=( const CTransformation &other ) = default;

	CTransformation( CTransformation &&other ) = default;
	CTransformation &operator=( CTransformation &&other ) = default;

	CVector<3> operator*( const CVector<3> &vec3 ) const;
};

inline CTransformation CTransformation::GetTranslationMatrix( const CVector<3> &vec3Translation )
{
	return CTransformation( {
		1, 0, 0, vec3Translation[ 0 ],
		0, 1, 0, vec3Translation[ 1 ],
		0, 0, 1, vec3Translation[ 2 ],
		0, 0, 0, 1
	} );
}

inline CTransformation CTransformation::GetRotationMatrix( const CVector<3> &vec3X, const CVector<3> &vec3Y, const CVector<3> &vec3Z )
{
	return CTransformation( {
		vec3X[ 0 ], vec3Y[ 0 ], vec3Z[ 0 ], 0,
		vec3X[ 1 ], vec3Y[ 1 ], vec3Z[ 1 ], 0,
		vec3X[ 2 ], vec3Y[ 2 ], vec3Z[ 2 ], 0,
		0, 0, 0, 1
	} );
}

inline CTransformation CTransformation::GetScaleMatrix( double dScale )
{
	return CTransformation( {
		dScale, 0, 0, 0,
		0, dScale, 0, 0,
		0, 0, dScale, 0,
		0, 0, 0, 1
	} );
}

inline CTransformation CTransformation::GetTRSMatrix( const CVector<3> &vec3Translation, const CVector<3> &vec3X, const CVector<3> &vec3Y, const CVector<3> &vec3Z, double dScale )
{
	return GetTranslationMatrix( vec3Translation ) * GetRotationMatrix( vec3X, vec3Y, vec3Z ) * GetScaleMatrix( dScale );
}

CTransformation::CTransformation( const std::array<double, 4 * 4> &adValues ) :
	CMatrix<4, 4>( adValues )
{

}

CTransformation::CTransformation( const CMatrix<4, 4> &mat ) :
	CMatrix<4, 4>( mat )
{

}

CTransformation::CTransformation( CMatrix<4, 4> &&mat ) :
	CMatrix<4, 4>( std::move( mat ) )
{

}