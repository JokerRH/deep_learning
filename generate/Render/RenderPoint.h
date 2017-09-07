#pragma once

#include "RenderObject.h"
#include "Matrix.h"
#include <sstream>
#include <opencv2\core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class CRenderPoint : public CRenderObject
{
public:
	CRenderPoint( const CVector<3> &vec3Point );
	~CRenderPoint( void ) override;
	CVector<3> GetMin( void ) const override;
	CVector<3> GetMax( void ) const override;
	void RenderPoints( cv::Mat &matImage, const cv::Scalar &color, int iRadius = 1, int iThickness = -1 ) const;
	void Transform( const CMatrix<3, 3> &mat );
	CRenderPoint &Shift( const CVector<3> &vec3 );
	CRenderPoint Shifted( const CVector<3> &vec3 ) const;
	
	friend std::wostream &operator<<( std::wostream &smOut, const CRenderPoint &point );
	std::wstring ToString( unsigned int uPrecision = 2 ) const override;

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

inline void CRenderPoint::RenderPoints( cv::Mat &matImage, const cv::Scalar &color, int iRadius, int iThickness ) const
{
	cv::circle( matImage, cv::Point( (int) ( m_vec3Point[ 0 ] * matImage.cols ), matImage.rows - (int) ( m_vec3Point[ 1 ] * matImage.rows ) ), iRadius, color, iThickness );
}

inline void CRenderPoint::Transform( const CMatrix<3, 3> &mat )
{
	m_vec3Point = mat * m_vec3Point;
}

inline CRenderPoint &CRenderPoint::Shift( const CVector<3> &vec3 )
{
	m_vec3Point += vec3;
	return *this;
}

inline CRenderPoint CRenderPoint::Shifted( const CVector<3> &vec3 ) const
{
	return CRenderPoint( *this ).Shift( vec3 );
}

inline std::wostream &operator<<( std::wostream &smOut, const CRenderPoint &point )
{
	smOut << point.m_vec3Point;
	return smOut;
}

inline std::wstring CRenderPoint::ToString( unsigned int uPrecision ) const
{
	std::wostringstream smOut;
	smOut.setf( std::ios_base::fixed, std::ios_base::floatfield );
	smOut.precision( uPrecision );

	operator<<( smOut, *this );
	return smOut.str( );
}