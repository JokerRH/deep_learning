#include "RenderPlane.h"

CVector<3> CRenderPlane::GetMin( void ) const
{
	return CVector<3>(
	{
		std::min( { m_avec3Points[ 0 ][ 0 ], m_avec3Points[ 1 ][ 0 ], m_avec3Points[ 2 ][ 0 ], m_avec3Points[ 3 ][ 0 ] } ),
		std::min( { m_avec3Points[ 0 ][ 1 ], m_avec3Points[ 1 ][ 1 ], m_avec3Points[ 2 ][ 1 ], m_avec3Points[ 3 ][ 1 ] } ),
		std::min( { m_avec3Points[ 0 ][ 2 ], m_avec3Points[ 1 ][ 2 ], m_avec3Points[ 2 ][ 2 ], m_avec3Points[ 3 ][ 2 ] } )
	} );
}

CVector<3> CRenderPlane::GetMax( void ) const
{
	return CVector<3>(
	{
		std::max( { m_avec3Points[ 0 ][ 0 ], m_avec3Points[ 1 ][ 0 ], m_avec3Points[ 2 ][ 0 ], m_avec3Points[ 3 ][ 0 ] } ),
		std::max( { m_avec3Points[ 0 ][ 1 ], m_avec3Points[ 1 ][ 1 ], m_avec3Points[ 2 ][ 1 ], m_avec3Points[ 3 ][ 1 ] } ),
		std::max( { m_avec3Points[ 0 ][ 2 ], m_avec3Points[ 1 ][ 2 ], m_avec3Points[ 2 ][ 2 ], m_avec3Points[ 3 ][ 2 ] } )
	} );
}

void CRenderPlane::RenderContent( cv::Mat &matImage, const cv::Scalar &color ) const
{
	cv::Point aPoints[ 4 ] =
	{
		cv::Point( (int) ( m_avec3Points[ 0 ][ 0 ] * matImage.cols ), matImage.rows - (int) ( m_avec3Points[ 0 ][ 1 ] * matImage.rows ) ),
		cv::Point( (int) ( m_avec3Points[ 1 ][ 0 ] * matImage.cols ), matImage.rows - (int) ( m_avec3Points[ 1 ][ 1 ] * matImage.rows ) ),
		cv::Point( (int) ( m_avec3Points[ 2 ][ 0 ] * matImage.cols ), matImage.rows - (int) ( m_avec3Points[ 2 ][ 1 ] * matImage.rows ) ),
		cv::Point( (int) ( m_avec3Points[ 3 ][ 0 ] * matImage.cols ), matImage.rows - (int) ( m_avec3Points[ 3 ][ 1 ] * matImage.rows ) ),
	};
	fillConvexPoly( matImage, aPoints, 4, color );
}

void CRenderPlane::Transform( const CMatrix<3, 3> &mat )
{
	m_avec3Points[ 0 ] = mat * m_avec3Points[ 0 ];
	m_avec3Points[ 1 ] = mat * m_avec3Points[ 1 ];
	m_avec3Points[ 2 ] = mat * m_avec3Points[ 2 ];
	m_avec3Points[ 3 ] = mat * m_avec3Points[ 3 ];
}

CRenderPlane &CRenderPlane::Shift( const CVector<3> &vec3 )
{
	m_avec3Points[ 0 ] += vec3;
	m_avec3Points[ 1 ] += vec3;
	m_avec3Points[ 2 ] += vec3;
	m_avec3Points[ 3 ] += vec3;
	return *this;
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