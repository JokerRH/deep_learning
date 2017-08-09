#pragma once

#include <opencv2/core/core.hpp>
#include "Image.h"
#include "Point.h"
#include "Camera.h"

#ifdef _MSC_VER
#	define NOMINMAX
#	include <Windows.h>
#else
#	include <sys/stat.h>
#endif

class CGazeCapture
{
public:
	static bool Init( CBaseCamera &camera, const char *szFile );
	static void Destroy( void );
	static bool OpenOrCreate( const std::string &sFile );
	static bool Exists( const std::string &sFile );
	static std::vector<CGazeCapture> Load( const std::string &sFile, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim );
	
	static std::string GetPath( const std::string &sFile );
	static std::string GetFile( const std::string &sFile );
	static std::string GetFileName( const std::string &sFile );
	
	CGazeCapture( CBaseCamera &camera, const char *szWindow, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim );
	CGazeCapture( CImage &img, CVector<3> vec3Point, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim );
	CGazeCapture( const CGazeCapture &other );
	~CGazeCapture( void );
	
	void Swap( CGazeCapture &other, bool fSwapChildren = true );
	CGazeCapture &operator=( const CGazeCapture &other );

	bool Write( void );

	CImage imgGaze;
	CVector<3> vec3Point;

	static double s_dEyeDistance;

private:
	static FILE *s_pFile;
	static std::string s_sName;
	static std::string s_sDataPath;
	static unsigned int s_uCurrentImage;
};

inline bool CGazeCapture::Exists( const std::string &sFile )
{
	struct stat buffer;   
	return ( stat( sFile.c_str( ), &buffer ) == 0 );
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
	vec3Point( other.vec3Point )
{

}

inline void CGazeCapture::Swap( CGazeCapture &other, bool fSwapChildren )
{
	vec3Point.Swap( other.vec3Point );
	imgGaze.Swap( other.imgGaze, fSwapChildren );
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