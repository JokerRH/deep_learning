#pragma once

#include "Vector.h"
#include "Matrix.h"
#include <opencv2/core/types.hpp>

class CRay
{
public:
	CRay( const CVector<3> &vec3Origin, const CVector<3> &vec3Dir );
	CRay( const CVector<3> &vec3Origin, CVector<2> vec2Amp );
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
	CRay &operator+=( const CVector<3> &other );
	CVector<2> AmplitudeRepresentation( void ) const;
	
	void Render( cv::Mat &matImage, const cv::Scalar &colorPoint, const cv::Scalar &colorLine, double dLength = 1, int iRadius = 3, int iPointThickness = -1, int iLineThickness = 1 ) const;

	friend std::wostream &operator<<( std::wostream &smOut, const CRay &ray );
	std::wstring ToString( unsigned int uPrecision = 2 ) const;

	CVector<3> m_vec3Origin;
	CVector<3> m_vec3Dir;
};

inline CRay operator*( const CMatrix<3, 3> &matTransform, const CRay &ray )
{
	return CRay( matTransform * ray.m_vec3Origin, matTransform * ray.m_vec3Dir );
}

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

inline CRay &CRay::operator+=( const CVector<3> &other )
{
	m_vec3Origin += other;
	return *this;
}

inline std::wostream &operator<<( std::wostream &smOut, const CRay &ray )
{
	smOut << ray.m_vec3Origin << "->" << ray.m_vec3Dir;
	return smOut;
}

inline std::wstring CRay::ToString( unsigned int uPrecision ) const
{
	std::wostringstream smOut;
	smOut.setf( std::ios_base::fixed, std::ios_base::floatfield );
	smOut.precision( uPrecision );

	operator<<( smOut, *this );
	return smOut.str( );
}