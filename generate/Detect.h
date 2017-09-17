#pragma once

#ifdef WITH_CAFFE

#include "Data.h"
#include "Render\Ray.h"
#include <memory>
#include <string>
#include <opencv2\core.hpp>

#pragma warning( push )
#pragma warning( disable: 4244 )
#pragma warning( disable: 4996 )
#define _SCL_SECURE_NO_WARNINGS
#include <caffe\caffe.hpp>
#pragma warning( pop )

class CDetect : public CData
{
public:
	static bool Init( const std::wstring &sNetwork );
	static void Terminate( void );
	CDetect( const cv::Mat &matImage, const cv::Rect &rectFace, double dFOV );
	~CDetect( void ) override = default;

	CRay rayEyeLeft;
	CRay rayEyeRight;

private:
	static bool SetMean( const std::string &sMeanFile );
	std::array<float, 8> Forward( cv::Mat matImage );

	static caffe::Net<float> *s_pNetwork;
	static cv::Size s_InputShape;
	static cv::Mat s_matMean;
};

#endif