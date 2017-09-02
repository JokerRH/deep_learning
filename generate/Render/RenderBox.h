#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "RenderObject.h"
#include <opencv2\core.hpp>

class CRenderLine;
class CRenderPlane;
class CRenderBox : public CRenderObject
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
	CRenderBox( const CVector<3> &vec3Min, const CVector<3> &vec3Max );
	~CRenderBox( void ) override;
	CVector<3> GetMin( void ) const override;
	CVector<3> GetMax( void ) const override;
	void RenderFrame( cv::Mat &matImage, const cv::Scalar &color, int iThickness = 2 ) const;
	void RenderContent( cv::Mat &matImage, const cv::Scalar &color ) const;
	void Transform( const CMatrix<3, 3> &mat );
	CRenderBox &Shift( const CVector<3> &vec3 );
	CRenderBox Shifted( const CVector<3> &vec3 ) const;
	CRenderLine GetLine( unsigned char fPlane, unsigned char fLine ) const;
	CRenderLine GetLines( unsigned char &fPlane, unsigned char &fLine ) const;
	CRenderPlane GetPlane( unsigned char fPlane ) const;
	CRenderPlane GetPlanes( unsigned char &fPlane ) const;
	
	std::string ToString( unsigned int uPrecision = 2 ) const override;

//private:
	std::array<unsigned char, 2> GetLineIndices( unsigned char &fPlane, unsigned char &fLine ) const;
	std::array<unsigned char, 4> GetPlaneIndices( unsigned char &fPlane ) const;
	CVector<3> m_avec3Points[ 8 ];
};

#include "RenderLine.h"
#include "RenderPlane.h"

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

inline CRenderBox::~CRenderBox( void )
{
	
}

inline void CRenderBox::RenderFrame( cv::Mat &matImage, const cv::Scalar &color, int iThickness ) const
{
	unsigned char fPlane = BOX_FRONT | BOX_BACK | BOX_LEFT | BOX_RIGHT | BOX_TOP | BOX_BOTTOM;
	unsigned char fLine;
	while( fPlane )
	{
		fLine = CRenderPlane::PLANE_LEFT | CRenderPlane::PLANE_BOTTOM | CRenderPlane::PLANE_RIGHT | CRenderPlane::PLANE_TOP;
		while( fLine )
			GetLines( fPlane, fLine ).RenderFrame( matImage, color, iThickness );
	}
}

inline void CRenderBox::RenderContent( cv::Mat &matImage, const cv::Scalar &color ) const
{
	unsigned char fPlane = BOX_FRONT | BOX_BACK | BOX_LEFT | BOX_RIGHT | BOX_TOP | BOX_BOTTOM;
	while( fPlane )
		GetPlanes( fPlane ).RenderContent( matImage, color );
}

inline CRenderBox CRenderBox::Shifted( const CVector<3> &vec3 ) const
{
	return CRenderBox( *this ).Shift( vec3 );
}

inline CRenderLine CRenderBox::GetLine( unsigned char fPlane, unsigned char fLine ) const
{
	return GetLines( fPlane, fLine );
}

inline CRenderLine CRenderBox::GetLines( unsigned char &fPlane, unsigned char &fLine ) const
{
	std::array<unsigned char, 2> abIndices = GetLineIndices( fPlane, fLine );
	return CRenderLine(
	{
		m_avec3Points[ abIndices[ 0 ] ],
		m_avec3Points[ abIndices[ 1 ] ]
	} );
}

inline CRenderPlane CRenderBox::GetPlane( unsigned char fPlane ) const
{
	return GetPlanes( fPlane );
}

inline CRenderPlane CRenderBox::GetPlanes( unsigned char &fPlane ) const
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

inline std::string CRenderBox::ToString( unsigned int uPrecision ) const
{
	return m_avec3Points[ 0 ].ToString( uPrecision ) + ", " + m_avec3Points[ 1 ].ToString( uPrecision ) + ", " + m_avec3Points[ 2 ].ToString( uPrecision ) + ", " + m_avec3Points[ 3 ].ToString( uPrecision ) + ", " + m_avec3Points[ 4 ].ToString( uPrecision ) + ", " + m_avec3Points[ 5 ].ToString( uPrecision ) + ", " + m_avec3Points[ 6 ].ToString( uPrecision ) + ", " + m_avec3Points[ 7 ].ToString( uPrecision );
}