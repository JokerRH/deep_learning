#include <stdlib.h>
#include "Compat.h"
#include "Detect.h"
#include "Display.h"
#include "Camera/Camera.h"
#include <thread>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

DWORD g_dwMainThreadID;

int Run_Live( const filestring_t &sExecPath )
{
	int iReturn = EXIT_FAILURE;
	if( !CDetect::Init( sExecPath ) )
		goto LIVE_EXIT;

	if( !CCamera::Init( ) )
		goto LIVE_DETECT;

	CCamera *pCamera;
	try
	{
		pCamera = CCamera::SelectCamera( );
	}
	catch( int i )
	{
		if( i == 27 )
			iReturn = EXIT_SUCCESS;

		goto LIVE_CANON;
	}

	if( !pCamera )
	{
		std::wcout << L"Press any key to continue..." << std::endl;
		(void) _getch( );
		iReturn = EXIT_SUCCESS;
		goto LIVE_CANON;
	}

	if( !pCamera->StartLiveView( ) )
		goto LIVE_CAMERA;

	cv::namedWindow( "Window", cv::WINDOW_FULLSCREEN );
	cv::moveWindow( "Window", 0, 0 );
	cv::setWindowProperty( "Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );

#ifndef _MSC_VER
	if( !MessageInit( g_dwMainThreadID, "Window" ) )
	{
		std::wcout << "Failed to initialize message loop" << std::endl;
		goto LIVE_WINDOW;
	}
#endif

	try
	{
		CDisplay::ShowLive( "Window", *pCamera );
	}
	catch( int i )
	{
		if( i == 27 )
			iReturn = EXIT_SUCCESS;

		goto LIVE_MESSAGE;
	}

	iReturn = EXIT_SUCCESS;

LIVE_MESSAGE:
#ifndef _MSC_VER
	MessageTerminate( );
LIVE_WINDOW:
#endif
	cv::destroyAllWindows( );
	pCamera->StopLiveView( );
LIVE_CAMERA:
	delete pCamera;
LIVE_CANON:
	CCamera::Terminate( );
LIVE_DETECT:
	CDetect::Terminate( );
LIVE_EXIT:
	if( iReturn != EXIT_SUCCESS )
	{
		std::wcout << L"Press any key to continue..." << std::endl;
		(void) _getch( );
	}

	return iReturn;
}

int Run_Image( const filestring_t &sExecPath, const filestring_t &sImagePath, double dFOV )
{
	int iReturn = EXIT_FAILURE;
	if( !CDetect::Init( sExecPath ) )
		goto LIVE_EXIT;

	{
		cv::Mat matImage( cv::imread( compat::ToString( sImagePath ), CV_LOAD_IMAGE_COLOR ) );
		if( matImage.empty( ) )
		{
			std::wcout << "Failed to load image \"" << compat::ToWString( sImagePath ) << "\"" << std::endl;
			goto LIVE_DETECT;
		}

		cv::namedWindow( "Window", cv::WINDOW_FULLSCREEN );
		cv::moveWindow( "Window", 0, 0 );
		cv::setWindowProperty( "Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );

#ifndef _MSC_VER
		if( !MessageInit( g_dwMainThreadID, "Window" ) )
		{
			std::wcout << "Failed to initialize message loop" << std::endl;
			goto LIVE_WINDOW;
		}
#endif

		try
		{
			CDisplay::ShowImage( "Window", matImage, dFOV );
		}
		catch( int i )
		{
			if( i == 27 )
				iReturn = EXIT_SUCCESS;

			goto LIVE_MESSAGE;
		}

		iReturn = EXIT_SUCCESS;
	}

LIVE_MESSAGE:
#ifndef _MSC_VER
	MessageTerminate( );
LIVE_WINDOW:
#endif
	cv::destroyAllWindows( );
LIVE_DETECT:
	CDetect::Terminate( );
LIVE_EXIT:
	if( iReturn != EXIT_SUCCESS )
	{
		std::wcout << L"Press any key to continue..." << std::endl;
		(void) _getch( );
	}

	return iReturn;
}

#ifdef _MSC_VER
int wmain( int argc, filechar_t **argv )
#else
int main( int argc, filechar_t **argv )
#endif
{
#ifdef _MSC_VER
	g_dwMainThreadID = GetCurrentThreadId( );
#else
	g_dwMainThreadID = 0;
#endif

	/*
	while( true )
	{
		int iKey = _getch( );
		std::wcout << L"Key: " << iKey << std::endl;
	}
	*/

	filestring_t sExecPath = compat::GetFullPathName_d( argv[ 0 ] );
	if( sExecPath.empty( ) )
	{
		std::wcerr << "Unable to get full path for executable folder" << std::endl;
		return EXIT_FAILURE;
	}
	sExecPath = compat::PathRemoveFileSpec_d( sExecPath );

	if( argc == 1 )
		return Run_Live( sExecPath );
	else if( argc == 3 )
	{
		filestring_t sImagePath( compat::GetFullPathName_d( argv[ 1 ] ) );
		double dFOV;
#ifdef _MSC_VER
		dFOV = wcstof( argv[ 2 ], nullptr );
#else
		dFOV = atof( argv[ 2 ] );
#endif

		return Run_Image( sExecPath, sImagePath, dFOV );
	}

	std::wcerr << "Invalid number of arguments" << std::endl;
	(void) _getch( );
	return EXIT_FAILURE;
}