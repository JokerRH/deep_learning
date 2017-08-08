#include "Image.h"
#include "BBox.h"
#include <opencv2\imgproc\imgproc.hpp>

#ifdef _MSC_VER
#	define NOMINMAX
#	include <wtypes.h>
#else
#	include <X11/Xlib.h>
#endif

void CImage::GetScreenResolution( unsigned int &uWidth, unsigned int &uHeight )
{
#ifdef _MSC_VER
	RECT desktop;
	GetWindowRect( GetDesktopWindow( ), &desktop );
	uWidth = desktop.right;
	uHeight = desktop.bottom;
#else
	Display *d = XOpenDisplay( nullptr );
	Screen *s = DefaultScreenOfDisplay( d );
	uWidth = s->width;
	uHeight = s->height;
#endif
}

void CImage::Show( const char *szWindow )
{
#ifdef _MSC_VER
	unsigned int uWidth;
	unsigned int uHeight;
	GetScreenResolution( uWidth, uHeight );
	cv::Mat mat( uHeight, uWidth, CV_8UC3, cv::Scalar::all( 255 ) );

	if( (unsigned int) matImage.cols > uWidth || (unsigned int) matImage.rows > uHeight )
	{
		double dScale = uWidth / (double) matImage.cols;
		if( matImage.rows * dScale > (double) uHeight )
			dScale = uHeight / (double) matImage.rows;

		cv::Size size( (int) matImage.cols * dScale, (int) matImage.rows * dScale );
		cv::Rect rect( ( uWidth - size.width ) / 2, ( uHeight - size.height ) / 2, size.width, size.height );
		cv::resize( matImage, mat( rect ), size );
	}
	else
	{
		cv::Rect rect( ( uWidth - matImage.cols ) / 2, ( uHeight - matImage.rows ) / 2, matImage.cols, matImage.rows );
		matImage.copyTo( mat( rect ) );
	}

	cv::imshow( szWindow, mat );
#else
	cv::imshow( szWindow, matImage );
#endif
}

void CImage::Crop( CBBox &box, unsigned int uLevel )
{
	register cv::Rect rect( box.GetPositionX( uLevel ), box.GetPositionY( uLevel ), box.GetWidth( uLevel ), box.GetHeight( uLevel ) );
	m_pParentBox = box.GetImage( uLevel );
	m_dWidth = rect.width / (double) m_pParentBox->GetWidth( 0 );
	m_dHeight = rect.height / (double) m_pParentBox->GetHeight( 0 );
	m_dPositionX = rect.x / (double) m_pParentBox->GetWidth( 0 );
	m_dPositionY = rect.y / (double) m_pParentBox->GetHeight( 0 );
	
	matImage = ( m_pParentBox->GetImage( 0 )->matImage )( rect ).clone( );
}