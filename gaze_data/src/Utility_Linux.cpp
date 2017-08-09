#ifndef _MSC_VER
#include "Utility.h"
#include <unistd.h>
#include <termios.h>
#include <opencv2\highgui.hpp>

int CUtility::Run( runfunc_t pfnRun, void *pParam )
{
	return (int) pfnRun( pParam );
}

unsigned char CUtility::WaitKey( unsigned int uMilliseconds )
{
	return cv::waitKey( uMilliseconds );
}

void CUtility::Cls( void )
{
	//CSI[2J clears screen, CSI[H moves the cursor to top-left corner
	std::cout << "\x1B[2J\x1B[H";
}

unsigned char CUtility::GetChar( void )
{
	unsigned char buf = 0;
	struct termios old = { 0 };
	if( tcgetattr( 0, &old ) < 0 )
		perror( "tcsetattr()" );

	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[ VMIN ] = 1;
	old.c_cc[ VTIME ] = 0;
	if( tcsetattr( 0, TCSANOW, &old ) < 0 )
		perror( "tcsetattr ICANON" );

	if( read( 0, &buf, 1 ) < 0 )
		perror( "read()" );

	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if( tcsetattr( 0, TCSADRAIN, &old ) < 0 )
		perror( "tcsetattr ~ICANON" );

	return buf;
}
#endif