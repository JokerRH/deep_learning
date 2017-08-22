#include "GazeData.h"
#include "Landmark.h"
#include "Image.h"
#include "Scenery.h"
#include "Render/RenderHelper.h"
#include "Render/Vector.h"
#include "Utility.h"
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <thread>
#include <algorithm>
#include <chrono>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#ifdef _MSC_VER
#	include <direct.h>
#endif

using namespace cv;

std::fstream CGazeData::s_File;
std::fstream CGazeData::s_FileWrite;
CQueue<CGazeData> CGazeData::s_Queue( 100 );
CQueue<CGazeData> CGazeData::s_QueueWrite( 100 );
CQueue<CGazeData> CGazeData::s_QueueRaw( 100 );
std::vector<pthread_t> CGazeData::s_vecThread;
std::vector<pthread_t> CGazeData::s_vecThreadWrite;
std::vector<pthread_t> CGazeData::s_vecThreadRaw;
std::vector<unsigned int> CGazeData::s_vecIgnore;

double CGazeData::s_dEyeDistance;
FILE *CGazeData::s_pFile;
std::string CGazeData::s_sName;
std::string CGazeData::s_sDataPath;
std::string CGazeData::s_sRawPath;
unsigned int CGazeData::s_uCurrentImage;

const std::regex CGazeData::s_regex_name( R"a(name=([\s\S]*).*)a" );
const std::regex CGazeData::s_regex_dist( R"a(dist=((?:\d+(?:\.\d+)?)|(?:\.\d+))cm.*)a" );
const std::regex CGazeData::s_regex_datapath( R"a(data=([\s\S]*).*)a" );
const std::regex CGazeData::s_regex_raw( R"a(raw=([\s\S]*).*)a" );
const std::regex CGazeData::s_regex_data( R"a(data:.*)a" );
const std::regex CGazeData::s_regex_line( R"a((\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\d+)\s+((?:\d+(?:\.\d+)?)|(?:\.\d+))\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)@\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)@\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\).*)a" );

bool CGazeData::OpenWrite( const std::string &sFile, bool fCreateDataFolder )
{
	if( fCreateDataFolder )
		s_sDataPath = CUtility::GetPath( sFile ) + CUtility::GetFileName( sFile ) + "/";

	s_uCurrentImage = 0;

	std::vector<std::string> vecLines;
	if( CUtility::Exists( sFile ) )
	{
		//File exists, check which images have been processed
		s_FileWrite.open( sFile, std::fstream::in );

		std::smatch match;
		unsigned char fFound = 0;
		std::string sLine;
		while( std::getline( s_FileWrite, sLine ) )
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

			std::regex_match( sLine, match, s_regex_datapath );
			if( match.size( ) )
			{
				s_sDataPath = match[ 1 ].str( );
				fFound |= 4;
				continue;
			}

			std::regex_match( sLine, match, s_regex_raw );
			if( match.size( ) )
			{
				s_sRawPath = match[ 1 ].str( );
				fFound |= 8;
				continue;
			}

			std::regex_match( sLine, match, s_regex_data );
			if( match.size( ) )
			{
				fFound |= 16;
				break;	//Start of data
			}
		}

		if( fFound != 31 )
		{
			fprintf( stderr, "File \"%s\" is missing fields\n", sFile.c_str( ) );
			s_FileWrite.close( );
			return false;
		}
		
		while( std::getline( s_FileWrite, sLine ) )
		{
			std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
			std::regex_match( sLine, match, s_regex_line );
			if( !match.size( ) )
				continue;

			unsigned int uCurrent = std::stoul( match[ 7 ].str( ) );
			s_vecIgnore.push_back( uCurrent );
			vecLines.push_back( sLine );
			s_uCurrentImage = std::max( (int) s_uCurrentImage, (int) uCurrent );
		}
		s_FileWrite.close( );
	}
	else
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

	s_FileWrite.open( sFile, std::fstream::out );
	s_FileWrite << "name=" << s_sName << "\n";
	s_FileWrite << "dist=" << std::to_string( s_dEyeDistance * 100 ) << "cm\n";
	s_FileWrite << "data=" << s_sDataPath << "\n";
	s_FileWrite << "raw=" << s_sRawPath << "\n";
	s_FileWrite << "\ndata:\n";
	for( auto line: vecLines )
		s_FileWrite << line << std::endl;

	CUtility::Cls( );
	printf( "Name        : %s\n", s_sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_dEyeDistance * 100 );
	printf( "Data path   : %s\n", s_sDataPath.c_str( ) );
	printf( "Raw path    : %s\n", s_sRawPath.c_str( ) );
	printf( "Last image  : %u\n", s_uCurrentImage );
	printf( "Processed   : %lu\n", s_vecIgnore.size( ) );
	unsigned char cKey;
	while( true )
	{
		cKey = CUtility::GetChar( );
		switch( cKey )
		{
		case 141:	//Numpad enter
		case 10:	//Enter
			s_vecThreadWrite.emplace_back( );
			pthread_create( &s_vecThreadWrite[ 0 ], nullptr, WriteThread, nullptr );
			return true;
		case 27:	//Escape
			return false;
		}
	}
}

void CGazeData::CloseWrite( void )
{
	for( std::vector<pthread_t>::iterator it = s_vecThreadWrite.begin( ); it < s_vecThreadWrite.end( ); it++ )
	{
		pthread_cancel( *it );
		pthread_join( *it, nullptr );
	}
	s_FileWrite.close( );
}

bool CGazeData::OpenRead( const std::string &sFile )
{
	if( !CUtility::Exists( sFile ) )
		return false;

	s_uCurrentImage = (unsigned int) -1;
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

			std::regex_match( sLine, match, s_regex_datapath );
			if( match.size( ) )
			{
				s_sDataPath = match[ 1 ].str( );
				fFound |= 4;
				continue;
			}

			std::regex_match( sLine, match, s_regex_raw );
			if( match.size( ) )
			{
				s_sRawPath = match[ 1 ].str( );
				fFound |= 8;
				continue;
			}

			std::regex_match( sLine, match, s_regex_data );
			if( match.size( ) )
			{
				fFound |= 16;
				break;	//Start of data
			}
		}

		if( fFound != 31 )
		{
			fprintf( stderr, "File \"%s\" is missing fields\n", sFile.c_str( ) );
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
	
	s_vecThread.emplace_back( );
	pthread_create( &s_vecThread[ 0 ], nullptr, ReadThread, nullptr );

	CUtility::Cls( );
	printf( "Name        : %s\n", s_sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_dEyeDistance * 100 );
	printf( "Data path   : %s\n", s_sDataPath.c_str( ) );
	printf( "Raw path    : %s\n", s_sRawPath.c_str( ) );
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

bool CGazeData::OpenReadRaw( const std::string &sFile )
{
	if( !CGazeCapture::OpenRead( sFile ) )
		return false;

	s_dEyeDistance = CGazeCapture::s_dEyeDistance;
	s_sName = CGazeCapture::s_sName;
	s_sRawPath = CGazeCapture::s_sDataPath;

	s_vecThreadRaw.emplace_back( );
	s_vecThreadRaw.emplace_back( );
	pthread_create( &s_vecThreadRaw[ 0 ], nullptr, ReadRawThread, nullptr );
	pthread_create( &s_vecThreadRaw[ 1 ], nullptr, ReadRawThread, nullptr );
	return true;
}

bool CGazeData::ReadAsync( CGazeData &val )
{
	do
	{
		val = s_Queue.Pop_Front( );
	} while( std::find( s_vecIgnore.begin( ), s_vecIgnore.end( ), val.m_uImage ) != s_vecIgnore.end( ) );
	if( val.m_uImage != (unsigned int) -1 )
		return true;

	for( std::vector<pthread_t>::iterator it = s_vecThread.begin( ); it < s_vecThread.end( ); it++ )
	{
		pthread_cancel( *it );
		pthread_join( *it, nullptr );
	}
	s_File.close( );
	return false;
}

void CGazeData::WriteAsync( void )
{
	s_QueueWrite.Push_Back( *this );
}

bool CGazeData::ReadRawAsync( CGazeData &val )
{
	val = s_QueueRaw.Pop_Front( );
	if( val.m_uImage != (unsigned int) -1 )
		return true;

	for( std::vector<pthread_t>::iterator it = s_vecThreadRaw.begin( ); it < s_vecThreadRaw.end( ); it++ )
	{
		pthread_cancel( *it );
		pthread_join( *it, nullptr );
	}
	s_File.close( );
	return false;
}

std::vector<CGazeData> CGazeData::GetGazeData( CGazeCapture &capture )
{
	std::vector<CGazeData> vecData;

	double dTanFOV = tan( capture.m_imgGaze.dFOV * M_PI / ( 2 * 180 ) );
	std::vector<CLandmark> vecLandmarks = CLandmark::GetLandmarks( capture.m_imgGaze );
	for( std::vector<CLandmark>::iterator it = vecLandmarks.begin( ); it < vecLandmarks.end( ); it++ )
		vecData.emplace_back( *it, capture.m_vec3Point, dTanFOV, capture.m_uImage );

	return vecData;
}

double CGazeData::GetDistance( double dMeterDif, double dPixelDif, double dPixelDiagonal, double dTanFOV )
{
	//Calculate distance from camera
	//tan( a ) = g1 / d <=> d = g1 / tan( a )
	//g1 / g2 = w / dif <=> g1 = w * g2 / dif
	//=> d = ( w * g2 ) / ( dif * tan( a ) ) = ( w / dif ) * ( g2 / tan( a ) )
	/*
	  +---
	  |   ---
	  |      ---
	  |         ---
	g1+-----       ---
	  |     ------    ---
	  g2          ------ ---
	  |                 ---a\
	  +-----------d----------+
	*/
	dMeterDif /= 2;
	return ( dPixelDiagonal / dPixelDif ) * ( dMeterDif / dTanFOV );
}

double CGazeData::GetPosition( double dDistance, double dPixelDif, double dPixelDiagonal, double dTanFOV )
{
	//tan( a ) = g1 / d <=> g1 = tan( a ) * d
	//g1 / g2 = w / dif <=> g2 = g1 * dif / w = tan( a ) * d * dif / w
	return dTanFOV * dDistance * dPixelDif / dPixelDiagonal;
}

CGazeData::CGazeData( CLandmark &landmark, const CVector<3> &vec3Point, double dTanFOV, unsigned int uImage ) :
	m_rayEyeLeft( CVector<3>( { 0 } ), CVector<3>( { 0 } ) ),
	m_rayEyeRight( CVector<3>( { 0 } ), CVector<3>( { 0 } ) ),
	m_imgGaze( *landmark.boxFace.GetImage( -1 ) ),
	m_boxFace( landmark.boxFace ),
	m_ptEyeLeft( landmark.ptEyeLeft ),
	m_ptEyeRight( landmark.ptEyeRight ),
	m_uImage( uImage )
{
	m_boxFace.TransferOwnership( m_imgGaze );
	m_ptEyeLeft.TransferOwnership( m_boxFace );
	m_ptEyeRight.TransferOwnership( m_boxFace );
	
	double dWidth = m_imgGaze.GetWidth( );
	double dHeight = m_imgGaze.GetHeight( );

	double dPixelDif;
	{
		CVector<2> vec2EyeLeft( { (double) ( m_ptEyeLeft.GetPositionX( -1 ) ), (double) ( m_ptEyeLeft.GetPositionY( -1 ) ) } );
		CVector<2> vec2EyeRight( { (double) ( m_ptEyeRight.GetPositionX( -1 ) ), (double) ( m_ptEyeRight.GetPositionY( -1 ) ) } );
		dPixelDif = ( vec2EyeRight - vec2EyeLeft ).Abs( );
	}
	double dPixelDiagonal = sqrt( dWidth * dWidth + dHeight * dHeight );
	double dDistance = GetDistance( s_dEyeDistance, dPixelDif, dPixelDiagonal, dTanFOV );

	CVector<3> vec3EyeLeft(
	{
		GetPosition( dDistance, ( 0.5 - m_ptEyeLeft.GetRelPositionX( -1 ) ) * dWidth, dPixelDiagonal, dTanFOV ),
		GetPosition( dDistance, ( 0.5 - m_ptEyeLeft.GetRelPositionY( -1 ) ) * dHeight, dPixelDiagonal, dTanFOV ),
		dDistance
	} );

	CVector<3> vec3EyeRight(
	{
		GetPosition( dDistance, ( 0.5 - m_ptEyeRight.GetRelPositionX( -1 ) ) * dWidth, dPixelDiagonal, dTanFOV ),
		GetPosition( dDistance, ( 0.5 - m_ptEyeRight.GetRelPositionY( -1 ) ) * dHeight, dPixelDiagonal, dTanFOV ),
		dDistance
	} );

	m_rayEyeLeft = CRay( vec3EyeLeft, vec3Point - vec3EyeLeft );
	m_rayEyeRight = CRay( vec3EyeRight, vec3Point - vec3EyeRight );
}

bool CGazeData::Adjust( const char *szWindow )
{
	while( true )
	{
		CImage imgDraw( m_imgGaze, "Image_Draw" );
		m_ptEyeLeft.Draw( imgDraw, Scalar( 0, 255, 255 ), 4 );
		m_ptEyeRight.Draw( imgDraw, Scalar( 0, 255, 255 ), 4 );
		
		//Write image number
		char szImage[ 8 ];
		sprintf( szImage, "%u", m_uImage );
		int iBaseline = 0;
		Size textSize = getTextSize( szImage, FONT_HERSHEY_SIMPLEX, 1, 3, &iBaseline );
		iBaseline += 3;
		Point ptText( imgDraw.matImage.cols - textSize.width - 5, textSize.height + 5 );
		putText( imgDraw.matImage, szImage, ptText,  FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 255, 255 ), 3 );
		
		imgDraw.Show( szWindow );
		unsigned char cKey;
		bool fContinue = true;
		while( fContinue )
		{
			cKey = CUtility::WaitKey( 0 );
			switch( cKey )
			{
			case 8:		//Backspace
				return false;
			case 141:	//Numpad enter
			case 10:	//Enter
				return true;
			case 27:	//Escape
				throw( 1 );
			case 'e':
				fContinue = false;
				break;
			}
		}

		//Adjust eye position
		CLandmark landmark( m_boxFace, m_ptEyeLeft, m_ptEyeRight );
		landmark.Adjust( szWindow );
		m_ptEyeLeft = landmark.ptEyeLeft;
		m_ptEyeRight = landmark.ptEyeRight;
		m_ptEyeLeft.TransferOwnership( m_boxFace );
		m_ptEyeRight.TransferOwnership( m_boxFace );
	}
}

bool CGazeData::DrawScenery( const char *szWindow )
{
	CScenery scenery( m_rayEyeLeft, m_rayEyeRight );
	CImage img( "Image_Scenery" );
	img.matImage = cv::Mat( 1050, 1050, CV_8UC3, cv::Scalar( 0, 0, 0 ) );

	//Write distance
	char szDistance[ 8 ];
	sprintf( szDistance, "%5.1fcm", m_rayEyeLeft.m_vec3Origin[ 2 ] * 100 );
	int iBaseline = 0;
	Size textSize = getTextSize( szDistance, FONT_HERSHEY_SIMPLEX, 1, 3, &iBaseline );
	iBaseline += 3;
	Point ptText( img.matImage.cols - textSize.width - 5, textSize.height + 5 );

	unsigned char cKey;
	bool fContinue = true;
	double dDegX = 211;
	double dDegY = 42;
	double dDegZ = 0;
	while( fContinue )
	{
		CImage imgDraw( img, "Image_Draw" );
		
		//printf( "Rot: (%f, %f, %f)\n", dDegX, dDegY, dDegZ );
		scenery.Transformed( CRenderHelper::GetRotationMatrix( dDegX, dDegY, dDegZ ) ).Fit( ).Draw( imgDraw );
		putText( imgDraw.matImage, szDistance, ptText,  FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 255, 255 ), 3 );
		imgDraw.Show( szWindow );

		cKey = CUtility::WaitKey( 0 );
		switch( cKey )
		{
		case 10:	//Enter
			fContinue = false;
			break;
		case 27:	//Escape
			return false;
		case 80:	//Pos1
			dDegX = 211;
			dDegY = 42;
			dDegZ = 0;
			break;
		case 81:	//Key_Left
			dDegY -= 1;
			if( dDegY < 0 )
				dDegY = 360;

			break;
		case 82:	//Key_Up
			dDegX -= 1;
			if( dDegX < 0 )
				dDegX = 360;

			break;
		case 83:	//Key_Right
			dDegY += 1;
			if( dDegY >= 360 )
				dDegY = 0;

			break;
		case 84:	//Key_Down
			dDegX += 1;
			if( dDegX >= 360 )
				dDegX = 0;

			break;
		case 85:	//Img_Up
			dDegZ -= 1;
			if( dDegZ < 0 )
				dDegZ = 360;

			break;
		case 86:	//Img_Down
			dDegZ += 1;
			if( dDegZ >= 360 )
				dDegZ = 0;

			break;
		case 176:	//Numpad_0
			dDegX = 90;
			dDegY = 0;
			dDegZ = 0;
			break;
		case 178:	//Numpad_2
			dDegX = 180;
			dDegY = 0;
			dDegZ = 0;
			break;
		case 180:	//Numpad_4
			dDegX = 180;
			dDegY = 90;
			dDegZ = 0;
			break;
		case 181:	//Numpad_5
			dDegX = 270;
			dDegY = 0;
			dDegZ = 0;
			break;
		case 182:	//Numpad_6
			dDegX = 180;
			dDegY = 270;
			dDegZ = 0;
			break;
		case 184:	//Numpad_8
			dDegX = 0;
			dDegY = 0;
			dDegZ = 180;
			break;
		}
	}

	return true;
}

void CGazeData::RandomizeFace( double dMaxScale )
{
	m_boxFace.Scale( CVector<2>( 
	{
		1 + ( rand( ) / (double) RAND_MAX ) * dMaxScale,
		1 + ( rand( ) / (double) RAND_MAX ) * dMaxScale
	} ) );

	m_boxFace.Shift( CVector<2>(
	{
		( rand( ) / (double) RAND_MAX ) * dMaxScale - dMaxScale / 2,
		( rand( ) / (double) RAND_MAX ) * dMaxScale - dMaxScale / 2
	} ) );
}

std::string CGazeData::ToString( unsigned int uPrecision ) const
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

	out << " " << m_uImage << " " << m_imgGaze.dFOV;
	CVector<2> vec2Amp = m_rayEyeLeft.AmplitudeRepresentation( );
	out << " (" << m_ptEyeLeft.GetRelPositionX( 0 ) << ", " << m_ptEyeLeft.GetRelPositionY( 0 ) << ")@(" << vec2Amp[ 0 ] << ", " << vec2Amp[ 1 ] << ")";
	vec2Amp = m_rayEyeRight.AmplitudeRepresentation( );
	out << " (" << m_ptEyeRight.GetRelPositionX( 0 ) << ", " << m_ptEyeRight.GetRelPositionY( 0 ) << ")@(" << vec2Amp[ 0 ] << ", " << vec2Amp[ 1 ] << ")";

	return out.str( );
}

void CGazeData::WriteImage( void ) const
{
	CImage img( m_imgGaze );
	CBBox boxFace( m_boxFace );
	img.Crop( boxFace );
	imwrite( s_sDataPath + "img_" + std::to_string( m_uImage ) + ".jpg", img.matImage );
}

CGazeData::CGazeData( std::string sLine )
{
	std::smatch match;
	std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
	std::regex_match( sLine, match, s_regex_line );
	if( !match.size( ) )
		throw 0;

	m_uImage = std::stoul( match[ 7 ].str( ) );
	if( std::find( s_vecIgnore.begin( ), s_vecIgnore.end( ), m_uImage ) != s_vecIgnore.end( ) )
		throw 2;	//Image on blacklist

	double dFOV = std::stod( match[ 8 ].str( ) );
	struct tm timeinfo = { 0 };
	timeinfo.tm_sec = std::stoi( match[ 6 ].str( ) );
	timeinfo.tm_min = std::stoi( match[ 5 ].str( ) );
	timeinfo.tm_hour = std::stoi( match[ 4 ].str( ) );
	timeinfo.tm_mday = std::stoi( match[ 3 ].str( ) );
	timeinfo.tm_mon = std::stoi( match[ 2 ].str( ) ) - 1;
	timeinfo.tm_year = std::stoi( match[ 1 ].str( ) ) - 1900;

	//Load raw image
	std::string str = s_sRawPath + "img_" + std::to_string( m_uImage ) + ".jpg";
	cv::Mat matImage = cv::imread( str, CV_LOAD_IMAGE_COLOR );
	if( !matImage.data )
	{
		fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
		throw 1;
	}
	m_imgGaze = CImage( matImage, dFOV, mktime( &timeinfo ), "Image_Gaze" );

	//Load face image
	str = s_sDataPath + "img_" + std::to_string( m_uImage ) + ".jpg";
	matImage = cv::imread( str, CV_LOAD_IMAGE_COLOR );
	if( !matImage.data )
	{
		fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
		throw 2;
	}
	CImage imgFace( matImage, dFOV, mktime( &timeinfo ), "Image_Face" );
	m_boxFace = FindTemplate( m_imgGaze, imgFace );

	CVector<2> vec2PYRight( { std::stod( match[ 15 ].str( ) ), std::stod( match[ 16 ].str( ) ) } );
	m_ptEyeRight = CPoint( m_boxFace, std::stod( match[ 13 ].str( ) ), std::stod( match[ 14 ].str( ) ), "Point_EyeLeft" );
	CVector<2> vec2PYLeft( { std::stod( match[ 11 ].str( ) ), std::stod( match[ 12 ].str( ) ) } );
	m_ptEyeLeft = CPoint( m_boxFace, std::stod( match[ 9 ].str( ) ), std::stod( match[ 10 ].str( ) ), "Point_EyeRight" );
	
	double dWidth = m_imgGaze.GetWidth( );
	double dHeight = m_imgGaze.GetHeight( );
	double dTanFOV = tan( m_imgGaze.dFOV * M_PI / ( 2 * 180 ) );

	double dPixelDif;
	{
		CVector<2> vec2EyeLeft( { (double) ( m_ptEyeLeft.GetPositionX( -1 ) ), (double) ( m_ptEyeLeft.GetPositionY( -1 ) ) } );
		CVector<2> vec2EyeRight( { (double) ( m_ptEyeRight.GetPositionX( -1 ) ), (double) ( m_ptEyeRight.GetPositionY( -1 ) ) } );
		dPixelDif = ( vec2EyeRight - vec2EyeLeft ).Abs( );
	}
	double dPixelDiagonal = sqrt( dWidth * dWidth + dHeight * dHeight );
	double dDistance = GetDistance( s_dEyeDistance, dPixelDif, dPixelDiagonal, dTanFOV );

	CVector<3> vec3EyeLeft(
	{
		GetPosition( dDistance, ( 0.5 - m_ptEyeLeft.GetRelPositionX( -1 ) ) * dWidth, dPixelDiagonal, dTanFOV ),
		GetPosition( dDistance, ( 0.5 - m_ptEyeLeft.GetRelPositionY( -1 ) ) * dHeight, dPixelDiagonal, dTanFOV ),
		dDistance
	} );

	CVector<3> vec3EyeRight(
	{
		GetPosition( dDistance, ( 0.5 - m_ptEyeRight.GetRelPositionX( -1 ) ) * dWidth, dPixelDiagonal, dTanFOV ),
		GetPosition( dDistance, ( 0.5 - m_ptEyeRight.GetRelPositionY( -1 ) ) * dHeight, dPixelDiagonal, dTanFOV ),
		dDistance
	} );

	m_rayEyeLeft = CRay( vec3EyeLeft, CVector<3>( { 0, 0, -1 } ), vec2PYLeft );
	m_rayEyeRight = CRay( vec3EyeRight, CVector<3>( { 0, 0, -1 } ), vec2PYRight );
	CVector<2> vec2Scale = m_rayEyeLeft.PointOfShortestDistance( m_rayEyeRight );
	m_rayEyeLeft *= vec2Scale[ 0 ];
	m_rayEyeRight *= vec2Scale[ 1 ];
}

void *CGazeData::ReadThread( void *pArgs )
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
	while( true )
		std::this_thread::sleep_for( std::chrono::seconds( 0xFFFFFFFF ) );
}

void *CGazeData::WriteThread( void *pArgs )
{
	CGazeData data;
	while( true )
	{
		data = s_QueueWrite.Pop_Front( );
		s_FileWrite << data.ToString( ) << std::endl;
		data.WriteImage( );
	}
}

void *CGazeData::ReadRawThread( void *pArgs )
{
	CGazeCapture capture;
	while( CGazeCapture::ReadAsync( capture ) )
	{
		if( std::find( s_vecIgnore.begin( ), s_vecIgnore.end( ), capture.m_uImage ) != s_vecIgnore.end( ) )
			continue;	//Image on blacklist

		double dTanFOV = tan( capture.m_imgGaze.dFOV * M_PI / ( 2 * 180 ) );
		std::vector<CLandmark> vecLandmarks = CLandmark::GetLandmarks( capture.m_imgGaze );
		for( std::vector<CLandmark>::iterator it = vecLandmarks.begin( ); it < vecLandmarks.end( ); it++ )
			s_QueueRaw.Emplace_Back( *it, capture.m_vec3Point, dTanFOV, capture.m_uImage );
	}

	s_QueueRaw.Emplace_Back( );
	while( true )
		std::this_thread::sleep_for( std::chrono::seconds( 0xFFFFFFFF ) );
}

CBBox CGazeData::FindTemplate( CImage &imgSrc, const CImage &imgTemplate )
{
	//Create the result matrix
	int iResultCols =  imgSrc.matImage.cols - imgTemplate.matImage.cols + 1;
	int iResultRows = imgSrc.matImage.rows - imgTemplate.matImage.rows + 1;
	cv::Mat matResult( iResultCols, iResultRows, CV_32FC1 );

	//Do the Matching and Normalize
	cv::matchTemplate( imgSrc.matImage, imgTemplate.matImage, matResult, CV_TM_SQDIFF );
	cv::normalize( matResult, matResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat( ) );

	//Localizing the best match with minMaxLoc
	double dMinVal;
	double dMaxVal;
	cv::Point ptMinLoc;
	cv::Point ptMaxLoc;
	cv::minMaxLoc( matResult, &dMinVal, &dMaxVal, &ptMinLoc, &ptMaxLoc, cv::Mat( ) );	
	
	return CBBox( imgSrc, cv::Rect( ptMinLoc, cv::Point( ptMinLoc.x + imgTemplate.matImage.cols , ptMinLoc.y + imgTemplate.matImage.rows ) ), -1, "Box_Face" );	//CV_TM_SQDIFF and CV_TM_SQDIFF_NORMED
	//return CBBox( imgSrc, cv::Rect( ptMinLoc, cv::Point( ptMaxLoc.x + imgTemplate.matImage.cols , ptMaxLoc.y + imgTemplate.matImage.rows ) ), -1, "Box_Face" );
}