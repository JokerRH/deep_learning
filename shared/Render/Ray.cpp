#include "Ray.h"
#include "Matrix.h"
#include "Transformation.h"
#include <opencv2/imgproc.hpp>
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

CRay::CRay( const CVector<3> &vec3Origin, CVector<2> vec2Amp ) :
	m_vec3Origin( vec3Origin ),
	m_vec3Dir( { 0, 0, 1 } )
{
	vec2Amp *= M_PI / 2;
	m_vec3Dir = CTransformation::GetRotationMatrixRad( vec2Amp[ 0 ], vec2Amp[ 1 ], 0 ) * m_vec3Dir;
}

CVector<2> CRay::PointOfShortestDistance( const CRay &other ) const
{
	double d = other.m_vec3Dir * m_vec3Dir;
	CMatrix<2, 2> mat( { -m_vec3Dir.Abs2( ), d, -d, other.m_vec3Dir.Abs2( ) } );
	CVector<3> vec3OriginDif( m_vec3Origin - other.m_vec3Origin );
	CVector<2> vec2Const( { vec3OriginDif * m_vec3Dir, vec3OriginDif * other.m_vec3Dir } );
	return mat.Invert( ) * vec2Const;
}

CVector<2> CRay::AmplitudeRepresentation( void ) const
{
	CVector<2> vec2Angles( {
		atan( m_vec3Dir[ 1 ] / m_vec3Dir[ 2 ] ),															//Pitch
		atan( m_vec3Dir[ 0 ] / sqrt( m_vec3Dir[ 1 ] * m_vec3Dir[ 1 ] + m_vec3Dir[ 2 ] * m_vec3Dir[ 2 ] ) )	//Yaw
	} );
	
	vec2Angles /= M_PI / 2;
	return vec2Angles;
}

CVector<3> CRay::GetMin( void ) const
{
	return CVector<3>::GetMin( { m_vec3Origin, m_vec3Origin + m_vec3Dir } );
}

CVector<3> CRay::GetMax( void ) const
{
	return CVector<3>::GetMax( { m_vec3Origin, m_vec3Origin + m_vec3Dir } );
}

void CRay::Render( cv::Mat &matImage, const cv::Scalar &colorPoint, const cv::Scalar &colorLine, double dLength, int iRadius, int iPointThickness, int iLineThickness ) const
{
	CVector<3> vec3Endp = ( *this )( dLength );
	cv::Point pt1( (int) ( ( m_vec3Origin[ 0 ] + 0.5 ) * matImage.cols ), matImage.rows - (int) ( ( m_vec3Origin[ 1 ] + 0.5 ) * matImage.rows ) );
	cv::Point pt2( (int) ( ( vec3Endp[ 0 ] + 0.5 ) * matImage.cols ), matImage.rows - (int) ( ( vec3Endp[ 1 ] + 0.5 ) * matImage.rows ) );
	cv::circle( matImage, pt1, iRadius, colorPoint, iPointThickness );
	cv::line( matImage, pt1, pt2, colorLine, iLineThickness );
}