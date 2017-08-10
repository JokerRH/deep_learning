#pragma once

#include <opencv2/core/core.hpp>
#include "Image.h"
#include "Point.h"
#include "Camera.h"
#include <regex>
#ifdef _MSC_VER
#	define NOMINMAX
#	include <Windows.h>
#endif

class CGazeCapture
{
public:
	static bool Init( const char *szFile );
	static void Destroy( void );
	static bool OpenOrCreate( const std::string &sFile );
	static std::vector<CGazeCapture> Load( const std::string &sFile );
	
	CGazeCapture( CBaseCamera &camera, const char *szWindow, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim );
	CGazeCapture( CImage &img, CVector<3> vec3Point );
	CGazeCapture( const CGazeCapture &other );
	~CGazeCapture( void );
	
	void Swap( CGazeCapture &other, bool fSwapChildren = true );
	CGazeCapture &operator=( const CGazeCapture &other );

	bool Write( void );

	CImage imgGaze;
	CVector<3> vec3Point;

	static std::string s_sName;
	static double s_dEyeDistance;
	static std::string s_sDataPath;

private:
	static FILE *s_pFile;
	static unsigned int s_uCurrentImage;
	
	static const std::regex s_regex_name;
	static const std::regex s_regex_dist;
	static const std::regex s_regex_data;
	static const std::regex s_regex_line;
};

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