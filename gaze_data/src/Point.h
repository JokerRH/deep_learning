#pragma once

#include <opencv2/core/core.hpp>
#include "BaseHighlighter.h"

class CPoint : public CBaseHighlighter
{
public:
	CPoint( void )
	{

	}

	CPoint( CBaseBBox &parentBox, const cv::Point &point, unsigned int uLevel, const char *szName ) :
		CBaseHighlighter( parentBox, point.x, point.y, uLevel, szName )
	{

	}

	CPoint( CBaseBBox &parentBox, double dPositionX, double dPositionY, const char *szName ) :
		CBaseHighlighter( parentBox, dPositionX, dPositionY, szName )
	{

	}

	CPoint( const CPoint &other ) :
		CBaseHighlighter( other )
	{

	}

	inline void Swap( CPoint &other, bool fSwapChildren = true )
	{
		CBaseHighlighter::Swap( other, fSwapChildren );
	}

	inline CPoint &operator=( const CPoint &other )
	{
		if( this != &other )
		{
			CPoint temp( other );
			Swap( temp, false );
		}
		return *this;
	}

	void Draw( const cv::Scalar &color, int iRadius = 1, int iThickness = -1, unsigned int uLevel = -1 );
	void Draw( CImage &img, const cv::Scalar &color, int iRadius = 1, int iThickness = -1 );
};

#if 0
	inline void Join( const CPoint &ptOther )
	{
		m_rPositionX = ( m_rPositionX + ptOther.m_rPositionX ) / 2;
		m_rPositionY = ( m_rPositionY + ptOther.m_rPositionY ) / 2;
	}
#endif