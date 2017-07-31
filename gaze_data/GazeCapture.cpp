#include "GazeCapture.h"
#include "Point.h"
#include <stdlib.h>
#include <time.h>

#ifdef _MSC_VER
#	include <wtypes.h>
#else
#	include <X11/Xlib.h>
#endif

using namespace cv;

void CGazeCapture::Init( cv::VideoCapture &cap )
{
	cap.grab( );
	srand( (unsigned int) time( nullptr ) );
}

void CGazeCapture::GetScreenResolution( unsigned int & uWidth, unsigned int & uHeight )
{
#ifdef _MSC_VER
	RECT desktop;
	GetWindowRect( GetDesktopWindow( ), &desktop );
	uWidth = desktop.right;
	uHeight = desktop.bottom;
#else
	Display *d = XOpenDisplay( nullptr );
	Screen *s = DefaultScreenOfDisplay( d );
	uWidth = s->width;
	uHeight = s->height;
#endif
}

CGazeCapture::CGazeCapture( VideoCapture &cap, const char *szWindow ) :
	imgGaze( "Image_Gaze" )
{
	{
		unsigned int uWidth;
		unsigned int uHeight;
		GetScreenResolution( uWidth, uHeight );
		imgGaze.matImage = Mat( uHeight, uWidth, CV_8UC3, Scalar( 127, 0, 0 ) );
	}

	ptGaze = CPoint( imgGaze, rand( ) / (double) RAND_MAX, rand( ) / (double) RAND_MAX, "Point_Gaze" );
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