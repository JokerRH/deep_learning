#pragma once

#include "Matrix.h"
#include "Vector.h"

class CRenderHelper
{
public:
	static CMatrix<3, 3> GetRotationMatrix( double dX, double dY, double dZ );
	static CMatrix<3, 3> GetTransformationMatrix( CVector<3> vec3X, CVector<3> vec3Y, CVector<3> vec3Z );

private:
	CRenderHelper( void ) = delete;
	CRenderHelper( const CRenderHelper &other ) = delete;
};

