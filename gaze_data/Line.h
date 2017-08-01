#pragma once

#include "BaseHighlighter.h"
#include "BaseBBox.h"
#include "Image.h"
#include <opencv2/imgproc/imgproc.hpp>

class CLine : public CBaseHighlighter
{
public:
	unsigned int GetPositionX2( unsigned int uLevel = -1 ) const;
	unsigned int GetPositionY2( unsigned int uLevel = -1 ) const;
	double GetRelPositionX2( unsigned int uLevel = -1 ) const;
	double GetRelPositionY2( unsigned int uLevel = -1 ) const;

	void TransferOwnership( unsigned int uLevel = 1 );
	void TransferOwnership( CBaseBBox &parentBox );

	CLine &operator=( const CLine &other );
	
	void Draw( const cv::Scalar &color, int iThickness = 2, int iLineType = 8, int iShift = 0, unsigned int uLevel = -1 );
	void Draw( CImage &img, const cv::Scalar &color, int iThickness = 2, int iLineType = 8, int iShift = 0 ) const;

	CLine( void );
	CLine( const char *szName );
	CLine( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uX2, unsigned int uY2, unsigned int uLevel, const char *szName );
	CLine( CBaseBBox &parentBox, double dX, double dY, double dX2, double dY2, const char *szName );
	CLine( const CLine &other );

	void Swap( CLine &other, bool fSwapChildren = true );

protected:
	double m_dPositionX2;
	double m_dPositionY2;
};

inline unsigned int CLine::GetPositionX2( unsigned int uLevel ) const
{
	register unsigned int uPositionX = m_pParentBox->GetPositionX( uLevel );
	register unsigned int uWidth = m_pParentBox->GetWidth( uLevel );
	return uPositionX + (unsigned int ) ( uWidth * m_dPositionX2 );
}

inline unsigned int CLine::GetPositionY2( unsigned int uLevel ) const
{
	register unsigned int uPositionY = m_pParentBox->GetPositionY( uLevel );
	register unsigned int uHeight = m_pParentBox->GetHeight( uLevel );
	return uPositionY + (unsigned int ) ( uHeight * m_dPositionY2 );
}

inline double CLine::GetRelPositionX2( unsigned int uLevel ) const
{
	register double dPositionX = m_pParentBox->GetRelPositionX( uLevel );
	register double dWidth = m_pParentBox->GetRelWidth( uLevel );
	return dPositionX + dWidth * m_dPositionX2;
}

inline double CLine::GetRelPositionY2( unsigned int uLevel ) const
{
	register double dPositionY = m_pParentBox->GetRelPositionY( uLevel );
	register double dHeight = m_pParentBox->GetRelHeight( uLevel );
	return dPositionY + dHeight * m_dPositionY2;
}

inline CLine &CLine::operator=( const CLine &other )
{
	if( this != &other )
	{
		CLine temp( other );
		Swap( temp, false );
	}
	return *this;
}

inline void CLine::Draw( const cv::Scalar &color, int iThickness, int iLineType, int iShift, unsigned int uLevel )
{
	cv::line( GetImage( uLevel )->matImage, cv::Point( GetPositionX( uLevel ), GetPositionY( uLevel ) ), cv::Point( GetPositionX2( uLevel ), GetPositionY2( uLevel ) ), color, iThickness, iLineType, iShift );
}

inline void CLine::Draw( CImage &img, const cv::Scalar &color, int iThickness, int iLineType, int iShift ) const
{
	cv::Point pt1( GetPositionX( -1 ) - img.GetPositionX( -1 ), GetPositionY( -1 ) - img.GetPositionY( -1 ) );
	cv::Point pt2( GetPositionX2( -1 ) - img.GetPositionX( -1 ), GetPositionY2( -1 ) - img.GetPositionY( -1 ) );
	cv::line( img.matImage, pt1, pt2, color, iThickness, iLineType, iShift );
}

inline CLine::CLine( void )
{

}

inline CLine::CLine( const char *szName ) :
	CBaseHighlighter( szName )
{

}

inline CLine::CLine( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uX2, unsigned int uY2, unsigned int uLevel, const char *szName ) :
	CBaseHighlighter( parentBox, uX, uY, uLevel, szName ),
	m_dPositionX2( uX2 / (double) parentBox.GetWidth( uLevel ) ),
	m_dPositionY2( uY2 / (double) parentBox.GetHeight( uLevel ) )
{
	
}

inline CLine::CLine( CBaseBBox &parentBox, double dX, double dY, double dX2, double dY2, const char *szName ) :
	CBaseHighlighter( parentBox, dX, dY, szName ),
	m_dPositionX2( dX2 ),
	m_dPositionY2( dY2 )
{
	
}

inline CLine::CLine( const CLine &other ) :
	CBaseHighlighter( other ),
	m_dPositionX2( other.m_dPositionX2 ),
	m_dPositionY2( other.m_dPositionY2 )
{

}

inline void CLine::Swap( CLine &other, bool fSwapChildren )
{
	CBaseHighlighter::Swap( other, fSwapChildren );
	std::swap( m_dPositionX2, other.m_dPositionX2 );
	std::swap( m_dPositionY2, other.m_dPositionY2 );
}