#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>
#include "BaseBBox.h"

class CBBox;
class CImage : public CBaseBBox
{
public:
	static void GetScreenResolution( unsigned int &uWidth, unsigned int &uHeight );

	CImage( void );
	CImage( const char *szName );
	CImage( const cv::Mat &mat, const char *szName );
	CImage( const cv::Mat &mat, double dFOV, time_t timestamp, const char *szName );
	CImage( CImage &img, const char *szName );
	CImage( CImage &parentImage, cv::Mat &matImage, const cv::Point &point, const char *szName );
	CImage( CImage &imgLeft, CImage &imgRight );

	CImage( const CImage &other ) = default;
	CImage &operator=( const CImage &other ) = default;

	CImage( CImage &&other ) = default;
	CImage &operator=( CImage &&other ) = default;

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
	void Show( const char *szWindow ) const;
	void Crop( CBBox &box, unsigned int uLevel = 0 );

	cv::Mat matImage;
	double dFOV;
	time_t timestamp;
};

inline CImage::CImage( void ) :
	dFOV( 0 ),
	timestamp( 0 )
{

}

inline CImage::CImage( const char *szName ) :
	CBaseBBox( szName ),
	dFOV( 0 ),
	timestamp( 0 )
{

}

inline CImage::CImage( const cv::Mat &mat, const char *szName ) :
	CBaseBBox( szName ),
	matImage( mat.clone( ) ),
	dFOV( 0 ),
	timestamp( 0 )
{

}

inline CImage::CImage( const cv::Mat &mat, double dFOV, time_t timestamp, const char *szName ) :
	CBaseBBox( szName ),
	matImage( mat.clone( ) ),
	dFOV( dFOV ),
	timestamp( timestamp )
{

}

inline CImage::CImage( CImage &img, const char *szName ) :
	CBaseBBox( img, 0, 0, 1, 1, szName ),
	matImage( img.matImage.clone( ) ),
	dFOV( img.dFOV ),
	timestamp( img.timestamp )
{

}

inline CImage::CImage( CImage &parentImage, cv::Mat &matImage, const cv::Point &point, const char *szName ) :
	CBaseBBox( parentImage, point.x / (double) parentImage.GetWidth( 0 ), point.y / (double) parentImage.GetHeight( 0 ), matImage.cols / (double) parentImage.GetWidth( 0 ), matImage.rows / (double) parentImage.GetHeight( 0 ), szName ),
	matImage( matImage ),
	dFOV( 0 ),
	timestamp( 0 )
{
	//printf( "Creating image \"%s\" of dim %ux%u/%4.2fx%4.2f at (%u, %u)/(%4.2f, %4.2f); Parent dim: %ux%u\n", szName, matImage.cols, matImage.rows, m_rWidth, m_rHeight, point.x, point.y, m_rPositionX, m_rPositionY, parentImage.GetWidth( 0 ), parentImage.GetHeight( 0 ) );
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

inline void CImage::TransferOwnership( CBaseBBox &parentBox )
{
	if( !m_pParentBox )
		return;

	CBaseBBox::TransferOwnership( parentBox );
}