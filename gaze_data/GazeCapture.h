#pragma once

#include <opencv2/core/core.hpp>
#include "Image.h"
#include "Point.h"

class CGazeCapture
{
public:
	static void Init( cv::VideoCapture &cap );
	static void GetScreenResolution( unsigned int &uWidth, unsigned int &uHeight );
	CGazeCapture( cv::VideoCapture &cap, const char *szWindow );
	inline CGazeCapture( const CGazeCapture &other ) :
		imgGaze( other.imgGaze ),
		ptGaze( other.ptGaze )
	{
		ptGaze.TransferOwnership( imgGaze );
	}
	~CGazeCapture( void );
	
	inline void Swap( CGazeCapture &other, bool fSwapChildren = true )
	{
		ptGaze.Swap( other.ptGaze, fSwapChildren );
		imgGaze.Swap( other.imgGaze, fSwapChildren );
		ptGaze.TransferOwnership( imgGaze );
	}

	inline CGazeCapture &operator=( const CGazeCapture &other )
	{
		if( this != &other )
		{
			CGazeCapture temp( other );
			Swap( temp, false );
		}
		return *this;
	}

	CImage imgGaze;
	CPoint ptGaze;
};

