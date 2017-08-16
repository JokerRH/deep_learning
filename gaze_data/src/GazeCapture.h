#pragma once

#include <opencv2/core/core.hpp>
#include "Image.h"
#include "Point.h"
#include "Camera.h"
#include "Queue.h"
#include <fstream>
#include <regex>
#ifdef _MSC_VER
#	define NOMINMAX
#	include <Windows.h>
#endif

class CGazeCapture
{
public:
	static bool OpenWrite( const std::string &sFile );
	static void CloseWrite( void );
	static bool OpenRead( const std::string &sFile );
	static bool ReadAsync( CGazeCapture &val );
	void WriteAsync( void );
	
	CGazeCapture( CBaseCamera &camera, const char *szWindow, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim );
	CGazeCapture( const CGazeCapture &other );
	CGazeCapture( void );
	
	void Swap( CGazeCapture &other, bool fSwapChildren = true );
	CGazeCapture &operator=( const CGazeCapture &other );

	std::string ToString( unsigned int uPrecision = std::numeric_limits< double >::max_digits10 ) const;
	void WriteImage( void ) const;

	CImage m_imgGaze;
	CVector<3> m_vec3Point;
	unsigned int m_uImage;	//Image number

	static std::string s_sName;
	static double s_dEyeDistance;
	static std::string s_sDataPath;

private:
	CGazeCapture( std::string sLine );
	static void *ReadThread( void *pArgs );
	static void *WriteThread( void *pArgs );

	static std::fstream s_File;
	static CQueue<CGazeCapture> s_Queue;
	static pthread_t s_Thread;
	
	static unsigned int s_uCurrentImage;
	
	static const std::regex s_regex_name;
	static const std::regex s_regex_dist;
	static const std::regex s_regex_data;
	static const std::regex s_regex_line;
	
	friend CQueue<CGazeCapture>;
};

inline CGazeCapture::CGazeCapture( const CGazeCapture &other ) :
	m_imgGaze( other.m_imgGaze ),
	m_vec3Point( other.m_vec3Point ),
	m_uImage( other.m_uImage )
{

}

inline void CGazeCapture::Swap( CGazeCapture &other, bool fSwapChildren )
{
	m_vec3Point.Swap( other.m_vec3Point );
	m_imgGaze.Swap( other.m_imgGaze, fSwapChildren );
	std::swap( m_uImage, other.m_uImage );
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

inline CGazeCapture::CGazeCapture( void ) :
	m_uImage( -1 )
{

}