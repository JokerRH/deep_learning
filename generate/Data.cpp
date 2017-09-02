#include "Data.h"
#include "Scenery.h"
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
#undef max
#undef min

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
	s_sPathWrite = GetPath( sFile );

	if( !CreateDirectory( s_sPathWrite.c_str( ), nullptr ) && GetLastError( ) != ERROR_ALREADY_EXISTS )
	{
		std::wcerr << "Unable to create directory \"" << s_sPathWrite << "\"" << std::endl;
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

std::vector<CData> CData::LoadData( const std::wstring &sFile, unsigned uCount )
{
	std::vector<CData> vecData;
	std::wfstream smFile( sFile, std::wfstream::in );
	if( !smFile.is_open( ) )
	{
		std::wcerr << "Unable to open file \"" << sFile << "\" for reading" << std::endl;
		return vecData;
	}

	std::wstring sLine;
	while( std::getline( smFile, sLine ) )
	{
		if( !uCount-- )
			break;

		try
		{
			vecData.emplace_back( sLine, std::wstring( ) );
		}
		catch( int )
		{

		}
	}

	smFile.close( );
	return vecData;
}

void CData::FindFilesRecursively( const std::wstring &sDir, const std::wstring &sPattern, std::vector<std::wstring> &vecsFiles )
{
	const WCHAR *lpFolder = sDir.c_str( );
	const WCHAR *lpFilePattern = sPattern.c_str( );
	WCHAR szFullPattern[ MAX_PATH ];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;

	//Process subdirectories
	PathCchCombine( szFullPattern, MAX_PATH, lpFolder, L"*" );
	hFindFile = FindFirstFile( szFullPattern, &FindFileData );
	if( hFindFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && FindFileData.cFileName[ 0 ] != '.' )
			{
				//Found subdirectory, process
				PathCchCombine( szFullPattern, MAX_PATH, lpFolder, FindFileData.cFileName );
				FindFilesRecursively( szFullPattern, lpFilePattern, vecsFiles );
			}
		} while( FindNextFile( hFindFile, &FindFileData ) );
		FindClose( hFindFile );
	}

	// Now we are going to look for the matching files
	PathCchCombine( szFullPattern, MAX_PATH, lpFolder, lpFilePattern );
	hFindFile = FindFirstFile( szFullPattern, &FindFileData );
	if( hFindFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
			{
				//Found file
				PathCchCombine( szFullPattern, MAX_PATH, lpFolder, FindFileData.cFileName );
				vecsFiles.push_back( szFullPattern );
			}
		} while( FindNextFile( hFindFile, &FindFileData ) );
		FindClose( hFindFile );
	}
}

std::wstring CData::GetPath( const std::wstring &sFile )
{
	WCHAR *szPath = (WCHAR*) _alloca( ( sFile.size( ) + 1 ) * sizeof( WCHAR ) );
	memcpy( (void *) szPath, sFile.c_str( ), ( sFile.size( ) + 1 ) * sizeof( WCHAR ) );
	PathCchRemoveFileSpec( szPath, ( sFile.size( ) + 1 ) * sizeof( WCHAR ) );
	return std::wstring( szPath );
}

static cv::Size GetScreenResolution( void )
{
	RECT desktop;
	GetWindowRect( GetDesktopWindow( ), &desktop );
	return cv::Size( desktop.right, desktop.bottom );
}

cv::Rect CData::ShowImage( const std::string &sWindow, const cv::Mat &matImage )
{
	cv::Mat mat( GetScreenResolution( ), CV_8UC3, cv::Scalar::all( 255 ) );

	double dScale = mat.cols / (double) matImage.cols;
	if( matImage.rows * dScale > (double) mat.rows )
		dScale = mat.rows / (double) matImage.rows;

	cv::Size size( (int) ( matImage.cols * dScale ), (int) ( matImage.rows * dScale ) );
	cv::Rect rect( ( mat.cols - size.width ) / 2, ( mat.rows - size.height ) / 2, size.width, size.height );
	cv::resize( matImage, mat( rect ), size );

	cv::imshow( sWindow, mat );
	return rect;
}

CData::CData( const std::wstring &sLine, const std::wstring &sPath )
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

	if( sPath.empty( ) )
		return;

	//Load image
	LoadImage( sPath );
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

void CData::Show( const std::string & sWindow )
{
	cv::Mat matScreen( GetScreenResolution( ), CV_8UC3, cv::Scalar::all( 255 ) );
	cv::Rect rectTotal;
	{
		cv::Size size( matImage.cols * 3, std::max( matImage.cols * 2, matImage.rows ) );
		double dScale = std::min( (double) matScreen.cols / size.width, (double) matScreen.rows / size.height );
		size = cv::Size( (int) ( size.width * dScale ), (int) ( size.height * dScale ) );
		rectTotal = cv::Rect( (int) ( matScreen.cols / 2.0 - size.width / 2.0 ), (int) ( matScreen.rows / 2.0 - size.height / 2.0 ), size.width, size.height );
	}
	cv::Rect rectScenery( rectTotal.x, rectTotal.y, (int) ( rectTotal.width * 2 / 3.0 ), (int) ( rectTotal.width * 2 / 3.0 ) );
	cv::Rect rectImage( rectTotal.x + rectScenery.width, rectTotal.y, rectTotal.width - rectScenery.width, (int) ( (double) ( rectTotal.width - rectScenery.width ) / matImage.cols * matImage.rows ) );
	
	//Draw face
	{
		cv::Mat matImage = this->matImage.clone( );
		double dScaleX = (double) rectImage.width / matImage.cols;
		double dScaleY = (double) rectImage.height / matImage.rows;
		cv::resize( matImage, matImage, rectImage.size( ) );
		cv::Rect rectFace( (int) ( rectFace.x * dScaleX ), (int) ( rectFace.y * dScaleY ), (int) ( rectFace.width * dScaleX ), (int) ( rectFace.height * dScaleY ) );
		cv::Point ptEyeLeft( (int) ( ptEyeLeft.x * dScaleX ), (int) ( ptEyeLeft.y * dScaleY ) );
		cv::Point ptEyeRight( (int) ( ptEyeRight.x * dScaleX ), (int) ( ptEyeRight.y * dScaleY ) );
		cv::rectangle( matImage, rectFace, cv::Scalar( 0, 0, 255 ), 2 );
		cv::circle( matImage, ptEyeLeft + rectFace.tl( ), 1, cv::Scalar( 0, 255, 0 ), -1 );
		cv::circle( matImage, ptEyeRight + rectFace.tl( ), 1, cv::Scalar( 255, 0, 0 ), -1 );
		matImage.copyTo( matScreen( rectImage ) );
	}

	CScenery scenery( *this );
	scenery.Fit( ).Draw( matScreen( rectScenery ) );
	cv::imshow( sWindow, matScreen );

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
			case 13:	//Enter
				return;
			case 27:
				throw 27;
			default:
				std::wcout << "Key: " << uKey << std::endl;
			}
			break;
		}
		}

		if( fReturn > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
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

bool CData::GetEyePos( const cv::Mat &matFace, const std::string &sWindow )
{
	ptEyeLeft = cv::Point( 0, 0 );
	ptEyeRight = cv::Point( 0, 0 );
	cv::Mat matDraw = matFace.clone( );
	cv::circle( matDraw, ptEyeLeft, 4, cv::Scalar( 0, 255, 0 ), -1 );
	cv::circle( matDraw, ptEyeRight, 4, cv::Scalar( 255, 0, 0 ), -1 );
	cv::Rect rect = ShowImage( sWindow, matDraw );

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
				rect = ShowImage( sWindow, matDraw );
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
	cv::Rect rect = ShowImage( sWindow, matDraw );
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
			rect = ShowImage( sWindow, matDraw );
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
			rect = ShowImage( sWindow, matDraw );
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

bool CData::LoadImage( const std::wstring &sImage, const std::string &sWindow )
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
		cv::resize( matImage, matComp, cv::Size( 500, (int) ( 500.0 / matImage.cols * matImage.rows ) ) );
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