#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "FrameProcessor.h"
#include "LandmarkCandidate.h"
#include "Landmark.h"

using namespace cv;

int CaptureVideo( void )
{
	VideoCapture cap( 0 );
	if( !cap.isOpened( ) )
	{
		fprintf( stderr, "Unable to open capture device\n" );
		return EXIT_FAILURE;
	}
	
	chdir( "/home/rainer/Dokumente/deep_learning/gaze_data/" );

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

#if 0
int CaptureVideo( void )
{
	VideoCapture cap( 0 );
	if( !cap.isOpened( ) )
	{
		fprintf( stderr, "Unable to open capture device\n" );
		return EXIT_FAILURE;
	}
	
	chdir( RESPATH );
	CFrameProcessor frameprocessor;

	Mat frame;
	bool fContinue = true;
	unsigned char c;
	while( fContinue )
	{
		cap >> frame;

		frameprocessor.SetImage( frame );
		frameprocessor.ProcessImage( );
		
		c = (unsigned char) waitKey( 5 );
		switch( c )
		{
		case 27:	//Escape
			fContinue = false;
			break;
		case 10:	//Enter
			{
				unsigned int uLandmarks;
				frameprocessor.ProcessImage( uLandmarks );
				break;
			}
		case 255:
			break;
		default:
			printf( "Key: %u\n", c & 0xFF );
		}
	}

	return EXIT_SUCCESS;
}
#endif

int main(int argc, char **argv)
{
	int iReturn = CaptureVideo( );
	destroyAllWindows( );
	return iReturn;
}
