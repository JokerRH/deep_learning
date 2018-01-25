#include "Webcam.h"
#include <iostream>

CWebcam::CWebcam( unsigned uIndex ) :
	m_Cap( uIndex )
{
	if( !m_Cap.isOpened( ) )
	{
		std::wcerr << "Unable to open capture device" << std::endl;
		throw( 0 );
	}

	m_Cap.grab( );
}

bool CWebcam::DownloadImage( cv::Mat &matImage, double &dFOV, void *pImageRef )
{
	dFOV = 74;
	m_Cap.grab( );
	m_Cap.grab( );
	m_Cap.grab( );
	m_Cap.grab( );
	m_Cap.grab( );
	m_Cap >> matImage;
	return true;
	//return m_Cap.retrieve( matImage );
}

bool CWebcam::DownloadLiveView( cv::Mat & matImage )
{
	m_Cap >> matImage;
	return true;
	//return m_Cap.retrieve( matImage );
}
