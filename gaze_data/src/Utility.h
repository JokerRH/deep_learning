#pragma once

#include <opencv2/core.hpp>
#ifdef _MSC_VER
#	include <Windows.h>
#else
#	include <sys/stat.h>
#endif

class CUtility
{
public:
	static unsigned char WaitKey( unsigned int uTime );
	static void Cls( void );
	static unsigned char GetChar( void );
	
	static bool Exists( const std::string &sFile );
	static std::string GetPath( const std::string &sFile );
	static std::string GetFile( const std::string &sFile );
	static std::string GetFileName( const std::string &sFile );

private:
	CUtility( void ) = delete;
	CUtility( const CUtility & ) = delete;
};

inline bool CUtility::Exists( const std::string &sFile )
{
	struct stat buffer;   
	return ( stat( sFile.c_str( ), &buffer ) == 0 );
}
inline std::string CUtility::GetPath( const std::string &sFile )
{
	size_t uLastDir = sFile.find_last_of( "/\\" );
	return sFile.substr( 0, uLastDir + 1 );
}

inline std::string CUtility::GetFile( const std::string &sFile )
{
	size_t uLastDir = sFile.find_last_of( "/\\" );
	return sFile.substr( uLastDir + 1 );
}

inline std::string CUtility::GetFileName( const std::string &sFile )
{
	std::string str = GetFile( sFile );
	size_t uLastDot = str.find_last_of( "." );
	if( uLastDot )
		return str.substr( 0, uLastDot );
	else
		return str;
}