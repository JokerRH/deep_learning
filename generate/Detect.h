#pragma once

#include "Data.h"
#include "Render\Ray.h"
#include <memory>
#include <string>
#include <caffe\caffe.hpp>

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
	static caffe::Net<float> *s_pNetwork;
};