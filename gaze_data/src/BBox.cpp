#include "BBox.h"
#include "Image.h"

void CBBox::Draw( const cv::Scalar &color, int iThickness, unsigned int uLevel )
{
	cv::rectangle( GetImage( uLevel )->matImage, cv::Rect( GetPositionX( uLevel ), GetPositionY( uLevel ), GetWidth( uLevel ), GetHeight( uLevel ) ), color, iThickness );
}

void CBBox::Draw( CImage &img, const cv::Scalar &color, int iThickness )
{
	cv::Rect rect( GetPositionX( -1 ) - img.GetPositionX( -1 ), GetPositionY( -1 ) - img.GetPositionY( -1 ), GetWidth( -1 ), GetHeight( -1 ) );
	cv::rectangle( img.matImage, rect, color, iThickness );
}