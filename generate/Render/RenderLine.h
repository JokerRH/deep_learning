#pragma once

#include "RenderObject.h"
#include "Matrix.h"
#include <sstream>
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
	CRenderLine &Shift( const CVector<3> &vec3 );
	CRenderLine Shifted( const CVector<3> &vec3 ) const;
	
	friend std::wostream &operator<<( std::wostream &smOut, const CRenderLine &line );
	std::wstring ToString( unsigned int uPrecision = 2 ) const override;

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
	cv::Point pt1( (int) ( m_avec3Points[ 0 ][ 0 ] * matImage.cols ), matImage.rows - (int) ( m_avec3Points[ 0 ][ 1 ] * matImage.rows ) );
	cv::Point pt2( (int) ( m_avec3Points[ 1 ][ 0 ] * matImage.cols ), matImage.rows - (int) ( m_avec3Points[ 1 ][ 1 ] * matImage.rows ) );
	cv::line( matImage, pt1, pt2, color, iThickness );
}

inline CRenderLine CRenderLine::Shifted( const CVector<3> &vec3 ) const
{
	return CRenderLine( *this ).Shift( vec3 );
}

inline std::wostream &operator<<( std::wostream &smOut, const CRenderLine &line )
{
	smOut << line.m_avec3Points[ 0 ] << ", " << line.m_avec3Points[ 1 ];
	return smOut;
}

inline std::wstring CRenderLine::ToString( unsigned int uPrecision ) const
{
	std::wostringstream smOut;
	smOut.setf( std::ios_base::fixed, std::ios_base::floatfield );
	smOut.precision( uPrecision );

	operator<<( smOut, *this );
	return smOut.str( );
}