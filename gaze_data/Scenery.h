#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Image.h"
#include "Ray.h"

class CScenery
{
public:
	CScenery( const CVector<3> &vec3MonitorPos, const CVector<3> &vec3MonitorDim, const CRay &rayEyeLeft, const CRay &rayEyeRight );
	CScenery Transform( const CMatrix<3, 3> &matTransform ) const;
	CScenery &Transformed( const CMatrix<3, 3> &matTransform );
	CScenery &Fit( void );
	void Draw( CImage &img ) const;
	
	std::string ToString( unsigned int uPrecision = 2 ) const;

private:
	CScenery( void );
	
	CVector<3> m_avec3Monitor[ 8 ];
	CVector<3> m_vec3Camera;
	CVector<3> m_avec3Face[ 8 ];
	CRay m_rayEyeLeft;
	CRay m_rayEyeRight;
};

