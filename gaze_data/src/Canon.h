#pragma once
#ifdef _MSC_VER

#include "Camera.h"
#include "Image.h"
#include <vector>
#include <EDSDK.h>
#include <Windows.h>
#include <opencv2\core\core.hpp>

#define CANON_SENSOR_DIAG 26.81976883

class CCanon : public CBaseCamera
{
public:
	static bool Init( void );
	static bool ThreadInit( void );
	static void Terminate( void );
	static void ThreadTerminate( void );
	static bool IsInitialized( void );
	static CCanon *SelectCamera( void );

	~CCanon( void ) override;
	bool TakePicture( CImage &img ) override;
	static const char *GetErrorMacro( EdsError err );

private:
	static EdsError EDSCALLBACK HandleObjectEvent( EdsObjectEvent event, EdsBaseRef object, EdsVoid *context );
	EdsError DownloadImage( EdsDirectoryItemRef directoryItem );

	CCanon( EdsCameraRef &camera );
	static bool s_fInitialized;

	EdsCameraRef m_Camera;

	CImage m_img;
	bool m_fHasImage;
};

inline bool CCanon::IsInitialized( void )
{
	return s_fInitialized;
}
#endif