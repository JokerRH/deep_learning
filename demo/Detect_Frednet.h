#pragma once

#ifdef WITH_FREDNET

#include "GazeData.h"
#include "Render/Ray.h"
#include "Frednet/dynamic_arrays.h"
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

	struct alexnet
	{
		array1D<float> conv1bias;
		array4D<float> conv1Weights;
		array1D<float> conv2bias;
		array4D<float> conv2Weights;
		array1D<float> conv3bias;
		array4D<float> conv3Weights;
		array1D<float> conv4bias;
		array4D<float> conv4Weights;
		array1D<float> conv5bias;
		array4D<float> conv5Weights;
		array1D<float> fc8Bias;
		array2D<float> fc8Weights;
	};
	
	static array3D<float> s_Image;
	static std::array<base_layer<float> *, 20> s_aLayers;
	static alexnet s_Network;
};

#endif