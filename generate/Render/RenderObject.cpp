#include "RenderObject.h"
#include "RenderBox.h"
#include <algorithm>
#include <math.h>

CVector<3> CRenderObject::GetMin( const std::initializer_list<const CRenderObject *> &list )
{
	CVector<3> vec3Min( { 0 } );
	CVector<3> vec3ObjectMin( { 0 } );
	for( std::initializer_list<const CRenderObject *>::const_iterator it = list.begin( ); it < list.end( ); it++ )
	{
		vec3ObjectMin = ( *it )->GetMin( );
		vec3Min[ 0 ] = std::min( vec3Min[ 0 ], vec3ObjectMin[ 0 ] );
		vec3Min[ 1 ] = std::min( vec3Min[ 1 ], vec3ObjectMin[ 1 ] );
		vec3Min[ 2 ] = std::min( vec3Min[ 2 ], vec3ObjectMin[ 2 ] );
	}

	return vec3Min;
}

CVector<3> CRenderObject::GetMax( const std::initializer_list<const CRenderObject *> &list )
{
	CVector<3> vec3Max( { 0 } );
	CVector<3> vec3ObjectMax( { 0 } );
	for( std::initializer_list<const CRenderObject *>::const_iterator it = list.begin( ); it < list.end( ); it++ )
	{
		vec3ObjectMax = ( *it )->GetMax( );
		vec3Max[ 0 ] = std::max( vec3Max[ 0 ], vec3ObjectMax[ 0 ] );
		vec3Max[ 1 ] = std::max( vec3Max[ 1 ], vec3ObjectMax[ 1 ] );
		vec3Max[ 2 ] = std::max( vec3Max[ 2 ], vec3ObjectMax[ 2 ] );
	}

	return vec3Max;
}

CVector<3> CRenderObject::GetDim( const std::initializer_list<const CRenderObject *> &list )
{
	CVector<3> vec3Dim = GetMin( list ) - GetMax( list );
	vec3Dim[ 0 ] = std::abs( vec3Dim[ 0 ] );
	vec3Dim[ 1 ] = std::abs( vec3Dim[ 1 ] );
	vec3Dim[ 2 ] = std::abs( vec3Dim[ 2 ] );
	return vec3Dim;
}

CRenderBox CRenderObject::GetBox( const std::initializer_list<const CRenderObject *> &list )
{
	return CRenderBox( GetMin( list ), GetMax( list ) );
}

CRenderObject::~CRenderObject( void )
{

}

CVector<3> CRenderObject::GetDim( void ) const
{
	CVector<3> vec3Dim = GetMin( ) - GetMax( );
	vec3Dim[ 0 ] = std::abs( vec3Dim[ 0 ] );
	vec3Dim[ 1 ] = std::abs( vec3Dim[ 1 ] );
	vec3Dim[ 2 ] = std::abs( vec3Dim[ 2 ] );
	return vec3Dim;
}

CRenderBox CRenderObject::GetBox( void ) const
{
	return CRenderBox( GetMin( ), GetMax( ) );
}