#include "BBox.h"

void CBBox::Draw( CImage &img, const cv::Scalar &color, int iThickness )
{
	cv::Rect rect( GetPositionX( -1 ) - img.GetPositionX( -1 ), GetPositionY( -1 ) - img.GetPositionY( -1 ), GetWidth( -1 ), GetHeight( -1 ) );
	cv::rectangle( *img.pmatImage, rect, color, iThickness );
}