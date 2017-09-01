#include "Data.h"
#include <iostream>
#include <wtypes.h>
#include <Windows.h>
#include <Windowsx.h>
#include <Shlwapi.h>
#include <sstream>
#include <regex>
#include <Pathcch.h>
#include <malloc.h>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#undef LoadImage

const std::wregex CData::s_regLine( LR"a((?:"((?:[^"]|"")*)"\s+|(\S+)\s+)\(\s*([+]?\d+)\s*,\s*([+]?\d+)\)[xX]\(\s*([+]?\d+)\s*,\s*([+]?\d+)\)\s+\(\s*([+]?\d+)\s*,\s*([+]?\d+)\)@\(\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\)\s+\(\s*([+]?\d+)\s*,\s*([+]?\d+)\)@\(\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\)\s+->\(\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\).*)a" );
cv::CascadeClassifier CData::s_FaceCascade;
std::wstring CData::s_sPathWrite;
std::wfstream CData::s_smFileWrite;
CQueue<CData> CData::s_QueueWrite( 10 );
std::vector<pthread_t> CData::s_vecThreadWrite;

#define HAARCASCADES_PATH R"(D:\Users\Rainer\Documents\Visual Studio 2015\Opencv-3.2.0\build\etc\haarcascades\)"
bool CData::Init( void )
{
	if( !s_FaceCascade.load( HAARCASCADES_PATH "haarcascade_frontalface_default.xml" ) )
	{
		std::wcerr << "Unable to load face cascade" << std::endl;
		return false;
	}

	return true;
}

bool CData::OpenWrite( const std::wstring &sFile, unsigned uNumThreads )
{
	WCHAR *szPath = (WCHAR*) _alloca( ( sFile.size( ) + 1) * sizeof( WCHAR ) );
	memcpy( (void *) szPath, sFile.c_str( ), ( sFile.size( ) + 1 ) * sizeof( WCHAR ) );
	PathCchRemoveFileSpec( szPath, ( sFile.size( ) + 1 ) * sizeof( WCHAR ) );
	s_sPathWrite = std::wstring( szPath );

	if( !CreateDirectory( szPath, nullptr ) && GetLastError( ) != ERROR_ALREADY_EXISTS )
	{
		std::wcerr << "Unable to create directory \"" << szPath << "\"" << std::endl;
		return false;
	}

	s_smFileWrite.open( sFile, std::wfstream::app );
	if( !s_smFileWrite.is_open( ) )
	{
		std::wcerr << "Unable to open file \"" << sFile << "\" for writing" << std::endl;
		return false;
	}

	for( unsigned u = 0; u < uNumThreads; u++ )
	{
		s_vecThreadWrite.emplace_back( );
		pthread_create( &s_vecThreadWrite.back( ), nullptr, WriteThread, nullptr );
	}
	return true;
}

void CData::CloseWrite( void )
{
	for( unsigned u = 0; u < s_vecThreadWrite.size( ); u++ )
		s_QueueWrite.Emplace_Back( );

	for( pthread_t &thread : s_vecThreadWrite )
		pthread_join( thread, nullptr );

	s_smFileWrite.close( );
}

CData::CData( const std::wstring &sLine, const std::wstring &sPath, bool fLoadImage )
{
	std::wsmatch match;
	std::regex_match( sLine, match, s_regLine );
	if( !match.size( ) )
	{
		std::wcerr << "Unable to parse line \"" << sLine << "\"" << std::endl;
		throw 0;
	}

	if( match[ 1 ].matched )
		sImage = std::regex_replace( match[ 1 ].str( ), std::wregex( L"\"\"" ), L"\"" );
	else
		sImage = match[ 2 ].str( );

	rectFace = cv::Rect( std::stoi( match[ 3 ].str( ) ), std::stoi( match[ 4 ].str( ) ), std::stoi( match[ 5 ].str( ) ), std::stoi( match[ 6 ].str( ) ) );
	ptEyeLeft = cv::Point( std::stoi( match[ 7 ].str( ) ), std::stoi( match[ 8 ].str( ) ) );
	vec3EyeLeft = CVector<3>( { std::stod( match[ 9 ].str( ) ), std::stod( match[ 10 ].str( ) ), std::stod( match[ 11 ].str( ) ) } );
	ptEyeRight = cv::Point( std::stoi( match[ 12 ].str( ) ), std::stoi( match[ 13 ].str( ) ) );
	vec3EyeRight = CVector<3>( { std::stod( match[ 14 ].str( ) ), std::stod( match[ 15 ].str( ) ), std::stod( match[ 16 ].str( ) ) } );
	vec3GazePoint = CVector<3>( { std::stod( match[ 17 ].str( ) ), std::stod( match[ 18 ].str( ) ), std::stod( match[ 19 ].str( ) ) } );

	if( !fLoadImage )
		return;

	//Load image
	LoadImage( sPath );
}

std::wstring CData::ToString( unsigned int uPrecision ) const
{
	std::wostringstream out;
	out.setf( std::ios_base::fixed, std::ios_base::floatfield );
	out.precision( uPrecision );

	out << "\"" << std::regex_replace( sImage, std::wregex( L"\"" ), L"\"\"" ) << "\"";
	out << " (" << rectFace.x << ", " << rectFace.y << ")X(" << rectFace.width << ", " << rectFace.height << ")";
	out << " (" << ptEyeLeft.x << ", " << ptEyeLeft.y << ")@(" << vec3EyeLeft[ 0 ] << ", " << vec3EyeLeft[ 1 ] << ", " << vec3EyeLeft[ 2 ] << ")";
	out << " (" << ptEyeRight.x << ", " << ptEyeRight.y << ")@(" << vec3EyeRight[ 0 ] << ", " << vec3EyeRight[ 1 ] << ", " << vec3EyeRight[ 2 ] << ")";
	out << " ->(" << vec3GazePoint[ 0 ] << ", " << vec3GazePoint[ 1 ] << ", " << vec3GazePoint[ 2 ] << ")";
	return out.str( );
}

void CData::WriteAsync( void )
{
	s_QueueWrite.Push_Back( *this );
}

bool CData::LoadImage( const std::wstring &sPath )
{
	WCHAR szFullPattern[ MAX_PATH ];
	PathCchCombine( szFullPattern, MAX_PATH, sPath.c_str( ), sImage.c_str( ) );
	std::wstring sImage( szFullPattern );

	matImage = cv::imread( std::string( sImage.begin( ), sImage.end( ) ) );
	if( matImage.empty( ) )
	{
		std::wcerr << "Unable to read image \"" << sImage << "\"" << std::endl;
		return false;
	}

	return true;
}

void *CData::WriteThread( void * )
{
	WCHAR szFullPattern[ MAX_PATH ];
	while( true )
	{
		CData data = s_QueueWrite.Pop_Front( );
		if( data.matImage.empty( ) )
			break;

		PathCchCombine( szFullPattern, MAX_PATH, s_sPathWrite.c_str( ), data.sImage.c_str( ) );
		if( !cv::imwrite( std::string( szFullPattern, szFullPattern + wcslen( szFullPattern ) ), data.matImage ) )
		{
			std::cerr << "Failed to write image to \"" << szFullPattern << "\"" << std::endl;
			continue;
		}
		s_smFileWrite << data.ToString( ) << std::endl;
	}

	return nullptr;
}

static void GetScreenResolution( unsigned int &uWidth, unsigned int &uHeight )
{
	RECT desktop;
	GetWindowRect( GetDesktopWindow( ), &desktop );
	uWidth = desktop.right;
	uHeight = desktop.bottom;
}

static cv::Rect Show( const std::string &sWindow, const cv::Mat &matImage )
{
	unsigned int uWidth;
	unsigned int uHeight;
	GetScreenResolution( uWidth, uHeight );
	cv::Mat mat( uHeight, uWidth, CV_8UC3, cv::Scalar::all( 255 ) );

	double dScale = uWidth / (double) matImage.cols;
	if( matImage.rows * dScale > (double) uHeight )
		dScale = uHeight / (double) matImage.rows;

	cv::Size size( (int) ( matImage.cols * dScale ), (int) ( matImage.rows * dScale ) );
	cv::Rect rect( ( uWidth - size.width ) / 2, ( uHeight - size.height ) / 2, size.width, size.height );
	cv::resize( matImage, mat( rect ), size );

	cv::imshow( sWindow, mat );
	return rect;
}

bool CData::GetEyePos( const cv::Mat &matFace, const std::string &sWindow )
{
	ptEyeLeft = cv::Point( 0, 0 );
	ptEyeRight = cv::Point( 0, 0 );
	cv::Mat matDraw = matFace.clone( );
	cv::circle( matDraw, ptEyeLeft, 4, cv::Scalar( 0, 255, 0 ), -1 );
	cv::circle( matDraw, ptEyeRight, 4, cv::Scalar( 255, 0, 0 ), -1 );
	cv::Rect rect = Show( sWindow, matDraw );

	MSG msg;
	BOOL fReturn;
	while( true )
	{
		if( ( fReturn = GetMessage( &msg, NULL, 0, 0 ) ) == -1 )
			throw 27;	//Error

		switch( msg.message )
		{
		case WM_QUIT:
			PostQuitMessage( 0 );
			throw 27;
		case WM_KEYDOWN:
			{
				unsigned uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_CHAR );
				switch( uKey )
				{
				case 8:		//Backspace
					throw 0;
				case 13:	//Enter
					return true;
				case 27:
					throw 27;
				case 69:	//'e'
					return false;
				default:
					std::wcout << "Key: " << uKey << std::endl;
				}
				break;
			}
		case WM_LBUTTONDOWN:
			{
				int iX = GET_X_LPARAM( msg.lParam );
				int iY = GET_Y_LPARAM( msg.lParam );
				iX -= rect.x;
				iY -= rect.y;
				if( iX < 0 || iY < 0 || iX > rect.width || iY > rect.height )
					break;

				cv::Point pt( (int) ( (double) iX / rect.width * rectFace.width ), (int) ( (double) iY / rect.height * rectFace.height ) );
				if( (double) iX / rect.width > 0.5 )
					ptEyeLeft = pt;
				else
					ptEyeRight = pt;

				matDraw = matFace.clone( );
				cv::circle( matDraw, ptEyeLeft, 4, cv::Scalar( 0, 255, 0 ), -1 );
				cv::circle( matDraw, ptEyeRight, 4, cv::Scalar( 255, 0, 0 ), -1 );
				rect = Show( sWindow, matDraw );
				break;
			}
		case WM_RBUTTONDOWN:
			return true;
		}

		if( fReturn > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
}

bool CData::GetFaceRect( const std::string &sWindow )
{
	cv::Mat matImage;
	cv::resize( this->matImage, matImage, cv::Size( 720, (int) ( 720.0 / this->matImage.cols * this->matImage.rows ) ) ); //Resize image to improve performance

	cv::Mat matDraw = matImage.clone( );
	cv::rectangle( matDraw, rectFace, cv::Scalar( 0, 0, 255 ), 3 );
	cv::Rect rect = Show( sWindow, matDraw );
	bool fOrigin = true;

	MSG msg;
	BOOL fReturn;
	while( true )
	{
		if( ( fReturn = GetMessage( &msg, NULL, 0, 0 ) ) == -1 )
			throw 27;	//Error

		switch( msg.message )
		{
		case WM_QUIT:
			PostQuitMessage( 0 );
			throw 27;
		case WM_KEYDOWN:
		{
			unsigned uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_CHAR );
			switch( uKey )
			{
			case 8:		//Backspace
				throw 0;
			case 13:	//Enter
				return true;
			case 27:
				throw 27;
			default:
				std::wcout << "Key: " << uKey << std::endl;
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			if( fOrigin )
				break;

			int iX = GET_X_LPARAM( msg.lParam );
			int iY = GET_Y_LPARAM( msg.lParam );
			iX -= rect.x;
			iY -= rect.y;
			if( iX < 0 || iY < 0 || iX > rect.width || iY > rect.height )
				break;

			cv::Point ptClick( (int) ( (double) iX / rect.width * matImage.cols ), (int) ( (double) iY / rect.height * matImage.rows ) );
			cv::Rect rectFace = cv::Rect( this->rectFace.tl( ), ptClick );	//Do not override class face while moving

			matDraw = matImage.clone( );
			cv::rectangle( matDraw, rectFace, cv::Scalar( 0, 255, 255 ), 1 );
			cv::Rect rect = Show( sWindow, matDraw );
			rect = Show( sWindow, matDraw );
			break;
		}
		case WM_LBUTTONDOWN:
		{
			int iX = GET_X_LPARAM( msg.lParam );
			int iY = GET_Y_LPARAM( msg.lParam );
			iX -= rect.x;
			iY -= rect.y;
			if( iX < 0 || iY < 0 || iX > rect.width || iY > rect.height )
				break;

			cv::Point ptClick( (int) ( (double) iX / rect.width * matImage.cols ), (int) ( (double) iY / rect.height * matImage.rows ) );
			if( fOrigin )
				rectFace = cv::Rect( ptClick, ptClick );
			else
				rectFace = cv::Rect( rectFace.tl( ), ptClick );

			fOrigin = !fOrigin;

			matDraw = matImage.clone( );
			cv::rectangle( matDraw, rectFace, cv::Scalar( 0, 0, 255 ), 1 );
			cv::Rect rect = Show( sWindow, matDraw );
			rect = Show( sWindow, matDraw );
			break;
		}
		case WM_RBUTTONDOWN:
			if( !fOrigin )
				break;

			rectFace = cv::Rect(
				(int) ( (double) rectFace.x / matImage.cols * this->matImage.cols ),
				(int) ( (double) rectFace.y / matImage.rows * this->matImage.rows ),
				(int) ( (double) rectFace.width / matImage.cols * this->matImage.cols ),
				(int) ( (double) rectFace.height / matImage.rows * this->matImage.rows ) );

			return true;
		}

		if( fReturn > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
}

bool CData::LoadImage( const std::wstring &sImage, const std::string sWindow )
{
	this->sImage = std::wstring( PathFindFileName( sImage.c_str( ) ) );
	matImage = cv::imread( std::string( sImage.begin( ), sImage.end( ) ) );
	if( matImage.empty( ) )
	{
		std::wcerr << "Unable to read image \"" << sImage << "\"" << std::endl;
		return false;
	}

	//Detect face
	{
		cv::Mat matComp;
		cv::resize( matImage, matComp, cv::Size( 500, 500 ) );
		std::vector<cv::Rect> vecFaces;
		s_FaceCascade.detectMultiScale( matComp, vecFaces, 1.1, 5, CV_HAAR_SCALE_IMAGE, cv::Size( 30, 30 ) );
		if( !vecFaces.size( ) )
			GetFaceRect( sWindow );
		else
			rectFace = cv::Rect(
				(int) ( (double) vecFaces.front( ).x / matComp.cols * matImage.cols ),
				(int) ( (double) vecFaces.front( ).y / matComp.rows * matImage.rows ),
				(int) ( (double) vecFaces.front( ).width / matComp.cols * matImage.cols ),
				(int) ( (double) vecFaces.front( ).height / matComp.rows * matImage.rows ) );
	}

	while( !GetEyePos( matImage( rectFace ), sWindow ) )
		GetFaceRect( sWindow );

	return true;
}