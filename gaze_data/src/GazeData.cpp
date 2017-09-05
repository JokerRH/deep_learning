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

#define GAZEDATA_READ_THREADS 4

CGazeData_Set CGazeData::s_DataSetRead;
CGazeData_Set CGazeData::s_DataSetWrite;
CQueue<CGazeData> CGazeData::s_QueueRead( 100 );
CQueue<CGazeData> CGazeData::s_QueueWrite( 100 );
std::vector<pthread_t> CGazeData::s_vecThreadRead;
std::vector<pthread_t> CGazeData::s_vecThreadWrite;
unsigned CGazeData::s_uNextImage;

bool CGazeData::OpenWrite( const std::string &sFile, bool fCreateDataFolder )
{
	if( CUtility::Exists( sFile ) )
		s_DataSetWrite = CGazeData_Set::LoadList( sFile );
	else
	{
		s_DataSetWrite = CGazeData_Set(
			std::vector<CGazeData_Set::gazedata>( ),
			s_DataSetRead.sName,
			s_DataSetRead.dEyeDistance,
			CUtility::GetPath( sFile ) + CUtility::GetFileName( sFile ) + "/",
			s_DataSetRead.sRawPath
		);

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
	printf( "Raw path    : %s\n", s_DataSetWrite.sRawPath.c_str( ) );
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
}

void CGazeData::CloseWrite( void )
{
	for( unsigned u = 0; u < s_vecThreadWrite.size( ); u++ )
		s_QueueWrite.Emplace_Back( );	//Signal threads to stop

	for( auto &thread: s_vecThreadWrite )
		pthread_join( thread, nullptr );

	s_DataSetWrite.CloseWrite( );
}

bool CGazeData::OpenRead( const std::string &sFile )
{
	if( !CUtility::Exists( sFile ) )
	{
		fprintf( stderr, "Failed to open file \"%s\": File not found\n", sFile.c_str( ) );
		CUtility::GetChar( );
		return false;
	}
		
	s_DataSetRead = CGazeData_Set::LoadList( sFile );
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
	printf( "Raw path    : %s\n", s_DataSetRead.sRawPath.c_str( ) );
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
			s_vecThreadRead.emplace_back( );
			pthread_create( &s_vecThreadRead[ 0 ], nullptr, ReadThread, nullptr );
			return true;
		case 27:	//Escape
			return false;
		}
	}
}

bool CGazeData::OpenReadRaw( const std::string &sFileRaw, const std::string &sFile )
{
	if( !CGazeCapture::OpenRead( sFileRaw ) )
		return false;

	s_uNextImage = 0;
	if( !sFile.empty( ) && CUtility::Exists( sFile ) )
	{
		s_DataSetRead = CGazeData_Set::LoadList( sFile );
		s_DataSetRead.Sort( );
		CheckDuplicates( s_DataSetRead, sFile );
		if( s_DataSetRead.vecData.size( ) )
			s_uNextImage = s_DataSetRead.vecData.back( ).uImage + 1;
	}
	else
	{
		s_DataSetRead = CGazeData_Set(
			std::vector<CGazeData_Set::gazedata>( ),
			CGazeCapture::s_DataSetRead.sName,
			CGazeCapture::s_DataSetRead.dEyeDistance,
			"/dev/null",
			CGazeCapture::s_DataSetRead.sDataPath
		);
	}

	CUtility::Cls( );
	printf( "Name        : %s\n", s_DataSetRead.sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_DataSetRead.dEyeDistance * 100 );
	printf( "Data path   : %s\n", s_DataSetRead.sDataPath.c_str( ) );
	printf( "Raw path    : %s\n", s_DataSetRead.sRawPath.c_str( ) );
	//printf( "Proc path   : %s\n", CGazeCapture::s_sDataPath.c_str( ) );
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
			for( unsigned u = 0; u < 2; u++ )
			{
				s_vecThreadRead.emplace_back( );
				pthread_create( &s_vecThreadRead.back( ), nullptr, ReadRawThread, nullptr );
			}
			return true;
		case 27:	//Escape
			return false;
		}
	}

	return true;
}

bool CGazeData::ReadAsync( CGazeData &val )
{
	static unsigned uFinished = 0;
	for( ; uFinished < s_vecThreadRead.size( ); uFinished++ )
	{
		val = s_QueueRead.Pop_Front( );
		if( val.m_uImage != (unsigned int) -1 )
			return true;
	}

	//All threads finished, join
	for( auto &thread : s_vecThreadRead )
		pthread_join( thread, nullptr );

	s_vecThreadRead.clear( );
	uFinished = 0;
	return false;
}

void CGazeData::WriteAsync( void )
{
	s_QueueWrite.Push_Back( *this );
}

bool CGazeData::Export( const std::string &sFile, const std::string &sPath, unsigned int uTestBatchSize, double dTrainValRatio )
{
	const std::string sTrainPath = sPath + "/train/";
	const std::string sValPath = sPath + "/val/";

	CUtility::CreateFolder( sPath );
	CUtility::CreateFolder( sTrainPath );
	CUtility::CreateFolder( sValPath );
	
	s_DataSetRead = CGazeData_Set::LoadList( sFile );
	s_DataSetRead.Sort( );
	CheckDuplicates( s_DataSetRead, sFile );
	if( s_DataSetRead.vecData.size( ) )
		s_uNextImage = s_DataSetRead.vecData.back( ).uImage + 1;
	else
		s_uNextImage = 0;
		
	{
		double dBatchCount = s_DataSetRead.vecData.size( ) * ( 1 - dTrainValRatio ) / uTestBatchSize;
		dBatchCount = round( dBatchCount );
		dTrainValRatio = 1 - dBatchCount * uTestBatchSize / s_DataSetRead.vecData.size( );
	}

	CUtility::Cls( );
	printf( "Name        : %s\n", s_DataSetRead.sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_DataSetRead.dEyeDistance * 100 );
	printf( "Data path   : %s\n", s_DataSetRead.sDataPath.c_str( ) );
	printf( "Raw path    : %s\n", s_DataSetRead.sRawPath.c_str( ) );
	printf( "Images      : %u\n", (unsigned) s_DataSetRead.vecData.size( ) );
	printf( "Next image  : %u\n", s_uNextImage );
	printf( "Validation  : %u\n", (unsigned) ( s_DataSetRead.vecData.size( ) * dTrainValRatio ) );
	printf( "Training    : %u\n", (unsigned) ( s_DataSetRead.vecData.size( ) * ( 1 - dTrainValRatio ) ) );

	unsigned char cKey = 255;
	while( cKey )
	{
		cKey = CUtility::GetChar( );
		switch( cKey )
		{
		case 141:	//Numpad enter
		case 10:	//Enter
			cKey = 0;
			break;
		case 27:	//Escape
			return true;
		}
	}
	
	s_DataSetRead.Shuffle( );
	CGazeData_Set setTrain(
		std::vector<CGazeData_Set::gazedata>( s_DataSetRead.vecData.begin( ), s_DataSetRead.vecData.begin( ) + (int) ( s_DataSetRead.vecData.size( ) * dTrainValRatio ) ),
		s_DataSetRead.sName,
		s_DataSetRead.dEyeDistance,
		s_DataSetRead.sDataPath,
		s_DataSetRead.sRawPath
	);
	
	CGazeData_Set setVal(
		std::vector<CGazeData_Set::gazedata>( s_DataSetRead.vecData.begin( ) + (int) ( s_DataSetRead.vecData.size( ) * dTrainValRatio ), s_DataSetRead.vecData.end( ) ),
		s_DataSetRead.sName,
		s_DataSetRead.dEyeDistance,
		s_DataSetRead.sDataPath,
		s_DataSetRead.sRawPath
	);
	
	CUtility::Cls( );
	CGazeData val;
	double dTotal;
	double dCurrent;

	//Process training data
	printf( "Writing training data to %s\n", sTrainPath.c_str( ) );
	s_DataSetRead = setTrain;
	for( unsigned uThreads = 0; uThreads < GAZEDATA_READ_THREADS; uThreads++ )
	{
		s_vecThreadRead.emplace_back( );
		pthread_create( &s_vecThreadRead.back( ), nullptr, ReadThread, nullptr );
	}
	dTotal = (double) s_DataSetRead.vecData.size( );
	dCurrent = 0;
	while( ReadAsync( val ) )
	{
		//val.RandomizeFace( 0.1 );
		val.WriteImage( sTrainPath );
		
		//Write %
		dCurrent++;
		printf( "\r%3.0f%%", dCurrent / dTotal * 100 );
		fflush( stdout );
	}
	putchar( '\n' );
	for( auto &thread: s_vecThreadRead )
		pthread_join( thread, nullptr );
	s_vecThreadRead.clear( );
	if( !setTrain.Export( sTrainPath ) )
		return false;
	
	//Process training data
	printf( "Writing validation data to %s\n", sValPath.c_str( ) );
	s_DataSetRead = setVal;
	for( unsigned uThreads = 0; uThreads < GAZEDATA_READ_THREADS; uThreads++ )
	{
		s_vecThreadRead.emplace_back( );
		pthread_create( &s_vecThreadRead.back( ), nullptr, ReadThread, nullptr );
	}
	dTotal = (double) s_DataSetRead.vecData.size( );
	dCurrent = 0;
	while( ReadAsync( val ) )
	{
		//val.RandomizeFace( 0.1 );
		val.WriteImage( sValPath );
		
		//Write %
		dCurrent++;
		printf( "\r%3.0f%%", dCurrent / dTotal * 100 );
		fflush( stdout );
	}
	putchar( '\n' );
	for( auto &thread : s_vecThreadRead )
		pthread_join( thread, nullptr );
	s_vecThreadRead.clear( );
	if( !setVal.Export( sValPath ) )
		return false;
	
	printf( "Done.\n" );
	CUtility::GetChar( );
	return true;
}

bool CGazeData::Import( const std::string &sPath, const std::string &sRaw )
{
	std::fstream fileData( sPath + "/data.txt", std::fstream::in );
	std::fstream fileLabel( sPath + "/labels.csv" );
	
	std::string sLineData;
	std::string sLineLabel;
	
	const std::regex regData( R"a(img_(\d+)\.jpg\s+\d+.*)a" );
	const std::regex regLabel( R"a(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)).*)a" );
	
	std::vector<CGazeData_Set::gazedata> vecData;
	while( true )
	{
		if( !std::getline( fileData, sLineData ) )
		{
			if( std::getline( fileLabel, sLineLabel ) )
				printf( "Warning: Data file shorter than label file\n" );

			break;
		}
		if( !std::getline( fileLabel, sLineLabel ) )
		{
			printf( "Warning: Label file shorter than data file\n" );
			break;
		}
		
		std::smatch matchData;
		std::replace( sLineData.begin( ), sLineData.end( ), '\r', ' ' );
		std::regex_match( sLineData, matchData, regData );
		if( !matchData.size( ) )
		{
			printf( "Data match failed: \"%s\"\n", sLineData.c_str( ) );
			continue;
		}
			
		std::smatch matchLabel;
		std::replace( sLineLabel.begin( ), sLineLabel.end( ), '\r', ' ' );
		std::regex_match( sLineLabel, matchLabel, regLabel );
		if( !matchLabel.size( ) )
		{
			printf( "Label match failed: \"%s\"\n", sLineLabel.c_str( ) );
			continue;
		}
		
		vecData.emplace_back(
			time( nullptr ),
			std::stoul( matchData[ 1 ].str( ) ),
			60,
			CVector<2>( { std::stod( matchLabel[ 1 ].str( ) ), std::stod( matchLabel[ 2 ].str( ) ) } ),
			CVector<2>( { std::stod( matchLabel[ 3 ].str( ) ), std::stod( matchLabel[ 4 ].str( ) ) } ),
			CVector<2>( { std::stod( matchLabel[ 5 ].str( ) ), std::stod( matchLabel[ 6 ].str( ) ) } ),
			CVector<2>( { std::stod( matchLabel[ 7 ].str( ) ), std::stod( matchLabel[ 8 ].str( ) ) } )
		);
	}
	
	s_DataSetRead = CGazeData_Set( vecData, "Import", 0.066, sPath, sRaw );

	CUtility::Cls( );
	printf( "Name        : %s\n", s_DataSetRead.sName.c_str( ) );
	printf( "Eye distance: %4.2fcm\n", s_DataSetRead.dEyeDistance * 100 );
	printf( "Data path   : %s\n", s_DataSetRead.sDataPath.c_str( ) );
	printf( "Raw path    : %s\n", s_DataSetRead.sRawPath.c_str( ) );
	printf( "Images      : %u\n", (unsigned) s_DataSetRead.vecData.size( ) );
	unsigned char cKey;
	while( true )
	{
		cKey = CUtility::GetChar( );
		switch( cKey )
		{
		case 141:	//Numpad enter
		case 10:	//Enter
			s_vecThreadRead.emplace_back( );
			pthread_create( &s_vecThreadRead[ 0 ], nullptr, ReadThread, nullptr );
			return true;
		case 27:	//Escape
			return false;
		}
	}
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

double CGazeData::GetTanFOV( double dMeterDif, double dPixelDif, double dPixelDiagonal, double dDistance )
{
	dMeterDif /= 2;
	return ( dPixelDiagonal / dPixelDif ) * ( dMeterDif / dDistance );
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
	double dDistance;
	if( dTanFOV != 0.0 )
		dDistance = GetDistance( s_DataSetRead.dEyeDistance, dPixelDif, dPixelDiagonal, dTanFOV );
	else
	{
		dDistance = 2.0;	//Columbian Gaze Dataset
		dTanFOV = GetTanFOV( s_DataSetRead.dEyeDistance, dPixelDif, dPixelDiagonal, dDistance );
	}

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
	
	CImage imgFace( m_imgGaze );
	imgFace.Crop( m_boxFace );
	cv::resize( imgFace.matImage, imgFace.matImage, cv::Size( 525, 525 ) );
	cv::circle( imgFace.matImage, cv::Point( m_ptEyeLeft.GetRelPositionX( 0 ) * imgFace.matImage.cols, m_ptEyeLeft.GetRelPositionY( 0 ) * imgFace.matImage.rows ), 1, cv::Scalar( 0, 255, 0 ), -1 );
	cv::circle( imgFace.matImage, cv::Point( m_ptEyeRight.GetRelPositionX( 0 ) * imgFace.matImage.cols, m_ptEyeRight.GetRelPositionY( 0 ) * imgFace.matImage.rows ), 1, cv::Scalar( 0, 255, 0 ), -1 );

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
		CImage( imgDraw, imgFace ).Show( szWindow );

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

void CGazeData::WriteImage( const std::string &sPath ) const
{
	CImage img( m_imgGaze );
	CBBox boxFace( m_boxFace );
	img.Crop( boxFace );
	imwrite( sPath + "img_" + std::to_string( m_uImage ) + ".jpg", img.matImage );
}

CGazeData_Set::gazedata CGazeData::ToData( void ) const
{
	return CGazeData_Set::gazedata(
		m_imgGaze.timestamp,
		m_uImage,
		m_imgGaze.dFOV,
		CVector<2>( { m_ptEyeLeft.GetRelPositionX( 0 ), m_ptEyeLeft.GetRelPositionY( 0 ) } ),
		m_rayEyeLeft.AmplitudeRepresentation( ),
		CVector<2>( { m_ptEyeRight.GetRelPositionX( 0 ), m_ptEyeRight.GetRelPositionY( 0 ) } ),
		m_rayEyeRight.AmplitudeRepresentation( )
	);
}

CGazeData::CGazeData( const CGazeData_Set &set, const CGazeData_Set::gazedata &data ) :
	m_uImage( data.uImage )
{
	//Load raw image
	std::string str = set.sRawPath + "img_" + std::to_string( data.uImage ) + ".jpg";
	cv::Mat matImage = cv::imread( str, CV_LOAD_IMAGE_COLOR );
	if( !matImage.data )
	{
		fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
		throw 0;
	}
	m_imgGaze = CImage( matImage, data.dFOV, data.time, "Image_Gaze" );

	//Load face image
	str = set.sDataPath + "img_" + std::to_string( data.uImage ) + ".jpg";
	matImage = cv::imread( str, CV_LOAD_IMAGE_COLOR );
	if( !matImage.data )
	{
		fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
		throw 1;
	}
	CImage imgFace( matImage, data.dFOV, data.time, "Image_Face" );
	m_boxFace = FindTemplate( m_imgGaze, imgFace );

	m_ptEyeRight = CPoint( m_boxFace, data.vec2EyeRight, "Point_EyeLeft" );
	m_ptEyeLeft = CPoint( m_boxFace, data.vec2EyeLeft, "Point_EyeRight" );
	
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
	double dDistance = GetDistance( s_DataSetRead.dEyeDistance, dPixelDif, dPixelDiagonal, dTanFOV );

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

	m_rayEyeLeft = CRay( vec3EyeLeft, CVector<3>( { 0, 0, -1 } ), data.vec2PYLeft );
	m_rayEyeRight = CRay( vec3EyeRight, CVector<3>( { 0, 0, -1 } ), data.vec2PYRight );
	CVector<2> vec2Scale = m_rayEyeLeft.PointOfShortestDistance( m_rayEyeRight );
	m_rayEyeLeft *= vec2Scale[ 0 ];
	m_rayEyeRight *= vec2Scale[ 1 ];
}

void *CGazeData::ReadThread( void * )
{
	CGazeData_Set::gazedata *pData;
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

void *CGazeData::WriteThread( void * )
{
	CGazeData data;
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

void *CGazeData::ReadRawThread( void * )
{
	CGazeCapture capture;
	while( CGazeCapture::Read( capture ) )
	{
		if( std::find_if( s_DataSetRead.vecData.begin( ), s_DataSetRead.vecData.end( ), [capture]( const CGazeData_Set::gazedata &data )
			{
				return ( data.uImage == capture.m_uImage );
			} ) != s_DataSetRead.vecData.end( ) )
			continue;	//Image already exists

		double dTanFOV = tan( capture.m_imgGaze.dFOV * M_PI / ( 2 * 180 ) );
		std::vector<CLandmark> vecLandmarks = CLandmark::GetLandmarks( capture.m_imgGaze );
		for( std::vector<CLandmark>::iterator it = vecLandmarks.begin( ); it < vecLandmarks.end( ); it++ )
			s_QueueRead.Emplace_Back( *it, capture.m_vec3Point, dTanFOV, capture.m_uImage );
	}

	s_QueueRead.Emplace_Back( );
	return nullptr;
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

void CGazeData::CheckDuplicates( CGazeData_Set &dataset, const std::string &sFile )
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

#undef GAZEDATA_READ_THREADS