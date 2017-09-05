#pragma once

#include <opencv2/core/core.hpp>
#include "Image.h"
#include "Point.h"
#include "Camera.h"
#include "Queue.h"
#include "FileFormat.h"
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
	static bool OpenRead( const std::string &sFile, bool fThreaded = true );
	static bool ReadAsync( CGazeCapture &val );
	static bool Read( CGazeCapture &val );
	void WriteAsync( void );
	static bool ImportCGD( const std::string &sCGDPath, const std::string &sFile = std::string( ) );
	
	CGazeCapture( CBaseCamera &camera, const char *szWindow, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim );
	CGazeCapture( void );

	CGazeCapture( const CGazeCapture &other ) = default;
	CGazeCapture &operator=( const CGazeCapture &other ) = default;

	CGazeCapture( CGazeCapture &&other ) = default;
	CGazeCapture &operator=( CGazeCapture &&other );

	void WriteImage( const std::string &sPath ) const;
	CGazeCapture_Set::gazecapture ToData( void ) const;

	CImage m_imgGaze;
	CVector<3> m_vec3Point;
	unsigned m_uImage;	//Image number
	std::string sImagePath;
	
	static CGazeCapture_Set s_DataSetRead;
	static CGazeCapture_Set s_DataSetWrite;

private:
	CGazeCapture( const CGazeCapture_Set &set, const CGazeCapture_Set::gazecapture &data );
	static void *ReadThread( void *pArgs );
	static void *WriteThread( void *pArgs );
	static void CheckDuplicates( CGazeCapture_Set &dataset, const std::string &sFile );

	static CQueue<CGazeCapture> s_QueueRead;
	static CQueue<CGazeCapture> s_QueueWrite;
	static std::vector<pthread_t> s_vecThreadRead;
	static std::vector<pthread_t> s_vecThreadWrite;
	static unsigned s_uNextImage;

	static const std::regex s_regex_name;
	static const std::regex s_regex_dist;
	static const std::regex s_regex_data;
	static const std::regex s_regex_line;
	
	friend CQueue<CGazeCapture>;
};

inline CGazeCapture::CGazeCapture( void ) :
	m_uImage( -1 )
{

}

inline CGazeCapture &CGazeCapture::operator=( CGazeCapture &&other )
{
	this->~CGazeCapture( );
	new( this ) CGazeCapture( std::move( other) );
	return *this;
}