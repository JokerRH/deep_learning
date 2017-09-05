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
	static CMatrix<uRows, uCols> Unit( void );
	CMatrix( const std::array<double, uCols * uRows> &adValues );
	CMatrix( const CMatrix<uRows, uCols> &other );
	CMatrix<uRows, uCols> &operator=( const CMatrix<uRows, uCols> &other );

	CMatrix( CMatrix<uRows, uCols> &&other );
	CMatrix<uRows, uCols> &operator=( CMatrix<uRows, uCols> &&other );
	
	double Determinant( void ) const;
	double *operator[]( size_t index );
	constexpr const double *operator[]( size_t index ) const;
	CVector<uRows> operator*( const CVector<uCols> &other ) const;
	CMatrix<uRows, uCols> operator*( const double &other ) const;
	CMatrix<uRows, uCols> &operator*=( const double &other );
	CMatrix<uRows, uCols> operator/( const double &other ) const;
	CMatrix<uRows, uCols> &operator/=( const double &other );
	template<unsigned int uCols2>
	CMatrix<uRows, uCols2> operator*( const CMatrix<uCols, uCols2> &other ) const;
	template<unsigned int uCols2>
	CMatrix<uRows, uCols> &operator*=( const CMatrix<uCols, uRows> &other );
	CMatrix<uRows, uCols> Inverse( void ) const;
	CMatrix<uRows, uCols> &Invert( void );

	std::string ToString( unsigned int uPrecision = 2 ) const;

private:
	CMatrix( void );
	double m_adValues[ uCols * uRows ];
};

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> CMatrix<uRows, uCols>::Unit( void )
{
	static_assert( uRows == uCols, "Unit matrix must be square" );

	std::array<double, uCols * uRows> adValues = { 0 };
	for( unsigned u = 0; u < uRows; u++ )
		adValues[ u * uRows + u ] = 1;

	return CMatrix<uRows, uCols>( adValues );
}

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

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> &CMatrix<uRows, uCols>::operator=( const CMatrix<uRows, uCols> &other )
{
	memcpy( m_adValues, other.m_adValues, uCols * uRows * sizeof( double ) );
	return *this;
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols>::CMatrix( CMatrix<uRows, uCols> &&other )
{
	memcpy( m_adValues, other.m_adValues, uCols * uRows * sizeof( double ) );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> & CMatrix<uRows, uCols>::operator=( CMatrix<uRows, uCols> &&other )
{
	memcpy( m_adValues, other.m_adValues, uCols * uRows * sizeof( double ) );
	return *this;
}

template<>
inline double CMatrix<2, 2>::Determinant( void ) const
{
	return m_adValues[ 0 ] * m_adValues[ 3 ] - m_adValues[ 1 ] * m_adValues[ 2 ];
}

template<>
inline double CMatrix<3, 3>::Determinant( void ) const
{
	return m_adValues[ 0 ] * ( m_adValues[ 4 ] * m_adValues[ 8 ] - m_adValues[ 5 ] * m_adValues[ 7 ] )	//A11 * ( A22 * A33 - A23 * A32 )
		- m_adValues[ 1 ] * ( m_adValues[ 3 ] * m_adValues[ 8 ] - m_adValues[ 5 ] * m_adValues[ 6 ] )	//A12 * ( A21 * A33 - A23 * A31 )
		+ m_adValues[ 2 ] * ( m_adValues[ 3 ] * m_adValues[ 7 ] - m_adValues[ 4 ] * m_adValues[ 6 ] );	//A13 * ( A21 * A32 - A22 * A31 )
}

template<unsigned int uRows, unsigned int uCols>
inline double CMatrix<uRows, uCols>::Determinant( void ) const
{
	static_assert( false, "Not yet implemented" );
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
	for( unsigned int u = 0; u < uRows * uCols; u++ )
		adValues[ u ] = m_adValues[ u ] * other;

	return CMatrix<uRows, uCols>( adValues );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> &CMatrix<uRows, uCols>::operator*=( const double & other )
{
	for( unsigned int u = 0; u < uRows * uCols; u++ )
		m_adValues[ u ] *= other;

	return *this;
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> CMatrix<uRows, uCols>::operator/( const double &other ) const
{
	std::array<double, uCols * uRows> adValues;
	for( unsigned int u = 0; u < uRows * uCols; u++ )
		adValues[ u ] = m_adValues[ u ] / other;

	return CMatrix<uRows, uCols>( adValues );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> &CMatrix<uRows, uCols>::operator/=( const double & other )
{
	for( unsigned int u = 0; u < uRows * uCols; u++ )
		m_adValues[ u ] /= other;

	return *this;
}

template<unsigned int uRows, unsigned int uCols>
template<unsigned int uCols2>
CMatrix<uRows, uCols2> CMatrix<uRows, uCols>::operator*( const CMatrix<uCols, uCols2> &other ) const
{
	CMatrix<uRows, uCols2> mat( { 0 } );
	for( unsigned int uCol2 = 0; uCol2 < uCols2; uCol2++ )
		for( unsigned int uRow = 0; uRow < uRows; uRow++ )
			for( unsigned int uCol = 0; uCol < uCols; uCol++ )
				mat[ uRow ][ uCol2 ] += ( *this )[ uRow ][ uCol ] * other[ uCol ][ uCol2 ];

	return mat;
}

template<unsigned int uRows, unsigned int uCols>
template<unsigned int uCols2>
CMatrix<uRows, uCols> &CMatrix<uRows, uCols>::operator*=( const CMatrix<uCols, uRows> &other )
{
	CMatrix<uRows, uCols2> mat( { 0 } );
	for( unsigned int uCol2 = 0; uCol2 < uCols2; uCol2++ )
		for( unsigned int uRow = 0; uRow < uRows; uRow++ )
			for( unsigned int uCol = 0; uCol < uCols; uCol++ )
				mat[ uRow ][ uCol2 ] += ( *this )[ uRow ][ uCol ] * other[ uCol ][ uCol2 ];

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

template<>
inline CMatrix<3, 3> CMatrix<3, 3>::Inverse( void ) const
{
	std::array<double, 3 * 3> adValues;
	adValues[ 0 ] = CMatrix<2, 2>( { m_adValues[ 4 ], m_adValues[ 5 ], m_adValues[ 7 ], m_adValues[ 8 ] } ).Determinant( );
	adValues[ 1 ] = CMatrix<2, 2>( { m_adValues[ 2 ], m_adValues[ 1 ], m_adValues[ 8 ], m_adValues[ 7 ] } ).Determinant( );
	adValues[ 2 ] = CMatrix<2, 2>( { m_adValues[ 1 ], m_adValues[ 2 ], m_adValues[ 4 ], m_adValues[ 5 ] } ).Determinant( );
	adValues[ 3 ] = CMatrix<2, 2>( { m_adValues[ 5 ], m_adValues[ 3 ], m_adValues[ 8 ], m_adValues[ 6 ] } ).Determinant( );
	adValues[ 4 ] = CMatrix<2, 2>( { m_adValues[ 0 ], m_adValues[ 2 ], m_adValues[ 6 ], m_adValues[ 8 ] } ).Determinant( );
	adValues[ 5 ] = CMatrix<2, 2>( { m_adValues[ 2 ], m_adValues[ 0 ], m_adValues[ 5 ], m_adValues[ 3 ] } ).Determinant( );
	adValues[ 6 ] = CMatrix<2, 2>( { m_adValues[ 3 ], m_adValues[ 4 ], m_adValues[ 6 ], m_adValues[ 7 ] } ).Determinant( );
	adValues[ 7 ] = CMatrix<2, 2>( { m_adValues[ 1 ], m_adValues[ 0 ], m_adValues[ 7 ], m_adValues[ 6 ] } ).Determinant( );
	adValues[ 8 ] = CMatrix<2, 2>( { m_adValues[ 0 ], m_adValues[ 1 ], m_adValues[ 3 ], m_adValues[ 4 ] } ).Determinant( );
	return CMatrix<3, 3>( adValues ) / this->Determinant( );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> CMatrix<uRows, uCols>::Inverse( void ) const
{
	static_assert( false, "Not yet implemented" );
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
	static_assert( false, "Not yet implemented" );
	return *this;
}

template<unsigned int uRows, unsigned int uCols>
inline std::string CMatrix<uRows, uCols>::ToString( unsigned int uPrecision ) const
{
	std::ostringstream out;
	out.setf( std::ios_base::fixed, std::ios_base::floatfield );
	out.precision( uPrecision );

	unsigned int u = 0;
	out << "((" << m_adValues[ u ];
	for( u++; u < uCols; u++ )
		out << ", " << m_adValues[ u ];

	out << ")";
	for( unsigned int uCol, uRow = 1; uRow < uRows; uRow++ )
	{
		out << ", (" << m_adValues[ u ];
		for( u++, uCol = 1; uCol < uCols; u++, uCol++ )
			out << ", " << m_adValues[ u ];

		out << ")";
	}

	out << ")";
	return out.str( );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols>::CMatrix( void )
{

}