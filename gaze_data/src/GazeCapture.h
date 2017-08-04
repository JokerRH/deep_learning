#pragma once

#include <opencv2/core/core.hpp>
#include "Image.h"
#include "Point.h"

#ifdef _MSC_VER
#	define NOMINMAX
#	include <Windows.h>
#else
#	include <sys/stat.h>
#endif

class CGazeCapture
{
public:
	static bool Init( cv::VideoCapture &cap, const char *szFile );
	static void Destroy( void );
	static void GetScreenResolution( unsigned int &uWidth, unsigned int &uHeight );
	static bool OpenOrCreate( const std::string &sFile );
	static bool Exists( const std::string &sFile );
	static void Cls( void );
	static unsigned char GetChar( void );
	static std::vector<CGazeCapture> Load( const std::string &sFile );
	
	static std::string GetPath( const std::string &sFile );
	static std::string GetFile( const std::string &sFile );
	static std::string GetFileName( const std::string &sFile );
	
	CGazeCapture( cv::VideoCapture &cap, const char *szWindow );
	CGazeCapture( const cv::Mat &mat, double dX, double dY, time_t timeCapture );
	CGazeCapture( const CGazeCapture &other );
	~CGazeCapture( void );
	
	void Swap( CGazeCapture &other, bool fSwapChildren = true );
	CGazeCapture &operator=( const CGazeCapture &other );

	bool Write( void );

	CImage imgGaze;
	CPoint ptGaze;
	time_t timeCapture;

	static double s_dEyeDistance;
	static double s_dFOV;

private:
	static FILE *s_pFile;
	static std::string s_sName;
	static std::string s_sDataPath;
	static unsigned int s_uCurrentImage;
};

inline bool CGazeCapture::Exists( const std::string &sFile )
{
#ifdef _MSC_VER
	LPCWSTR szFile = std::wstring( sFile.begin( ), sFile.end( ) ).c_str( );
	GetFileAttributes( szFile );
	return ( INVALID_FILE_ATTRIBUTES == GetFileAttributes( szFile ) && GetLastError( ) == ERROR_FILE_NOT_FOUND );
#else
	struct stat buffer;   
	return ( stat( sFile.c_str( ), &buffer ) == 0 );
#endif
}

inline std::string CGazeCapture::GetPath( const std::string &sFile )
{
	size_t uLastDir = sFile.find_last_of( "/\\" );
	return sFile.substr( 0, uLastDir + 1 );
}

inline std::string CGazeCapture::GetFile( const std::string &sFile )
{
	size_t uLastDir = sFile.find_last_of( "/\\" );
	return sFile.substr( uLastDir + 1 );
}

inline std::string CGazeCapture::GetFileName( const std::string &sFile )
{
	std::string str = GetFile( sFile );
	size_t uLastDot = str.find_last_of( "." );
	if( uLastDot )
		return str.substr( 0, uLastDot );
	else
		return str;
}

inline CGazeCapture::CGazeCapture( const CGazeCapture &other ) :
	imgGaze( other.imgGaze ),
	ptGaze( other.ptGaze )
{
	ptGaze.TransferOwnership( imgGaze );
}

inline void CGazeCapture::Swap( CGazeCapture &other, bool fSwapChildren )
{
	ptGaze.Swap( other.ptGaze, fSwapChildren );
	imgGaze.Swap( other.imgGaze, fSwapChildren );
	ptGaze.TransferOwnership( imgGaze );
}

inline CGazeCapture &CGazeCapture::operator=( const CGazeCapture &other )
{
	if( this != &other )
	{
		CGazeCapture temp( other );
		Swap( temp, false );
	}
	return *this;
}