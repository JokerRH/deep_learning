#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "BBox.h"
#include "Point.h"
#include "Image.h"

struct landmark_face
{
	landmark_face( const CBBox &boxFace ) :
		boxFace( boxFace )
	{

	}

	CBBox boxFace;
	std::vector<CBBox> vecEyes;
};

struct face_landmark
{
	face_landmark( CBBox &boxFace ) :
		boxFace( boxFace )
	{

	}
	
	inline void Draw( unsigned int uLevel = -1 )
	{
		boxFace.Draw( cv::Scalar( 0, 0, 255 ), 2, uLevel );
		if( uLevel != (unsigned int) -1 )
			uLevel++;

		ptEyeLeft.Draw( cv::Scalar( 0, 255, 255 ), 1, -1, uLevel );
		ptEyeRight.Draw( cv::Scalar( 0, 255, 255 ), 1, -1, uLevel );
	}
	
	inline void Draw( CImage &img )
	{
		boxFace.Draw( img, cv::Scalar( 0, 0, 255 ) );
		ptEyeLeft.Draw( img, cv::Scalar( 0, 255, 255 ) );
		ptEyeRight.Draw( img, cv::Scalar( 0, 255, 255 ) );
	}
	
	CBBox boxFace;
	CPoint ptEyeLeft;
	CPoint ptEyeRight;
};

class CFrameProcessor
{
public:
	CFrameProcessor( void );
	~CFrameProcessor( void );
	
	void ProcessImage( void );
	landmark_face *GetFaces( unsigned int &uFaces );
	face_landmark *ProcessImage( unsigned int &uLandmarks );
	
	inline void SetImage( const cv::Mat &mat )
	{
		m_matImage = mat.clone( );
	}
	
	inline void Show( const char *szWindow )
	{
		m_Image.Show( szWindow );
	}

private:
	landmark_face *DetectFaces( CImage &imgGray, unsigned int &uFaces );

	cv::Mat m_matImage;
	CImage m_Image;

	cv::CascadeClassifier m_FaceCascade;
	cv::CascadeClassifier m_EyeCascade;
};