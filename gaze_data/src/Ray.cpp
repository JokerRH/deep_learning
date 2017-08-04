#include "Ray.h"
#include "Render/Matrix.h"
#include <opencv2/imgproc/imgproc.hpp>

CVector<2> CRay::PointOfShortestDistance( const CRay &other ) const
{
	double d = other.m_vec3Dir * m_vec3Dir;
	CMatrix<2, 2> mat( { -m_vec3Dir.Abs2( ), d, -d, other.m_vec3Dir.Abs2( ) } );
	CVector<3> vec3OriginDif( m_vec3Origin - other.m_vec3Origin );
	CVector<2> vec2Const( { vec3OriginDif * m_vec3Dir, vec3OriginDif * other.m_vec3Dir } );
	return mat.Invert( ) * vec2Const;
}

void CRay::Render( CImage &img, const cv::Scalar &colorPoint, const cv::Scalar &colorLine, double dLength, int iRadius, int iPointThickness, int iLineThickness ) const
{
	CVector<3> vec3Endp = ( *this )( dLength );
	cv::Point pt1( (int) ( m_vec3Origin[ 0 ] * img.matImage.cols ), (int) ( m_vec3Origin[ 1 ] * img.matImage.rows ) );
	cv::Point pt2( (int) ( vec3Endp[ 0 ] * img.matImage.cols ), (int) ( vec3Endp[ 1 ] * img.matImage.rows ) );
	cv::circle( img.matImage, pt1, iRadius, colorPoint, iPointThickness );
	cv::line( img.matImage, pt1, pt2, colorLine, iLineThickness );
}