#pragma once

#ifdef WITH_FREDNET

#include "GazeData.h"
#include "Render/Ray.h"
#include "Frednet/dynamic_arrays.h"
#include "Frednet/base_layer.h"
#include <memory>
#include <string>
#include <opencv2/core/core.hpp>

#define IMAGE_WIDTH		256
#define IMAGE_HEIGHT	256
#define IMAGE_DEPTH		3

class CDetect : public CGazeData
{
public:
	static bool Init( const filestring_t &sPath );
	static void Terminate( void );
	static std::vector<cv::Rect> GetFaces( const cv::Mat &matImage );

	CDetect( const cv::Mat &matImage, const cv::Rect &rectFace, double dFOV, const filestring_t &sPath = filestring_t( ) );
	~CDetect( void ) override = default;

private:
	std::array<float, 8> Forward( cv::Mat matImage );
	
	static array3D<float> s_Image;
	static std::array<base_layer<float> *, 20> s_aLayers;
};

#endif