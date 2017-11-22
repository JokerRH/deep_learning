#ifdef WITH_EDSDK
#include "Canon.h"
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <EDSDK.h>
#include <EDSDKErrors.h>
#include <EDSDKTypes.h>
#include <opencv2\imgproc.hpp>
#include <opencv2\imgcodecs.hpp>
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

#pragma comment( lib, "EDSDK.lib" )

bool CCanon::Init( void )
{
	EdsError err = EdsInitializeSDK( );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to initialize EDSDK\n" );
		return false;
	}

	return true;
}

bool CCanon::ThreadInit( void )
{
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	return true;
}

void CCanon::Terminate( void )
{
	EdsTerminateSDK( );
}

void CCanon::ThreadTerminate( void )
{
	CoUninitialize( );
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
			EdsRelease( camera );
			continue;
		}

		vecCameras.emplace_back( deviceInfo.szDeviceDescription );
		EdsRelease( camera );
	}

	return vecCameras;
}

CCanon::CCanon( unsigned uIndex )
{
	EdsUInt32 uCount = 0;
	EdsCameraListRef cameraList;
	EdsError err = EdsGetCameraList( &cameraList );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to retrieve camera list\n" );
		throw 0;
	}

	err = EdsGetChildCount( cameraList, &uCount );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to get camera count\n" );
		throw 0;
	}

	if( uIndex >= uCount )
	{
		std::wcerr << "No canon camera with index " << uIndex << std::endl;
		throw 0;
	}

	err = EdsGetChildAtIndex( cameraList, uIndex, &m_Camera );
	if( err != EDS_ERR_OK )
	{
		std::wcerr << "Failed to get camera at index " << uIndex << std::endl;
		EdsRelease( cameraList );
		throw 0;
	}

	EdsRelease( cameraList );

	err = EdsOpenSession( m_Camera );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to set open session with camera\n" );
		EdsRelease( m_Camera );
		throw( 0 );
	}

	err = EdsSetObjectEventHandler( m_Camera, kEdsObjectEvent_All, HandleObjectEvent, this );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to set object handler for camera\n" );
		EdsRelease( m_Camera );
		throw( 0 );
	}

	err = EdsSetPropertyEventHandler( m_Camera, kEdsPropertyEvent_All, HandlePropertyEvent, this );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to set property handler for camera\n" );
		EdsRelease( m_Camera );
		throw( 0 );
	}

	EdsUInt32 dwSaveTo = kEdsSaveTo_Host;
	EdsCapacity newCapacity = { 0x7FFFFFFF, 0x1000, 1 };
	err = EdsSetPropertyData( m_Camera, kEdsPropID_SaveTo, 0, sizeof( EdsUInt32 ), &dwSaveTo );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to set save target\n" );
		goto EXCEPTION;
	}

	err = EdsSetCapacity( m_Camera, newCapacity );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to set capacity\n" );
		goto EXCEPTION;
	}
	return;

EXCEPTION:
	err = EdsCloseSession( m_Camera );
	if( err != EDS_ERR_OK )
		fprintf( stderr, "Warning: Failed to close session with camera\n" );

	EdsRelease( m_Camera );
	throw 0;
}

CCanon::~CCanon( void )
{
	EdsError err = EdsCloseSession( m_Camera );
	if( err != EDS_ERR_OK )
		fprintf( stderr, "Warning: Failed to close session with camera\n" );

	EdsRelease( m_Camera );
}

bool CCanon::TakePicture( void )
{
	EdsError err = EdsSendCommand( m_Camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_Completely );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to take picture: %s\n", GetErrorMacro( err ) );
		return false;
	}

	err = EdsSendCommand( m_Camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_OFF );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to take picture\n" );
		return false;
	}

	return true;
}

bool CCanon::TakePicture( cv::Mat &matImage, double &dFOV )
{
	if( !TakePicture( ) )
		return false;

	MSG msg;
	BOOL fReturn;
	while( true )
	{
		if( ( fReturn = GetMessage( &msg, NULL, 0, 0 ) ) == -1 )
			throw 27;	//Error

		switch( msg.message )
		{
		case WM_QUIT:
			std::wcout << "Quit message" << std::endl;
			PostQuitMessage( 0 );
			throw 27;
		case WM_KEYDOWN:
			{
				unsigned uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_CHAR );
				switch( uKey )
				{
				case 27:
					throw 27;
				}
			}
			break;
		case CAMERA_IMAGE_READY:
			return ( (CCanon *) msg.wParam )->DownloadImage( matImage, dFOV, (void *) msg.lParam );
		}

		if( fReturn > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
}

bool CCanon::StartLiveView( void )
{
	EdsError err = EDS_ERR_OK;
	// Get the output device for the live view image
	EdsUInt32 device;
	err = EdsGetPropertyData( m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof( device ), &device );
	// PC live view starts by setting the PC as the output device for the live view image.
	if( err == EDS_ERR_OK )
	{
		device |= kEdsEvfOutputDevice_PC;
		err = EdsSetPropertyData( m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof( device ), &device );
		return true;
	}
	else
		std::cout << "Error starting live view: " << err << std::endl;

	return false;

	// A property change event notification is issued from the camera if property settings are made successfully.
	// Start downloading of the live view image once the property change notification arrives.
}

void CCanon::StopLiveView( void )
{
	EdsError err = EDS_ERR_OK;
	// Get the output device for the live view image
	EdsUInt32 device;
	err = EdsGetPropertyData( m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof( device ), &device );
	// PC live view ends if the PC is disconnected from the live view image output device.
	if( err == EDS_ERR_OK )
	{
		device &= ~kEdsEvfOutputDevice_PC;
		err = EdsSetPropertyData( m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof( device ), &device );
	}
}

void CCanon::WaitForLiveView( void )
{
	MSG msg;
	BOOL fReturn;
	cv::Mat matImage;
	while( true )
	{
		if( ( fReturn = GetMessage( &msg, NULL, 0, 0 ) ) == -1 )
			throw 27;	//Error

		switch( msg.message )
		{
		case WM_QUIT:
			std::wcout << "Quit message" << std::endl;
			PostQuitMessage( 0 );
			throw 27;
		case WM_KEYDOWN:
			{
				unsigned uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_CHAR );
				switch( uKey )
				{
				case 27:
					throw 27;
				}
			}
			break;
		case CAMERA_LIVEVIEW_READY:
			return;
		}

		if( fReturn > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
}

bool CCanon::DownloadImage( cv::Mat & matImage, double & dFOV, void *pImageRef )
{
	bool fReturn = false;
	EdsStreamRef stream;
	EdsDirectoryItemInfo dirItemInfo;
	EdsError err = EdsGetDirectoryItemInfo( (EdsDirectoryItemRef) pImageRef, &dirItemInfo );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to get information about directory item: %s\n", GetErrorMacro( err ) );
		goto RETURN;
	}

	if( ( dirItemInfo.format & 0xFF ) != kEdsTargetImageType_Jpeg )
	{
		EdsDownloadCancel( (EdsDirectoryItemRef) pImageRef );
		goto RETURN;
	}

	//Create memory stream
	err = EdsCreateMemoryStream( dirItemInfo.size, &stream );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to create memory stream: %s\n", GetErrorMacro( err ) );
		goto RETURN;
	}

	//Download
	err = EdsDownload( (EdsDirectoryItemRef) pImageRef, dirItemInfo.size, stream );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to download item: %s\n", GetErrorMacro( err ) );
		goto STREAM;
	}

	err = EdsDownloadComplete( (EdsDirectoryItemRef) pImageRef );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to complete download: %s\n", GetErrorMacro( err ) );
		goto STREAM;
	}

	EdsImageRef image;
	err = EdsCreateImageRef( stream, &image );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to get image from stream: %s\n", GetErrorMacro( err ) );
		goto STREAM;
	}

	EdsRational ratVal;
	err = EdsGetPropertyData( image, kEdsPropID_FocalLength, 0, sizeof( EdsRational ), &ratVal );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to get focal length: %s\n", GetErrorMacro( err ) );
		goto IMAGE;
	}
	//printf( "Focal length: %f\n", ratVal.numerator / (double) ratVal.denominator );

	EdsRelease( stream );
	EdsImageInfo imageInfo;
	err = EdsGetImageInfo( image, kEdsImageSrc_FullView, &imageInfo );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to get image info: %s\n", GetErrorMacro( err ) );
		EdsRelease( image );
		goto RETURN;
	}

	err = EdsCreateMemoryStream( 0, &stream );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to create memory stream: %s\n", GetErrorMacro( err ) );
		EdsRelease( image );
		goto RETURN;
	}

	err = EdsGetImage( image, kEdsImageSrc_FullView, kEdsTargetImageType_RGB, imageInfo.effectiveRect, imageInfo.effectiveRect.size, stream );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to get image: %s\n", GetErrorMacro( err ) );
		goto IMAGE;
	}

	EdsRelease( image );

	unsigned char *pbData;
	err = EdsGetPointer( stream, (EdsVoid**) &pbData );
	if( err != EDS_ERR_OK )
	{
		fprintf( stderr, "Failed to get image data: %s\n", GetErrorMacro( err ) );
		goto STREAM;
	}

	cv::cvtColor( cv::Mat( imageInfo.height, imageInfo.width, CV_8UC3, pbData ), matImage, CV_BGR2RGB );
	dFOV = 2 * 180 / M_PI * atan( CANON_SENSOR_DIAG / ( 2 * ratVal.numerator / (double) ratVal.denominator ) );

	fReturn = true;
	goto STREAM;

IMAGE:
	EdsRelease( image );
STREAM:
	EdsRelease( stream );
RETURN:
	return fReturn;
}

bool CCanon::DownloadLiveView( cv::Mat & matImage )
{
	EdsError err = EDS_ERR_OK;
	EdsEvfImageRef image = NULL;
	EdsStreamRef stream = NULL;
	unsigned char* data = NULL;
	EdsUInt64 qwSize = 0;
	bool fReturn = false;

	err = EdsCreateMemoryStream( 0, &stream );
	if( err != EDS_ERR_OK )
	{
		std::cout << "Download Live View Image Error in Function EdsCreateMemoryStream: " << err << "\n";
		goto RETURN;
	}

	err = EdsCreateEvfImageRef( stream, &image );
	if( err != EDS_ERR_OK )
	{
		std::cout << "Download Live View Image Error in Function EdsCreateEvfImageRef: " << err << "\n";
		goto STREAM;
	}

	err = EdsDownloadEvfImage( m_Camera, image );
	switch( err )
	{
	case EDS_ERR_OK:
		break;
	case EDS_ERR_OBJECT_NOTREADY:
		goto IMAGE;
	default:
		std::cout << "Download Live View Image Error in Function EdsDownloadEvfImage: " << err << "\n";
		goto IMAGE;
	}

	err = EdsGetPointer( stream, (EdsVoid **) &data );
	if( err != EDS_ERR_OK )
	{
		std::cout << "Download Live View Image Error in Function EdsGetPointer: " << err << "\n";
		goto IMAGE;
	}

	err = EdsGetLength( stream, &qwSize );
	if( err != EDS_ERR_OK )
	{
		std::cout << "Download Live View Image Error in Function EdsGetLength: " << err << "\n";
		goto IMAGE;
	}

	matImage = cv::imdecode( std::vector<char>( data, data + qwSize ), 1 );

	fReturn = true;
IMAGE:
	EdsRelease( image );
STREAM:
	EdsRelease( stream );
RETURN:
	return fReturn;
}

const char *CCanon::GetErrorMacro( EdsError err )
{
	switch( err )
	{
	case EDS_ISSPECIFIC_MASK:
		return "EDS_ISSPECIFIC_MASK";
	case EDS_COMPONENTID_MASK:
		return "EDS_COMPONENTID_MASK";
	case EDS_RESERVED_MASK:
		return "EDS_RESERVED_MASK";
	case EDS_ERRORID_MASK:
		return "EDS_ERRORID_MASK";

		/*-----------------------------------------------------------------------
		ED-SDK Base Component IDs
		------------------------------------------------------------------------*/
	case EDS_CMP_ID_CLIENT_COMPONENTID:
		return "EDS_CMP_ID_CLIENT_COMPONENTID";
	case EDS_CMP_ID_LLSDK_COMPONENTID:
		return "EDS_CMP_ID_LLSDK_COMPONENTID";
	case EDS_CMP_ID_HLSDK_COMPONENTID:
		return "EDS_CMP_ID_HLSDK_COMPONENTID";

		/*-----------------------------------------------------------------------
		ED-SDK Functin Success Code
		------------------------------------------------------------------------*/
	case EDS_ERR_OK:
		return "EDS_ERR_OK";

		/*-----------------------------------------------------------------------
		ED-SDK Generic Error IDs
		------------------------------------------------------------------------*/
		/* Miscellaneous errors */
	case EDS_ERR_UNIMPLEMENTED:
		return "EDS_ERR_UNIMPLEMENTED";
	case EDS_ERR_INTERNAL_ERROR:
		return "EDS_ERR_INTERNAL_ERROR";
	case EDS_ERR_MEM_ALLOC_FAILED:
		return "EDS_ERR_MEM_ALLOC_FAILED";
	case EDS_ERR_MEM_FREE_FAILED:
		return "EDS_ERR_MEM_FREE_FAILED";
	case EDS_ERR_OPERATION_CANCELLED:
		return "EDS_ERR_OPERATION_CANCELLED";
	case EDS_ERR_INCOMPATIBLE_VERSION:
		return "EDS_ERR_INCOMPATIBLE_VERSION";
	case EDS_ERR_NOT_SUPPORTED:
		return "EDS_ERR_NOT_SUPPORTED";
	case EDS_ERR_UNEXPECTED_EXCEPTION:
		return "EDS_ERR_UNEXPECTED_EXCEPTION";
	case EDS_ERR_PROTECTION_VIOLATION:
		return "EDS_ERR_PROTECTION_VIOLATION";
	case EDS_ERR_MISSING_SUBCOMPONENT:
		return "EDS_ERR_MISSING_SUBCOMPONENT";
	case EDS_ERR_SELECTION_UNAVAILABLE:
		return "EDS_ERR_SELECTION_UNAVAILABLE";

		/* File errors */
	case EDS_ERR_FILE_IO_ERROR:
		return "EDS_ERR_FILE_IO_ERROR";
	case EDS_ERR_FILE_TOO_MANY_OPEN:
		return "EDS_ERR_FILE_TOO_MANY_OPEN";
	case EDS_ERR_FILE_NOT_FOUND:
		return "EDS_ERR_FILE_NOT_FOUND";
	case EDS_ERR_FILE_OPEN_ERROR:
		return "EDS_ERR_FILE_OPEN_ERROR";
	case EDS_ERR_FILE_CLOSE_ERROR:
		return "EDS_ERR_FILE_CLOSE_ERROR";
	case EDS_ERR_FILE_SEEK_ERROR:
		return "EDS_ERR_FILE_SEEK_ERROR";
	case EDS_ERR_FILE_TELL_ERROR:
		return "EDS_ERR_FILE_TELL_ERROR";
	case EDS_ERR_FILE_READ_ERROR:
		return "EDS_ERR_FILE_READ_ERROR";
	case EDS_ERR_FILE_WRITE_ERROR:
		return "EDS_ERR_FILE_WRITE_ERROR";
	case EDS_ERR_FILE_PERMISSION_ERROR:
		return "EDS_ERR_FILE_PERMISSION_ERROR";
	case EDS_ERR_FILE_DISK_FULL_ERROR:
		return "EDS_ERR_FILE_DISK_FULL_ERROR";
	case EDS_ERR_FILE_ALREADY_EXISTS:
		return "EDS_ERR_FILE_ALREADY_EXISTS";
	case EDS_ERR_FILE_FORMAT_UNRECOGNIZED:
		return "EDS_ERR_FILE_FORMAT_UNRECOGNIZED";
	case EDS_ERR_FILE_DATA_CORRUPT:
		return "EDS_ERR_FILE_DATA_CORRUPT";
	case EDS_ERR_FILE_NAMING_NA:
		return "EDS_ERR_FILE_NAMING_NA";

		/* Directory errors */
	case EDS_ERR_DIR_NOT_FOUND:
		return "EDS_ERR_DIR_NOT_FOUND";
	case EDS_ERR_DIR_IO_ERROR:
		return "EDS_ERR_DIR_IO_ERROR";
	case EDS_ERR_DIR_ENTRY_NOT_FOUND:
		return "EDS_ERR_DIR_ENTRY_NOT_FOUND";
	case EDS_ERR_DIR_ENTRY_EXISTS:
		return "EDS_ERR_DIR_ENTRY_EXISTS";
	case EDS_ERR_DIR_NOT_EMPTY:
		return "EDS_ERR_DIR_NOT_EMPTY";

		/* Property errors */
	case EDS_ERR_PROPERTIES_UNAVAILABLE:
		return "EDS_ERR_PROPERTIES_UNAVAILABLE";
	case EDS_ERR_PROPERTIES_MISMATCH:
		return "EDS_ERR_PROPERTIES_MISMATCH";
	case EDS_ERR_PROPERTIES_NOT_LOADED:
		return "EDS_ERR_PROPERTIES_NOT_LOADED";

		/* Function Parameter errors */
	case EDS_ERR_INVALID_PARAMETER:
		return "EDS_ERR_INVALID_PARAMETER";
	case EDS_ERR_INVALID_HANDLE:
		return "EDS_ERR_INVALID_HANDLE";
	case EDS_ERR_INVALID_POINTER:
		return "EDS_ERR_INVALID_POINTER";
	case EDS_ERR_INVALID_INDEX:
		return "EDS_ERR_INVALID_INDEX";
	case EDS_ERR_INVALID_LENGTH:
		return "EDS_ERR_INVALID_LENGTH";
	case EDS_ERR_INVALID_FN_POINTER:
		return "EDS_ERR_INVALID_FN_POINTER";
	case EDS_ERR_INVALID_SORT_FN:
		return "EDS_ERR_INVALID_SORT_FN";

		/* Device errors */
	case EDS_ERR_DEVICE_NOT_FOUND:
		return "EDS_ERR_DEVICE_NOT_FOUND";
	case EDS_ERR_DEVICE_BUSY:
		return "EDS_ERR_DEVICE_BUSY";
	case EDS_ERR_DEVICE_INVALID:
		return "EDS_ERR_DEVICE_INVALID";
	case EDS_ERR_DEVICE_EMERGENCY:
		return "EDS_ERR_DEVICE_EMERGENCY";
	case EDS_ERR_DEVICE_MEMORY_FULL:
		return "EDS_ERR_DEVICE_MEMORY_FULL";
	case EDS_ERR_DEVICE_INTERNAL_ERROR:
		return "EDS_ERR_DEVICE_INTERNAL_ERROR";
	case EDS_ERR_DEVICE_INVALID_PARAMETER:
		return "EDS_ERR_DEVICE_INVALID_PARAMETER";
	case EDS_ERR_DEVICE_NO_DISK:
		return "EDS_ERR_DEVICE_NO_DISK";
	case EDS_ERR_DEVICE_DISK_ERROR:
		return "EDS_ERR_DEVICE_DISK_ERROR";
	case EDS_ERR_DEVICE_CF_GATE_CHANGED:
		return "EDS_ERR_DEVICE_CF_GATE_CHANGED";
	case EDS_ERR_DEVICE_DIAL_CHANGED:
		return "EDS_ERR_DEVICE_DIAL_CHANGED";
	case EDS_ERR_DEVICE_NOT_INSTALLED:
		return "EDS_ERR_DEVICE_NOT_INSTALLED";
	case EDS_ERR_DEVICE_STAY_AWAKE:
		return "EDS_ERR_DEVICE_STAY_AWAKE";
	case EDS_ERR_DEVICE_NOT_RELEASED:
		return "EDS_ERR_DEVICE_NOT_RELEASED";

		/* Stream errors */
	case EDS_ERR_STREAM_IO_ERROR:
		return "EDS_ERR_STREAM_IO_ERROR";
	case EDS_ERR_STREAM_NOT_OPEN:
		return "EDS_ERR_STREAM_NOT_OPEN";
	case EDS_ERR_STREAM_ALREADY_OPEN:
		return "EDS_ERR_STREAM_ALREADY_OPEN";
	case EDS_ERR_STREAM_OPEN_ERROR:
		return "EDS_ERR_STREAM_OPEN_ERROR";
	case EDS_ERR_STREAM_CLOSE_ERROR:
		return "EDS_ERR_STREAM_CLOSE_ERROR";
	case EDS_ERR_STREAM_SEEK_ERROR:
		return "EDS_ERR_STREAM_SEEK_ERROR";
	case EDS_ERR_STREAM_TELL_ERROR:
		return "EDS_ERR_STREAM_TELL_ERROR";
	case EDS_ERR_STREAM_READ_ERROR:
		return "EDS_ERR_STREAM_READ_ERROR";
	case EDS_ERR_STREAM_WRITE_ERROR:
		return "EDS_ERR_STREAM_WRITE_ERROR";
	case EDS_ERR_STREAM_PERMISSION_ERROR:
		return "EDS_ERR_STREAM_PERMISSION_ERROR";
	case EDS_ERR_STREAM_COULDNT_BEGIN_THREAD:
		return "EDS_ERR_STREAM_COULDNT_BEGIN_THREAD";
	case EDS_ERR_STREAM_BAD_OPTIONS:
		return "EDS_ERR_STREAM_BAD_OPTIONS";
	case EDS_ERR_STREAM_END_OF_STREAM:
		return "EDS_ERR_STREAM_END_OF_STREAM";

		/* Communications errors */
	case EDS_ERR_COMM_PORT_IS_IN_USE:
		return "EDS_ERR_COMM_PORT_IS_IN_USE";
	case EDS_ERR_COMM_DISCONNECTED:
		return "EDS_ERR_COMM_DISCONNECTED";
	case EDS_ERR_COMM_DEVICE_INCOMPATIBLE:
		return "EDS_ERR_COMM_DEVICE_INCOMPATIBLE";
	case EDS_ERR_COMM_BUFFER_FULL:
		return "EDS_ERR_COMM_BUFFER_FULL";
	case EDS_ERR_COMM_USB_BUS_ERR:
		return "EDS_ERR_COMM_USB_BUS_ERR";

		/* Lock/Unlock */
	case EDS_ERR_USB_DEVICE_LOCK_ERROR:
		return "EDS_ERR_USB_DEVICE_LOCK_ERROR";
	case EDS_ERR_USB_DEVICE_UNLOCK_ERROR:
		return "EDS_ERR_USB_DEVICE_UNLOCK_ERROR";

		/* STI/WIA */
	case EDS_ERR_STI_UNKNOWN_ERROR:
		return "EDS_ERR_STI_UNKNOWN_ERROR";
	case EDS_ERR_STI_INTERNAL_ERROR:
		return "EDS_ERR_STI_INTERNAL_ERROR";
	case EDS_ERR_STI_DEVICE_CREATE_ERROR:
		return "EDS_ERR_STI_DEVICE_CREATE_ERROR";
	case EDS_ERR_STI_DEVICE_RELEASE_ERROR:
		return "EDS_ERR_STI_DEVICE_RELEASE_ERROR";
	case EDS_ERR_DEVICE_NOT_LAUNCHED:
		return "EDS_ERR_DEVICE_NOT_LAUNCHED";

	case EDS_ERR_ENUM_NA:
		return "EDS_ERR_ENUM_NA";
	case EDS_ERR_INVALID_FN_CALL:
		return "EDS_ERR_INVALID_FN_CALL";
	case EDS_ERR_HANDLE_NOT_FOUND:
		return "EDS_ERR_HANDLE_NOT_FOUND";
	case EDS_ERR_INVALID_ID:
		return "EDS_ERR_INVALID_ID";
	case EDS_ERR_WAIT_TIMEOUT_ERROR:
		return "EDS_ERR_WAIT_TIMEOUT_ERROR";

		/* PTP */
	case EDS_ERR_SESSION_NOT_OPEN:
		return "EDS_ERR_SESSION_NOT_OPEN";
	case EDS_ERR_INVALID_TRANSACTIONID:
		return "EDS_ERR_INVALID_TRANSACTIONID";
	case EDS_ERR_INCOMPLETE_TRANSFER:
		return "EDS_ERR_INCOMPLETE_TRANSFER";
	case EDS_ERR_INVALID_STRAGEID:
		return "EDS_ERR_INVALID_STRAGEID";
	case EDS_ERR_DEVICEPROP_NOT_SUPPORTED:
		return "EDS_ERR_DEVICEPROP_NOT_SUPPORTED";
	case EDS_ERR_INVALID_OBJECTFORMATCODE:
		return "EDS_ERR_INVALID_OBJECTFORMATCODE";
	case EDS_ERR_SELF_TEST_FAILED:
		return "EDS_ERR_SELF_TEST_FAILED";
	case EDS_ERR_PARTIAL_DELETION:
		return "EDS_ERR_PARTIAL_DELETION";
	case EDS_ERR_SPECIFICATION_BY_FORMAT_UNSUPPORTED:
		return "EDS_ERR_SPECIFICATION_BY_FORMAT_UNSUPPORTED";
	case EDS_ERR_NO_VALID_OBJECTINFO:
		return "EDS_ERR_NO_VALID_OBJECTINFO";
	case EDS_ERR_INVALID_CODE_FORMAT:
		return "EDS_ERR_INVALID_CODE_FORMAT";
	case EDS_ERR_UNKNOWN_VENDOR_CODE:
		return "EDS_ERR_UNKNOWN_VENDOR_CODE";
	case EDS_ERR_CAPTURE_ALREADY_TERMINATED:
		return "EDS_ERR_CAPTURE_ALREADY_TERMINATED";
	case EDS_ERR_PTP_DEVICE_BUSY:
		return "EDS_ERR_PTP_DEVICE_BUSY";
	case EDS_ERR_INVALID_PARENTOBJECT:
		return "EDS_ERR_INVALID_PARENTOBJECT";
	case EDS_ERR_INVALID_DEVICEPROP_FORMAT:
		return "EDS_ERR_INVALID_DEVICEPROP_FORMAT";
	case EDS_ERR_INVALID_DEVICEPROP_VALUE:
		return "EDS_ERR_INVALID_DEVICEPROP_VALUE";
	case EDS_ERR_SESSION_ALREADY_OPEN:
		return "EDS_ERR_SESSION_ALREADY_OPEN";
	case EDS_ERR_TRANSACTION_CANCELLED:
		return "EDS_ERR_TRANSACTION_CANCELLED";
	case EDS_ERR_SPECIFICATION_OF_DESTINATION_UNSUPPORTED:
		return "EDS_ERR_SPECIFICATION_OF_DESTINATION_UNSUPPORTED";
	case EDS_ERR_NOT_CAMERA_SUPPORT_SDK_VERSION:
		return "EDS_ERR_NOT_CAMERA_SUPPORT_SDK_VERSION";

		/* PTP Vendor */
	case EDS_ERR_UNKNOWN_COMMAND:
		return "EDS_ERR_UNKNOWN_COMMAND";
	case EDS_ERR_OPERATION_REFUSED:
		return "EDS_ERR_OPERATION_REFUSED";
	case EDS_ERR_LENS_COVER_CLOSE:
		return "EDS_ERR_LENS_COVER_CLOSE";
	case EDS_ERR_LOW_BATTERY:
		return "EDS_ERR_LOW_BATTERY";
	case EDS_ERR_OBJECT_NOTREADY:
		return "EDS_ERR_OBJECT_NOTREADY";
	case EDS_ERR_CANNOT_MAKE_OBJECT:
		return "EDS_ERR_CANNOT_MAKE_OBJECT";
	case EDS_ERR_MEMORYSTATUS_NOTREADY:
		return "EDS_ERR_MEMORYSTATUS_NOTREADY";

		/* Take Picture errors */
	case EDS_ERR_TAKE_PICTURE_AF_NG:
		return "EDS_ERR_TAKE_PICTURE_AF_NG";
	case EDS_ERR_TAKE_PICTURE_RESERVED:
		return "EDS_ERR_TAKE_PICTURE_RESERVED";
	case EDS_ERR_TAKE_PICTURE_MIRROR_UP_NG:
		return "EDS_ERR_TAKE_PICTURE_MIRROR_UP_NG";
	case EDS_ERR_TAKE_PICTURE_SENSOR_CLEANING_NG:
		return "EDS_ERR_TAKE_PICTURE_SENSOR_CLEANING_NG";
	case EDS_ERR_TAKE_PICTURE_SILENCE_NG:
		return "EDS_ERR_TAKE_PICTURE_SILENCE_NG";
	case EDS_ERR_TAKE_PICTURE_NO_CARD_NG:
		return "EDS_ERR_TAKE_PICTURE_NO_CARD_NG";
	case EDS_ERR_TAKE_PICTURE_CARD_NG:
		return "EDS_ERR_TAKE_PICTURE_CARD_NG";
	case EDS_ERR_TAKE_PICTURE_CARD_PROTECT_NG:
		return "EDS_ERR_TAKE_PICTURE_CARD_PROTECT_NG";
	case EDS_ERR_TAKE_PICTURE_MOVIE_CROP_NG:
		return "EDS_ERR_TAKE_PICTURE_MOVIE_CROP_NG";
	case EDS_ERR_TAKE_PICTURE_STROBO_CHARGE_NG:
		return "EDS_ERR_TAKE_PICTURE_STROBO_CHARGE_NG";
	case EDS_ERR_TAKE_PICTURE_NO_LENS_NG:
		return "EDS_ERR_TAKE_PICTURE_NO_LENS_NG";
	case EDS_ERR_TAKE_PICTURE_SPECIAL_MOVIE_MODE_NG:
		return "EDS_ERR_TAKE_PICTURE_SPECIAL_MOVIE_MODE_NG";
	case EDS_ERR_TAKE_PICTURE_LV_REL_PROHIBIT_MODE_NG:
		return "EDS_ERR_TAKE_PICTURE_LV_REL_PROHIBIT_MODE_NG";


	case EDS_ERR_LAST_GENERIC_ERROR_PLUS_ONE:
		return "EDS_ERR_LAST_GENERIC_ERROR_PLUS_ONE";
	default:
		return "Unknown error code";
	}
	return nullptr;
}

EdsError EDSCALLBACK CCanon::HandleObjectEvent( EdsObjectEvent event, EdsBaseRef object, EdsVoid *pContext )
{
	switch( event )
	{
	case kEdsObjectEvent_DirItemRequestTransfer:
		PostThreadMessage( g_dwMainThreadID, CAMERA_IMAGE_READY, (WPARAM) pContext, (LPARAM) object );
		break;
		//default:
		//printf( "Received object event\n" );
	}
	return EDS_ERR_OK;
}

EdsError EDSCALLBACK CCanon::HandlePropertyEvent( EdsPropertyEvent event, EdsPropertyID propertyID, EdsUInt32 dwParam, EdsVoid *pContext )
{
	switch( event )
	{
	case kEdsPropertyEvent_PropertyChanged:
		switch( propertyID )
		{
		case kEdsPropID_Evf_OutputDevice:
			PostThreadMessage( g_dwMainThreadID, CAMERA_LIVEVIEW_READY, (WPARAM) pContext, 0 );
			break;
			//default:
			//printf( "Property %d changed\n", propertyID );
		}
		break;
		//default:
		//printf( "Received property event %d\n", event );
	}
	return EDS_ERR_OK;
}
#endif