#include "RenderHelper.h"
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

CMatrix<3, 3> CRenderHelper::GetRotationMatrix( double dX, double dY, double dZ )
{
	double dCos = cos( dX * M_PI / 180 );
	double dSin = sin( dX * M_PI / 180 );
	CMatrix<3, 3> matX(
	{
		1, 0, 0,
		0, dCos, -dSin,
		0, dSin, dCos
	} );

	dCos = cos( dY * M_PI / 180 );
	dSin = sin( dY * M_PI / 180 );
	CMatrix<3, 3> matY(
	{
		dCos, 0, dSin,
		0, 1, 0,
		-dSin, 0, dCos
	} );

	dCos = cos( dZ * M_PI / 180 );
	dSin = sin( dZ * M_PI / 180 );
	CMatrix<3, 3> matZ(
	{
		dCos, -dSin, 0,
		dSin, dCos, 0,
		0, 0, 1
	} );

	return matX * matY * matZ;
}