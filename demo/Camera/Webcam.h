#pragma once

#include "Camera.h"
#ifdef _MSC_VER
#	include <opencv2/videocapture.hpp>
#else
#	include <opencv2/highgui/highgui.hpp>
#endif

class CWebcam : public CCamera
{
public:
	static std::vector<std::string> GetCameraList( void );

	CWebcam( unsigned uIndex );
	~CWebcam( void ) override = default;

	bool TakePicture( void );
	bool TakePicture( cv::Mat &matImage, double &dFOV );
	bool StartLiveView( void );

	bool DownloadImage( cv::Mat &matImage, double &dFOV, void *pImageRef );
	bool DownloadLiveView( cv::Mat &matImage );

private:
	cv::VideoCapture m_Cap;
};

inline std::vector<std::string> CWebcam::GetCameraList( void )
{
	return std::vector<std::string>( { "default" } );
}

inline bool CWebcam::TakePicture( void )
{
	PostThreadMessage( g_dwMainThreadID, CAMERA_IMAGE_READY, (WPARAM) this, 0 );
	return true;
}

inline bool CWebcam::TakePicture( cv::Mat &matImage, double &dFOV )
{
	return DownloadImage( matImage, dFOV, nullptr );
}

inline bool CWebcam::StartLiveView( void )
{
	PostThreadMessage( g_dwMainThreadID, CAMERA_LIVEVIEW_READY, (WPARAM) this, 0 );
	return true;
}