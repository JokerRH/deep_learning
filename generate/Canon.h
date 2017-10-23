#pragma once
#ifdef _MSC_VER

#include <vector>
#include <EDSDK.h>
#include <Windows.h>
#include <opencv2\core.hpp>

#define CANON_SENSOR_DIAG 26.81976883

#define CANON_LIVEVIEW_READY	WM_USER

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
	bool TakePicture( cv::Mat &matImage );
	bool StartLiveView( void );
	void StopLiveView( void );
	void WaitForLiveView( void );

	bool DownloadLiveView( cv::Mat &matImage );

	static const char *GetErrorMacro( EdsError err );

private:
	static EdsError EDSCALLBACK HandleObjectEvent( EdsObjectEvent event, EdsBaseRef object, EdsVoid *context );
	static EdsError EDSCALLBACK HandlePropertyEvent( EdsPropertyEvent event, EdsPropertyID propertyID, EdsUInt32 dwParam, EdsVoid *pContext );
	EdsError DownloadImage( EdsDirectoryItemRef directoryItem );

	CCanon( EdsCameraRef &camera );
	static bool s_fInitialized;

	EdsCameraRef m_Camera;
};

inline bool CCanon::IsInitialized( void )
{
	return s_fInitialized;
}
#endif