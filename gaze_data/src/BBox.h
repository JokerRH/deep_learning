#pragma once

#include <opencv2/core/core.hpp>
#include "BaseBBox.h"
#include "Point.h"

class CImage;
class CBBox : public CBaseBBox
{
public:
	CBBox( CBaseBBox &baseBBox, const cv::Rect &rect, const char *szName ) :
		CBaseBBox( baseBBox, rect.x, rect.y, rect.width, rect.height, szName )
	{

	}
	
	CBBox( CPoint &pt, float rWidth, float rHeight, const char *szName ) :
		CBaseBBox( pt.m_rPositionX - rWidth / 2, pt.m_rPositionY - rHeight / 2, rWidth, rHeight, *pt.m_pParentBox, szName )
	{

	}
	
	CBBox( const CBaseBBox &baseBBox ) :
		CBaseBBox( baseBBox )
	{

	}

	inline void Draw( const cv::Scalar &color, int iThickness = 2, unsigned int uLevel = -1 )
	{
		cv::rectangle( *GetImage( uLevel )->pmatImage, cv::Rect( GetPositionX( uLevel ), GetPositionY( uLevel ), GetWidth( uLevel ), GetHeight( uLevel ) ), color, iThickness );
	}
	
	void Draw( CImage &img, const cv::Scalar &color, int iThickness = 2 );

	inline void Scale( float rScaleX, float rScaleY )
	{
		m_rWidth *= rScaleX;
		m_rHeight *= rScaleY;
	}
	
	inline void Shift( float rShiftX, float rShiftY )
	{
		m_rPositionX += m_rWidth * rShiftX;
		m_rPositionY += m_rHeight * rShiftY;
	}

	inline CBBox &operator=( const CBBox &other )
	{
		memcpy( this, &other, sizeof( CBBox ) );
		return *this;
	}
};