#pragma once

#include <string>
#include <vector>
#ifdef _MSC_VER
#	include <Windows.h>
#	include <Pathcch.h>
#	include <Shlwapi.h>

#	undef ERROR
#	undef STRICT
#	undef min
#	undef max
#else
#	include <sys/stat.h>
#	include <sys/types.h>
#	include <unistd.h>
#	include <stdlib.h>
#endif

#ifdef _MSC_VER
typedef wchar_t filechar_t;
typedef std::wstring filestring_t;

#define CFSTR( str )	L##str
#else
typedef char filechar_t;
typedef std::string filestring_t;

#define CFSTR( str )	#str
#endif

namespace compat
{
	std::string ToString( const filestring_t &sString );
	std::wstring ToWString( const filestring_t &sString );
	filestring_t PathCombine_d( const filestring_t &sPath, const filestring_t &sAppend );
	int CreateDirectory_d( const filestring_t &sPath );
	filestring_t PathRemoveFileSpec_d( const filestring_t &sPath );
	filestring_t PathFindFileName_d( const filestring_t &sPath );
	void FindFilesRecursively( const filestring_t &sDir, const filestring_t &sPattern, std::vector<filestring_t> &vecsFiles );
	bool PathFileExists_d( const filestring_t &sPath );
	bool PathFolderExists_d( const filestring_t &sPath );
	filestring_t GetFullPathName_d( const filestring_t &sPath );
}

#ifdef _MSC_VER
inline std::string compat::ToString( const filestring_t &sString )
{
	return std::string( sString.c_str( ), sString.c_str( ) + sString.length( ) );
}

inline std::wstring compat::ToWString( const filestring_t &sString )
{
	return sString;
}

inline filestring_t compat::PathCombine_d( const filestring_t &sPath, const filestring_t &sAppend )
{
	filechar_t szPath[ MAX_PATH ];
	PathCchCombine( szPath, MAX_PATH, sPath.c_str( ), sAppend.c_str( ) );
	return filestring_t( szPath );
}

inline filestring_t compat::PathFindFileName_d( const filestring_t &sPath )
{
	return filestring_t( PathFindFileName( sPath.c_str( ) ) );
}

inline bool compat::PathFileExists_d( const filestring_t &sPath )
{
	return PathFileExists( sPath.c_str( ) ) && !( GetFileAttributes( sPath.c_str( ) ) & FILE_ATTRIBUTE_DIRECTORY );
}

inline bool compat::PathFolderExists_d( const filestring_t & sPath )
{
	return PathFileExists( sPath.c_str( ) ) && ( GetFileAttributes( sPath.c_str( ) ) & FILE_ATTRIBUTE_DIRECTORY );
}

inline filestring_t compat::GetFullPathName_d( const filestring_t & sPath )
{
	WCHAR szPath[ MAX_PATH ];
	if( !GetFullPathName( sPath.c_str( ), MAX_PATH, szPath, nullptr ) )
		return filestring_t( );

	return filestring_t( szPath );
}
#else
inline std::string compat::ToString( const filestring_t &sString )
{
	return sString;
}

inline std::wstring compat::ToWString( const filestring_t &sString )
{
	return std::wstring( sString.begin( ), sString.end( ) );
}

inline filestring_t compat::PathCombine_d( const filestring_t &sPath, const filestring_t &sAppend )
{
	return sPath + "/" + sAppend;
}

inline bool compat::PathFileExists_d( const filestring_t &sPath )
{
	struct stat sb;
	return !stat( sPath.c_str( ), &sb ) && S_ISREG( sb.st_mode );
}

inline bool compat::PathFolderExists_d( const filestring_t & sPath )
{
	struct stat sb;
	return !stat( sPath.c_str( ), &sb ) && S_ISDIR( sb.st_mode );
}

inline filestring_t compat::GetFullPathName_d( const filestring_t & sPath )
{
	char *szPath = realpath( sPath.c_str( ), nullptr );
	filestring_t sFull( szPath );
	free( szPath );
	return sFull;
}
#endif