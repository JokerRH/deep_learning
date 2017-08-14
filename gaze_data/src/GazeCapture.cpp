#include "GazeCapture.h"
#include "Point.h"
#include "Scenery.h"
#include "Utility.h"
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>

#ifdef _MSC_VER
#	include <direct.h>
#endif

#ifdef max
#	undef max
#endif

#ifdef min
#	undef min
#endif

using namespace cv;

double CGazeCapture::s_dEyeDistance;
FILE *CGazeCapture::s_pFile;
std::string CGazeCapture::s_sName;
std::string CGazeCapture::s_sDataPath;
unsigned int CGazeCapture::s_uCurrentImage;

const std::regex CGazeCapture::s_regex_name( R"a(name=([\s\S]*).*)a" );
const std::regex CGazeCapture::s_regex_dist( R"a(dist=((?:\d+(?:\.\d+)?)|(?:\.\d+))cm.*)a" );
const std::regex CGazeCapture::s_regex_data( R"a(data:.*)a" );
const std::regex CGazeCapture::s_regex_line( R"a((\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\d+)\s+((?:\d+(?:\.\d+)?)|(?:\.\d+))\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\).*)a" );

bool CGazeCapture::Init( const char *szFile )
{
	if( !OpenOrCreate( std::string( szFile ) ) )
		return false;

	srand( (unsigned int) time( nullptr ) );
	return true;
}

void CGazeCapture::Destroy( void )
{
	fclose( s_pFile );
}

bool CGazeCapture::OpenOrCreate( const std::string &sFile )
{
	s_sDataPath = CUtility::GetPath( sFile ) + CUtility::GetFileName( sFile ) + "/";
	s_uCurrentImage = 0;
	if( !CUtility::Exists( sFile ) )
	{
		CUtility::Cls( );
		printf( "Creating new profile\n" );
		printf( "Name                : " );
		getline( std::cin, s_sName );
		printf( "Eye distance (in cm): " );
		std::string str;
		getline( std::cin, str );
		s_dEyeDistance = std::stod( str ) / 100;

#ifdef _MSC_VER
		if( _mkdir( s_sDataPath.c_str( ) ) && errno != EEXIST )
		{
			perror( "Error creating directory" );
			return false;
		}
#else
		if( mkdir( s_sDataPath.c_str( ), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) && errno != EEXIST )
		{
			perror( "Error creating directory" );
			return false;
		}
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
		fputs( "cm\n\ndata:\n", s_pFile );
	}
	else
	{
		//File exists, open
		std::ifstream file( sFile );
		std::smatch match;
		unsigned char fFound = 0;
		std::string sLine;
		while( std::getline( file, sLine ) )
		{
			std::regex_match( sLine, match, s_regex_name );
			if( match.size( ) )
			{
				s_sName = match[ 1 ].str( );
				fFound |= 1;
				continue;
			}

			std::regex_match( sLine, match, s_regex_dist );
			if( match.size( ) )
			{
				s_dEyeDistance = std::stod( match[ 1 ].str( ) ) / 100;
				fFound |= 2;
				continue;
			}
			
			std::regex_match( sLine, match, s_regex_data );
			if( match.size( ) )
			{
				fFound |= 4;
				break;	//Start of data
			}
		}

		if( fFound != 7 )
		{
			fprintf( stderr, "File \"%s\" is missing fields\n", sFile.c_str( ) );
			return false;
		}

		while( std::getline( file, sLine ) )
		{
			std::regex_match( sLine, match, s_regex_line );
			if( match.size( ) )
			{
				unsigned int u = std::stoul( match[ 7 ].str( ) );
				s_uCurrentImage = std::max( s_uCurrentImage, u );
			}
		}
		s_uCurrentImage++;

		s_pFile = fopen( sFile.c_str( ), "a" );
		if( !s_pFile )
		{
			fprintf( stderr, "Unable to open file \"%s\"\n", sFile.c_str( ) );
			return false;
		}
	}

	CUtility::Cls( );
	printf( "Name        : %s\n", s_sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_dEyeDistance * 100 );
	printf( "Data path   : %s\n", s_sDataPath.c_str( ) );
	printf( "Next image  : %u\n", s_uCurrentImage );
	unsigned char cKey;
	while( true )
	{
		cKey = CUtility::GetChar( );
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

std::vector<CGazeCapture> CGazeCapture::Load( const std::string &sFile )
{
	std::vector<CGazeCapture> vecCaptures;
	if( !CUtility::Exists( sFile ) )
	{
		CUtility::Cls( );
		printf( "File \"%s\" does not exist.\n", sFile.c_str( ) );
		CUtility::GetChar( );
		return vecCaptures;
	}

	s_sDataPath = CUtility::GetPath( sFile ) + CUtility::GetFileName( sFile ) + "/";
	s_uCurrentImage = 0;
	
	std::ifstream file( sFile );
	std::smatch match;
	unsigned char fFound = 0;
	std::string sLine;
	while( std::getline( file, sLine ) )
	{
		std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
		std::regex_match( sLine, match, s_regex_name );
		if( match.size( ) )
		{
			s_sName = match[ 1 ].str( );
			fFound |= 1;
			continue;
		}

		std::regex_match( sLine, match, s_regex_dist );
		if( match.size( ) )
		{
			s_dEyeDistance = std::stod( match[ 1 ].str( ) ) / 100;
			fFound |= 2;
			continue;
		}

		std::regex_match( sLine, match, s_regex_data );
		if( match.size( ) )
		{
			fFound |= 4;
			break;	//Start of data
		}
	}

	if( fFound != 7 )
	{
		CUtility::Cls( );
		fprintf( stderr, "File \"%s\" is missing fields\n", sFile.c_str( ) );
		printf( "File \"%s\" is missing fields\n", sFile.c_str( ) );
		if( !( fFound & 1 ) )
			printf( "Missing Name field\n" );
		if( !( fFound & 2 ) )
			printf( "Missing Distance field\n" );
		if( !( fFound & 4 ) )
			printf( "Missing Data field\n" );
		CUtility::GetChar( );
		return vecCaptures;
	}

	while( std::getline( file, sLine ) )
	{
		std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
		Load( vecCaptures, sLine );
	}
	s_uCurrentImage++;

	//CUtility::Cls( );
	printf( "Name        : %s\n", s_sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_dEyeDistance * 100 );
	printf( "Data path   : %s\n", s_sDataPath.c_str( ) );
	printf( "Images      : %u\n", s_uCurrentImage );
	unsigned char cKey;
	while( true )
	{
		cKey = CUtility::GetChar( );
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

CGazeCapture::CGazeCapture( CBaseCamera &camera, const char *szWindow, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim ) :
	imgGaze( "Image_Gaze" ),
	vec3Point( { 0 } )
{
	{
		unsigned int uWidth;
		unsigned int uHeight;
		CImage::GetScreenResolution( uWidth, uHeight );
		imgGaze.matImage = Mat( uHeight, uWidth, CV_8UC3, Scalar( 127, 0, 0 ) );
	}

	CPoint ptGaze = CPoint( imgGaze, rand( ) / (double) RAND_MAX, rand( ) / (double) RAND_MAX, "Point_Gaze" );
	ptGaze.Draw( Scalar( 255, 255, 255 ), 5 );

	imgGaze.Show( szWindow );
	unsigned char cKey;
	bool fContinue = true;
	while( fContinue )
	{
		cKey = CUtility::WaitKey( 0 );
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

	camera.TakePicture( imgGaze );
	vec3Point = CVector<3>(
	{
		-( vec3ScreenTL[ 0 ] + ptGaze.GetRelPositionX( -1 ) * vec3ScreenDim[ 0 ] ),
		vec3ScreenTL[ 1 ] + ptGaze.GetRelPositionY( -1 ) * vec3ScreenDim[ 1 ],
		vec3ScreenTL[ 2 ]
	} );
}

CGazeCapture::CGazeCapture( CImage &img, CVector<3> vec3Point ) :
	imgGaze( img, "Image_Gaze" ),
	vec3Point( vec3Point )
{
	imgGaze.TransferOwnership( -1 );	//Make top level image
}

CGazeCapture::~CGazeCapture( )
{
	
}

bool CGazeCapture::Write( void )
{
	struct tm *timeinfo = localtime( &imgGaze.timestamp );
	char szDate[ 20 ];
	strftime( szDate, 20, "%F %T", timeinfo ); //YYYY-MM-DD HH:MM:SS
	fputs( szDate, s_pFile );
	fprintf( s_pFile, " %u %f (%f, %f, %f)\n", s_uCurrentImage, imgGaze.dFOV, vec3Point[ 0 ], vec3Point[ 1 ], vec3Point[ 2 ] );

	imwrite( s_sDataPath + "img_" + std::to_string( s_uCurrentImage++ ) + ".jpg", imgGaze.matImage );
	return true;
}

bool CGazeCapture::Load( std::vector<CGazeCapture> &vecData, const std::string &sLine )
{
	std::smatch match;
	std::regex_match( sLine, match, s_regex_line );
	if( !match.size( ) )
		return false;

	CVector<3> vec3Point( { std::stod( match[ 9 ].str( ) ), std::stod( match[ 10 ].str( ) ), std::stod( match[ 11 ].str( ) ) } );
	double dFOV = std::stod( match[ 8 ].str( ) );
	
	unsigned int uCurrent = std::stoul( match[ 7 ].str( ) );
	s_uCurrentImage = std::max( s_uCurrentImage, uCurrent );
	
	struct tm timeinfo = { 0 };
	timeinfo.tm_sec = std::stoi( match[ 6 ].str( ) );
	timeinfo.tm_min = std::stoi( match[ 5 ].str( ) );
	timeinfo.tm_hour = std::stoi( match[ 4 ].str( ) );
	timeinfo.tm_mday = std::stoi( match[ 3 ].str( ) );
	timeinfo.tm_mon = std::stoi( match[ 2 ].str( ) ) - 1;
	timeinfo.tm_year = std::stoi( match[ 1 ].str( ) ) - 1900;
	
	//Load image
	std::string str = s_sDataPath + "img_" + std::to_string( uCurrent ) + ".jpg";
	cv::Mat matImage = imread( str, CV_LOAD_IMAGE_COLOR );
	if( !matImage.data )
	{
		fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
		return false;
	}
	CImage img( matImage, dFOV, mktime( &timeinfo ), "Image_Gaze" );
	
	vecData.emplace_back( img, vec3Point );
	return true;
}