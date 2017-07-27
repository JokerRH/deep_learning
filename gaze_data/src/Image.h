#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "BaseBBox.h"

class CBBox;
class CImage : public CBaseBBox
{
public:
	CImage( void ) :
		CBaseBBox( "Image_Unassigned" )
	{

	}

	CImage( const char *szName ) :
		CBaseBBox( szName )
	{

	}
	
	CImage( const char *szName, cv::Mat &mat ) :
		CBaseBBox( szName ),
		pmatImage( &mat )
	{

	}
	
	CImage( const CImage &img ) :
		CBaseBBox( img ),
		pmatImage( img.pmatImage )
	{

	}
	
	inline CImage( CImage &img, const char *szName, cv::Mat &mat ) :
		CBaseBBox( 0, 0, 1, 1, img, szName ),
		pmatImage( &mat )
	{
		*pmatImage = img.pmatImage->clone( );
	}

	inline CImage( CImage &parentImage, cv::Mat &matImage, const cv::Point &point, const char *szName ) :
		CBaseBBox( point.x / (float) parentImage.GetWidth( 0 ), point.y / (float) parentImage.GetHeight( 0 ), matImage.cols / (float) parentImage.GetWidth( 0 ), matImage.rows / (float) parentImage.GetHeight( 0 ), parentImage, szName ),
		pmatImage( &matImage )
	{
		//printf( "Creating image \"%s\" of dim %ux%u/%4.2fx%4.2f at (%u, %u)/(%4.2f, %4.2f); Parent dim: %ux%u\n", szName, matImage.cols, matImage.rows, m_rWidth, m_rHeight, point.x, point.y, m_rPositionX, m_rPositionY, parentImage.GetWidth( 0 ), parentImage.GetHeight( 0 ) );
	}

	inline unsigned int GetPositionX( unsigned int uLevel = -1 ) const override
	{
		if( !m_pParentBox || !uLevel-- )
			return (unsigned int) m_rPositionX;

		return (unsigned int) ( m_pParentBox->GetPositionX( uLevel ) + m_pParentBox->GetWidth( uLevel ) * m_rPositionX );
	}
	
	inline unsigned int GetPositionY( unsigned int uLevel = -1 ) const override
	{
		if( !m_pParentBox || !uLevel-- )
			return (unsigned int) m_rPositionY;

		return (unsigned int) ( m_pParentBox->GetPositionY( uLevel ) + m_pParentBox->GetHeight( uLevel ) * m_rPositionY );
	}
	
	inline unsigned int GetWidth( unsigned int uLevel = -1 ) const override
	{
		if( !m_pParentBox || !uLevel-- )
			return pmatImage->cols;

		return (unsigned int) ( m_pParentBox->GetWidth( uLevel ) * m_rWidth );
	}
	
	inline unsigned int GetHeight( unsigned int uLevel = -1 ) const override
	{
		if( !m_pParentBox || !uLevel-- )
			return pmatImage->rows;

		return (unsigned int) ( m_pParentBox->GetHeight( uLevel ) * m_rHeight );
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
	
	inline void TransferOwnership( CBaseBBox &baseBBox ) override
	{
		if( !m_pParentBox )
			return;

		CBaseBBox::TransferOwnership( baseBBox );
	}
	
	inline CImage &operator=( const CImage &other )
	{
		memcpy( this, &other, sizeof( CImage ) );
		return *this;
	}
	
	inline void Show( const char *szWindow )
	{
		cv::imshow( szWindow, *pmatImage );
	}
	
	void Crop( CBBox &box, unsigned int uLevel = 0 );

	cv::Mat *pmatImage;
};