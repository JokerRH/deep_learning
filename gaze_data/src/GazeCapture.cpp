#include "GazeCapture.h"
#include "Point.h"
#include <stdlib.h>

#ifdef _MSC_VER

#else
#include <X11/Xlib.h>
#endif

using namespace cv;

void CGazeCapture::Init( cv::VideoCapture &cap )
{
	cap.grab( );
	srand( time( nullptr ) );
}

CGazeCapture::CGazeCapture( VideoCapture &cap, const char *szWindow ) :
	imgGaze( "Image_Gaze" )
{
	{
		Display *d = XOpenDisplay( nullptr );
		Screen *s = DefaultScreenOfDisplay( d );
		imgGaze.matImage = Mat( s->height, s->width, CV_8UC3, Scalar( 127, 0, 0 ) );
	}

	CPoint ptGaze( imgGaze, rand( ) / (float) RAND_MAX, rand( ) / (float) RAND_MAX, "Point_Gaze" );
	ptGaze.Draw( Scalar( 255, 255, 255 ), 5 );

	imgGaze.Show( szWindow );
	unsigned char cKey;
	bool fContinue = true;
	while( fContinue )
	{
		cKey = (unsigned char) waitKey( 0 );
		switch( cKey )
		{
		case 8:		//Backspace
			throw( 0 );
		case 141:	//Numpad enter
		case 10:	//Enter
			fContinue = false;
			break;
		case 27:	//Escape
			throw( 1 );
		}
	}

	cap.grab( );
	cap.grab( );
	cap.grab( );
	cap.grab( );
	cap.grab( );
	cap.retrieve( imgGaze.matImage );
	imgGaze.Show( szWindow );
	waitKey( 0 );
}

CGazeCapture::~CGazeCapture( )
{
	
}

