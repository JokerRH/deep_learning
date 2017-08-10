#pragma once

#include "Matrix.h"
#include "Vector.h"

class CRenderHelper
{
public:
	static CMatrix<3, 3> GetRotationMatrix( double dX, double dY, double dZ );
	static CMatrix<3, 3> GetTransformationMatrix( CVector<3> vec3X, CVector<3> vec3Y, CVector<3> vec3Z );
	static CMatrix<3, 3> GetTransformationMatrix( double dScale );

private:
	CRenderHelper( void ) = delete;
	CRenderHelper( const CRenderHelper &other ) = delete;
};

inline CMatrix<3, 3> CRenderHelper::GetTransformationMatrix( double dScale )
{
	return CMatrix<3, 3>(
	{
		dScale, 0, 0,
		0, dScale, 0,
		0, 0, dScale
	} );
}