#pragma once
#ifdef WITH_EDSDK

#include "Camera.h"
#include <vector>
#include <EDSDK.h>
#include <Windows.h>
#include <opencv2\core.hpp>

#define CANON_SENSOR_DIAG 26.81976883

class CCanon : public CCamera
{
public:
	static bool Init( void );
	static bool ThreadInit( void );
	static void Terminate( void );
	static void ThreadTerminate( void );

	static std::vector<std::string> GetCameraList( void );

	CCanon( unsigned uIndex );

	~CCanon( void ) override;
	bool TakePicture( void ) override;
	bool TakePicture( cv::Mat &matImage, double &dFOV ) override;
	bool StartLiveView( void ) override;
	void StopLiveView( void ) override;
	void WaitForLiveView( void ) override;

	bool DownloadImage( cv::Mat &matImage, double &dFOV, void *pImageRef ) override;
	bool DownloadLiveView( cv::Mat &matImage ) override;

	static const char *GetErrorMacro( EdsError err );

private:
	static EdsError EDSCALLBACK HandleObjectEvent( EdsObjectEvent event, EdsBaseRef object, EdsVoid *pContext );
	static EdsError EDSCALLBACK HandlePropertyEvent( EdsPropertyEvent event, EdsPropertyID propertyID, EdsUInt32 dwParam, EdsVoid *pContext );

	CCanon( EdsCameraRef &camera );

	EdsCameraRef m_Camera;
};
#endif