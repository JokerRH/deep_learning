#pragma once

#include <opencv2/core/core.hpp>
#include "BaseBBox.h"
#include "Image.h"

class CBBox;
class CPoint
{
public:
	CPoint( void )
	{

	}

	CPoint( CBaseBBox &baseBBox, const cv::Point &point ) :
		m_pParentBox( &baseBBox ),
		m_rPositionX( point.x / (float) baseBBox.GetWidth( 0 ) ),
		m_rPositionY( point.y / (float) baseBBox.GetHeight( 0 ) )
	{

	}

	CPoint( CBaseBBox &baseBBox, float rPositionX, float rPositionY ) :
		m_pParentBox( &baseBBox ),
		m_rPositionX( rPositionX ),
		m_rPositionY( rPositionY )
	{

	}

	inline unsigned int GetPositionX( unsigned int uLevel = -1 ) const
	{
		return (unsigned int) ( m_pParentBox->GetPositionX( uLevel ) + m_pParentBox->GetWidth( uLevel ) * m_rPositionX );
	}

	inline unsigned int GetPositionY( unsigned int uLevel = -1 ) const
	{
		return (unsigned int) ( m_pParentBox->GetPositionY( uLevel ) + m_pParentBox->GetHeight( uLevel ) * m_rPositionY );
	}
	
	inline float GetRelPositionX( unsigned int uLevel = 0 ) const
	{
		if( !uLevel-- )
			return m_rPositionX;

		return m_pParentBox->GetRelPositionX( uLevel ) + m_pParentBox->GetRelWidth( uLevel ) * m_rPositionX;
	}

	inline float GetRelPositionY( unsigned int uLevel = 0 ) const
	{
		if( !uLevel-- )
			return m_rPositionY;

		return m_pParentBox->GetRelPositionY( uLevel ) + m_pParentBox->GetRelHeight( uLevel ) * m_rPositionY;
	}
	
	inline CImage *GetImage( unsigned int uLevel = -1 )
	{
		return m_pParentBox->GetImage( uLevel );
	}

	inline CBaseBBox *GetParent( unsigned int uLevel = -1 )
	{
		if( !m_pParentBox || !uLevel-- )
			return nullptr;

		return m_pParentBox->GetParent( uLevel );
	}
	
	inline void TransferOwnership( unsigned int uLevel = 1 )
	{
		while( uLevel-- )
		{
			if( !m_pParentBox->m_pParentBox )
				break;	//Parent is top level image

			m_rPositionX = m_pParentBox->m_rPositionX + m_pParentBox->m_rWidth * m_rPositionX;
			m_rPositionY = m_pParentBox->m_rPositionY + m_pParentBox->m_rHeight * m_rPositionY;

			m_pParentBox = m_pParentBox->m_pParentBox;
		}
	}
	
	virtual inline void TransferOwnership( CBaseBBox &baseBBox )
	{
		m_rPositionX = ( GetPositionX( ) - baseBBox.GetPositionX( ) ) / (float) baseBBox.GetWidth( );
		m_rPositionY = ( GetPositionY( ) - baseBBox.GetPositionY( ) ) / (float) baseBBox.GetHeight( );
		m_pParentBox = &baseBBox;
	}
	
	inline void Join( const CPoint &ptOther )
	{
		m_rPositionX = ( m_rPositionX + ptOther.m_rPositionX ) / 2;
		m_rPositionY = ( m_rPositionY + ptOther.m_rPositionY ) / 2;
	}
	
	inline void Draw( const cv::Scalar &color, int iRadius = 1, int iThickness = -1, unsigned int uLevel = -1 )
	{
		cv::circle( *m_pParentBox->GetImage( uLevel )->pmatImage, cv::Point( GetPositionX( uLevel ), GetPositionY( uLevel ) ), iRadius, color, iThickness );
	}
	
	inline void Draw( CImage &img, const cv::Scalar &color, int iRadius = 1, int iThickness = -1 )
	{
		cv::Point pt( GetPositionX( -1 ) - img.GetPositionX( -1 ), GetPositionY( -1 ) - img.GetPositionY( -1 ) );
		cv::circle( *img.pmatImage, pt, iRadius, color, iThickness );
	}

private:
	CBaseBBox *m_pParentBox;
	float m_rPositionX;	//Left X-position in % of parent's width
	float m_rPositionY;	//Top Y-position in % of parent's height
	
	friend CBBox;
};