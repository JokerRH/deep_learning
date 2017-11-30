#pragma once

#include "Data.h"
#include "Compat.h"
#include "Render/Ray.h"

class CGazeData : public CData
{
public:
	CGazeData( const CData &data );
	~CGazeData( void ) override = default;

	std::string ToString( unsigned uPrecision = std::numeric_limits<double>::max_digits10 ) const;

	CRay rayEyeLeft;
	CRay rayEyeRight;

protected:
	CGazeData( const cv::Mat &matImage, const cv::Rect &rectFace, const filestring_t &sPath );
};

inline CGazeData::CGazeData( const cv::Mat &matImage, const cv::Rect &rectFace, const filestring_t &sPath ) :
	CData( matImage, rectFace, sPath )
{

}