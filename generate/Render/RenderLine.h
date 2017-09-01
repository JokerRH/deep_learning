#pragma once

#include "RenderObject.h"
#include "Matrix.h"
#include <opencv2/imgproc/imgproc.hpp>

class CRenderLine : public CRenderObject
{
public:
	CRenderLine( const std::array<CVector<3>, 2> &avec3Points );
	~CRenderLine( void ) override;
	CVector<3> GetMin( void ) const override;
	CVector<3> GetMax( void ) const override;
	void RenderFrame( cv::Mat &matImage, const cv::Scalar &color, int iThickness = 2 ) const;
	void Transform( const CMatrix<3, 3> &mat );
	void Shift( const CVector<3> &vec3 );
	
	std::string ToString( unsigned int uPrecision = 2 ) const override;

private:
	CVector<3> m_avec3Points[ 2 ];
};

inline CRenderLine::CRenderLine( const std::array<CVector<3>, 2> &avec3Points ) :
	m_avec3Points
	{
		avec3Points[ 0 ],
		avec3Points[ 1 ]
	}
{
	
}

inline CRenderLine::~CRenderLine( void )
{
	
}

inline void CRenderLine::RenderFrame( cv::Mat &matImage, const cv::Scalar &color, int iThickness ) const
{
	cv::Point pt1( (int) ( m_avec3Points[ 0 ][ 0 ] * matImage.cols ), (int) ( m_avec3Points[ 0 ][ 1 ] * matImage.rows ) );
	cv::Point pt2( (int) ( m_avec3Points[ 1 ][ 0 ] * matImage.cols ), (int) ( m_avec3Points[ 1 ][ 1 ] * matImage.rows ) );
	cv::line( matImage, pt1, pt2, color, iThickness );
}

inline std::string CRenderLine::ToString( unsigned int uPrecision ) const
{
	return m_avec3Points[ 0 ].ToString( uPrecision ) + ", " + m_avec3Points[ 1 ].ToString( uPrecision );
}