#include "Camera.h"
#include "Webcam.h"
#include "../Compat.h"
#include <iostream>
#include <opencv2/core.hpp>

#ifdef _MSC_VER
#include "Canon.h"
#include <conio.h>
#endif

void CCamera::WaitForLiveView( void )
{
	MSG msg;
	BOOL fReturn;
	cv::Mat matImage;
	while( true )
	{
		if( ( fReturn = GetMessage( &msg, NULL, 0, 0 ) ) == -1 )
			throw 27;	//Error

		switch( msg.message )
		{
		case WM_QUIT:
			std::wcout << "Quit message" << std::endl;
			PostQuitMessage( 0 );
			throw 27;
		case WM_KEYDOWN:
			{
				unsigned uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_CHAR );
				switch( uKey )
				{
				case 27:
					throw 27;
				}
			}
			break;
		case CAMERA_LIVEVIEW_READY:
			return;
		}

		if( fReturn > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
}

bool CCamera::Init( void )
{
#ifdef WITH_EDSDK
	return CCanon::Init( );
#else
	return true;
#endif
}

bool CCamera::ThreadInit( void )
{
#ifdef WITH_EDSDK
	return CCanon::ThreadInit( );
#else
	return true;
#endif
}

void CCamera::Terminate( void )
{
#ifdef WITH_EDSDK
	CCanon::Terminate( );
#endif
}

void CCamera::ThreadTerminate( void )
{
#ifdef WITH_EDSDK
	CCanon::ThreadTerminate( );
#endif
}

CCamera * CCamera::SelectCamera( void )
{
	unsigned int uSelected = 0;
	std::vector<std::string> vecCamerasW = CWebcam::GetCameraList( );
	std::vector<std::string> vecCameras( vecCamerasW );
#ifdef WITH_EDSDK
	std::vector<std::string> vecCamerasC = CCanon::GetCameraList( );
	vecCameras.insert( vecCameras.end( ), vecCamerasC.begin( ), vecCamerasC.end( ) );
#endif

	unsigned uCount = (unsigned) vecCameras.size( );
	if( !uCount )
	{
		Cls( );
		std::wcout << "No camera available" << std::endl;
		return nullptr;
	}

	while( uCount )
	{
		Cls( );
		printf( "Please select a camera:\n" );
		for( unsigned int u = 0; u < uCount; u++ )
		{
			if( uSelected == u )
				printf( "[%u] %s\n", u, vecCameras[ u ].c_str( ) );
			else
				printf( " %u  %s\n", u, vecCameras[ u ].c_str( ) );
		}

		int iKey = _getch( );
		switch( iKey )
		{
		case 0:
		case 224:
			iKey = _getch( );
			switch( iKey )
			{
			case 72:	//Key_Up
				if( uSelected )
					uSelected--;

				break;
			case 80:	//Key_Down
				if( uSelected < uCount - 1 )
					uSelected++;

				break;
				//default:
				//printf( "Advanced key: %d\n", iKey );
				//_getch( );
			}
			break;
		case 141:	//Numpad enter
		case 13:	//Enter
			uCount = 0;
			break;
		case 27:	//Escape
			throw 27;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if( (unsigned) ( iKey - '0' ) >= uCount )
				break;

			uSelected = iKey - '0';
			uCount = 0;
			break;
			//default:
			//printf( "Key: %d\n", iKey );
			//_getch( );
		}
	}

	if( uSelected < vecCamerasW.size( ) )
		return new CWebcam( uSelected );

	uSelected -= (unsigned) vecCamerasW.size( );
#ifdef WITH_EDSDK
	if( uSelected < vecCamerasC.size( ) )
		return new CCanon( uSelected );
#endif

	std::wcerr << "Camera index out of bounds!" << std::endl;
	return nullptr;
}

void CCamera::Cls( void )
{
#ifdef _MSC_VER
	COORD topLeft = { 0, 0 };
	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo( hConsole, &screen );
	FillConsoleOutputCharacterA( hConsole, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
	FillConsoleOutputAttribute( hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
	SetConsoleCursorPosition( hConsole, topLeft );
#else
	printf( "\033[H\033[J" );
#endif
}