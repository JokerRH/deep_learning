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
#include <regex>

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

std::fstream CGazeCapture::s_File;
CQueue<CGazeCapture> CGazeCapture::s_Queue( 10 );
pthread_t CGazeCapture::s_Thread;

CGazeCapture_Set CGazeCapture::s_DataSetRead;
CGazeCapture_Set CGazeCapture::s_DataSetWrite;
double CGazeCapture::s_dEyeDistance;
std::string CGazeCapture::s_sName;
std::string CGazeCapture::s_sDataPath;
unsigned int CGazeCapture::s_uCurrentImage;

const std::regex CGazeCapture::s_regex_name( R"a(name=([\s\S]*).*)a" );
const std::regex CGazeCapture::s_regex_dist( R"a(dist=((?:\d+(?:\.\d+)?)|(?:\.\d+))cm.*)a" );
const std::regex CGazeCapture::s_regex_data( R"a(data:.*)a" );
const std::regex CGazeCapture::s_regex_line( R"a((\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\d+)\s+((?:\d+(?:\.\d+)?)|(?:\.\d+))\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\).*)a" );

bool CGazeCapture::OpenWrite( const std::string &sFile )
{
	if( CUtility::Exists( sFile ) )
		s_DataSetWrite = CGazeCapture_Set::LoadList( sFile );
	else
	{
		std::string sName;
		double dEyeDistance;
		CUtility::Cls( );
		printf( "Creating new profile\n" );
		printf( "Name                : " );
		getline( std::cin, sName );
		printf( "Eye distance (in cm): " );
		std::string str;
		getline( std::cin, str );
		dEyeDistance = std::stod( str ) / 100;
		
		s_DataSetWrite = CGazeCapture_Set( std::vector<CGazeCapture_Set::gazecapture>( ), sName, dEyeDistance, CUtility::GetPath( sFile ) + CUtility::GetFileName( sFile ) + "/" );
	}
	
	s_DataSetWrite.Sort( );
	CheckDuplicates( s_DataSetWrite, sFile );
	if( s_DataSetWrite.vecData.size( ) )
		s_uNextImage = s_DataSetWrite.vecData.back( ).uImage + 1;
	else
		s_uNextImage = 0;
		
	CUtility::Cls( );
	printf( "Name        : %s\n", s_DataSetWrite.sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_DataSetWrite.dEyeDistance * 100 );
	printf( "Data path   : %s\n", s_DataSetWrite.sDataPath.c_str( ) );
	printf( "Images      : %u\n", (unsigned) s_DataSetWrite.vecData.size( ) );
	printf( "Next image  : %u\n", s_uNextImage );
	unsigned char cKey;
	while( true )
	{
		cKey = CUtility::GetChar( );
		switch( cKey )
		{
		case 141:	//Numpad enter
		case 10:	//Enter
			s_DataSetWrite.OpenWrite( sFile );
			s_vecThreadWrite.emplace_back( );
			pthread_create( &s_vecThreadWrite[ 0 ], nullptr, WriteThread, nullptr );
			return true;
		case 27:	//Escape
			return false;
		}
	}
	
#if 0
	s_sDataPath = CUtility::GetPath( sFile ) + CUtility::GetFileName( sFile ) + "/";
	if( !CUtility::Exists( sFile ) )
	{
		s_uCurrentImage = 0;

		CUtility::Cls( );
		printf( "Creating new profile\n" );
		printf( "Name                : " );
		getline( std::cin, s_sName );
		printf( "Eye distance (in cm): " );
		std::string str;
		getline( std::cin, str );
		s_dEyeDistance = std::stod( str ) / 100;

		if( !CUtility::CreateFolder( s_sDataPath ) )
			return false;

		s_File.open( sFile, std::fstream::out );
		s_File << "name=" << s_sName << "\n";
		s_File << "dist=" << std::to_string( s_dEyeDistance * 100 ) << "cm\n";
		s_File << "\ndata:\n";
	}
	else
	{
		s_uCurrentImage = (unsigned int) -1;

		//File exists, open
		s_File.open( sFile, std::fstream::in );
		{
			std::smatch match;
			unsigned char fFound = 0;
			std::string sLine;
			while( std::getline( s_File, sLine ) )
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
				s_File.close( );
				return false;
			}

			while( std::getline( s_File, sLine ) )
			{
				std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
				std::regex_match( sLine, match, s_regex_line );
				if( match.size( ) )
				{
					unsigned int u = std::stoul( match[ 7 ].str( ) );
					s_uCurrentImage = std::max( (int) s_uCurrentImage, (int) u );
				}
			}
			s_uCurrentImage++;
		}
		
		s_File.close( );
		s_File.open( sFile, std::fstream::out | std::fstream::app );
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
			pthread_create( &s_Thread, nullptr, WriteThread, nullptr );
			return true;
		case 27:	//Escape
			s_File.close( );
			return false;
		}
	}
#endif
}

void CGazeCapture::CloseWrite( void )
{
	for( std::vector<pthread_t>::iterator it = s_vecThreadWrite.begin( ); it < s_vecThreadWrite.end( ); it++ )
	{
		pthread_cancel( *it );
		pthread_join( *it, nullptr );
	}
	s_DataSetWrite.CloseWrite( );

#if 0
	s_Queue.Emplace_Back( );
	pthread_join( s_Thread, nullptr );
	s_File.close( );
#endif
}

bool CGazeCapture::OpenRead( const std::string &sFile )
{
	if( !CUtility::Exists( sFile ) )
	{
		CUtility::Cls( );
		printf( "File \"%s\" does not exist.\n", sFile.c_str( ) );
		CUtility::GetChar( );
		return false;
	}

	s_sDataPath = CUtility::GetPath( sFile ) + CUtility::GetFileName( sFile ) + "/";
	s_uCurrentImage = (unsigned int) -1;
	
	s_File.open( sFile, std::fstream::in );

	//Read header
	{
		std::smatch match;
		std::string sLine;
		unsigned char fFound = 0;
		while( std::getline( s_File, sLine ) )
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
			s_File.close( );
			return false;
		}
		
		std::streampos posData = s_File.tellg( );
		while( std::getline( s_File, sLine ) )
		{
			std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
			std::regex_match( sLine, match, s_regex_line );
			if( !match.size( ) )
				continue;

			s_uCurrentImage = std::max( (int) s_uCurrentImage, (int) std::stoul( match[ 7 ].str( ) ) );
		}
		s_uCurrentImage++;
		s_File.clear();
		s_File.seekg( posData );
	}
	
	pthread_create( &s_Thread, nullptr, ReadThread, nullptr );

	CUtility::Cls( );
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
			return true;
		case 27:	//Escape
			s_File.close( );
			return false;
		}
	}
}

bool CGazeCapture::ReadAsync( CGazeCapture &val )
{
	printf( "Test1\n" );
	val = s_Queue.Pop_Front( );
	printf( "Test2\n" );
	if( val.m_uImage != (unsigned int) -1 )
		return true;
		
	printf( "Test3\n" );

	pthread_join( s_Thread, nullptr );
	s_File.close( );
	return false;
}

void CGazeCapture::WriteAsync( void )
{
	s_Queue.Push_Back( *this );
}

bool CGazeCapture::ImportCGD( const std::string &sCGDPath, const std::string &sFile )
{
	std::vector<std::string> vecFiles = CUtility::GetFilesInDir( sCGDPath );
	const std::regex regFile( R"a(\d+_\d+m_([+-]?\d+)P_([+-]?\d+)V_([+-]?\d+)H.jpg)a" );
	std::smatch match;
	std::vector<CGazeCapture_Set::gazecapture> vecData;
	unsigned uImage = 0;
	for( const auto &sFile: vecFiles )
	{
		std::string str( CUtility::GetFileName( sFile ) );
		std::regex_match( str, match, regFile );
		if( !match.size( ) )
			continue;

		if( std::stod( match[ 1 ].str( ) ) != 0.0 )
			continue; //Ignore side view

		vecData.emplace_back( time( nullptr ), uImage++, 0, CVector<3>( {
			atan( std::stod( match[ 3 ].str( ) ) ) / 2.5,
			atan( std::stod( match[ 2 ].str( ) ) ) / 2.5,
			-0.5,
		} ), str );
	}
}

CGazeCapture::CGazeCapture( CBaseCamera &camera, const char *szWindow, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim ) :
	m_imgGaze( "Image_Gaze" ),
	m_vec3Point( { 0 } ),
	m_uImage( s_uCurrentImage++ )
{
	{
		unsigned int uWidth;
		unsigned int uHeight;
		CImage::GetScreenResolution( uWidth, uHeight );
		m_imgGaze.matImage = Mat( uHeight, uWidth, CV_8UC3, Scalar( 127, 0, 0 ) );
	}

	{
		CPoint ptGaze = CPoint( m_imgGaze, rand( ) / (double) RAND_MAX, rand( ) / (double) RAND_MAX, "Point_Gaze" );
		ptGaze.Draw( Scalar( 0, 190, 190 ), 5 );
		m_vec3Point = CVector<3>(
		{
			-( vec3ScreenTL[ 0 ] + ptGaze.GetRelPositionX( -1 ) * vec3ScreenDim[ 0 ] ),
			vec3ScreenTL[ 1 ] + ptGaze.GetRelPositionY( -1 ) * vec3ScreenDim[ 1 ],
			vec3ScreenTL[ 2 ]
		} );
	}

	m_imgGaze.Show( szWindow );
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

	camera.TakePicture( m_imgGaze );
}

std::string CGazeCapture::ToString( unsigned int uPrecision ) const
{
	std::ostringstream out;
	out.setf( std::ios_base::fixed, std::ios_base::floatfield );
	out.precision( uPrecision );

	//Write date
	{
		struct tm *timeinfo = localtime( &m_imgGaze.timestamp );
		char szDate[ 20 ];
		strftime( szDate, 20, "%F %T", timeinfo ); //YYYY-MM-DD HH:MM:SS
		out << szDate;
	}
	
	out << " " << m_uImage << " " << m_imgGaze.dFOV << " (" << m_vec3Point[ 0 ] << ", " << m_vec3Point[ 1 ] << ", " << m_vec3Point[ 2 ] << ")";
	return out.str( );
}

void CGazeCapture::WriteImage( void ) const
{
	imwrite( s_sDataPath + "img_" + std::to_string( m_uImage ) + ".jpg", m_imgGaze.matImage );
}

CGazeCapture::CGazeCapture( std::string sLine )
{
	std::smatch match;
	std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
	std::regex_match( sLine, match, s_regex_line );
	if( !match.size( ) )
		throw 0;

	m_vec3Point = CVector<3>( { std::stod( match[ 9 ].str( ) ), std::stod( match[ 10 ].str( ) ), std::stod( match[ 11 ].str( ) ) } );
	double dFOV = std::stod( match[ 8 ].str( ) );
	
	m_uImage = std::stoul( match[ 7 ].str( ) );
	
	struct tm timeinfo = { 0 };
	timeinfo.tm_sec = std::stoi( match[ 6 ].str( ) );
	timeinfo.tm_min = std::stoi( match[ 5 ].str( ) );
	timeinfo.tm_hour = std::stoi( match[ 4 ].str( ) );
	timeinfo.tm_mday = std::stoi( match[ 3 ].str( ) );
	timeinfo.tm_mon = std::stoi( match[ 2 ].str( ) ) - 1;
	timeinfo.tm_year = std::stoi( match[ 1 ].str( ) ) - 1900;
	
	//Load image
	std::string str = s_sDataPath + "img_" + std::to_string( m_uImage ) + ".jpg";
	
	cv::Mat matImage = imread( str.c_str( ), CV_LOAD_IMAGE_COLOR );
	if( matImage.empty( ) )
	{
		printf( "Test\n" );
		fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
		throw 1;
	}
	m_imgGaze = CImage( matImage, dFOV, mktime( &timeinfo ), "Image_Gaze" );
}

void *CGazeCapture::ReadThread( void *pArgs )
{
	std::string sLine;
	std::smatch match;
	while( std::getline( s_File, sLine ) )
	{
		try
		{
			s_Queue.Emplace_Back( sLine );
		}
		catch( int )
		{

		}
	}

	s_Queue.Emplace_Back( );
	return nullptr;
}

void *CGazeCapture::WriteThread( void *pArgs )
{
	CGazeCapture capture;
	while( true )
	{
		capture = s_Queue.Pop_Front( );
		if( capture.m_uImage == (unsigned int) -1 )
			break;

		s_File << capture.ToString( ) << std::endl;
		capture.WriteImage( );
	}
	
	return nullptr;
}