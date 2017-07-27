#pragma once

#include <vector>
#include <deque>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "BBox.h"

class CLandmarkCandidate
{
public:
	static std::vector<CLandmarkCandidate> GetCandidates( CImage &img );
	static void Init( void );
	CLandmarkCandidate( const CBBox &boxFace ) :
		boxFace( boxFace )
	{

	}
	void Draw( CImage &img );

	CBBox boxFace;
	std::deque<CBBox> aEyes;
	std::deque<CBBox> aNose;

private:

	static cv::CascadeClassifier s_FaceCascade;
	static cv::CascadeClassifier s_EyeCascade;
	static cv::CascadeClassifier s_NoseCascade;
};