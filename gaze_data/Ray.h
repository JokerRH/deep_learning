#pragma once

#include "Vector.h"

class CRay
{
public:
	CRay( const CVector<3> &vec3Origin, const CVector<3> &vec3Dir );
	CRay( const CRay &other );

	CVector<3> operator()( const double &other ) const;
	
	/**
	 * @brief Calculates the points of shortest distance
	 * @param other Vector to calculate the distance from
	 * @return Vector containing the multiplicator for the closest points. First value is index for \e other, second is for \e this
	 */
	CVector<2> PointOfShortestDistance( const CRay &other ) const;

	std::string ToString( unsigned int uPrecision = 2 ) const;

private:
	CVector<3> m_vec3Origin;
	CVector<3> m_vec3Dir;
};

inline CRay::CRay( const CVector<3> &vec3Origin, const CVector<3> &vec3Dir ) :
	m_vec3Origin( vec3Origin ),
	m_vec3Dir( vec3Dir )
{

}

inline CRay::CRay( const CRay &other ) :
	m_vec3Origin( other.m_vec3Origin ),
	m_vec3Dir( other.m_vec3Dir )
{

}

inline CVector<3> CRay::operator()( const double &other ) const
{
	return m_vec3Origin + m_vec3Dir * other;
}

inline std::string CRay::ToString( unsigned int uPrecision ) const
{
	return m_vec3Origin.ToString( uPrecision ) + "->" + m_vec3Dir.ToString( uPrecision );
}