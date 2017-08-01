#pragma once

#include "Matrix.h"

class CRenderHelper
{
public:
	static CMatrix<3, 3> GetRotationMatrix( double dX, double dY, double dZ );

private:
	CRenderHelper( void ) = delete;
	CRenderHelper( const CRenderHelper &other ) = delete;
};

