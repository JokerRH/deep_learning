#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "LandmarkCandidate.h"
#include "Landmark.h"
#include "GazeCapture.h"
#include "Config.h"
#include "Ray.h"
#include "GazeData.h"
#include "Scenery.h"
#include "Render/RenderHelper.h"
#include "Canon.h"
#include "Camera.h"
#include "Utility.h"

#ifdef _MSC_VER
#	include <direct.h>
#	include <Windows.h>
#else
#	include <unistd.h>
#	include <strings.h>
#endif

using namespace cv;

CConfig g_Config;

int EditDataset( const char *szFile )
{
	CBaseCamera::Init( );
	CBaseCamera *pCamera;
	try
	{
		pCamera = CBaseCamera::SelectCamera( );
	}
	catch( int i )
	{
		if( i == 1 )
		{
			CBaseCamera::Terminate( );
			return EXIT_SUCCESS;
		}

		throw;
	}

	srand( (unsigned int) time( nullptr ) );
	if( !CGazeCapture::OpenWrite( szFile ) )
		return EXIT_SUCCESS;

	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );
	
	CUtility::ShowCursor( false, "Window" );
	while( true )
	{
		try
		{
			CGazeCapture capture( *pCamera, "Window", g_Config.vec3MonitorPos, g_Config.vec3MonitorDim );
			capture.WriteAsync( );
		}
		catch( int i )
		{
			if( i != 1 )
				throw;
			
			break;
		}
	}
	CUtility::ShowCursor( true, "Window" );

	CGazeCapture::CloseWrite( );
	delete pCamera;
	CBaseCamera::Terminate( );
	return EXIT_SUCCESS;
}

int ProcessDataset( const char *szSrc, const char *szDst )
{	
	CLandmarkCandidate::Init( );
	CScenery::SetScenery( g_Config.vec3MonitorPos, g_Config.vec3MonitorDim );
	
	srand( (unsigned int) time( nullptr ) );
	if( !CGazeData::OpenReadRaw( std::string( szSrc ) ) )
		return EXIT_SUCCESS;
	
	if( !CGazeData::OpenWrite( std::string( szDst ) ) )
		return EXIT_SUCCESS;

	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );

	try
	{
		CGazeData data;
		while( CGazeData::ReadRawAsync( data ) )
			data.WriteAsync( );
	}
	catch( int i )
	{
		if( i != 1 )
			throw;
	}
	
	CGazeData::CloseWrite( );

	destroyAllWindows( );
	return EXIT_SUCCESS;
}

int AdjustDataset( const char *szSrc, const char *szDst )
{	
	CLandmarkCandidate::Init( );
	CScenery::SetScenery( g_Config.vec3MonitorPos, g_Config.vec3MonitorDim );
	
	srand( (unsigned int) time( nullptr ) );
	if( !CGazeData::OpenRead( std::string( szSrc ) ) )
		return EXIT_SUCCESS;
	
	if( !CGazeData::OpenWrite( std::string( szDst ), false ) )
		return EXIT_SUCCESS;

	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );

	try
	{
		CGazeData data;
		while( CGazeData::ReadAsync( data ) )
		{
			if( data.Adjust( "Window" ) )
				data.WriteAsync( );
		}
	}
	catch( int i )
	{
		if( i != 1 )
			throw;
	}
	
	CGazeData::CloseWrite( );

	destroyAllWindows( );
	return EXIT_SUCCESS;
}

int ShowDataset( const char *szFile )
{
	if( !CGazeData::OpenRead( std::string( szFile ) ) )
		return EXIT_SUCCESS;

	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );

	CScenery::SetScenery( g_Config.vec3MonitorPos, g_Config.vec3MonitorDim );

	CGazeData data;
	while( CGazeData::ReadAsync( data ) )
		data.DrawScenery( "Window" );

	destroyAllWindows( );
	return EXIT_SUCCESS;
}

int TestImage( void )
{
	(void) CBaseCamera::Init( );
	CBaseCamera *pCamera;
	try
	{
		pCamera = CBaseCamera::SelectCamera( );
	}
	catch( int i )
	{
		if( i == 1 )
		{
			CBaseCamera::Terminate( );
			return EXIT_SUCCESS;
		}

		throw;
	}

	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );

	unsigned char cKey;
	CImage imgTest( "Image_Test" );
	bool fContinue = true;
	bool fContinueKey;
	while( fContinue )
	{
		pCamera->TakePicture( imgTest );
		imgTest.Show( "Window" );

		fContinueKey = true;
		while( fContinueKey )
		{
			cKey = CUtility::WaitKey( 0 );
			switch( cKey )
			{
			case 27:	//Escape
				fContinue = false;
				fContinueKey = false;
				break;
			case 10:	//Enter
				fContinueKey = false;
				break;
			}
		}
	}

	destroyAllWindows( );
	CBaseCamera::Terminate( );
	delete pCamera;
	return EXIT_SUCCESS;
}

int RenderTest( void )
{
	CVector<3> vec3Monitor( { -0.2375, -0.02, -0.02 } );
	CVector<3> vec3MonitorDim( { 0.475, -0.298, -0.03 } );
	
	CVector<3> vec3EyeLeft( { 0.0325, -0.02, 0.5 } );
	CVector<3> vec3EyeRight( { -0.0325, 0, 0.5 } );
	CVector<3> vec3Gaze( { 0.2, -0.15, -0.02 } );
	CRay rayEyeLeft( vec3EyeLeft, vec3Gaze - vec3EyeLeft );
	CRay rayEyeRight( vec3EyeRight, vec3Gaze - vec3EyeRight );
	
	CScenery scenery( vec3Monitor, vec3MonitorDim, rayEyeLeft, rayEyeRight );
	CImage img( "Image_Scenery" );
	img.matImage = cv::Mat( 1050, 1050, CV_8UC3, cv::Scalar( 0, 0, 0 ) );

	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );

	unsigned char cKey;
	bool fContinue = true;
	double dDegX = 0;
	double dDegY = 0;
	while( fContinue )
	{
		CImage imgDraw( img, "Image_Draw" );
		
		scenery.Transformed( CRenderHelper::GetRotationMatrix( dDegX, dDegY, 0 ) ).Fit( ).Draw( imgDraw );
		imgDraw.Show( "Window" );

		//cKey = (unsigned char) waitKey( 0 );
		cKey = CUtility::WaitKey( 0 );
		switch( cKey )
		{
		case 27:	//Escape
			fContinue = false;
			break;
		case 81:	//Key)_Left
			dDegY -= 1;
			if( dDegY < 0 )
				dDegY = 360;

			break;
		case 82:	//Key_Up
			dDegX -= 1;
			if( dDegX < 0 )
				dDegX = 360;

			break;
		case 83:	//Key_Right
			dDegY += 1;
			if( dDegY > 360 )
				dDegY = 0;

			break;
		case 84:	//Key_Down
			dDegX += 1;
			if( dDegX > 360 )
				dDegX = 0;

			break;
		default:
			printf( "Key: %u\n", cKey & 0xFF );
		}
	}

	destroyAllWindows( );
	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
/*
#ifdef _MSC_VER
	if( _chdir( WORKING_DIRECTORY ) )
	{
		perror( "Error switching working directory" );
		return EXIT_FAILURE;
	}
#else
	if( chdir( WORKING_DIRECTORY ) )
	{
		perror( "Error switching working directory" );
		return EXIT_FAILURE;
	}
#endif
*/

	g_Config = CConfig( "./config.cfg" );

	if( argc == 2 )
	{
#ifdef _MSC_VER
		if( !_stricmp( argv[ 1 ], "test" ) )
#else
		if( !strcasecmp( argv[ 1 ], "test" ) )
#endif
		{
			return TestImage( );
		}
	}
	else if( argc == 3 )
	{
#ifdef _MSC_VER
		if( !_stricmp( argv[ 1 ], "edit" ) )
#else
		if( !strcasecmp( argv[ 1 ], "edit" ) )
#endif
		{
			return EditDataset( argv[ 2 ] );
		}
#ifdef _MSC_VER
		else if( !_stricmp( argv[ 1 ], "show" ) )
#else
		else if( !strcasecmp( argv[ 1 ], "show" ) )
#endif
		{
			return ShowDataset( argv[ 2 ] );
		}
	}
	else if( argc == 4 )
	{
#ifdef _MSC_VER
		if( !_stricmp( argv[ 1 ], "proc" ) )
#else
		if( !strcasecmp( argv[ 1 ], "proc" ) )
#endif
		{
			return ProcessDataset( argv[ 2 ], argv[ 3 ] );
		}
		
#ifdef _MSC_VER
		if( !_stricmp( argv[ 1 ], "adj" ) )
#else
		if( !strcasecmp( argv[ 1 ], "adj" ) )
#endif
		{
			return AdjustDataset( argv[ 2 ], argv[ 3 ] );
		}
	}
	
	fprintf( stderr, "Invalid arguments\n" );
	(void) getchar( );
	return EXIT_FAILURE;
}
