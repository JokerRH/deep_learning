#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "LandmarkCandidate.h"
#include "Landmark.h"
#include "GazeCapture.h"
#include "Config.h"

#ifdef _MSC_VER
#	include<direct.h>
#else
#	include <unistd.h>
#endif

using namespace cv;

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

	return EXIT_SUCCESS;
}

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

	CGazeCapture::Init(cap );
	std::vector<CGazeCapture> vecGaze;
	while( true )
	{
		try
		{
			vecGaze.emplace_back( cap, "Window" );
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

int main(int argc, char **argv)
{
#ifdef _MSC_VER
	_chdir( WORKING_DIRECTORY );
#else
	chdir( WORKING_DIRECTORY );
#endif

	//int iReturn = CaptureVideo( );
	int iReturn = CaptureGaze( );
	destroyAllWindows( );

#ifdef _MSC_VER
	system( "PAUSE" );
#endif
	return iReturn;
}
