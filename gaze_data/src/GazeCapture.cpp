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

CGazeCapture_Set CGazeCapture::s_DataSetRead;
CGazeCapture_Set CGazeCapture::s_DataSetWrite;
CQueue<CGazeCapture> CGazeCapture::s_QueueRead( 100 );
CQueue<CGazeCapture> CGazeCapture::s_QueueWrite( 25 );
std::vector<std::thread> CGazeCapture::s_vecThreadRead;
std::vector<std::thread> CGazeCapture::s_vecThreadWrite;
unsigned CGazeCapture::s_uNextImage;

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
		
		if( !CUtility::CreateFolder( s_DataSetWrite.sDataPath ) )
			return false;
	
		if( !s_DataSetWrite.WriteHeader( sFile ) )
			return false;
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
			s_vecThreadWrite.emplace_back( WriteThread, nullptr );
			return true;
		case 27:	//Escape
			return false;
		}
	}
}

void CGazeCapture::CloseWrite( void )
{
	for( unsigned u = 0; u < s_vecThreadWrite.size( ); u++ )
		s_QueueWrite.Emplace_Back( );	//Signal threads to stop

	for( auto &thread : s_vecThreadWrite )
		thread.join( );

	s_DataSetWrite.CloseWrite( );
}

bool CGazeCapture::OpenRead( const std::string &sFile, bool fThreaded )
{
	if( !CUtility::Exists( sFile ) )
	{
		fprintf( stderr, "Failed to open file \"%s\": File not found\n", sFile.c_str( ) );
		CUtility::GetChar( );
		return false;
	}

	s_DataSetRead = CGazeCapture_Set::LoadList( sFile );
	s_DataSetRead.Sort( );
	CheckDuplicates( s_DataSetRead, sFile );
	if( s_DataSetRead.vecData.size( ) )
		s_uNextImage = s_DataSetRead.vecData.back( ).uImage + 1;
	else
		s_uNextImage = 0;

	CUtility::Cls( );
	printf( "Name        : %s\n", s_DataSetRead.sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_DataSetRead.dEyeDistance * 100 );
	printf( "Data path   : %s\n", s_DataSetRead.sDataPath.c_str( ) );
	printf( "Images      : %u\n", (unsigned) s_DataSetRead.vecData.size( ) );
	printf( "Next image  : %u\n", s_uNextImage );
	unsigned char cKey;
	while( true )
	{
		cKey = CUtility::GetChar( );
		switch( cKey )
		{
		case 141:	//Numpad enter
		case 10:	//Enter
			if( !fThreaded )
				return true;

			s_vecThreadRead.emplace_back( ReadThread, nullptr );
			return true;
		case 27:	//Escape
			return false;
		}
	}
}

bool CGazeCapture::ReadAsync( CGazeCapture &val )
{
	static unsigned uFinished = 0;
	for( ; uFinished < s_vecThreadRead.size( ); uFinished++ )
	{
		val = s_QueueRead.Pop_Front( );
		if( val.m_uImage != (unsigned int) -1 )
			return true;
	}

	//All threads finished, join
	for( auto &thread: s_vecThreadRead )
		thread.join( );

	s_vecThreadRead.clear( );
	uFinished = 0;
	return false;
}

bool CGazeCapture::Read( CGazeCapture &val )
{
	CGazeCapture_Set::gazecapture *pData;
	while( ( pData = s_DataSetRead.GetNext( ) ) )
	{
		try
		{
			val = CGazeCapture( s_DataSetRead, *pData );
			return true;
		}
		catch( int )
		{

		}
	}
	
	return false;
}

void CGazeCapture::WriteAsync( void )
{
	s_QueueWrite.Push_Back( *this );
}

bool CGazeCapture::ImportCGD( const std::string &sCGDPath, const std::string &sFile )
{
	std::vector<std::string> vecFiles = CUtility::GetFilesInDir( sCGDPath );
	const std::regex regFile( R"a(\d+_\d+m_([+-]?\d+)P_([+-]?\d+)V_([+-]?\d+)H)a" );
	std::smatch match;
	std::vector<CGazeCapture_Set::gazecapture> vecData;
	s_uNextImage = 0;
	
	if( !sFile.empty( ) )
	{
		s_DataSetRead = CGazeCapture_Set::LoadList( sFile );
		s_DataSetRead.Sort( );
		s_uNextImage = s_DataSetRead.vecData.back( ).uImage + 1;
	}
	
	unsigned uTotal = 0;
	for( const auto &sFile: vecFiles )
	{
		std::string str( CUtility::GetFileName( sFile ) );
		std::regex_match( str, match, regFile );
		if( !match.size( ) )
			continue;
		
		uTotal++;
		if( std::find_if( s_DataSetRead.vecData.begin( ), s_DataSetRead.vecData.end( ), [sFile]( const CGazeCapture_Set::gazecapture &capture )
			{
				return capture.sImage == sFile;
			} ) != s_DataSetRead.vecData.end( ) )
			continue;	//Already loaded

		vecData.emplace_back( time( nullptr ), s_uNextImage++, 0, CVector<3>( {
			atan( std::stod( match[ 3 ].str( ) ) ) / 2.5,
			atan( std::stod( match[ 2 ].str( ) ) ) / 2.5,
			-0.5,
		} ), sFile );
	}
	
	unsigned uProcessed = (unsigned) s_DataSetRead.vecData.size( );
	s_DataSetRead = CGazeCapture_Set(
		vecData,
		"Various",
		6.6,
		"/dev/null"
	);
	
	CUtility::Cls( );
	printf( "Dataset     : Columbian Gaze Dataset\n" );
	printf( "Data path   : %s\n", sCGDPath.c_str( ) );
	printf( "Total       : %u\n", uTotal );
	printf( "Processed   : %u\n", uProcessed );
	printf( "Images      : %u\n", (unsigned) s_DataSetRead.vecData.size( ) );
	printf( "Next image  : %u\n", s_uNextImage );
	unsigned char cKey;
	while( true )
	{
		cKey = CUtility::GetChar( );
		switch( cKey )
		{
		case 141:	//Numpad enter
		case 10:	//Enter
			for( unsigned u = 0; u < 4; u++ )
				s_vecThreadRead.emplace_back( ReadThread, nullptr );

			return true;
		case 27:	//Escape
			return false;
		}
	}
}

CGazeCapture::CGazeCapture( CBaseCamera &camera, const char *szWindow, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim ) :
	m_imgGaze( "Image_Gaze" ),
	m_vec3Point( { 0 } )
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

	m_uImage = s_uNextImage++;
	camera.TakePicture( m_imgGaze );
}

void CGazeCapture::WriteImage( const std::string &sPath ) const
{
	imwrite( sPath + "img_" + std::to_string( m_uImage ) + ".jpg", m_imgGaze.matImage );
}

CGazeCapture_Set::gazecapture CGazeCapture::ToData( void ) const
{
	return CGazeCapture_Set::gazecapture(
		m_imgGaze.timestamp,
		m_uImage,
		m_imgGaze.dFOV,
		m_vec3Point,
		sImagePath
	);
}

CGazeCapture::CGazeCapture( const CGazeCapture_Set &set, const CGazeCapture_Set::gazecapture &data ) :
	m_vec3Point( data.vec3Gaze ),
	m_uImage( data.uImage ),
	sImagePath( data.sImage )
{
	std::string str = data.sImage;
	if( str.empty( ) )
		str = set.sDataPath + "img_" + std::to_string( m_uImage ) + ".jpg";
#if 0
	else do
	{
		const std::regex regFile( R"a((\d+_\d+m_)([+-]?\d+)(P_[+-]?\d+V_[+-]?\d+H.jpg))a" );
		std::smatch match;
		std::regex_match( data.sImage, match, regFile );
		if( !match.size( ) )
			break;
		
		//This is an image from the Columbian gaze dataset, check for parent
		if( std::stoi( match[ 2 ].str( ) ) == 0 )
			break;	//This is a parent image
		
		std::string sParent = match[ 1 ].str( ) + "0" + match[ 3 ].str( );
		auto it = std::find_if( set.vecData.begin( ), set.vecData.end( ), [sParent]( const CGazeCapture_Set::gazecapture &other )
			{
				return other.sImage == sParent;
			} );
		
		if( it == set.vecData.end( ) )
		{
			printf( "Warning: parent image \"%s\" not found\n", sParent.c_str( ) );
			break;
		}
		
		m_uParent = it->uImage;
	} while( 0 );
#endif

	cv::Mat matImage = imread( str.c_str( ), CV_LOAD_IMAGE_COLOR );
	if( matImage.empty( ) )
	{
		fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
		throw 0;
	}
	m_imgGaze = CImage( matImage, data.dFOV, data.time, "Image_Gaze" );
}

void *CGazeCapture::ReadThread( void * )
{
	CGazeCapture_Set::gazecapture *pData;
	while( ( pData = s_DataSetRead.GetNext( ) ) )
	{
		try
		{
			s_QueueRead.Emplace_Back( s_DataSetRead, *pData );
		}
		catch( int )
		{

		}
	}

	s_QueueRead.Emplace_Back( );
	return nullptr;
}

void *CGazeCapture::WriteThread( void * )
{
	CGazeCapture data;
	while( true )
	{
		data = s_QueueWrite.Pop_Front( );
		if( data.m_uImage == (unsigned) -1 )
			break;

		s_DataSetWrite.Write( data.ToData( ) );
		data.WriteImage( s_DataSetWrite.sDataPath );
	}
	
	return nullptr;
}

void CGazeCapture::CheckDuplicates( CGazeCapture_Set &dataset, const std::string &sFile )
{
	unsigned int uDuplicates = dataset.CheckDuplicates( false );
	if( !uDuplicates )
		return;

	CUtility::Cls( );
	printf( "Found %u duplicates while loading file \"%s\". Do you wish to remove and update the file? (y/n)\n", uDuplicates, sFile.c_str( ) );
	while( true )
	{
		switch( CUtility::GetChar( ) )
		{
		case 'y':
			dataset.CheckDuplicates( true );
			dataset.WriteAll( sFile );
			return;
		case 'n':
			return;
		case 27:	//Escape
			throw 1;
		}
	}
}