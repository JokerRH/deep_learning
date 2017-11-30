#pragma once

#include "../Compat.h"
#include <vector>
#include <opencv2/core.hpp>

#define CAMERA_LIVEVIEW_READY	WM_USER
#define CAMERA_IMAGE_READY		WM_USER + 1

class CCamera
{
public:
	static bool Init( void );
	static bool ThreadInit( void );
	static void Terminate( void );
	static void ThreadTerminate( void );
	static CCamera *SelectCamera( void );

	virtual ~CCamera( void ) = default;

	virtual bool TakePicture( void ) = 0;
	virtual bool TakePicture( cv::Mat &matImage, double &dFOV ) = 0;
	virtual bool StartLiveView( void ) = 0;
	virtual void StopLiveView( void );
	virtual void WaitForLiveView( void );

	virtual bool DownloadImage( cv::Mat &matImage, double &dFOV, void *pImageRef ) = 0;
	virtual bool DownloadLiveView( cv::Mat &matImage ) = 0;

private:
	static void Cls( void );
};

inline void CCamera::StopLiveView( void )
{
	return;
}