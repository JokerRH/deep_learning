#pragma once

#include <array>
#include <iomanip>
#include <sstream>
#include <string.h>

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
	CVector<uRows> &operator/( const double &other );
	CVector<uRows> operator+( const CVector<uRows> &other ) const;
	CVector<uRows> &operator+=( const CVector<uRows> &other );
	CVector<uRows> operator-( const CVector<uRows> &other ) const;
	CVector<uRows> &operator-=( const CVector<uRows> &other );
	double Abs2( void ) const;
	double Abs( void ) const;
	CVector<uRows> Normalized( void ) const;
	CVector<uRows> &Normalize( void );
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
inline CVector<uRows> &CVector<uRows>::operator/( const double &other )
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
inline std::string CVector<uRows>::ToString( unsigned int uPrecision ) const
{
	std::ostringstream out;
	unsigned int u = 0;
	out << "(" << std::setprecision( uPrecision ) << m_adValues[ u++ ];
	for( ; u < uRows; u++ )
		out << ", " << std::setprecision( uPrecision ) << m_adValues[ u ];

	out << ")";
	return out.str( );
}