#include "GazeCapture.h"
#include "Point.h"
#include <stdlib.h>
#include <time.h>
#include <regex>
#include <fstream>
#include <iostream>
#ifdef _MSC_VER
#	include <wtypes.h>
#	include <direct.h>
#	include <conio.h>
#else
#	include <X11/Xlib.h>
#	include <unistd.h>
#	include <termios.h>
#endif

using namespace cv;

double CGazeCapture::s_dEyeDistance;
double CGazeCapture::s_dFOV;
FILE *CGazeCapture::s_pFile;
std::string CGazeCapture::s_sName;
std::string CGazeCapture::s_sDataPath;
unsigned int CGazeCapture::s_uCurrentImage;

bool CGazeCapture::Init( cv::VideoCapture &cap, const char *szFile )
{
	if( !OpenOrCreate( std::string( szFile ) ) )
		return false;

	cap.grab( );
	srand( (unsigned int) time( nullptr ) );
	return true;
}

void CGazeCapture::Destroy( void )
{
	fclose( s_pFile );
}

void CGazeCapture::GetScreenResolution( unsigned int & uWidth, unsigned int & uHeight )
{
#ifdef _MSC_VER
	RECT desktop;
	GetWindowRect( GetDesktostatic unsigned int s_uCurrentImage;pWindow( ), &desktop );
	uWidth = desktop.right;
	uHeight = desktop.bottom;
#else
	Display *d = XOpenDisplay( nullptr );
	Screen *s = DefaultScreenOfDisplay( d );
	uWidth = s->width;
	uHeight = s->height;
#endif
}

bool CGazeCapture::OpenOrCreate( const std::string &sFile )
{
	s_sDataPath = GetPath( sFile ) + GetFileName( sFile ) + "/";
	s_uCurrentImage = 0;
	if( !Exists( sFile ) )
	{
		Cls( );
		printf( "Creating new profile\n" );
		printf( "Name                : " );
		getline( std::cin, s_sName );
		printf( "Eye distance (in cm): " );
		std::string str;
		getline( std::cin, str );
		s_dEyeDistance = std::stod( str ) / 100;
		printf( "Camera FOV          : " );
		getline( std::cin, str );
		s_dFOV = std::stod( str );

#ifdef _MSC_VER
		_mkdir( s_sDataPath.c_str( ) );
#else
		mkdir( s_sDataPath.c_str( ), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
#endif

		s_pFile = fopen( sFile.c_str( ), "w" );
		if( !s_pFile )
		{
			fprintf( stderr, "Unable to open file \"%s\"\n", sFile.c_str( ) );
			return false;
		}
		fputs( "name=", s_pFile );
		fputs( s_sName.c_str( ), s_pFile );
		fputs( "\ndist=", s_pFile );
		fputs( std::to_string( s_dEyeDistance * 100 ).c_str( ), s_pFile );
		fputs( "cm\nfov=", s_pFile );
		fputs( std::to_string( s_dFOV ).c_str( ), s_pFile );
		fputs( "\n\ndata:\n", s_pFile );
	}
	else
	{
		//File exists, open
		std::ifstream file( sFile );
		const std::regex regex_name( R"a(name=([\s\S]*))a" );
		const std::regex regex_dist( R"a(dist=((?:\d+(?:\.\d+)?)|(?:\.\d+))cm)a" );
		const std::regex regex_data( R"a(data:)a" );
		const std::regex regex_fov( R"a(fov=((?:\d+(?:\.\d+)?)|(?:\.\d+)))a" );
		std::smatch match;
		unsigned char fFound = 0;
		std::string sLine;
		while( std::getline( file, sLine ) )
		{
			std::regex_match( sLine, match, regex_name );
			if( match.size( ) )
			{
				s_sName = match[ 1 ].str( );
				fFound |= 1;
				continue;
			}

			std::regex_match( sLine, match, regex_dist );
			if( match.size( ) )
			{
				s_dEyeDistance = std::stod( match[ 1 ].str( ) ) / 100;
				fFound |= 2;
				continue;
			}
			
			std::regex_match( sLine, match, regex_fov );
			if( match.size( ) )
			{
				s_dFOV = std::stod( match[ 1 ].str( ) );
				fFound |= 4;
				continue;
			}
			
			std::regex_match( sLine, match, regex_data );
			if( match.size( ) )
			{
				fFound |= 8;
				break;	//Start of data
			}
		}

		if( fFound != 15 )
		{
			fprintf( stderr, "File \"%s\" is missing fields\n", sFile.c_str( ) );
			return false;
		}

		const std::regex regex_line( R"a((\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\d+)\s+((?:\d+(?:\.\d+)?)|(?:\.\d+))\s+((?:\d+(?:\.\d+)?)|(?:\.\d+)))a" );
		while( std::getline( file, sLine ) )
		{
			std::regex_match( sLine, match, regex_line );
			if( match.size( ) )
				s_uCurrentImage = std::max( s_uCurrentImage, (unsigned int) std::stoul( match[ 7 ].str( ) ) );
		}
		s_uCurrentImage++;

		s_pFile = fopen( sFile.c_str( ), "a" );
		if( !s_pFile )
		{
			fprintf( stderr, "Unable to open file \"%s\"\n", sFile.c_str( ) );
			return false;
		}
	}

	Cls( );
	printf( "Name        : %s\n", s_sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_dEyeDistance * 100 );
	printf( "Camera FOV  : %3.1f°\n", s_dFOV );
	printf( "Data path   : %s\n", s_sDataPath.c_str( ) );
	printf( "Next image  : %u\n", s_uCurrentImage );
	unsigned char cKey;
	while( true )
	{
		cKey = GetChar( );
		switch( cKey )
		{
		case 141:	//Numpad enter
		case 10:	//Enter
			return true;
		case 27:	//Escape
			throw( 1 );
		}
	}
}

void CGazeCapture::Cls( void )
{
#ifdef _MSC_VER
	COORD topLeft  = { 0, 0 };
	HANDLE console = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo( console, &screen );
	FillConsoleOutputCharacterA( console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
	FillConsoleOutputAttribute( console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
	SetConsoleCursorPosition( console, topLeft );
#else
	//CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    std::cout << "\x1B[2J\x1B[H";
#endif
}

unsigned char CGazeCapture::GetChar( void )
{
#ifdef _MSC_VER
	return _getch( );
#else
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
#endif
}

std::vector<CGazeCapture> CGazeCapture::Load( const std::string &sFile )
{
	std::vector<CGazeCapture> vecCaptures;
	if( !Exists( sFile ) )
		return vecCaptures;

	s_sDataPath = GetPath( sFile ) + GetFileName( sFile ) + "/";
	s_uCurrentImage = 0;
	
	std::ifstream file( sFile );
	const std::regex regex_name( R"a(name=([\s\S]*))a" );
	const std::regex regex_dist( R"a(dist=((?:\d+(?:\.\d+)?)|(?:\.\d+))cm)a" );
	const std::regex regex_fov( R"a(fov=((?:\d+(?:\.\d+)?)|(?:\.\d+)))a" );
	const std::regex regex_data( R"a(data:)a" );
	std::smatch match;
	unsigned char fFound = 0;
	std::string sLine;
	while( std::getline( file, sLine ) )
	{
		std::regex_match( sLine, match, regex_name );
		if( match.size( ) )
		{
			s_sName = match[ 1 ].str( );
			fFound |= 1;
			continue;
		}

		std::regex_match( sLine, match, regex_dist );
		if( match.size( ) )
		{
			s_dEyeDistance = std::stod( match[ 1 ].str( ) ) / 100;
			fFound |= 2;
			continue;
		}

		std::regex_match( sLine, match, regex_fov );
		if( match.size( ) )
		{
			s_dFOV = std::stod( match[ 1 ].str( ) );
			fFound |= 4;
			continue;
		}

		std::regex_match( sLine, match, regex_data );
		if( match.size( ) )
		{
			fFound |= 8;
			break;	//Start of data
		}
	}

	if( fFound != 15 )
	{
		fprintf( stderr, "File \"%s\" is missing fields\n", sFile.c_str( ) );
		return vecCaptures;
	}

	const std::regex regex_line( R"a((\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\d+)\s+((?:\d+(?:\.\d+)?)|(?:\.\d+))\s+((?:\d+(?:\.\d+)?)|(?:\.\d+)))a" );
	struct tm timeinfo = { 0 };
	cv::Mat matImage;
	double dX;
	double dY;
	unsigned int uCurrent;
	std::string str;
	while( std::getline( file, sLine ) )
	{
		std::regex_match( sLine, match, regex_line );
		if( match.size( ) )
		{
			dY = std::stod( match[ 9 ].str( ) );
			dX = std::stod( match[ 8 ].str( ) );
			uCurrent = std::stoul( match[ 7 ].str( ) );
			s_uCurrentImage = std::max( s_uCurrentImage, uCurrent );
			timeinfo.tm_sec = std::stoi( match[ 6 ].str( ) );
			timeinfo.tm_min = std::stoi( match[ 5 ].str( ) );
			timeinfo.tm_hour = std::stoi( match[ 4 ].str( ) );
			timeinfo.tm_mday = std::stoi( match[ 3 ].str( ) );
			timeinfo.tm_mon = std::stoi( match[ 2 ].str( ) ) - 1;
			timeinfo.tm_year = std::stoi( match[ 1 ].str( ) ) - 1900;
			str = s_sDataPath + "img_" + std::to_string( uCurrent ) + ".jpg";
			matImage = imread( str, CV_LOAD_IMAGE_COLOR );
			if( !matImage.data )
			{
				fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
				continue;
			}
			vecCaptures.emplace_back( matImage, dX, dY, mktime( &timeinfo ) );
		}
	}

	Cls( );
	printf( "Name        : %s\n", s_sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_dEyeDistance * 100 );
	printf( "Camera FOV  : %3.1f°\n", s_dFOV );
	printf( "Data path   : %s\n", s_sDataPath.c_str( ) );
	printf( "Images      : %u\n", s_uCurrentImage );
	unsigned char cKey;
	while( true )
	{
		cKey = GetChar( );
		switch( cKey )
		{
		case 141:	//Numpad enter
		case 10:	//Enter
			return vecCaptures;
		case 27:	//Escape
			throw( 1 );
		}
	}

	return vecCaptures;
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
	timeCapture = time( nullptr );
	//imgGaze.Show( szWindow );
	//waitKey( 0 );
}

CGazeCapture::CGazeCapture( const cv::Mat &mat, double dX, double dY, time_t timeCapture ) :
	imgGaze( mat, "Image_Gaze" ),
	ptGaze( imgGaze, dX, dY, "Point_Gaze" ),
	timeCapture( timeCapture )
{

}

CGazeCapture::~CGazeCapture( )
{
	
}

bool CGazeCapture::Write( void )
{
	struct tm *timeinfo = localtime( &timeCapture );
	char szDate[ 20 ];
	strftime( szDate, 20, "%F %T", timeinfo ); //YYYY-MM-DD HH:MM:SS
	fputs( szDate, s_pFile );
	fprintf( s_pFile, " %u %f %f\n", s_uCurrentImage, ptGaze.GetRelPositionX( 0 ), ptGaze.GetRelPositionY( 0 ) );

	imwrite( s_sDataPath + "img_" + std::to_string( s_uCurrentImage++ ) + ".jpg", imgGaze.matImage );
	return true;
}