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
#else
#	include <unistd.h>
#	include <strings.h>
#endif

using namespace cv;

const CVector<3> vec3MonitorPos( { 0.2375, -0.02, -0.02 } );
const CVector<3> vec3MonitorDim( { -0.475, -0.298, -0.03 } );

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
	
	while( true )
	{
		try
		{
			CGazeCapture capture( *pCamera, "Window", vec3MonitorPos, vec3MonitorDim );
			capture.WriteAsync( );
		}
		catch( int i )
		{
			if( i != 1 )
				throw;
			
			break;
		}
	}

	CGazeCapture::CloseWrite( );
	delete pCamera;
	CBaseCamera::Terminate( );
	return EXIT_SUCCESS;
}

int ProcessDataset( const char *szSrc, const char *szDst )
{	
	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );
	
	CLandmarkCandidate::Init( );
	CScenery::SetScenery( vec3MonitorPos, vec3MonitorDim );
	
	srand( (unsigned int) time( nullptr ) );
	if( !CGazeCapture::OpenRead( std::string( szSrc ) ) )
		return EXIT_SUCCESS;
	
	if( !CGazeData::OpenWrite( std::string( szDst ) ) )
		return EXIT_SUCCESS;

	CGazeCapture capture;
	while( CGazeCapture::ReadAsync( capture ) )
	{
		std::vector<CGazeData> vecData = CGazeData::GetGazeData( capture, "Window" );
		for( std::vector<CGazeData>::iterator it = vecData.begin( ); it < vecData.end( ); it++ )
			it->WriteAsync( );
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

	CScenery::SetScenery( vec3MonitorPos, vec3MonitorDim );

	CGazeData data;
	while( CGazeData::ReadAsync( data ) )
		data.DrawScenery( "Window" );

	destroyAllWindows( );
	return EXIT_SUCCESS;
}

int CaptureVideo( void )
{
	VideoCapture cap( 0 );
	if( !cap.isOpened( ) )
	{
		fprintf( stderr, "Unable to open capture device\n" );
		return EXIT_FAILURE;
	}

	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );

	CLandmarkCandidate::Init( );

	CImage imgFrame( "Image_Frame" );
	CImage imgDraw( "Image_Draw" );
	
	unsigned char cKey;
	bool fContinue = true;
	while( fContinue )
	{
		cap >> imgFrame.matImage;

		std::vector<CLandmarkCandidate> vecCandidates = CLandmarkCandidate::GetCandidates( imgFrame );
		imgDraw = CImage( imgFrame, "Image_Draw" );
		for( std::vector<CLandmarkCandidate>::iterator pCandidate = vecCandidates.begin( ); pCandidate < vecCandidates.end( ); pCandidate++ )
			pCandidate->Draw( imgDraw );

		imgDraw.Show( "Window" );

		cKey = (unsigned char) waitKey( 5 );
		switch( cKey )
		{
		case 27:	//Escape
			fContinue = false;
			break;
		case 141:	//Numpad enter
		case 10:	//Enter
			try
			{
				CLandmark::GetLandmarks( vecCandidates, "Window" );
			}
			catch( int i )
			{
				if( i != 1 )
					throw;
			}
			break;
		case 255:
			break;
		}
	}

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

#if 0
int CaptureGaze( void )
{
	VideoCapture cap( 0 );
	if( !cap.isOpened( ) )
	{
		fprintf( stderr, "Unable to open capture device\n" );
		return EXIT_FAILURE;
	}

	namedWindow( "Window", CV_WINDOW_NORMAL );
	setWindowProperty( "Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );

	CLandmarkCandidate::Init( );
	CGazeCapture::Init( cap, "asdf" );
	CVector<3> vec3MonitorPos( { 0.2375, -0.02, -0.02 } );
	CVector<3> vec3MonitorDim( { -0.475, -0.298, -0.03 } );
	CScenery::SetScenery( vec3MonitorPos, vec3MonitorDim );
	
	std::vector<CGazeCapture> vecGaze;
	bool fContinue = true;
	while( fContinue )
	{
		try
		{
			vecGaze.emplace_back( cap, "Window" );
			std::vector<CGazeData> vecGazeData = CGazeData::GetGazeData( { vecGaze.back( ) }, 0.065, 60, vec3MonitorPos, vec3MonitorDim, "Window" );
			for( std::vector<CGazeData>::iterator it = vecGazeData.begin( ); it < vecGazeData.end( ); it++ )
				if( !it->DrawScenery( "Window" ) )
				{
					fContinue = false;
					break;
				}
		}
		catch( int i )
		{
			if( i == 0 )
				continue;
			if( i == 1 )
				break;
			
			throw;
		}
	}
	return EXIT_SUCCESS;
}
#endif

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
	}
	
	fprintf( stderr, "Invalid arguments\n" );
	(void) getchar( );
	return EXIT_FAILURE;
}
