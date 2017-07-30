#pragma once

#include <array>
#include <assert.h>
#include <algorithm>
#include "Vector.h"

template<unsigned int uRows, unsigned int uCols>
class CMatrix
{
public:
	inline CMatrix( const std::array<double, uCols * uRows> &adValues )
	{
		memcpy( m_adValues, adValues.data( ), uCols * uRows * sizeof( double ) );
	}

	inline CMatrix( const CMatrix<uRows, uCols> &other )
	{
		memcpy( m_adValues, other.m_adValues, uCols * uRows * sizeof( double ) );
	}

	inline double *operator[]( size_t index )
	{
		return m_adValues + index * uCols;
	}

	inline constexpr const double *operator[]( size_t index ) const
	{
		return m_adValues + index * uCols;
	}

	CVector<uRows> operator*( const CVector<uCols> &other ) const;
	CMatrix<uRows, uCols> operator*( const double &other ) const;
	CMatrix<uRows, uCols> &operator*=( const double &other );
	CMatrix<uRows, uCols> Inverse( void ) const;
	CMatrix<uRows, uCols> &Invert( void );

	double Determinant( void ) const;

	std::string ToString( unsigned int uPrecision = 2 ) const;

private:
	double m_adValues[ uCols * uRows ];
};

template<unsigned int uRows, unsigned int uCols>
inline CVector<uRows> CMatrix<uRows, uCols>::operator*( const CVector<uCols> &other ) const
{
	std::array<double, uRows> adValues;
	for( unsigned int uCol, uRow = 0; uRow < uRows; uRow++ )
	{
		adValues[ uRows ] = 0;
		for( uCol = 0; uCol < uCols; uCol++ )
			adValues[ uRows ] += ( *this )[ uRow ][ uCol ] * other[ uCol ];
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