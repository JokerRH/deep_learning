#include "..\..\generate\Camera.h"
#pragma once

#include "Image.h"
#include <opencv2/videoio/videoio.hpp>

class CBaseCamera
{
public:
	static bool Init( void );
	static void Terminate( void );
	static CBaseCamera *SelectCamera( void );
	
	virtual ~CBaseCamera( void );
	virtual bool TakePicture( CImage &img ) = 0;
};

class CCamera : public CBaseCamera
{
public:
	CCamera( unsigned int uIndex );
	~CCamera( void ) override;
	bool TakePicture( CImage &img ) override;

private:
	cv::VideoCapture m_Cap;
};

inline CBaseCamera::~CBaseCamera( void )
{
	
}

inline CCamera::~CCamera( void )
{
	
}

inline void CCamera::StopLiveView( void )
{
}
