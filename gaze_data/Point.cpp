#include "Point.h"
#include "Image.h"
#include <opencv2/imgproc/imgproc.hpp>

void CPoint::Draw( const cv::Scalar &color, int iRadius, int iThickness, unsigned int uLevel )
{
	cv::circle( GetImage( uLevel )->matImage, cv::Point( GetPositionX( uLevel ), GetPositionY( uLevel ) ), iRadius, color, iThickness );
}

void CPoint::Draw( CImage &img, const cv::Scalar &color, int iRadius, int iThickness )
{
	cv::Point pt( GetPositionX( -1 ) - img.GetPositionX( -1 ), GetPositionY( -1 ) - img.GetPositionY( -1 ) );
	cv::circle( img.matImage, pt, iRadius, color, iThickness );
}