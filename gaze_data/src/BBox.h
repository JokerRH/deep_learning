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
	
	inline CBBox( CBaseBBox &parentBox, double dX, double dY, double dWidth, double dHeight, const char *szName ) :
		CBaseBBox( parentBox, dX, dY, dWidth, dHeight, szName )
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