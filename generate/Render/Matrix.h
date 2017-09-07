#pragma once

#include <array>
#include <assert.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
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

	template<unsigned int uRows, unsigned int uCols>
	friend std::wostream &operator<<( std::wostream &smOut, const CMatrix<uRows, uCols> &mat );
	std::wstring ToString( unsigned int uPrecision = 2 ) const;

private:
	CMatrix( void );
	std::array<double, uCols * uRows> m_adValues;
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
inline CMatrix<uRows, uCols>::CMatrix( const std::array<double, uCols * uRows> &adValues ) :
	m_adValues( adValues )
{

}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols>::CMatrix( const CMatrix<uRows, uCols> &other ) :
	m_adValues( other.m_adValues )
{

}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> &CMatrix<uRows, uCols>::operator=( const CMatrix<uRows, uCols> &other )
{
	m_adValues = other.m_adValues;
	return *this;
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols>::CMatrix( CMatrix<uRows, uCols> &&other ) :
	m_adValues( std::move( other.m_adValues ) )
{

}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> & CMatrix<uRows, uCols>::operator=( CMatrix<uRows, uCols> &&other )
{
	m_adValues = std::move( other.m_adValues );
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

#define A( row, col )	m_adValues[ ( row - 1 ) * 4 + col - 1 ]
template<>
inline double CMatrix<4, 4>::Determinant( void ) const
{
	return A( 1, 1 ) * ( A( 2, 2 ) * ( A( 3, 3 ) * A( 4, 4 ) - A( 3, 4 ) * A( 4, 3 ) ) + A( 2, 3 ) * ( A( 3, 4 ) * A( 4, 2 ) - A( 3, 2 ) * A( 4, 4 ) ) + A( 2, 4 ) * ( A( 3, 2 ) * A( 4, 3 ) - A( 3, 3 ) * A( 4, 2 ) ) )	//A11 * ( A22 * ( A33 * A44 - A34 * A43 ) + A23 * ( A34 * A42 - A32 * A44 ) + A24 * ( A32 * A43 - A33 * A42 ) )
		+ A( 1, 2 ) * ( A( 2, 1 ) * ( A( 3, 4 ) * A( 4, 3 ) - A( 3, 3 ) * A( 4, 4 ) ) + A( 2, 3 ) * ( A( 3, 1 ) * A( 4, 4 ) - A( 3, 4 ) * A( 4, 1 ) ) + A( 2, 4 ) * ( A( 3, 3 ) * A( 4, 1 ) - A( 3, 1 ) * A( 4, 3 ) ) )		//A12 * ( A21 * ( A34 * A43 - A33 * A44 ) + A23 * ( A31 * A44 - A34 * A41 ) + A24 * ( A33 * A41 - A31 * A43 ) )
		+ A( 1, 3 ) * ( A( 2, 1 ) * ( A( 3, 2 ) * A( 4, 4 ) - A( 3, 4 ) * A( 4, 2 ) ) + A( 2, 2 ) * ( A( 3, 4 ) * A( 4, 1 ) - A( 3, 1 ) * A( 4, 4 ) ) + A( 2, 4 ) * ( A( 3, 1 ) * A( 4, 2 ) - A( 3, 2 ) * A( 4, 1 ) ) )		//A13 * ( A21 * ( A32 * A44 - A34 * A42 ) + A22 * ( A34 * A41 - A31 * A44 ) + A24 * ( A31 * A42 - A32 * A41 ) )
		+ A( 1, 4 ) * ( A( 2, 1 ) * ( A( 3, 3 ) * A( 4, 2 ) - A( 3, 2 ) * A( 4, 3 ) ) + A( 2, 2 ) * ( A( 3, 1 ) * A( 4, 3 ) - A( 3, 3 ) * A( 4, 1 ) ) + A( 2, 3 ) * ( A( 3, 2 ) * A( 4, 1 ) - A( 3, 1 ) * A( 4, 2 ) ) );	//A14 * ( A21 * ( A33 * A42 - A32 * A43 ) + A22 * ( A31 * A43 - A33 * A41 ) + A23 * ( A32 * A41 - A31 * A42 ) )
}
#undef A

template<unsigned int uRows, unsigned int uCols>
inline double CMatrix<uRows, uCols>::Determinant( void ) const
{
	static_assert( false, "Not yet implemented" );
	return 0.0;
}

template<unsigned int uRows, unsigned int uCols>
inline double *CMatrix<uRows, uCols>::operator[]( size_t index )
{
	assert( index < uRows );
	return m_adValues.data( ) + index * uCols;
}

template<unsigned int uRows, unsigned int uCols>
inline constexpr const double *CMatrix<uRows, uCols>::operator[]( size_t index ) const
{
	assert( index < uRows );
	return m_adValues.data( ) + index * uCols;
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

#define B( row, col, row1, col1, row2, col2 ) m_adValues[ ( row - 1 ) * 4 + col - 1 ] * ( m_adValues[ ( row1 - 1 ) * 4 + col1 - 1 ] * m_adValues[ ( row2 - 1 ) * 4 + col2 - 1 ] - m_adValues[ ( row1 - 1 ) * 4 + col2 - 1 ] * m_adValues[ ( row2 - 1 ) * 4 + col1 - 1 ] )
template<>
inline CMatrix<4, 4> CMatrix<4, 4>::Inverse( void ) const
{
	std::array<double, 4 * 4> adValues;
	adValues[ 0 ] = B( 2, 2, 3, 3, 4, 4 ) + B( 2, 3, 3, 4, 4, 2 ) + B( 2, 4, 3, 2, 4, 3 );
	adValues[ 1 ] = B( 1, 2, 3, 4, 4, 3 ) + B( 1, 3, 3, 2, 4, 4 ) + B( 1, 4, 3, 3, 4, 2 );
	adValues[ 2 ] = B( 1, 2, 2, 3, 4, 4 ) + B( 1, 3, 2, 4, 4, 2 ) + B( 1, 4, 2, 2, 4, 3 );
	adValues[ 3 ] = B( 1, 2, 2, 4, 3, 3 ) + B( 1, 3, 2, 2, 3, 4 ) + B( 1, 4, 2, 3, 3, 2 );
	adValues[ 4 ] = B( 2, 1, 3, 4, 4, 3 ) + B( 2, 3, 3, 1, 4, 4 ) + B( 2, 4, 3, 3, 4, 1 );
	adValues[ 5 ] = B( 1, 1, 3, 3, 4, 4 ) + B( 1, 3, 3, 4, 4, 1 ) + B( 1, 4, 3, 1, 4, 3 );
	adValues[ 6 ] = B( 1, 1, 2, 4, 4, 3 ) + B( 1, 3, 2, 1, 4, 4 ) + B( 1, 4, 2, 3, 4, 1 );
	adValues[ 7 ] = B( 1, 1, 2, 3, 3, 4 ) + B( 1, 3, 2, 4, 3, 1 ) + B( 1, 4, 2, 1, 3, 3 );
	adValues[ 8 ] = B( 2, 1, 3, 2, 4, 4 ) + B( 2, 2, 3, 4, 4, 1 ) + B( 2, 4, 3, 1, 4, 2 );
	adValues[ 9 ] = B( 1, 1, 3, 4, 4, 2 ) + B( 1, 2, 3, 1, 4, 4 ) + B( 1, 4, 3, 2, 4, 1 );
	adValues[ 10 ] = B( 1, 1, 2, 2, 4, 4 ) + B( 1, 2, 2, 4, 4, 1 ) + B( 1, 4, 2, 1, 4, 2 );
	adValues[ 11 ] = B( 1, 1, 2, 4, 3, 2 ) + B( 1, 2, 2, 1, 3, 4 ) + B( 1, 4, 2, 2, 3, 1 );
	adValues[ 12 ] = B( 2, 1, 3, 3, 4, 2 ) + B( 2, 2, 3, 1, 4, 3 ) + B( 2, 3, 3, 2, 4, 1 );
	adValues[ 13 ] = B( 1, 1, 3, 2, 4, 3 ) + B( 1, 2, 3, 3, 4, 1 ) + B( 1, 3, 3, 1, 4, 2 );
	adValues[ 14 ] = B( 1, 1, 2, 3, 4, 2 ) + B( 1, 2, 2, 1, 4, 3 ) + B( 1, 3, 2, 2, 4, 1 );
	adValues[ 15 ] = B( 1, 1, 2, 2, 3, 3 ) + B( 1, 2, 2, 3, 3, 1 ) + B( 1, 3, 2, 1, 3, 2 );
	return CMatrix<4, 4>( adValues ) / this->Determinant( );
}
#undef B

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> CMatrix<uRows, uCols>::Inverse( void ) const
{
	static_assert( false, "Not yet implemented" );
	return CMatrix<uRows, uCols>( );
}

template<>
inline CMatrix<2, 2> &CMatrix<2, 2>::Invert( void )
{
	double d = 1 / Determinant( );
	std::swap( m_adValues[ 0 ], m_adValues[ 3 ] );
	m_adValues[ 0 ] *= d;
	m_adValues[ 1 ] = -m_adValues[ 1 ] * d;
	m_adValues[ 2 ] = -m_adValues[ 2 ] * d;
	m_adValues[ 3 ] *= d;
	return *this;
}

template<>
inline CMatrix<3, 3> &CMatrix<3, 3>::Invert( void )
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
	m_adValues = adValues;
	double dDeterminant = Determinant( );
	for( double &d : m_adValues )
		d /= dDeterminant;
	return *this;
}

#define B( row, col, row1, col1, row2, col2 ) m_adValues[ ( row - 1 ) * 4 + col - 1 ] * ( m_adValues[ ( row1 - 1 ) * 4 + col1 - 1 ] * m_adValues[ ( row2 - 1 ) * 4 + col2 - 1 ] - m_adValues[ ( row1 - 1 ) * 4 + col2 - 1 ] * m_adValues[ ( row2 - 1 ) * 4 + col1 - 1 ] )
template<>
inline CMatrix<4, 4> &CMatrix<4, 4>::Invert( void )
{
	std::array<double, 4 * 4> adValues;
	adValues[ 0 ] = B( 2, 2, 3, 3, 4, 4 ) + B( 2, 3, 3, 4, 4, 2 ) + B( 2, 4, 3, 2, 4, 3 );
	adValues[ 1 ] = B( 1, 2, 3, 4, 4, 3 ) + B( 1, 3, 3, 2, 4, 4 ) + B( 1, 4, 3, 3, 4, 2 );
	adValues[ 2 ] = B( 1, 2, 2, 3, 4, 4 ) + B( 1, 3, 2, 4, 4, 2 ) + B( 1, 4, 2, 2, 4, 3 );
	adValues[ 3 ] = B( 1, 2, 2, 4, 3, 3 ) + B( 1, 3, 2, 2, 3, 4 ) + B( 1, 4, 2, 3, 3, 2 );
	adValues[ 4 ] = B( 2, 1, 3, 4, 4, 3 ) + B( 2, 3, 3, 1, 4, 4 ) + B( 2, 4, 3, 3, 4, 1 );
	adValues[ 5 ] = B( 1, 1, 3, 3, 4, 4 ) + B( 1, 3, 3, 4, 4, 1 ) + B( 1, 4, 3, 1, 4, 3 );
	adValues[ 6 ] = B( 1, 1, 2, 4, 4, 3 ) + B( 1, 3, 2, 1, 4, 4 ) + B( 1, 4, 2, 3, 4, 1 );
	adValues[ 7 ] = B( 1, 1, 2, 3, 3, 4 ) + B( 1, 3, 2, 4, 3, 1 ) + B( 1, 4, 2, 1, 3, 3 );
	adValues[ 8 ] = B( 2, 1, 3, 2, 4, 4 ) + B( 2, 2, 3, 4, 4, 1 ) + B( 2, 4, 3, 1, 4, 2 );
	adValues[ 9 ] = B( 1, 1, 3, 4, 4, 2 ) + B( 1, 2, 3, 1, 4, 4 ) + B( 1, 4, 3, 2, 4, 1 );
	adValues[ 10 ] = B( 1, 1, 2, 2, 4, 4 ) + B( 1, 2, 2, 4, 4, 1 ) + B( 1, 4, 2, 1, 4, 2 );
	adValues[ 11 ] = B( 1, 1, 2, 4, 3, 2 ) + B( 1, 2, 2, 1, 3, 4 ) + B( 1, 4, 2, 2, 3, 1 );
	adValues[ 12 ] = B( 2, 1, 3, 3, 4, 2 ) + B( 2, 2, 3, 1, 4, 3 ) + B( 2, 3, 3, 2, 4, 1 );
	adValues[ 13 ] = B( 1, 1, 3, 2, 4, 3 ) + B( 1, 2, 3, 3, 4, 1 ) + B( 1, 3, 3, 1, 4, 2 );
	adValues[ 14 ] = B( 1, 1, 2, 3, 4, 2 ) + B( 1, 2, 2, 1, 4, 3 ) + B( 1, 3, 2, 2, 4, 1 );
	adValues[ 15 ] = B( 1, 1, 2, 2, 3, 3 ) + B( 1, 2, 2, 3, 3, 1 ) + B( 1, 3, 2, 1, 3, 2 );
	m_adValues = adValues;
	double dDeterminant = Determinant( );
	for( double &d : m_adValues )
		d /= dDeterminant;
	return *this;
}
#undef B

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols> &CMatrix<uRows, uCols>::Invert( void )
{
	static_assert( false, "Not yet implemented" );
	return *this;
}

template<unsigned int uRows, unsigned int uCols>
std::wostream &operator<<( std::wostream &smOut, const CMatrix<uRows, uCols> &mat )
{
	unsigned int u = 0;
	smOut << "((" << mat.m_adValues[ u ];
	for( u++; u < uCols; u++ )
		smOut << ", " << mat.m_adValues[ u ];

	smOut << ")";
	for( unsigned int uCol, uRow = 1; uRow < uRows; uRow++ )
	{
		smOut << ", (" << mat.m_adValues[ u ];
		for( u++, uCol = 1; uCol < uCols; u++, uCol++ )
			smOut << ", " << mat.m_adValues[ u ];

		smOut << ")";
	}

	smOut << ")";
	return smOut;
}

template<unsigned int uRows, unsigned int uCols>
inline std::wstring CMatrix<uRows, uCols>::ToString( unsigned int uPrecision ) const
{
	std::wostringstream smOut;
	smOut.setf( std::ios_base::fixed, std::ios_base::floatfield );
	smOut.precision( uPrecision );

	operator<<( smOut, *this );
	return smOut.str( );
}

template<unsigned int uRows, unsigned int uCols>
inline CMatrix<uRows, uCols>::CMatrix( void )
{

}