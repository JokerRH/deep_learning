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
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

double CGazeData::s_dEyeDistance;
FILE *CGazeData::s_pFile;
std::string CGazeData::s_sName;
std::string CGazeData::s_sDataPath;
std::string CGazeData::s_sRawPath;
unsigned int CGazeData::s_uCurrentImage;

const std::regex CGazeData::s_regex_name( R"a(name=([\s\S]*))a" );
const std::regex CGazeData::s_regex_dist( R"a(dist=((?:\d+(?:\.\d+)?)|(?:\.\d+))cm)a" );
const std::regex CGazeData::s_regex_raw( R"a(raw=([\s\S]*))a" );
const std::regex CGazeData::s_regex_data( R"a(data:)a" );
const std::regex CGazeData::s_regex_line( R"a((\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\d+)\s+((?:\d+(?:\.\d+)?)|(?:\.\d+))\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)@\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)@\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\))a" );

bool CGazeData::Init( const char *szFile )
{
	if( !OpenOrCreate( std::string( szFile ) ) )
		return false;

	srand( (unsigned int) time( nullptr ) );
	return true;
}

void CGazeData::Destroy( void )
{
	fclose( s_pFile );
}

bool CGazeData::OpenOrCreate( const std::string &sFile )
{
	s_sDataPath = CUtility::GetPath( sFile ) + CUtility::GetFileName( sFile ) + "/";
	s_uCurrentImage = 0;
	if( 1 || !CUtility::Exists( sFile ) )
	{
		s_dEyeDistance = CGazeCapture::s_dEyeDistance;
		s_sName = CGazeCapture::s_sName;
		s_sRawPath = CGazeCapture::s_sDataPath;

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
		fputs( "cm\nraw=", s_pFile );
		fputs( s_sRawPath.c_str( ), s_pFile );
		fputs( "\n\ndata:\n", s_pFile );
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
			
			std::regex_match( sLine, match, s_regex_raw );
			if( match.size( ) )
			{
				s_sRawPath = match[ 1 ].str( );
				fFound |= 4;
				continue;
			}
			
			std::regex_match( sLine, match, s_regex_data );
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
	printf( "Raw path    : %s\n", s_sRawPath.c_str( ) );
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

std::vector<CGazeData> CGazeData::Load( const std::string &sFile )
{
	std::vector<CGazeData> vecCaptures;
	if( !CUtility::Exists( sFile ) )
		return vecCaptures;

	s_sDataPath = CUtility::GetPath( sFile ) + CUtility::GetFileName( sFile ) + "/";
	s_uCurrentImage = 0;
	
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

		std::regex_match( sLine, match, s_regex_raw );
		if( match.size( ) )
		{
			s_sRawPath = match[ 1 ].str( );
			fFound |= 4;
			continue;
		}

		std::regex_match( sLine, match, s_regex_data );
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

	while( std::getline( file, sLine ) )
	{
		Load( vecCaptures, sLine );
	}
	s_uCurrentImage++;

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
			return vecCaptures;
		case 27:	//Escape
			throw( 1 );
		}
	}

	return vecCaptures;
}

std::vector<CGazeData> CGazeData::GetGazeData( std::vector<CGazeCapture> vecGaze, const char *szWindow )
{
	std::vector<CGazeData> vecData;

	for( std::vector<CGazeCapture>::iterator pGaze = vecGaze.begin( ); pGaze < vecGaze.end( ); pGaze++ )
	{
		double dTanFOV = tan( pGaze->imgGaze.dFOV * M_PI / ( 2 * 180 ) );
		std::vector<CLandmark> vecLandmarks = CLandmark::GetLandmarks( pGaze->imgGaze, szWindow );
		for( std::vector<CLandmark>::iterator it = vecLandmarks.begin( ); it < vecLandmarks.end( ); it++ )
			vecData.emplace_back( *it, pGaze->vec3Point, dTanFOV );
	}

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

CGazeData::CGazeData( CLandmark &landmark, const CVector<3> &vec3Point, double dTanFOV ) :
	m_rayEyeLeft( CVector<3>( { 0 } ), CVector<3>( { 0 } ) ),
	m_rayEyeRight( CVector<3>( { 0 } ), CVector<3>( { 0 } ) ),
	m_imgGaze( *landmark.boxFace.GetImage( -1 ) ),
	m_boxFace( landmark.boxFace ),
	m_ptEyeLeft( landmark.ptEyeLeft ),
	m_ptEyeRight( landmark.ptEyeRight )
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

CGazeData::CGazeData( const CImage &img, const CBBox &boxFace, const CPoint &ptEyeLeft, const CPoint &ptEyeRight, const CVector<2> &vec2PYLeft, const CVector<2> &vec2PYRight, double dTanFOV ) :
	m_rayEyeLeft( CVector<3>( { 0 } ), CVector<3>( { 0 } ) ),
	m_rayEyeRight( CVector<3>( { 0 } ), CVector<3>( { 0 } ) ),
	m_imgGaze( img ),
	m_boxFace( boxFace ),
	m_ptEyeLeft( ptEyeLeft ),
	m_ptEyeRight( ptEyeRight )
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

	m_rayEyeLeft = CRay( vec3EyeLeft, CVector<3>( { 0, 0, -1 } ), vec2PYLeft );
	m_rayEyeRight = CRay( vec3EyeRight, CVector<3>( { 0, 0, -1 } ), vec2PYRight );
	CVector<2> vec2Scale = m_rayEyeLeft.PointOfShortestDistance( m_rayEyeRight );
	m_rayEyeLeft *= vec2Scale[ 0 ];
	m_rayEyeRight *= vec2Scale[ 1 ];
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

bool CGazeData::Write( void )
{
	CBBox boxRand( m_boxFace );
	boxRand.Scale( CVector<2>( 
	{
		1 + ( rand( ) / (double) RAND_MAX ) * 0.1,
		1 + ( rand( ) / (double) RAND_MAX ) * 0.1
	} ) );
	boxRand.Shift( CVector<2>(
	{
		( rand( ) / (double) RAND_MAX ) * 0.1 - 0.05,
		( rand( ) / (double) RAND_MAX ) * 0.1 - 0.05
	} ) );
	
	CPoint ptEyeLeft( m_ptEyeLeft );
	CPoint ptEyeRight( m_ptEyeRight );
	ptEyeLeft.TransferOwnership( boxRand );
	ptEyeRight.TransferOwnership( boxRand );

	struct tm *timeinfo = localtime( &m_imgGaze.timestamp );
	char szDate[ 20 ];
	strftime( szDate, 20, "%F %T", timeinfo ); //YYYY-MM-DD HH:MM:SS
	fputs( szDate, s_pFile );
	fprintf( s_pFile, " %u %f", s_uCurrentImage, m_imgGaze.dFOV );
	CVector<2> vec2Amp = m_rayEyeLeft.AmplitudeRepresentation( );
	fprintf( s_pFile, " (%f, %f)@(%f, %f)", ptEyeLeft.GetRelPositionX( 0 ), ptEyeLeft.GetRelPositionY( 0 ), vec2Amp[ 0 ], vec2Amp[ 1 ] );;
	vec2Amp = m_rayEyeRight.AmplitudeRepresentation( );
	fprintf( s_pFile, " (%f, %f)@(%f, %f)\n", ptEyeRight.GetRelPositionX( 0 ), ptEyeRight.GetRelPositionY( 0 ), vec2Amp[ 0 ], vec2Amp[ 1 ] );;

	CImage img( m_imgGaze, "Image_Face" );
	img.Crop( boxRand );
	imwrite( s_sDataPath + "img_" + std::to_string( s_uCurrentImage++ ) + ".jpg", img.matImage );
	return true;
}

bool CGazeData::Load( std::vector<CGazeData> &vecData, const std::string &sLine )
{
	std::smatch match;
	std::regex_match( sLine, match, s_regex_line );
	if( !match.size( ) )
		return false;

	unsigned int uCurrent = std::stoul( match[ 7 ].str( ) );
	s_uCurrentImage = std::max( s_uCurrentImage, uCurrent );

	double dFOV = std::stod( match[ 8 ].str( ) );
	struct tm timeinfo = { 0 };
	timeinfo.tm_sec = std::stoi( match[ 6 ].str( ) );
	timeinfo.tm_min = std::stoi( match[ 5 ].str( ) );
	timeinfo.tm_hour = std::stoi( match[ 4 ].str( ) );
	timeinfo.tm_mday = std::stoi( match[ 3 ].str( ) );
	timeinfo.tm_mon = std::stoi( match[ 2 ].str( ) ) - 1;
	timeinfo.tm_year = std::stoi( match[ 1 ].str( ) ) - 1900;

	//Load raw image
	std::string str = s_sRawPath + "img_" + std::to_string( uCurrent ) + ".jpg";
	cv::Mat matImage = imread( str, CV_LOAD_IMAGE_COLOR );
	if( !matImage.data )
	{
		fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
		return false;
	}
	CImage img( matImage, dFOV, mktime( &timeinfo ), "Image_Gaze" );

	//Load face image
	str = s_sDataPath + "img_" + std::to_string( uCurrent ) + ".jpg";
	matImage = imread( str, CV_LOAD_IMAGE_COLOR );
	if( !matImage.data )
	{
		fprintf( stderr, "Warning: Could not open or find the image \"%s\"\n", str.c_str( ) );
		return false;
	}
	CImage imgFace( matImage, dFOV, mktime( &timeinfo ), "Image_Face" );
	CBBox boxFace = FindTemplate( img, imgFace );

	CVector<2> vec2PYRight( { std::stod( match[ 15 ].str( ) ), std::stod( match[ 16 ].str( ) ) } );
	CPoint ptEyeRight( boxFace, std::stod( match[ 13 ].str( ) ), std::stod( match[ 14 ].str( ) ), "Point_EyeLeft" );
	CVector<2> vec2PYLeft( { std::stod( match[ 11 ].str( ) ), std::stod( match[ 12 ].str( ) ) } );
	CPoint ptEyeLeft( boxFace, std::stod( match[ 9 ].str( ) ), std::stod( match[ 10 ].str( ) ), "Point_EyeRight" );

	vecData.emplace_back( img, boxFace, ptEyeLeft, ptEyeRight, vec2PYLeft, vec2PYRight, tan( dFOV * M_PI / ( 2 * 180 ) ) );
	return true;
}

CBBox CGazeData::FindTemplate( CImage &imgSrc, const CImage &imgTemplate )
{
	//Create the result matrix
	int iResultCols =  imgSrc.matImage.cols - imgTemplate.matImage.cols + 1;
	int iResultRows = imgSrc.matImage.rows - imgTemplate.matImage.rows + 1;
	cv::Mat matResult( iResultCols, iResultRows, CV_32FC1 );

	//Do the Matching and Normalize
	cv::matchTemplate( imgSrc.matImage, imgTemplate.matImage, matResult, CV_TM_SQDIFF );
	cv::normalize( matResult, matResult, 0, 1, NORM_MINMAX, -1, cv::Mat( ) );

	//Localizing the best match with minMaxLoc
	double dMinVal;
	double dMaxVal;
	cv::Point ptMinLoc;
	cv::Point ptMaxLoc;
	cv::minMaxLoc( matResult, &dMinVal, &dMaxVal, &ptMinLoc, &ptMaxLoc, cv::Mat( ) );	
	
	return CBBox( imgSrc, cv::Rect( ptMinLoc, cv::Point( ptMinLoc.x + imgTemplate.matImage.cols , ptMinLoc.y + imgTemplate.matImage.rows ) ), -1, "Box_Face" );	//CV_TM_SQDIFF and CV_TM_SQDIFF_NORMED
	//return CBBox( imgSrc, cv::Rect( ptMinLoc, cv::Point( ptMaxLoc.x + imgTemplate.matImage.cols , ptMaxLoc.y + imgTemplate.matImage.rows ) ), -1, "Box_Face" );
}