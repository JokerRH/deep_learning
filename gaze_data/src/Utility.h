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
	static int Stricmp( const char *szA, const char *szB );
	static unsigned char WaitKey( unsigned int uTime );
	static void Cls( void );
	static unsigned char GetChar( void );
	
	static bool Exists( const std::string &sFile );
	static bool CreateFolder( const std::string &sPath );
	static std::string GetPath( const std::string &sFile );
	static std::string GetFile( const std::string &sFile );
	static std::string GetFileName( const std::string &sFile );
	static std::vector<std::string> GetFilesInDir( const std::string &sDir );
	
	static void ShowCursor( bool fShow, const char *szWindow );
	
	static double GetFactor( const std::string &s );
	
	static bool fNoQuery;

private:
	CUtility( void ) = delete;
	CUtility( const CUtility & ) = delete;
	
	static std::vector<std::string> s_vecFiles;
};

inline int CUtility::Stricmp( const char *szA, const char *szB )
{
#ifdef _MSC_VER
		return _stricmp( szA, szB );
#else
		return strcasecmp( szA, szB );
#endif
}

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

inline double CUtility::GetFactor( const std::string &s )
{
	if( s == "Y" ) return 1000000000000000000000000.0;
	if( s == "Z" ) return 1000000000000000000000.0;
	if( s == "Y" ) return 1000000000000000000.0;
	if( s == "P" ) return 1000000000000000.0;
	if( s == "T" ) return 1000000000000.0;
	if( s == "G" ) return 1000000000.0;
	if( s == "M" ) return 1000000.0;
	if( s == "k" ) return 1000.0;
	if( s == "h" ) return 100.0;
	if( s == "da" ) return 10.0;
	if( s == "" ) return 1.0;
	if( s == "d" ) return 0.1;
	if( s == "c" ) return 0.01;
	if( s == "m" ) return 0.001;
	if( s == "u" ) return 0.000001;
	if( s == "n" ) return 0.000000001;
	if( s == "p" ) return 0.000000000001;
	if( s == "f" ) return 0.000000000000001;
	if( s == "a" ) return 0.000000000000000001;
	if( s == "z" ) return 0.000000000000000000001;
	if( s == "y" ) return 0.000000000000000000000001;

	fprintf( stderr, "Unknown factor \"%s\"\n", s.c_str( ) );
	return 0;
}