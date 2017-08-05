#include "Canon.h"
#include <stdio.h>

bool CCanon::s_fInitialized = false;

#ifdef _MSC_VER
#include <EDSDK.h>
#include <EDSDKErrors.h>
#include <EDSDKTypes.h>

bool CCanon::Init( void )
{
	if( s_fInitialized )
		return false;

	EdsError err = EdsInitializeSDK( );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to initialize EDSDK\n" );
		return false;
	}

	s_fInitialized = true;
	return true;

TERMINATE:
	err = EdsTerminateSDK( );
	if( err != EDS_ERR_OK )
		fprintf( stderr, "Failed to terminate EDSDK\n" );

	return false;
}

void CCanon::Terminate( void )
{
	if( s_fInitialized )
		EdsTerminateSDK( );
}

std::vector<std::string> CCanon::GetCameraList( void )
{
	std::vector<std::string> vecCameras;

	EdsUInt32 uCount = 0;
	EdsCameraListRef cameraList;
	EdsError err = EdsGetCameraList( &cameraList );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to retrieve camera list\n" );
		return vecCameras;
	}

	err = EdsGetChildCount( cameraList, &uCount );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to get camera count\n" );
		return vecCameras;
	}

	EdsCameraRef camera;
	EdsDeviceInfo deviceInfo;
	for( unsigned int u = 0; u < uCount; u++ )
	{
		err = EdsGetChildAtIndex( cameraList, u, &camera );
		if( err != EDS_ERR_OK )
		{
			fprintf( stderr, "Warning: Failed to get camera %u\n", u );
			continue;
		}

		err = EdsGetDeviceInfo( camera, &deviceInfo );
		if( err != EDS_ERR_OK )
		{
			fprintf( stderr, "Warning: Failed to get information about camera %u\n", u );
			continue;
		}

		vecCameras.emplace_back( deviceInfo.szDeviceDescription );
	}

	return vecCameras;
}
#else
bool CCanon::Init( void )
{
	return false;
}

void CCanon::Terminate( void )
{

}

std::vector<std::string> CCanon::GetCameraList( void )
{
	return std::vector<std::string>( );
}
#endif