#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Image.h"
#include <array>
#include <opencv2/imgproc/imgproc.hpp>

/****************************************\
 * CRenderLine                          *
\****************************************/
class CRenderLine
{
public:
	CRenderLine( const std::array<CVector<3>, 2> &avec3Points );
	void RenderFrame( CImage &img, const cv::Scalar &color, int iThickness = 2 ) const;
	void Transform( const CMatrix<3, 3> &mat );
	void Shift( const CVector<3> &mat );

private:
	CVector<3> m_avec3Points[ 2 ];
};

/****************************************\
 * CRenderPlane                         *
\****************************************/
class CRenderPlane
{
public:
	enum
	{
		PLANE_LEFT = 1,
		PLANE_BOTTOM = 2,
		PLANE_RIGHT = 3,
		PLANE_TOP = 4
	};

	CRenderPlane( const std::array<CVector<3>, 4> &avec3Points );
	void RenderFrame( CImage &img, const cv::Scalar &color, int iThickness = 2 ) const;
	void RenderContent( CImage &img, const cv::Scalar &color ) const;
	void Transform( const CMatrix<3, 3> &mat );
	void Shift( const CVector<3> &mat );
	CRenderLine GetLine( unsigned char &fLine ) const;

private:
	std::array<unsigned char, 2> GetLineIndices( unsigned char &fLine ) const;
	CVector<3> m_avec3Points[ 4 ];
};

/****************************************\
 * CRenderBox                           *
\****************************************/
class CRenderBox
{
public:
	enum
	{
		BOX_FRONT = 1,
		BOX_BACK = 2,
		BOX_LEFT = 4,
		BOX_RIGHT = 8,
		BOX_TOP = 16,
		BOX_BOTTOM = 32
	};

	CRenderBox( const std::array<CVector<3>, 8> &avec3Points );
	void RenderFrame( CImage &img, const cv::Scalar &color, int iThickness = 2 ) const;
	void RenderContent( CImage &img, const cv::Scalar &color ) const;
	void Transform( const CMatrix<3, 3> &mat );
	void Shift( const CVector<3> &mat );
	CRenderLine GetLine( unsigned char &fPlane, unsigned char &fLine ) const;
	CRenderPlane GetPlane( unsigned char &fPlane ) const;

private:
	std::array<unsigned char, 2> GetLineIndices( unsigned char &fPlane, unsigned char &fLine ) const;
	std::array<unsigned char, 4> GetPlaneIndices( unsigned char &fPlane ) const;
	CVector<3> m_avec3Points[ 8 ];
};

/****************************************\
 * CRenderLine                          *
\****************************************/
inline CRenderLine::CRenderLine( const std::array<CVector<3>, 2> &avec3Points ) :
	m_avec3Points
	{
		avec3Points[ 0 ],
		avec3Points[ 1 ]
	}
{
	
}

inline void CRenderLine::RenderFrame( CImage &img, const cv::Scalar &color, int iThickness ) const
{
	cv::Point pt1( m_avec3Points[ 0 ][ 0 ] * img.matImage.cols, m_avec3Points[ 0 ][ 1 ] * img.matImage.rows );
	cv::Point pt2( m_avec3Points[ 1 ][ 0 ] * img.matImage.cols, m_avec3Points[ 1 ][ 1 ] * img.matImage.rows );
	cv::line( img.matImage, pt1, pt2, iThickness );
}

/****************************************\
 * CRenderPlane                         *
\****************************************/
inline CRenderPlane::CRenderPlane( const std::array<CVector<3>, 4> &avec3Points ) :
	m_avec3Points
	{
		avec3Points[ 0 ],
		avec3Points[ 1 ],
		avec3Points[ 2 ],
		avec3Points[ 3 ]
	}
{

}

inline void CRenderPlane::RenderFrame( CImage &img, const cv::Scalar &color, int iThickness ) const
{
	unsigned char fLine = PLANE_LEFT | PLANE_BOTTOM | PLANE_RIGHT | PLANE_TOP;
	while( fLine )
		GetLine( fLine ).RenderFrame( img, color, iThickness );
}

inline CRenderLine CRenderPlane::GetLine( unsigned char &fLine ) const
{
	std::array<unsigned char, 2> abIndices = GetLineIndices( fLine );
	return CRenderLine(
	{
		m_avec3Points[ abIndices[ 0 ] ],
		m_avec3Points[ abIndices[ 1 ] ]
	} );
}

/****************************************\
 * CRenderBox                           *
\****************************************/
inline CRenderBox::CRenderBox( const std::array<CVector<3>, 8> &avec3Points ) :
	m_avec3Points
	{
		avec3Points[ 0 ],
		avec3Points[ 1 ],
		avec3Points[ 2 ],
		avec3Points[ 3 ],
		avec3Points[ 4 ],
		avec3Points[ 5 ],
		avec3Points[ 6 ],
		avec3Points[ 7 ]
	}
{

}

inline void CRenderBox::RenderFrame( CImage &img, const cv::Scalar &color, int iThickness ) const
{
	unsigned char fPlane = BOX_FRONT | BOX_BACK | BOX_LEFT | BOX_RIGHT | BOX_TOP | BOX_BOTTOM;
	unsigned char fLine;
	while( fPlane )
	{
		fLine = CRenderPlane::PLANE_LEFT | CRenderPlane::PLANE_BOTTOM | CRenderPlane::PLANE_RIGHT | CRenderPlane::PLANE_TOP;
		while( fLine )
			GetLine( fPlane, fLine ).RenderFrame( img, color, iThickness );
	}
}

inline void CRenderBox::RenderContent( CImage &img, const cv::Scalar &color ) const
{
	unsigned char fPlane = BOX_FRONT | BOX_BACK | BOX_LEFT | BOX_RIGHT | BOX_TOP | BOX_BOTTOM;
	while( fPlane )
		GetPlane( fPlane ).RenderContent( img, color );
}

inline CRenderLine CRenderBox::GetLine( unsigned char &fPlane, unsigned char &fLine ) const
{
	std::array<unsigned char, 2> abIndices = GetLineIndices( fPlane, fLine );
	return CRenderLine(
	{
		m_avec3Points[ abIndices[ 0 ] ],
		m_avec3Points[ abIndices[ 1 ] ]
	} );
}

inline CRenderPlane CRenderBox::GetPlane( unsigned char &fPlane ) const
{
	std::array<unsigned char, 4> abIndices = GetPlaneIndices( fPlane );
	return CRenderPlane(
	{
		m_avec3Points[ abIndices[ 0 ] ],
		m_avec3Points[ abIndices[ 1 ] ],
		m_avec3Points[ abIndices[ 2 ] ],
		m_avec3Points[ abIndices[ 3 ] ]
	} );
}