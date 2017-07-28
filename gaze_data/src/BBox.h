#pragma once

#include <opencv2/core/core.hpp>
#include "BaseBBox.h"

class CImage;
class CBBox : public CBaseBBox
{
public:
	CBBox( void )
	{

	}

	inline CBBox( CBaseBBox &parentBox, const cv::Rect &rect, unsigned int uLevel, const char *szName ) :
		CBaseBBox( parentBox, rect.x, rect.y, rect.width, rect.height, uLevel, szName )
	{

	}
	
	inline CBBox( const CBBox &other ) :
		CBaseBBox( other )
	{

	}
	
	inline void Swap( CBBox &other, bool fSwapChildren = true )
	{
		CBaseBBox::Swap( other, fSwapChildren );
	}

	inline CBBox &operator=( const CBBox &other )
	{
		if( this != &other )
		{
			CBBox temp( other );
			Swap( temp, false );
		}
		return *this;
	}

	void Draw( const cv::Scalar &color, int iThickness = 2, unsigned int uLevel = -1 );
	void Draw( CImage &img, const cv::Scalar &color, int iThickness = 2 );
};

#if 0
	CBBox( CPoint &pt, float rWidth, float rHeight, const char *szName ) :
		CBaseBBox( pt.m_rPositionX - rWidth / 2, pt.m_rPositionY - rHeight / 2, rWidth, rHeight, *pt.m_pParentBox, szName )
	{

	}

	CBBox( const CBaseBBox &parentBox ) :
		CBaseBBox( parentBox )
	{

	}

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
#endif