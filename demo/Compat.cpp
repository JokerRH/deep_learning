#include "Compat.h"
#include <iostream>
#include <string.h>

#ifdef _MSC_VER
#	include <malloc.h>
#else
#	include <alloca.h>
#	include <libgen.h>
#	include <fts.h>
#	include <fnmatch.h>
#	include <termios.h>
#	include <stdio.h>
#	include <sys/ioctl.h>
#	include <linux/kd.h>
#	include <errno.h>
#endif

#ifdef _MSC_VER
int compat::CreateDirectory_d( const filestring_t &sPath )
{
	if( !CreateDirectory( sPath.c_str( ), nullptr ) )
		return 0;

	if( GetLastError( ) == ERROR_ALREADY_EXISTS )
		return 0;

	return 2;
}

filestring_t compat::PathRemoveFileSpec_d( const filestring_t &sPath )
{
	filechar_t *szPath = (filechar_t *) _alloca( ( sPath.size( ) + 1 ) * sizeof( filechar_t ) );
	if( !szPath )
	{
		std::wcerr << "Unable to allcate memory for path" << std::endl;
		throw 0;
	}
	memcpy( (void *) szPath, sPath.c_str( ), ( sPath.size( ) + 1 ) * sizeof( filechar_t ) );
	PathCchRemoveFileSpec( szPath, ( sPath.size( ) + 1 ) * sizeof( filechar_t ) );
	return filestring_t( szPath );
}

void compat::FindFilesRecursively( const filestring_t &sDir, const filestring_t &sPattern, std::vector<filestring_t> &vecsFiles )
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
#else
int _getch( void )
{
	static int iLast = 0;
	union
	{
		int iKey;
		char abKey[ 4 ];
	};

	if( iLast )
	{
		iKey = iLast;
		iLast = 0;
		return iKey;
	}

	struct termios oldattr, newattr;

	tcgetattr( STDIN_FILENO, &oldattr );
	newattr = oldattr;
	newattr.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newattr );

	(void) read( STDIN_FILENO, &iKey, sizeof( int ) );

	tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );

	//ASCII escape sequence to keycode
	if( abKey[ 0 ] == 27 && abKey[ 1 ] == '[' )
	{
		if( abKey[ 2 ] == 'A' && abKey[ 3 ] == 0 ) { iKey = 0; iLast = 72; }		//ARROW_UP
		else if( abKey[ 2 ] == 'B' && abKey[ 3 ] == 0 ) { iKey = 0; iLast = 80; }	//ARROW_DOWN
		else if( abKey[ 2 ] == 'C' && abKey[ 3 ] == 0 ) { iKey = 0; iLast = 77; }	//ARROW_RIGHT
		else if( abKey[ 2 ] == 'D' && abKey[ 3 ] == 0 ) { iKey = 0; iLast = 75; }	//ARROW_LEFT
	}

	if( iKey == 10 )
		iKey = 13;

	return iKey;
}

int compat::CreateDirectory_d( const filestring_t &sPath )
{
	if( !mkdir( sPath.c_str( ), 0700 ) )
		return 0;

	if( errno == EEXIST )
	{
		struct stat sb;

		if( stat( sPath.c_str( ), &sb ) )
			return 1;

		if( S_ISDIR( sb.st_mode ) )
			return 0;

		return 1;
	}

	return 2;
}

filestring_t compat::PathRemoveFileSpec_d( const filestring_t &sPath )
{
	filechar_t *szPath = (filechar_t *) alloca( ( sPath.size( ) + 1 ) * sizeof( filechar_t ) );
	if( !szPath )
	{
		std::wcerr << "Unable to allcate memory for path" << std::endl;
		throw 0;
	}
	memcpy( (void *) szPath, sPath.c_str( ), ( sPath.size( ) + 1 ) * sizeof( filechar_t ) );
	szPath = dirname( szPath );
	return filestring_t( szPath );
}

filestring_t compat::PathFindFileName_d( const filestring_t &sPath )
{
	filechar_t *szPath = (filechar_t *) alloca( ( sPath.size( ) + 1 ) * sizeof( filechar_t ) );
	if( !szPath )
	{
		std::wcerr << "Unable to allcate memory for path" << std::endl;
		throw 0;
	}
	memcpy( (void *) szPath, sPath.c_str( ), ( sPath.size( ) + 1 ) * sizeof( filechar_t ) );
	szPath = basename( szPath );
	return filestring_t( szPath );
}

void compat::FindFilesRecursively( const filestring_t &sDir, const filestring_t &sPattern, std::vector<filestring_t> &vecsFiles )
{
	char *const aszPath[ ] = { (char *const) sDir.c_str( ), nullptr };
	FTS *pFileSystem = fts_open( aszPath, FTS_COMFOLLOW, nullptr );
	if( !pFileSystem )
	{
		std::wcerr << L"Unable to read directory \"" << sDir.c_str( ) << "\"" << std::endl;
		return;
	}

	FTSENT *pNode;
	while( ( pNode = fts_read( pFileSystem ) ) )
	{
		if( !fnmatch( sPattern.c_str( ), pNode->fts_name, 0 ) )
			vecsFiles.push_back( pNode->fts_path );
	}

	fts_close( pFileSystem );
}
#endif
