#ifdef _MSC_VER
#include "Utility.h"
#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <direct.h>
#include <opencv2\highgui.hpp>

unsigned int VK2VSC( unsigned int uKey )
{
	switch( uKey )
	{
	case 71:	//Pos1
		return 80;
	case 72:	//Arrow_Up
		return 82;
	case 80:	//Arrow_Down
		return 84;
	case 75:	//Arrow_Left
		return 81;
	case 77:	//Arrow_Right
		return 83;
	case 73:	//Img_Up
		return 85;
	case 81:	//Img_Down
		return 86;
	default:
		printf( "Unhandled key %u\n", uKey );
		return 0;
	}
}

unsigned char CUtility::WaitKey( unsigned int uMilliseconds )
{
	unsigned int uKey;
	MSG msg;
	bool fContinue = true;
	BOOL fReturn;
	while( fContinue )
	{
		if( ( fReturn = GetMessage( &msg, NULL, 0, 0 ) ) == -1 )
			return EXIT_FAILURE;	//Error

		switch( msg.message )
		{
		case WM_QUIT:
			PostQuitMessage( 0 );
			fContinue = false;
			uKey = 0;
			break;
		case WM_KEYDOWN:
			uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_CHAR );
			switch( uKey )
			{
			case 0:
				uKey = VK2VSC( MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_VSC ) );
				break;
			case 13:	//Enter
				uKey = 10;
				break;
			}
			fContinue = false;
			break;
		case WM_KEYUP:
			break;
		}

		if( fReturn > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return (unsigned char) uKey;
}

void CUtility::Cls( void )
{
	COORD topLeft = { 0, 0 };
	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo( hConsole, &screen );
	FillConsoleOutputCharacterA( hConsole, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
	FillConsoleOutputAttribute( hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
	SetConsoleCursorPosition( hConsole, topLeft );
}

unsigned char CUtility::GetChar( void )
{
	int iKey = _getch( );
	switch( iKey )
	{
	case 13:
		iKey = 10;
		break;
	case 224:
		iKey = (int) VK2VSC( (unsigned int) _getch( ) );
		break;
	}

	return (unsigned char) iKey;
}

bool CUtility::CreateFolder( const std::string &sPath )
{
	if( _mkdir( sPath.c_str( ) ) && errno != EEXIST )
	{
		perror( "Error creating directory" );
		return false;
	}

	return true;
}

void CUtility::ShowCursor( bool fShow, const char *szWindow )
{
	::ShowCursor( fShow ? TRUE : FALSE );
}
#endif