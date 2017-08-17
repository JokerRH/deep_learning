#pragma once

#include "Line.h"
#include "BaseBBox.h"
#include "Image.h"
#include <opencv2/imgproc/imgproc.hpp>

class CArrowedLine : public CLine
{
public:
	CArrowedLine( void ) = default;
	CArrowedLine( const CArrowedLine &other ) = default;
	CArrowedLine &operator=( const CArrowedLine &other ) = default;

	CArrowedLine( CArrowedLine &&other ) = default;
	CArrowedLine &operator=( CArrowedLine &&other ) = default;
	
	void Draw( const cv::Scalar &color, int iThickness = 2, int iLineType = 8, int iShift = 0, double dTipLength = 0.1, unsigned int uLevel = -1 );
	void Draw( const CImage &img, const cv::Scalar &color, int iThickness = 2, int iLineType = 8, int iShift = 0, double dTipLength = 0.1 );

	CArrowedLine( const char *szName );
	CArrowedLine( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uX2, unsigned int uY2, unsigned int uLevel, const char *szName );
	CArrowedLine( CBaseBBox &parentBox, double dX, double dY, double dX2, double dY2, const char *szName );
};

inline void CArrowedLine::Draw( const cv::Scalar &color, int iThickness, int iLineType, int iShift, double dTipLength, unsigned int uLevel )
{
	cv::arrowedLine( GetImage( uLevel )->matImage, cv::Point( GetPositionX( uLevel ), GetPositionY( uLevel ) ), cv::Point( GetPositionX2( uLevel ), GetPositionY2( uLevel ) ), color, iThickness, iLineType, iShift );
}

inline void CArrowedLine::Draw( const CImage &img, const cv::Scalar &color, int iThickness, int iLineType, int iShift, double dTipLength )
{
	cv::Point pt1( GetPositionX( -1 ) - img.GetPositionX( -1 ), GetPositionY( -1 ) - img.GetPositionY( -1 ) );
	cv::Point pt2( GetPositionX2( -1 ) - img.GetPositionX( -1 ), GetPositionY2( -1 ) - img.GetPositionY( -1 ) );
	cv::arrowedLine( img.matImage, pt1, pt2, color, iThickness, iLineType, iShift, dTipLength );
}

inline CArrowedLine::CArrowedLine( const char *szName ) :
	CLine( szName )
{

}

inline CArrowedLine::CArrowedLine( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uX2, unsigned int uY2, unsigned int uLevel, const char *szName ) :
	CLine( parentBox, uX, uY, uX2, uY2, uLevel, szName )
{
	
}

inline CArrowedLine::CArrowedLine( CBaseBBox &parentBox, double dX, double dY, double dX2, double dY2, const char *szName ) :
	CLine( parentBox, dX, dY, dX2, dY2, szName )
{
	
}