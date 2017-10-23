#pragma once

#include <array>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <math.h>

class CTransformation;
template<unsigned int uRows>
class CVector
{
public:
	static_assert( uRows > 0, "Vector dimension must be greater than 0" );

	CVector( const std::array<double, uRows> &adValues );
	CVector( void );

	CVector( const CVector<uRows> &other );
	CVector<uRows> &operator=( const CVector<uRows> &other );

	CVector( CVector<uRows> &&other );
	CVector<uRows> &operator=( CVector<uRows> &&other );

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
	CVector<3> CrossProduct( const CVector<3> &other );
	CVector<3> &MakeCrossProduct( const CVector<3> &other );

	static CVector<uRows> GetMin( const std::initializer_list<const CVector<uRows>> &list );
	static CVector<uRows> GetMax( const std::initializer_list<const CVector<uRows>> &list );

	template<unsigned int uRows>
	friend std::wostream &operator<<( std::wostream& smOut, const CVector<uRows> &dt );
	std::wstring ToString( unsigned int uPrecision = 2 ) const;
	double Angle( const CVector<3> &other ) const;

private:
	std::array<double, uRows> m_adValues;
};

template<unsigned int uRows>
inline CVector<uRows>::CVector( const std::array<double, uRows> &adValues ) :
	m_adValues( adValues )
{

}

template<unsigned int uRows>
inline CVector<uRows>::CVector( void ) :
	m_adValues{ 0 }
{

}

template<unsigned int uRows>
inline CVector<uRows>::CVector( const CVector<uRows> &other ) :
	m_adValues( other.m_adValues )
{

}

template<unsigned int uRows>
inline CVector<uRows> &CVector<uRows>::operator=( const CVector<uRows> &other )
{
	m_adValues = other.m_adValues;
	return *this;
}

template<unsigned int uRows>
inline CVector<uRows>::CVector( CVector<uRows> &&other ) :
	m_adValues( std::move( other.m_adValues ) )
{

}

template<unsigned int uRows>
inline CVector<uRows> & CVector<uRows>::operator=( CVector<uRows> &&other )
{
	m_adValues = std::move( other.m_adValues );
	return *this;
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
	std::array<double, uRows> adValues = m_adValues;
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
	std::array<double, uRows> adValues = m_adValues;
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
inline CVector<uRows> CVector<uRows>::GetMin( const std::initializer_list<const CVector<uRows>> &list )
{
	if( !list.size( ) )
		return CVector<3>( { 0 } );

	CVector<uRows> vecReturn( *list.begin( ) );
	for( const CVector<uRows> *pvec = list.begin( ) + 1; pvec < list.end( ); pvec++ )
		for( unsigned u = 0; u < uRows; u++ )
			vecReturn.m_adValues[ u ] = std::min( vecReturn.m_adValues[ u ], pvec->m_adValues[ u ] );

	return vecReturn;
}

template<unsigned int uRows>
inline CVector<uRows> CVector<uRows>::GetMax( const std::initializer_list<const CVector<uRows>>& list )
{
	if( !list.size( ) )
		return CVector<3>( { 0 } );

	CVector<uRows> vecReturn( *list.begin( ) );
	for( const CVector<uRows> *pvec = list.begin( ) + 1; pvec < list.end( ); pvec++ )
		for( unsigned u = 0; u < uRows; u++ )
			vecReturn.m_adValues[ u ] = std::max( vecReturn.m_adValues[ u ], pvec->m_adValues[ u ] );

	return vecReturn;
}

template<unsigned int uRows>
inline std::wostream &operator<<( std::wostream& smOut, const CVector<uRows> &vec )
{
	unsigned int u = 0;
	smOut << "(" << vec.m_adValues[ u++ ];
	for( ; u < uRows; u++ )
		smOut << ", " << vec.m_adValues[ u ];

	smOut << ")";
	return smOut;
}

template<unsigned int uRows>
inline std::wstring CVector<uRows>::ToString( unsigned int uPrecision ) const
{
	std::wostringstream smOut;
	smOut.setf( std::ios_base::fixed, std::ios_base::floatfield );
	smOut.precision( uPrecision );

	operator<<( smOut, *this );
	return smOut.str( );
}

template<unsigned int uRows>
inline double CVector<uRows>::Angle( const CVector<3> &other ) const
{
	return acos( this->Normalized( ) * other.Normalized( ) );
}