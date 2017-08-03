#pragma once

#include "RenderObject.h"
#include "../Image.h"
#include "Matrix.h"

class CRenderLine;
class CRenderPlane : public CRenderObject
{
public:
	enum
	{
		PLANE_LEFT = 1,
		PLANE_BOTTOM = 2,
		PLANE_RIGHT = 4,
		PLANE_TOP = 8
	};

	CRenderPlane( const std::array<CVector<3>, 4> &avec3Points );
	~CRenderPlane( void ) override;
	CVector<3> GetMin( void ) const override;
	CVector<3> GetMax( void ) const override;
	void RenderFrame( CImage &img, const cv::Scalar &color, int iThickness = 2 ) const;
	void RenderContent( CImage &img, const cv::Scalar &color ) const;
	void Transform( const CMatrix<3, 3> &mat );
	void Shift( const CVector<3> &vec3 );
	CRenderLine GetLine( unsigned char fLine ) const;
	CRenderLine GetLines( unsigned char &fLine ) const;
	
	std::string ToString( unsigned int uPrecision = 2 ) const override;

private:
	std::array<unsigned char, 2> GetLineIndices( unsigned char &fLine ) const;
	CVector<3> m_avec3Points[ 4 ];
};

#include "RenderLine.h"

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

inline CRenderPlane::~CRenderPlane( void )
{
	
}

inline void CRenderPlane::RenderFrame( CImage &img, const cv::Scalar &color, int iThickness ) const
{
	unsigned char fLine = PLANE_LEFT | PLANE_BOTTOM | PLANE_RIGHT | PLANE_TOP;
	while( fLine )
		GetLines( fLine ).RenderFrame( img, color, iThickness );
}

inline CRenderLine CRenderPlane::GetLine( unsigned char fLine ) const
{
	return GetLines( fLine );
}

inline CRenderLine CRenderPlane::GetLines( unsigned char &fLine ) const
{
	std::array<unsigned char, 2> abIndices = GetLineIndices( fLine );
	return CRenderLine(
	{
		m_avec3Points[ abIndices[ 0 ] ],
		m_avec3Points[ abIndices[ 1 ] ]
	} );
}

inline std::string CRenderPlane::ToString( unsigned int uPrecision ) const
{
	return m_avec3Points[ 0 ].ToString( uPrecision ) + ", " + m_avec3Points[ 1 ].ToString( uPrecision ) + ", " + m_avec3Points[ 2 ].ToString( uPrecision ) + ", " + m_avec3Points[ 3 ].ToString( uPrecision );
}