#pragma once
#ifdef _MSC_VER

#include <vector>
#include <EDSDK.h>
#include <Windows.h>
#include <opencv2\core.hpp>

#define CANON_SENSOR_DIAG 26.81976883

#define CANON_LIVEVIEW_READY	WM_USER
#define CANON_IMAGE_READY		WM_USER + 1

class CCanon
{
public:
	static bool Init( void );
	static bool ThreadInit( void );
	static void Terminate( void );
	static void ThreadTerminate( void );
	static bool IsInitialized( void );
	static CCanon *SelectCamera( void );

	~CCanon( void );
	bool TakePicture( void );
	bool TakePicture( cv::Mat &matImage, double &dFOV );
	bool StartLiveView( void );
	void StopLiveView( void );
	void WaitForLiveView( void );

	bool DownloadImage( cv::Mat &matImage, double &dFOV, EdsDirectoryItemRef directoryItem );
	bool DownloadLiveView( cv::Mat &matImage );

	static const char *GetErrorMacro( EdsError err );

private:
	static EdsError EDSCALLBACK HandleObjectEvent( EdsObjectEvent event, EdsBaseRef object, EdsVoid *pContext );
	static EdsError EDSCALLBACK HandlePropertyEvent( EdsPropertyEvent event, EdsPropertyID propertyID, EdsUInt32 dwParam, EdsVoid *pContext );

	CCanon( EdsCameraRef &camera );
	static bool s_fInitialized;

	EdsCameraRef m_Camera;
};

inline bool CCanon::IsInitialized( void )
{
	return s_fInitialized;
}
#endif