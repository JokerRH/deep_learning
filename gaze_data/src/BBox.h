#pragma once

#include <opencv2/core/core.hpp>
#include "BaseBBox.h"

class CImage;
class CBBox : public CBaseBBox
{
public:
	CBBox( void ) = default;
	CBBox( CBaseBBox &parentBox, const cv::Rect &rect, unsigned int uLevel, const char *szName );
	CBBox( CBaseBBox &parentBox, double dX, double dY, double dWidth, double dHeight, const char *szName );

	CBBox( const CBBox &other ) = default;
	CBBox &operator=( const CBBox &other ) = default;

	CBBox( CBBox &&other ) = default;
	CBBox &operator=( CBBox &&other ) = default;

	void Draw( const cv::Scalar &color, int iThickness = 2, unsigned int uLevel = -1 );
	void Draw( CImage &img, const cv::Scalar &color, int iThickness = 2 );
};

inline CBBox::CBBox( CBaseBBox &parentBox, const cv::Rect &rect, unsigned int uLevel, const char *szName ) :
	CBaseBBox( parentBox, rect.x, rect.y, rect.width, rect.height, uLevel, szName )
{

}

inline CBBox::CBBox( CBaseBBox &parentBox, double dX, double dY, double dWidth, double dHeight, const char *szName ) :
	CBaseBBox( parentBox, dX, dY, dWidth, dHeight, szName )
{

}