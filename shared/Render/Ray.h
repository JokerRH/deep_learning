#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Transformation.h"
#include "RenderObject.h"
#include <opencv2/core/types_c.h>

class CRay : public CRenderObject
{
public:
	CRay( const CVector<3> &vec3Origin, const CVector<3> &vec3Dir );
	CRay( const CVector<3> &vec3Origin, CVector<2> vec2Amp );
	CRay( void );
	~CRay( void ) override = default;

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
	friend CRay operator*( const CTransformation &matTransform, const CRay &ray );
	CRay &operator*=( const CTransformation &matTransform );
	CRay &operator*=( const double &other );
	CRay &operator/=( const double &other );
	CRay &operator+=( const CVector<3> &other );
	CVector<2> AmplitudeRepresentation( void ) const;

	CVector<3> GetMin( void ) const override;
	CVector<3> GetMax( void ) const override;
	
	void Render( cv::Mat &matImage, const cv::Scalar &colorPoint, const cv::Scalar &colorLine, double dLength = 1, int iRadius = 3, int iPointThickness = -1, int iLineThickness = 1 ) const;

	friend std::wostream &operator<<( std::wostream &smOut, const CRay &ray );
	std::wstring ToString( unsigned int uPrecision = 2 ) const override;

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

inline CRay operator*( const CTransformation &matTransform, const CRay &ray )
{
	CRay ret( ray );
	ret.m_vec3Origin *= matTransform;
	ret.m_vec3Dir *= matTransform.RSMatrix( );
	return ret;
}

inline CRay &CRay::operator*=( const CTransformation &matTransform )
{
	m_vec3Origin *= matTransform;
	m_vec3Dir *= matTransform.RSMatrix( );
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