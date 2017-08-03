#pragma once

#include <array>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <math.h>

template<unsigned int uRows>
class CVector
{
public:
	static_assert( uRows > 0, "Vector dimension must be greater than 0" );

	CVector( const std::array<double, uRows> &adValues );
	CVector( const CVector<uRows> &other );

	double &operator[]( size_t index );
	constexpr const double &operator[]( size_t index ) const;
	double operator*( const CVector<uRows> &other ) const;
	CVector<uRows> operator*( const double &other ) const;
	CVector<uRows> &operator*=( const double &other );
	CVector<uRows> operator/( const double &other ) const;
	CVector<uRows> &operator/=( const double &other );
	CVector<uRows> operator+( const CVector<uRows> &other ) const;
	CVector<uRows> &operator+=( const CVector<uRows> &other );
	CVector<uRows> operator-( const CVector<uRows> &other ) const;
	CVector<uRows> &operator-=( const CVector<uRows> &other );
	CVector<uRows> operator-( void ) const;
	double Abs2( void ) const;
	double Abs( void ) const;
	CVector<uRows> Normalized( void ) const;
	CVector<uRows> &Normalize( void );
	CVector<uRows> UnitVector( void ) const;
	CVector<uRows> &MakeUnitVector( void );
	CVector<3> CrossProduct( const CVector<3> &other );
	CVector<3> &MakeCrossProduct( const CVector<3> &other );
	std::string ToString( unsigned int uPrecision = 2 ) const;

private:
	double m_adValues[ uRows ];
};

template<unsigned int uRows>
inline CVector<uRows>::CVector( const std::array<double, uRows> &adValues )
{
	memcpy( m_adValues, adValues.data( ), uRows * sizeof( double ) );
}

template<unsigned int uRows>
inline CVector<uRows>::CVector( const CVector<uRows> &other )
{
	memcpy( m_adValues, other.m_adValues, uRows * sizeof( double ) );
}

template<unsigned int uRows>
inline double &CVector<uRows>::operator[]( size_t index )
{
	return m_adValues[ index ];
}

template<unsigned int uRows>
inline constexpr const double &CVector<uRows>::operator[]( size_t index ) const
{
	return m_adValues[ index ];
}

template<unsigned int uRows>
inline double CVector<uRows>::operator*( const CVector<uRows> &other ) const
{
	double d = 0;
	for( unsigned int u = 0; u < uRows; u++ )
		d += m_adValues[ u ] * other.m_adValues[ u ];

	return d;
}

template<unsigned int uRows>
inline CVector<uRows> CVector<uRows>::operator*( const double &other ) const
{
	std::array<double, uRows> adValues;
	for( unsigned int u = 0; u < uRows; u++ )
		adValues[ u ] = m_adValues[ u ] * other;

	return CVector<uRows>( adValues );
}

template<unsigned int uRows>
inline CVector<uRows> &CVector<uRows>::operator*=( const double &other )
{
	for( unsigned int u = 0; u < uRows; u++ )
		m_adValues[ u ] *= other;

	return *this;
}

template<unsigned int uRows>
inline CVector<uRows> CVector<uRows>::operator/( const double &other ) const
{
	std::array<double, uRows> adValues;
	for( unsigned int u = 0; u < uRows; u++ )
		adValues[ u ] = m_adValues[ u ] / other;

	return CVector<uRows>( adValues );
}

template<unsigned int uRows>
inline CVector<uRows> &CVector<uRows>::operator/=( const double &other )
{
	for( unsigned int u = 0; u < uRows; u++ )
		m_adValues[ u ] /= other;

	return *this;
}

template<unsigned int uRows>
inline CVector<uRows> CVector<uRows>::operator+( const CVector<uRows> &other ) const
{
	std::array<double, uRows> adValues;
	std::copy( m_adValues, m_adValues + uRows, adValues.begin( ) );
	for( unsigned int u = 0; u < uRows; u++ )
		adValues[ u ] += other.m_adValues[ u ];

	return CVector<uRows>( adValues );
}

template<unsigned int uRows>
inline CVector<uRows> &CVector<uRows>::operator+=( const CVector<uRows> &other )
{
	for( unsigned int u = 0; u < uRows; u++ )
		m_adValues[ u ] += other.m_adValues[ u ];

	return *this;
}

template<unsigned int uRows>
inline CVector<uRows> CVector<uRows>::operator-( const CVector<uRows> &other ) const
{
	std::array<double, uRows> adValues;
	std::copy( m_adValues, m_adValues + uRows, adValues.begin( ) );
	for( unsigned int u = 0; u < uRows; u++ )
		adValues[ u ] -= other.m_adValues[ u ];

	return CVector<uRows>( adValues );
}

template<unsigned int uRows>
inline CVector<uRows> &CVector<uRows>::operator-=( const CVector<uRows> &other )
{
	for( unsigned int u = 0; u < uRows; u++ )
		m_adValues[ u ] -= other.m_adValues[ u ];

	return *this;
}

template<unsigned int uRows>
inline CVector<uRows> CVector<uRows>::operator-( void ) const
{
	CVector<uRows> vec( *this );
	for( unsigned int u = 0; u < uRows; u++ )
		vec.m_adValues[ u ] = -vec.m_adValues[ u ];

	return vec;
}

template<unsigned int uRows>
inline double CVector<uRows>::Abs2( void ) const
{
	double d = 0;
	for( unsigned int u = 0; u < uRows; u++ )
		d += m_adValues[ u ] * m_adValues[ u ];

	return d;
}

template<unsigned int uRows>
inline double CVector<uRows>::Abs( void ) const
{
	return sqrt( Abs2( ) );
}

template<unsigned int uRows>
inline CVector<uRows> CVector<uRows>::Normalized( void ) const
{
	return *this / Abs( );
}

template<unsigned int uRows>
inline CVector<uRows> &CVector<uRows>::Normalize( void )
{
	*this /= Abs( );
	return *this;
}

template<unsigned int uRows>
CVector<uRows> CVector<uRows>::UnitVector( void ) const
{
	return ( *this ) / Abs( );
}

template<unsigned int uRows>
CVector<uRows> &CVector<uRows>::MakeUnitVector( void )
{
	( *this ) /= Abs( );
	return *this;
}

template<>
inline CVector<3> CVector<3>::CrossProduct( const CVector<3> &other )
{
	return CVector<3>(
	{
		( *this )[ 1 ] * other[ 2 ] - ( *this )[ 2 ] * other[ 1 ],
		( *this )[ 2 ] * other[ 0 ] - ( *this )[ 0 ] * other[ 2 ],
		( *this )[ 0 ] * other[ 1 ] - ( *this )[ 1 ] * other[ 0 ]
	} );
}

template<>
inline CVector<3> &CVector<3>::MakeCrossProduct( const CVector<3> &other )
{
	double dX = ( *this )[ 1 ] * other[ 2 ] - ( *this )[ 2 ] * other[ 1 ];
	double dY = ( *this )[ 2 ] * other[ 0 ] - ( *this )[ 0 ] * other[ 2 ];
	double dZ = ( *this )[ 0 ] * other[ 1 ] - ( *this )[ 1 ] * other[ 0 ];
	m_adValues[ 0 ] = dX;
	m_adValues[ 1 ] = dY;
	m_adValues[ 2 ] = dZ;
	return *this;
}

template<unsigned int uRows>
inline std::string CVector<uRows>::ToString( unsigned int uPrecision ) const
{
	std::ostringstream out;
	out.setf( std::ios_base::fixed, std::ios_base::floatfield );
	out.precision( uPrecision );

	unsigned int u = 0;
	out << "(" << m_adValues[ u++ ];
	for( ; u < uRows; u++ )
		out << ", " << m_adValues[ u ];

	out << ")";
	return out.str( );
}