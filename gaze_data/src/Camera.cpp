#include "Camera.h"
#include "Canon.h"

using namespace cv;

CBaseCamera *CBaseCamera::SelectCamera( void )
{
#ifdef _MSC_VER
	CBaseCamera *pCamera = CCanon::SelectCamera( );
	if( !pCamera )
		return new CCamera( 0 );

	return pCamera;
#else
	return new CCamera( 0 );
#endif
}

CCamera::CCamera( unsigned int uIndex ) :
	m_Cap( uIndex )
{
	if( !m_Cap.isOpened( ) )
	{
		fprintf( stderr, "Unable to open capture device\n" );
		throw( 0 );
	}
}

bool CCamera::TakePicture( CImage &img )
{
	m_Cap.grab( );
	m_Cap.grab( );
	m_Cap.grab( );
	m_Cap.grab( );
	m_Cap.grab( );
	m_Cap.retrieve( img.matImage );
	return true;
}