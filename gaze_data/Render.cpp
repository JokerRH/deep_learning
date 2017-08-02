#include "Render.h"

/****************************************\
 * CRenderLine                          *
\****************************************/
void CRenderLine::Transform( const CMatrix<3, 3> &mat )
{
	m_avec3Points[ 0 ] = mat * m_avec3Points[ 0 ];
	m_avec3Points[ 1 ] = mat * m_avec3Points[ 1 ];
}

void CRenderLine::Shift( const CVector<3> &vec )
{
	m_avec3Points[ 0 ] += vec;
	m_avec3Points[ 1 ] += vec;
}

/****************************************\
 * CRenderPlane                         *
\****************************************/
void CRenderPlane::RenderContent( CImage &img, const cv::Scalar &color ) const
{
	unsigned int uWidth = img.matImage.cols;
	unsigned int uHeight = img.matImage.rows;
	cv::Point aPoints[ 4 ] =
	{
		cv::Point( m_avec3Points[ 0 ][ 0 ] * uWidth, m_avec3Points[ 0 ][ 1 ] * uHeight ),
		cv::Point( m_avec3Points[ 1 ][ 0 ] * uWidth, m_avec3Points[ 1 ][ 1 ] * uHeight ),
		cv::Point( m_avec3Points[ 2 ][ 0 ] * uWidth, m_avec3Points[ 2 ][ 1 ] * uHeight ),
		cv::Point( m_avec3Points[ 3 ][ 0 ] * uWidth, m_avec3Points[ 3 ][ 1 ] * uHeight ),
	};
	fillConvexPoly( img.matImage, aPoints, 4, color );
}

void CRenderPlane::Transform( const CMatrix<3, 3> &mat )
{
	m_avec3Points[ 0 ] = mat * m_avec3Points[ 0 ];
	m_avec3Points[ 1 ] = mat * m_avec3Points[ 1 ];
	m_avec3Points[ 2 ] = mat * m_avec3Points[ 2 ];
	m_avec3Points[ 3 ] = mat * m_avec3Points[ 3 ];
}

void CRenderPlane::Shift( const CVector<3> &vec )
{
	m_avec3Points[ 0 ] += vec;
	m_avec3Points[ 1 ] += vec;
	m_avec3Points[ 2 ] += vec;
	m_avec3Points[ 3 ] += vec;
}

std::array<unsigned char, 2> CRenderPlane::GetLineIndices( unsigned char &fLine ) const
{
	std::array<unsigned char, 2> abIndices;
	if( fLine & PLANE_LEFT )
	{
		abIndices[ 0 ] = 0;
		abIndices[ 1 ] = 1;
		fLine &= ~PLANE_LEFT;
	}
	else if( fLine & PLANE_BOTTOM )
	{
		abIndices[ 0 ] = 1;
		abIndices[ 1 ] = 2;
		fLine &= ~PLANE_BOTTOM;
	}
	else if( fLine & PLANE_RIGHT )
	{
		abIndices[ 0 ] = 2;
		abIndices[ 1 ] = 3;
		fLine &= ~PLANE_RIGHT;
	}
	else if( fLine & PLANE_TOP )
	{
		abIndices[ 0 ] = 3;
		abIndices[ 1 ] = 0;
		fLine &= ~PLANE_TOP;
	}
	else
	{
		fprintf( stderr, "Unknown line in CRenderPlane::GetLineIndices\n" );
		assert( false );
	}

	return abIndices;
}

/****************************************\
 * CRenderBox                           *
\****************************************/
void CRenderBox::Transform( const CMatrix<3, 3> &mat )
{
	m_avec3Points[ 0 ] = mat * m_avec3Points[ 0 ];
	m_avec3Points[ 1 ] = mat * m_avec3Points[ 1 ];
	m_avec3Points[ 2 ] = mat * m_avec3Points[ 2 ];
	m_avec3Points[ 3 ] = mat * m_avec3Points[ 3 ];
	m_avec3Points[ 4 ] = mat * m_avec3Points[ 4 ];
	m_avec3Points[ 5 ] = mat * m_avec3Points[ 5 ];
	m_avec3Points[ 6 ] = mat * m_avec3Points[ 6 ];
	m_avec3Points[ 7 ] = mat * m_avec3Points[ 7 ];
}

void CRenderBox::Shift( const CVector<3> &vec )
{
	m_avec3Points[ 0 ] += vec;
	m_avec3Points[ 1 ] += vec;
	m_avec3Points[ 2 ] += vec;
	m_avec3Points[ 3 ] += vec;
	m_avec3Points[ 4 ] += vec;
	m_avec3Points[ 5 ] += vec;
	m_avec3Points[ 6 ] += vec;
	m_avec3Points[ 7 ] += vec;
}

#define BOX( plane, a, b, c, d )\
	if( fPlane & plane )\
	{\
		if( fLine & CRenderPlane::PLANE_LEFT )\
		{\
			abIndices[ 0 ] = a;\
			abIndices[ 0 ] = b;\
			fLine &= ~CRenderPlane::PLANE_LEFT;\
		}\
		else if( fLine & CRenderPlane::PLANE_BOTTOM )\
		{\
			abIndices[ 0 ] = b;\
			abIndices[ 1 ] = c;\
			fLine &= ~CRenderPlane::PLANE_BOTTOM;\
		}\
		else if( fLine & CRenderPlane::PLANE_RIGHT )\
		{\
			abIndices[ 0 ] = c;\
			abIndices[ 1 ] = d;\
			fLine &= ~CRenderPlane::PLANE_RIGHT;\
		}\
		else if( fLine & CRenderPlane::PLANE_TOP )\
		{\
			abIndices[ 0 ] = d;\
			abIndices[ 1 ] = a;\
			fLine &= ~CRenderPlane::PLANE_TOP;\
		}\
		else\
		{\
			fprintf( stderr, "Unknown line in CRenderBox::GetLineIndices\n" );\
			assert( false );\
		}\
		if( !fLine )\
			fPlane &= ~plane;\
	}

std::array<unsigned char, 2> CRenderBox::GetLineIndices( unsigned char &fPlane, unsigned char &fLine ) const
{
	std::array<unsigned char, 2> abIndices;
	BOX( BOX_FRONT, 0, 1, 2, 3 )
	else BOX( BOX_BACK, 7, 6, 5, 4 )
	else BOX( BOX_LEFT, 4, 5, 1, 0 )
	else BOX( BOX_RIGHT, 3, 2, 6, 7 )
	else BOX( BOX_TOP, 4, 0, 3, 7 )
	else BOX( BOX_BOTTOM, 0, 5, 6, 2 )
	else
	{
		fprintf( stderr, "Unknown plane in CRenderBox::GetLineIndices\n" );
		assert( false );
	}
	
	return abIndices;
}
#undef BOX

std::array<unsigned char, 4> CRenderBox::GetPlaneIndices( unsigned char &fPlane ) const
{
	std::array<unsigned char, 4> abIndices;
	if( fPlane & BOX_FRONT )
	{
		abIndices[ 0 ] = 0;
		abIndices[ 1 ] = 1;
		abIndices[ 2 ] = 2;
		abIndices[ 3 ] = 3;
		fPlane &= ~BOX_FRONT;
	}
	else if( fPlane & BOX_BACK )
	{
		abIndices[ 0 ] = 4;
		abIndices[ 1 ] = 5;
		abIndices[ 2 ] = 6;
		abIndices[ 3 ] = 7;
		fPlane &= ~BOX_BACK;
	}
	else if( fPlane & BOX_LEFT )
	{
		abIndices[ 0 ] = 0;
		abIndices[ 1 ] = 4;
		abIndices[ 2 ] = 5;
		abIndices[ 3 ] = 1;
		fPlane &= ~BOX_LEFT;
	}
	else if( fPlane & BOX_RIGHT )
	{
		abIndices[ 0 ] = 3;
		abIndices[ 1 ] = 2;
		abIndices[ 2 ] = 6;
		abIndices[ 3 ] = 7;
		fPlane &= ~BOX_RIGHT;
	}
	else if( fPlane & BOX_TOP )
	{
		abIndices[ 0 ] = 0;
		abIndices[ 1 ] = 3;
		abIndices[ 2 ] = 7;
		abIndices[ 3 ] = 4;
		fPlane &= ~BOX_TOP;
	}
	else if( fPlane & BOX_BOTTOM )
	{
		abIndices[ 0 ] = 1;
		abIndices[ 1 ] = 2;
		abIndices[ 2 ] = 6;
		abIndices[ 3 ] = 5;
		fPlane &= ~BOX_BOTTOM;
	}
	else
	{
		fprintf( stderr, "Unknown plane in CRenderBox::GetPlaneIndices\n" );
		assert( false );
	}

	return abIndices;
}