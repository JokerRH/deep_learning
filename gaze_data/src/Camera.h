#pragma once

#include "Image.h"
#include <opencv2\videoio\videoio.hpp>

class CBaseCamera
{
public:
	static bool Init( void );
	static void Terminate( void );
	static CBaseCamera *SelectCamera( void );
	virtual bool TakePicture( CImage &img ) = 0;
};

class CCamera : public CBaseCamera
{
public:
	CCamera( unsigned int uIndex );
	bool TakePicture( CImage &img ) override;

private:
	cv::VideoCapture m_Cap;
};