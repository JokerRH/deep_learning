#pragma once

#include <array>
#include <assert.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string.h>
#include "Vector.h"

template<unsigned int uRows, unsigned int uCols>
class CMatrix
{
public:
	CMatrix( const std::array<double, uCols * uRows> &adValues );
	CMatrix( const CMatrix<uRows, uCols> &other );
	
	double Determinant( void ) const;
	double *operator[]( size_t index );
	constexpr const double *operator[]( size_t index ) const;
	CVector<uRows> operator*( const CVector<uCols> &other ) const;
	CMatrix<uRows, uCols> operator*( const double &other ) const;
	CMatrix<uRows, uCols> &operator*=( const double &other );
	template<unsigned int uCols2>
	CMatrix<uRows, uCols2> operator*( const CMatrix<uCols, uCols2> &other ) const;
	template<unsigned int uCols2>
	CMatrix<uRows, uCols> &operator*=( const CMatrix<uCols, uRows> &other );
	CMatrix<uRows, uCols> Inverse( void ) const;
	CMatrix<uRows, uCols> &Invert( void );
	void Swap( CMatrix<uRows, uCols> &other );

	std::string ToString( unsigned int uPrecision = 2 ) const;

private:
	CMatrix( void );
	double m_adValues[ uCols * uRows ];
};

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols>::CMatrix( const std::array<double, uCols * uRows> &adValues )
{
	memcpy( m_adValues, adValues.data( ), uCols * uRows * sizeof( double ) );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols>::CMatrix( const CMatrix<uRows, uCols> &other )
{
	memcpy( m_adValues, other.m_adValues, uCols * uRows * sizeof( double ) );
}

template<>
inline double CMatrix<2, 2>::Determinant( void ) const
{
	return m_adValues[ 0 ] * m_adValues[ 3 ] - m_adValues[ 1 ] * m_adValues[ 2 ];
}

template<unsigned int uRows, unsigned int uCols>
inline double CMatrix<uRows, uCols>::Determinant( void ) const
{
	assert( false );
	return 0.0;
}

template<unsigned int uRows, unsigned int uCols>
inline double *CMatrix<uRows, uCols>::operator[]( size_t index )
{
	return m_adValues + index * uCols;
}

template<unsigned int uRows, unsigned int uCols>
inline constexpr const double *CMatrix<uRows, uCols>::operator[]( size_t index ) const
{
	return m_adValues + index * uCols;
}

template<unsigned int uRows, unsigned int uCols>
inline CVector<uRows> CMatrix<uRows, uCols>::operator*( const CVector<uCols> &other ) const
{
	std::array<double, uRows> adValues;
	for( unsigned int uCol, uRow = 0; uRow < uRows; uRow++ )
	{
		adValues[ uRow ] = 0;
		for( uCol = 0; uCol < uCols; uCol++ )
			adValues[ uRow ] += ( *this )[ uRow ][ uCol ] * other[ uCol ];
	}

	return CVector<uRows>( adValues );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> CMatrix<uRows, uCols>::operator*( const double &other ) const
{
	std::array<double, uCols * uRows> adValues;
	for( unsigned int u = 0; u < uRows; u++ )
		adValues[ u ] = m_adValues[ u ] * other;

	return CMatrix<uRows, uCols>( adValues );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> &CMatrix<uRows, uCols>::operator*=( const double & other )
{
	for( unsigned int u = 0; u < uRows; u++ )
		m_adValues[ u ] *= other;

	return *this;
}

template<unsigned int uRows, unsigned int uCols, unsigned int uCols2>
CMatrix<uRows, uCols2> CMatrix<uRows, uCols>::operator*( const CMatrix<uCols, uCols2> &other ) const;
{
	CMatrix<uRows, uCols2> mat;
	for( unsigned int uCol2 = 0; uCol2 < uCols2; uCol2++ )
		for( unsigned int uRow = 0; uRow < uRows, uRow++ )
			for( unsigned int uCol = 0; uCol < uCols; uCol++ )
				mat[ uRow ][ uCol2 ] = ( *this )[ uRow ][ uCol ] * other[ uCol ][ uCol2 ];

	return mat;
}

template<unsigned int uRows, unsigned int uCols, unsigned int uCols2>
CMatrix<uRows, uCols> &CMatrix<uRows, uCols>::operator*=( const CMatrix<uCols, uRows> &other )
{
	CMatrix<uRows, uCols2> mat;
	for( unsigned int uCol2 = 0; uCol2 < uCols2; uCol2++ )
		for( unsigned int uRow = 0; uRow < uRows, uRow++ )
			for( unsigned int uCol = 0; uCol < uCols; uCol++ )
				mat[ uRow ][ uCol2 ] = ( *this )[ uRow ][ uCol ] * other[ uCol ][ uCol2 ];

	Swap( mat );
	return *this;
}

template<>
inline CMatrix<2, 2> CMatrix<2, 2>::Inverse( void ) const
{
	std::array<double, 2 * 2> adValues;
	double d = 1 / Determinant( );
	adValues[ 0 ] = m_adValues[ 3 ] * d;
	adValues[ 1 ] = -m_adValues[ 1 ] * d;
	adValues[ 2 ] = -m_adValues[ 2 ] * d;
	adValues[ 3 ] = m_adValues[ 0 ] * d;
	return CMatrix<2, 2>( adValues );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> CMatrix<uRows, uCols>::Inverse( void ) const
{
	assert( false );
	return CMatrix<uRows, uCols>( );
}

template<>
inline CMatrix<2, 2>& CMatrix<2, 2>::Invert( void )
{
	double d = 1 / Determinant( );
	std::swap( m_adValues[ 0 ], m_adValues[ 3 ] );
	m_adValues[ 0 ] *= d;
	m_adValues[ 1 ] = -m_adValues[ 1 ] * d;
	m_adValues[ 2 ] = -m_adValues[ 2 ] * d;
	m_adValues[ 3 ] *= d;
	return *this;
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols>& CMatrix<uRows, uCols>::Invert( void )
{
	assert( false );
	return *this;
}

template<unsigned int uRows, unsigned int uCols>
inline void CMatrix<uRows, uCols>::Swap( CMatrix<uRows, uCols> &other )
{
	swap_ranges( m_adValues, m_adValues + uRows * uCols, other.m_adValues );
}

template<unsigned int uRows, unsigned int uCols>
inline std::string CMatrix<uRows, uCols>::ToString( unsigned int uPrecision ) const
{
	std::ostringstream out;
	unsigned int u = 0;
	out << "((" << std::setprecision( uPrecision ) << m_adValues[ u++ ];
	for( ; u < uCols; u++ )
		out << ", " << std::setprecision( uPrecision ) << m_adValues[ u ];

	out << ")";
	for( unsigned int uCol, uRow = 1; uRow < uRows; uRow++ )
	{
		out << ", (" << std::setprecision( uPrecision ) << m_adValues[ u++ ];;
		for( uCol = 1; uCol < uCols; uCol++, u++ )
			out << ", " << std::setprecision( uPrecision ) << m_adValues[ u ];

		out << ")";
	}

	out << ")";
	return out.str( );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols>::CMatrix( void )
{

}