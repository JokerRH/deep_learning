#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "BaseBBox.h"

class CBBox;
class CImage : public CBaseBBox
{
public:
	CImage( void )
	{

	}

	CImage( const char *szName ) :
		CBaseBBox( szName )
	{

	}

	CImage( cv::Mat &mat, const char *szName ) :
		CBaseBBox( szName ),
		matImage( mat )
	{

	}
	
	CImage( const CImage &other ) :
		CBaseBBox( other ),
		matImage( other.matImage )
	{

	}
	
	inline CImage( CImage &img, const char *szName ) :
		CBaseBBox( img, 0, 0, 1, 1, szName ),
		matImage( img.matImage.clone( ) )
	{

	}

	inline CImage( CImage &parentImage, cv::Mat &matImage, const cv::Point &point, const char *szName ) :
		CBaseBBox( parentImage, point.x / (double) parentImage.GetWidth( 0 ), point.y / (double) parentImage.GetHeight( 0 ), matImage.cols / (double) parentImage.GetWidth( 0 ), matImage.rows / (double) parentImage.GetHeight( 0 ), szName ),
		matImage( matImage )
	{
		//printf( "Creating image \"%s\" of dim %ux%u/%4.2fx%4.2f at (%u, %u)/(%4.2f, %4.2f); Parent dim: %ux%u\n", szName, matImage.cols, matImage.rows, m_rWidth, m_rHeight, point.x, point.y, m_rPositionX, m_rPositionY, parentImage.GetWidth( 0 ), parentImage.GetHeight( 0 ) );
	}

	inline void Swap( CImage &other, bool fSwapChildren = true )
	{
		CBaseBBox::Swap( other, fSwapChildren );
		cv::swap( matImage, other.matImage );
	}

	inline CImage &operator=( const CImage &other )
	{
		if( this != &other )
		{
			CImage temp( other );
			Swap( temp, false );
		}
		return *this;
	}

	inline unsigned int GetPositionX( unsigned int uLevel = -1 ) const override
	{
		if( !m_pParentBox || !uLevel-- )
			return 0;

		return (unsigned int) ( m_pParentBox->GetPositionX( uLevel ) + m_pParentBox->GetWidth( uLevel ) * m_dPositionX );
	}
	
	inline unsigned int GetPositionY( unsigned int uLevel = -1 ) const override
	{
		if( !m_pParentBox || !uLevel-- )
			return 0;

		return (unsigned int) ( m_pParentBox->GetPositionY( uLevel ) + m_pParentBox->GetHeight( uLevel ) * m_dPositionY );
	}
	
	inline unsigned int GetWidth( unsigned int uLevel = -1 ) const override
	{
		if( !m_pParentBox || !uLevel-- )
			return matImage.cols;

		return (unsigned int) ( m_pParentBox->GetWidth( uLevel ) * m_dWidth );
	}
	
	inline unsigned int GetHeight( unsigned int uLevel = -1 ) const override
	{
		if( !m_pParentBox || !uLevel-- )
			return matImage.rows;

		return (unsigned int) ( m_pParentBox->GetHeight( uLevel ) * m_dHeight );
	}
	
	CImage *GetImage( unsigned int uLevel = -1 ) override
	{
		if( !m_pParentBox || !uLevel-- )
			return this;

		return m_pParentBox->GetImage( uLevel );
	}
	
	inline void TransferOwnership( unsigned int uLevel = 1 ) override
	{
		if( !m_pParentBox )
			return;

		CBaseBBox::TransferOwnership( uLevel );
	}

	inline void TransferOwnership( CBaseBBox &parentBox ) override
	{
		if( !m_pParentBox )
			return;

		CBaseBBox::TransferOwnership( parentBox );
	}

	inline void Show( const char *szWindow )
	{
		cv::imshow( szWindow, matImage );
	}
	
	void Crop( CBBox &box, unsigned int uLevel = 0 );

	cv::Mat matImage;
};

#if 0
	inline CImage &operator=( const CImage &other )
	{
		memcpy( this, &other, sizeof( CImage ) );
		return *this;
	}
#endif