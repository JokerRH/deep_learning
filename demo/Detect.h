#pragma once

#ifdef WITH_CAFFE

#include "GazeData.h"
#include "Render/Ray.h"
#include <memory>
#include <string>
#include <opencv2/core/core.hpp>

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4267 )
#	pragma warning( disable: 4244 )
#endif
#include <caffe/caffe.hpp>
#ifdef _MSC_VER
#	pragma warning( pop )
#endif

class CDetect : public CGazeData
{
public:
	static bool Init( const filestring_t &sPath );
	static void Terminate( void );
	static std::vector<cv::Rect> GetFaces( const cv::Mat &matImage );

	CDetect( const cv::Mat &matImage, const cv::Rect &rectFace, double dFOV, const std::wstring &sPath = std::wstring( ) );
	~CDetect( void ) override = default;

private:
	static bool SetMean( const std::string &sMeanFile );
	std::array<float, 8> Forward( cv::Mat matImage );

	static caffe::Net<float> *s_pNetwork;
	static cv::Size s_InputShape;
	static cv::Mat s_matMean;
};

#endif