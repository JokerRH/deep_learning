#ifndef _MSC_VER
#include "Utility.h"
#include <unistd.h>
#include <termios.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <ftw.h>

#include <gtk/gtk.h>

//`pkg-config --cflags gtk+-2.0`
//`pkg-config --libs gtk+-2.0`

std::vector<std::string> CUtility::s_vecFiles;

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

bool CUtility::CreateFolder( const std::string &sPath )
{
	if( mkdir( sPath.c_str( ), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) && errno != EEXIST )
	{
		perror( "Error creating directory" );
		return false;
	}
	
	return true;
}

std::vector<std::string> CUtility::GetFilesInDir( const std::string &sDir )
{
	s_vecFiles.clear( );
	ftw( sDir.c_str( ), []( const char *szPath, const struct stat *pStat, int fType )->int
		{
			if( fType != FTW_F )
				return 0; //Continue
			
			s_vecFiles.push_back( szPath );
			return 0; //Continue
		}, 16 );

	return s_vecFiles;
}

void CUtility::ShowCursor( bool fShow, const char *szWindow )
{
	GtkWidget *pWindow = (GtkWidget *) cvGetWindowHandle( szWindow );
	GdkCursor *pCursor;
	if( fShow )
		pCursor = nullptr;
	else
		pCursor = gdk_cursor_new_for_display( gdk_display_get_default( ), GDK_BLANK_CURSOR );

	gdk_window_set_cursor( gtk_widget_get_window( pWindow ), pCursor );
}
#endif