#pragma once

#include "Render/Vector.h"
#include "Render/Matrix.h"
#include "Image.h"
#include <opencv2/core/types.hpp>

class CRay
{
public:
	CRay( const CVector<3> &vec3Origin, const CVector<3> &vec3Dir );
	CRay( const CVector<3> &vec3Origin, const CVector<3> &vec3Fwd, CVector<2> vec2Amp );
	CRay( void );

	CRay( const CRay &other ) = default;
	CRay &operator=( const CRay &other ) = default;

	CRay( CRay &&other ) = default;
	CRay &operator=( CRay &&other ) = default;

	CVector<3> operator()( const double &other ) const;
	
	/**
	 * @brief Calculates the points of shortest distance
	 * @param other Vector to calculate the distance from
	 * @return Vector containing the multiplicator for the closest points. First value is index for \e this, second is for \e other
	 */
	CVector<2> PointOfShortestDistance( const CRay &other ) const;
	CRay Transformed( const CMatrix<3, 3> &matTransform ) const;
	CRay &Transform( const CMatrix<3, 3> &matTransform );
	CRay Shifted( const CVector<3> &vec3 ) const;
	CRay &Shift( const CVector<3> &vec3 );
	CRay &operator*=( const double &other );
	CRay &operator/=( const double &other );
	CVector<2> AmplitudeRepresentation( void ) const;	
	
	void Render( CImage &img, const cv::Scalar &colorPoint, const cv::Scalar &colorLine, double dLength = 1, int iRadius = 3, int iPointThickness = -1, int iLineThickness = 1 ) const;

	std::string ToString( unsigned int uPrecision = 2 ) const;

	CVector<3> m_vec3Origin;
	CVector<3> m_vec3Dir;
};

inline CRay::CRay( const CVector<3> &vec3Origin, const CVector<3> &vec3Dir ) :
	m_vec3Origin( vec3Origin ),
	m_vec3Dir( vec3Dir )
{

}

inline CRay::CRay( void ) :
	m_vec3Origin( { 0 } ),
	m_vec3Dir( { 0 } )
{

}

inline CVector<3> CRay::operator()( const double &other ) const
{
	return m_vec3Origin + m_vec3Dir * other;
}

inline CRay CRay::Transformed( const CMatrix<3, 3> &matTransform ) const
{
	return CRay( matTransform * m_vec3Origin, matTransform * m_vec3Dir );
}

inline CRay &CRay::Transform( const CMatrix<3, 3> &matTransform )
{
	m_vec3Origin = matTransform * m_vec3Origin;
	m_vec3Dir = matTransform * m_vec3Dir;
	return *this;
}

inline CRay CRay::Shifted( const CVector<3> &vec3 ) const
{
	return CRay( m_vec3Origin + vec3, m_vec3Dir );
}

inline CRay &CRay::Shift( const CVector<3> &vec3 )
{
	m_vec3Origin += vec3;
	return *this;
}

inline CRay &CRay::operator*=( const double &other )
{
	m_vec3Dir *= other;
	return *this;
}

inline CRay &CRay::operator/=( const double &other )
{
	m_vec3Dir /= other;
	return *this;
}

inline std::string CRay::ToString( unsigned int uPrecision ) const
{
	return m_vec3Origin.ToString( uPrecision ) + "->" + m_vec3Dir.ToString( uPrecision );
}