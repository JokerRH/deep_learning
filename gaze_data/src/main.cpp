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
	if( !CGazeData::OpenReadRaw( std::string( szSrc ), std::string( szDst ) ) )
		return EXIT_SUCCESS;
	
	if( !CGazeData::OpenWrite( std::string( szDst ) ) )
		return EXIT_SUCCESS;

	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );

	try
	{
		CGazeData data;
		while( CGazeData::ReadAsync( data ) )
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
		if( !CUtility::Stricmp( argv[ 1 ], "test" ) )
			return TestImage( );
	}
	else if( argc == 3 )
	{
		if( !CUtility::Stricmp( argv[ 1 ], "edit" ) )
			return EditDataset( argv[ 2 ] );
		else if( !CUtility::Stricmp( argv[ 1 ], "show" ) )
			return ShowDataset( argv[ 2 ] );
	}
	else if( argc == 4 )
	{
		if( !CUtility::Stricmp( argv[ 1 ], "proc" ) )
			return ProcessDataset( argv[ 2 ], argv[ 3 ] );
		else if( !CUtility::Stricmp( argv[ 1 ], "adj" ) )
			return AdjustDataset( argv[ 2 ], argv[ 3 ] );
		else if( !CUtility::Stricmp( argv[ 1 ], "exp" ) )
			return CGazeData::Export( argv[ 2 ], argv[ 3 ] ) ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	else if( argc == 5 )
	{
		if( !CUtility::Stricmp( argv[ 1 ], "exp" ) )
			return CGazeData::Export( argv[ 2 ], argv[ 3 ], atof( argv[ 4 ] ) ) ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	
	fprintf( stderr, "Invalid arguments\n" );
	(void) getchar( );
	return EXIT_FAILURE;
}
