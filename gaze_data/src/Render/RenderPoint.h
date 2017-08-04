#pragma once

#include "RenderObject.h"
#include "Matrix.h"
#include "../Image.h"
#include <opencv2/imgproc/imgproc.hpp>

class CRenderPoint : public CRenderObject
{
public:
	CRenderPoint( const CVector<3> &vec3Point );
	~CRenderPoint( void ) override;
	CVector<3> GetMin( void ) const override;
	CVector<3> GetMax( void ) const override;
	void RenderPoints( CImage &img, const cv::Scalar &color, int iRadius = 1, int iThickness = -1 ) const;
	void Transform( const CMatrix<3, 3> &mat );
	void Shift( const CVector<3> &vec3 );
	
	std::string ToString( unsigned int uPrecision = 2 ) const override;

private:
	CVector<3> m_vec3Point;
};

inline CRenderPoint::CRenderPoint( const CVector<3> &vec3Point ) :
	m_vec3Point( vec3Point )
{

}

inline CRenderPoint::~CRenderPoint( void )
{

}

inline CVector<3> CRenderPoint::GetMin( void ) const
{
	return m_vec3Point;
}

inline CVector<3> CRenderPoint::GetMax( void ) const
{
	return m_vec3Point;
}

inline void CRenderPoint::RenderPoints( CImage &img, const cv::Scalar &color, int iRadius, int iThickness ) const
{
	cv::circle( img.matImage, cv::Point( (int) ( m_vec3Point[ 0 ] * img.matImage.cols ), (int) ( m_vec3Point[ 1 ] * img.matImage.rows ) ), iRadius, color, iThickness );
}

inline void CRenderPoint::Transform( const CMatrix<3, 3> &mat )
{
	m_vec3Point = mat * m_vec3Point;
}

inline void CRenderPoint::Shift( const CVector<3> &vec3 )
{
	m_vec3Point += vec3;
}
	
inline std::string CRenderPoint::ToString( unsigned int uPrecision ) const
{
	return m_vec3Point.ToString( uPrecision );
}