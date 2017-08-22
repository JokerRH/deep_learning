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
	
	CLandmarkCandidate( const CLandmarkCandidate &other );
	CLandmarkCandidate &operator=( const CLandmarkCandidate &other ) = default;

	CLandmarkCandidate( CLandmarkCandidate &&other ) = default;
	CLandmarkCandidate &operator=( CLandmarkCandidate &&other );
	~CLandmarkCandidate( void );

	inline CLandmarkCandidate( const CBBox &boxFace ) :
		boxFace( boxFace )
	{

	}

	void Draw( CImage &img );

	CBBox boxFace;
	std::deque<CBBox> aEyes;

private:
	static cv::CascadeClassifier s_FaceCascade;
	static cv::CascadeClassifier s_EyeCascade;
};

inline CLandmarkCandidate::CLandmarkCandidate( const CLandmarkCandidate &other ) :
	boxFace( other.boxFace ),
	aEyes( other.aEyes.begin( ), other.aEyes.end( ) )
{
	for( auto eye: aEyes )
			eye.TransferOwnership( boxFace );
}

inline CLandmarkCandidate &CLandmarkCandidate::operator=( CLandmarkCandidate &&other )
{
	this->~CLandmarkCandidate( );
	new( this ) CLandmarkCandidate( std::move( other ) );
	return *this;
}

inline CLandmarkCandidate::~CLandmarkCandidate( void )
{
	aEyes.clear( );
}