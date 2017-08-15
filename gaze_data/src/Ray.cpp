#include "Ray.h"
#include "Render/Matrix.h"
#include "Render/RenderHelper.h"
#include <opencv2/imgproc/imgproc.hpp>
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

CRay::CRay( const CVector<3> &vec3Origin, const CVector<3> &vec3Fwd, CVector<2> vec2Amp ) :
	m_vec3Origin( vec3Origin ),
	m_vec3Dir( vec3Fwd )
{
	vec2Amp *= M_PI / 2;
	m_vec3Dir = CRenderHelper::GetRotationMatrixRad( vec2Amp[ 0 ], vec2Amp[ 1 ], 0 ) * m_vec3Dir;
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
	CVector<2> vec2Angles( { 0 } );
	
	vec2Angles[ 0 ] = -atan( m_vec3Dir[ 1 ] / m_vec3Dir[ 2 ] );																//Pitch
	vec2Angles[ 1 ] = -atan( m_vec3Dir[ 0 ] / sqrt( m_vec3Dir[ 1 ] * m_vec3Dir[ 1 ] + m_vec3Dir[ 2 ] * m_vec3Dir[ 2 ] ) );	//Yaw
	
	vec2Angles /= M_PI / 2;
	return vec2Angles;
}

void CRay::Render( CImage &img, const cv::Scalar &colorPoint, const cv::Scalar &colorLine, double dLength, int iRadius, int iPointThickness, int iLineThickness ) const
{
	CVector<3> vec3Endp = ( *this )( dLength );
	cv::Point pt1( (int) ( m_vec3Origin[ 0 ] * img.matImage.cols ), (int) ( m_vec3Origin[ 1 ] * img.matImage.rows ) );
	cv::Point pt2( (int) ( vec3Endp[ 0 ] * img.matImage.cols ), (int) ( vec3Endp[ 1 ] * img.matImage.rows ) );
	cv::circle( img.matImage, pt1, iRadius, colorPoint, iPointThickness );
	cv::line( img.matImage, pt1, pt2, colorLine, iLineThickness );
}