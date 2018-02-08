#ifdef _MSC_VER
#include "Utility.h"
#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <direct.h>
#include <opencv2\highgui.hpp>
#include <PathCch.h>

bool CUtility::fNoQuery = false;

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
	if( fNoQuery )
		return 10;

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

typedef std::wstring filestring_t;
void FindFilesRecursively( const filestring_t &sDir, const filestring_t &sPattern, std::vector<filestring_t> &vecsFiles )
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

std::vector<std::string> CUtility::GetFilesInDir( const std::string &sDir )
{
	std::vector<std::wstring> vecRes;
	FindFilesRecursively( std::wstring( sDir.begin( ), sDir.end( ) ), L"*.*", vecRes );

	std::vector<std::string> vecRet;
	for( std::wstring &str : vecRes )
		vecRet.emplace_back( std::string( str.begin( ), str.end( ) ) );

	return vecRet;
}
#endif