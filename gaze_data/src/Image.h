#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "BaseBBox.h"

class CBBox;
class CImage : public CBaseBBox
{
public:
	CImage( void );
	CImage( const char *szName );
	CImage( cv::Mat &mat, const char *szName );
	CImage( const CImage &other );
	CImage( CImage &img, const char *szName );
	CImage( CImage &parentImage, cv::Mat &matImage, const cv::Point &point, const char *szName );
	void Swap( CImage &other, bool fSwapChildren = true );
	CImage &operator=( const CImage &other );
	unsigned int GetPositionX( unsigned int uLevel = -1 ) const override;
	unsigned int GetPositionY( unsigned int uLevel = -1 ) const override;
	unsigned int GetWidth( unsigned int uLevel = -1 ) const override;
	unsigned int GetHeight( unsigned int uLevel = -1 ) const override;
	double GetRelPositionX( unsigned int uLevel = -1 ) const override;
	double GetRelPositionY( unsigned int uLevel = -1 ) const override;
	double GetRelWidth( unsigned int uLevel = -1 ) const override;
	double GetRelHeight( unsigned int uLevel = -1 ) const override;
	CImage *GetImage( unsigned int uLevel = -1 ) override;
	void TransferOwnership( unsigned int uLevel = 1 ) override;
	void TransferOwnership( CBaseBBox &parentBox ) override;
	void Show( const char *szWindow );
	void Crop( CBBox &box, unsigned int uLevel = 0 );

	cv::Mat matImage;
};

inline CImage::CImage( void )
{

}

inline CImage::CImage( const char *szName ) :
	CBaseBBox( szName )
{

}

inline CImage::CImage( cv::Mat &mat, const char *szName ) :
	CBaseBBox( szName ),
	matImage( mat )
{

}

inline CImage::CImage( const CImage &other ) :
	CBaseBBox( other ),
	matImage( other.matImage )
{

}

inline CImage::CImage( CImage &img, const char *szName ) :
	CBaseBBox( img, 0, 0, 1, 1, szName ),
	matImage( img.matImage.clone( ) )
{

}

inline CImage::CImage( CImage &parentImage, cv::Mat &matImage, const cv::Point &point, const char *szName ) :
	CBaseBBox( parentImage, point.x / (double) parentImage.GetWidth( 0 ), point.y / (double) parentImage.GetHeight( 0 ), matImage.cols / (double) parentImage.GetWidth( 0 ), matImage.rows / (double) parentImage.GetHeight( 0 ), szName ),
	matImage( matImage )
{
	//printf( "Creating image \"%s\" of dim %ux%u/%4.2fx%4.2f at (%u, %u)/(%4.2f, %4.2f); Parent dim: %ux%u\n", szName, matImage.cols, matImage.rows, m_rWidth, m_rHeight, point.x, point.y, m_rPositionX, m_rPositionY, parentImage.GetWidth( 0 ), parentImage.GetHeight( 0 ) );
}

inline void CImage::Swap( CImage &other, bool fSwapChildren )
{
	CBaseBBox::Swap( other, fSwapChildren );
	cv::swap( matImage, other.matImage );
}

inline CImage &CImage::operator=( const CImage &other )
{
	if( this != &other )
	{
		CImage temp( other );
		Swap( temp, false );
	}
	return *this;
}

inline unsigned int CImage::GetPositionX( unsigned int uLevel ) const
{
	if( !m_pParentBox || !uLevel-- )
		return 0;

	return (unsigned int) ( m_pParentBox->GetPositionX( uLevel ) + m_pParentBox->GetWidth( uLevel ) * m_dPositionX );
}

inline unsigned int CImage::GetPositionY( unsigned int uLevel ) const
{
	if( !m_pParentBox || !uLevel-- )
		return 0;

	return (unsigned int) ( m_pParentBox->GetPositionY( uLevel ) + m_pParentBox->GetHeight( uLevel ) * m_dPositionY );
}

inline unsigned int CImage::GetWidth( unsigned int uLevel ) const
{
	if( !m_pParentBox || !uLevel-- )
		return matImage.cols;

	return (unsigned int) ( m_pParentBox->GetWidth( uLevel ) * m_dWidth );
}

inline unsigned int CImage::GetHeight( unsigned int uLevel ) const
{
	if( !m_pParentBox || !uLevel-- )
		return matImage.rows;

	return (unsigned int) ( m_pParentBox->GetHeight( uLevel ) * m_dHeight );
}

inline double CImage::GetRelPositionX( unsigned int uLevel ) const
{
	if( !m_pParentBox || !uLevel-- )
		return 0;

	return m_pParentBox->GetRelPositionX( uLevel ) + m_pParentBox->GetRelWidth( uLevel ) * m_dPositionX;
}

inline double CImage::GetRelPositionY( unsigned int uLevel ) const
{
	if( !m_pParentBox || !uLevel-- )
		return 0;

	return m_pParentBox->GetRelPositionY( uLevel ) + m_pParentBox->GetRelHeight( uLevel ) * m_dPositionY;
}

inline double CImage::GetRelWidth( unsigned int uLevel ) const
{
	if( !m_pParentBox || !uLevel-- )
		return 1;

	return m_pParentBox->GetRelWidth( uLevel ) * m_dWidth;
}

inline double CImage::GetRelHeight( unsigned int uLevel ) const
{
	if( !m_pParentBox || !uLevel-- )
		return 1;

	return m_pParentBox->GetRelHeight( uLevel ) * m_dHeight;
}

inline CImage *CImage::GetImage( unsigned int uLevel )
{
	if( !m_pParentBox || !uLevel-- )
		return this;

	return m_pParentBox->GetImage( uLevel );
}

inline void CImage::TransferOwnership( unsigned int uLevel )
{
	if( !m_pParentBox )
		return;

	CBaseBBox::TransferOwnership( uLevel );
}

inline void CImage::TransferOwnership( CBaseBBox &parentBox )
{
	if( !m_pParentBox )
		return;

	CBaseBBox::TransferOwnership( parentBox );
}

inline void CImage::Show( const char *szWindow )
{
	cv::imshow( szWindow, matImage );
}