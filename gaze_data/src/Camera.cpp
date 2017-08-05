#include "Camera.h"
#include "Canon.h"

bool CCamera::Init( )
{
	CCanon::Init( );
	return false;
}

void CCamera::Terminate( )
{
	CCanon::Terminate( );
}
