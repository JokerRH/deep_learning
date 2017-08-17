#pragma once

#include <opencv2/core/core.hpp>
#include "BaseHighlighter.h"

class CPoint : public CBaseHighlighter
{
public:
	CPoint( void ) = default;
	CPoint( CBaseBBox &parentBox, const cv::Point &point, unsigned int uLevel, const char *szName );
	CPoint( CBaseBBox &parentBox, double dPositionX, double dPositionY, const char *szName );
	CPoint( CBaseBBox &parentBox, const CVector<2> &vec2Pos, const char *szName );

	CPoint( const CPoint &other ) = default;
	CPoint &operator=( const CPoint &other ) = default;

	CPoint( CPoint &&other ) = default;
	CPoint &operator=( CPoint &&other ) = default;

	void Draw( const cv::Scalar &color, int iRadius = 1, int iThickness = -1, unsigned int uLevel = -1 );
	void Draw( CImage &img, const cv::Scalar &color, int iRadius = 1, int iThickness = -1 );
};

inline CPoint::CPoint( CBaseBBox &parentBox, const cv::Point &point, unsigned int uLevel, const char *szName ) :
	CBaseHighlighter( parentBox, point.x, point.y, uLevel, szName )
{

}

inline CPoint::CPoint( CBaseBBox &parentBox, double dPositionX, double dPositionY, const char *szName ) :
	CBaseHighlighter( parentBox, dPositionX, dPositionY, szName )
{

}

inline CPoint::CPoint( CBaseBBox &parentBox, const CVector<2> &vec2Pos, const char *szName ) :
	CBaseHighlighter( parentBox, vec2Pos, szName )
{

}