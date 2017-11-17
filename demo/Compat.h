#pragma once

#include <string>
#include <vector>
#ifdef _MSC_VER
#	include <Windows.h>
#	include <Pathcch.h>
#	include <Shlwapi.h>
#else
#	include <sys/stat.h>
#	include <sys/types.h>
#	include <unistd.h>
#endif

#ifdef _MSC_VER
typedef wchar_t filechar_t;
typedef std::wstring filestring_t;
#else
typedef char filechar_t;
typedef std::string filestring_t;
#endif

namespace compat
{
	std::string ToString( const filestring_t &sString );
	filestring_t PathCombine_d( const filestring_t &sPath, const filestring_t &sAppend );
	int CreateDirectory_d( const filestring_t &sPath );
	filestring_t PathRemoveFileSpec_d( const filestring_t &sPath );
	filestring_t PathFindFileName_d( const filestring_t &sPath );
	void FindFilesRecursively( const filestring_t &sDir, const filestring_t &sPattern, std::vector<filestring_t> &vecsFiles );
}

#ifdef _MSC_VER
inline std::string compat::ToString( const filestring_t &sString )
{
	return std::string( sString.c_str( ), sString.c_str( ) + sString.length( ) );
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
#else
inline std::string compat::ToString( const filestring_t &sString )
{
	return sString;
}

inline filestring_t compat::PathCombine_d( const filestring_t &sPath, const filestring_t &sAppend )
{
	return sPath + "/" + sAppend;
}
#endif