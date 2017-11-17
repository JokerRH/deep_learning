#pragma once

#include "Matrix.h"
#include "Vector.h"

/*
Lemma
	Multiplying rotation matrix and rotation matrix results in rotation matrix
Lemma
	Multiplying transformation matrix and transformation matrix results in transformation matrix
Lemma
	The inverse of rotation matrix is its transpose
Lemma
	The inverse of transformation matrix [R|t] always exists as [R^T | - R^T t]
*/

class CTransformation : public CMatrix<4, 4>
{
public:
	static CTransformation GetTranslationMatrix( const CVector<3> &vec3Translation );
	static CTransformation GetRotationMatrix( const CVector<3> &vec3X, const CVector<3> &vec3Y, const CVector<3> &vec3Z );
	static CTransformation GetRotationMatrixRad( double dPitch, double dYaw, double dRoll );
	static CTransformation GetRotationMatrix( double dPitch, double dYaw, double dRoll );
	static CTransformation GetScaleMatrix( double dScale );
	static CTransformation GetTRSMatrix( const CVector<3> &vec3Translation, const CVector<3> &vec3X, const CVector<3> &vec3Y, const CVector<3> &vec3Z, double dScale );
	static CTransformation GetTRSMatrix( const CVector<3> &vec3Translation, const CMatrix<3, 3> matRS );

	CTransformation( const std::array<double, 4 * 4> &adValues );
	CTransformation( const CMatrix<4, 4> &mat );
	CTransformation( CMatrix<4, 4> &&mat );
	CTransformation( const CTransformation &other ) = default;
	CTransformation &operator=( const CTransformation &other ) = default;

	CTransformation( CTransformation &&other ) = default;
	CTransformation &operator=( CTransformation &&other ) = default;

	friend CVector<3> &operator*=( CVector<3> &vec3, const CTransformation &matTransform );
	CVector<3> operator*( const CVector<3> &vec3 ) const;
	CTransformation operator*( const CTransformation &mat ) const;
	CTransformation &operator*=( const CTransformation &mat );

	CTransformation Inverse( void ) const;
	CTransformation RSMatrix( void ) const;
};

inline CVector<3> &operator*=( CVector<3> &vec3, const CTransformation &matTransform )
{
	std::array<double, 3> adValues;
	adValues[ 0 ] = matTransform.m_adValues[ 0 ] * vec3[ 0 ] + matTransform.m_adValues[ 1 ] * vec3[ 1 ] + matTransform.m_adValues[ 2 ] * vec3[ 2 ] + matTransform.m_adValues[ 3 ];
	adValues[ 1 ] = matTransform.m_adValues[ 4 ] * vec3[ 0 ] + matTransform.m_adValues[ 5 ] * vec3[ 1 ] + matTransform.m_adValues[ 6 ] * vec3[ 2 ] + matTransform.m_adValues[ 7 ];
	adValues[ 2 ] = matTransform.m_adValues[ 8 ] * vec3[ 0 ] + matTransform.m_adValues[ 9 ] * vec3[ 1 ] + matTransform.m_adValues[ 10 ] * vec3[ 2 ] + matTransform.m_adValues[ 11 ];
	vec3 = CVector<3>( adValues );
	return vec3;
}

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

inline CTransformation::CTransformation( const std::array<double, 4 * 4> &adValues ) :
	CMatrix<4, 4>( adValues )
{

}

inline CTransformation::CTransformation( const CMatrix<4, 4> &mat ) :
	CMatrix<4, 4>( mat )
{

}

inline CTransformation::CTransformation( CMatrix<4, 4> &&mat ) :
	CMatrix<4, 4>( std::move( mat ) )
{

}

inline CTransformation CTransformation::operator*( const CTransformation &mat ) const
{
	return CMatrix<4, 4>::operator*( mat );
}

inline CTransformation &CTransformation::operator*=( const CTransformation &mat )
{
	CMatrix<4, 4>::operator*=( mat );
	return *this;
}

inline CTransformation CTransformation::RSMatrix( void ) const
{
	return CTransformation( {
		m_adValues[ 0 ], m_adValues[ 1 ], m_adValues[ 2 ], 0,
		m_adValues[ 4 ], m_adValues[ 5 ], m_adValues[ 6 ], 0,
		m_adValues[ 8 ], m_adValues[ 9 ], m_adValues[ 10 ], 0,
		0, 0, 0, 1
	} );
}