#pragma once

#include <vector>
#include <deque>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "BBox.h"
#include "Image.h"

class CLandmarkCandidate
{
public:
	static std::vector<CLandmarkCandidate> GetCandidates( CImage &img );
	static void Init( void );
	inline CLandmarkCandidate( const CBBox &boxFace ) :
		boxFace( boxFace )
	{

	}
	
	CLandmarkCandidate( const CLandmarkCandidate &other ) :
		boxFace( other.boxFace ),
		aEyes( other.aEyes.begin( ), other.aEyes.end( ) )
	{
		for( std::deque<CBBox>::iterator it = aEyes.begin( ); it < aEyes.end( ); it++ )
			it->TransferOwnership( boxFace );
	}

	void Draw( CImage &img );

	CBBox boxFace;
	std::deque<CBBox> aEyes;

private:
	static cv::CascadeClassifier s_FaceCascade;
	static cv::CascadeClassifier s_EyeCascade;
};