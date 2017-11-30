#include "RenderObject.h"
#include "RenderBox.h"
#include <algorithm>
#include <math.h>

CVector<3> CRenderObject::GetMin( const CRenderObject *const *pStart, const CRenderObject *const *pEnd )
{
	CVector<3> vec3Min( { 0 } );
	CVector<3> vec3ObjectMin( { 0 } );
	for( const CRenderObject *const *pObject = pStart; pObject < pEnd; pObject++ )
	{
		vec3ObjectMin = ( *pObject )->GetMin( );
		vec3Min = CVector<3>::GetMin( { vec3Min, vec3ObjectMin } );
	}

	return vec3Min;
}

CVector<3> CRenderObject::GetMax( const CRenderObject *const *pStart, const CRenderObject *const *pEnd )
{
	CVector<3> vec3Max( { 0 } );
	CVector<3> vec3ObjectMax( { 0 } );
	for( const CRenderObject *const *pObject = pStart; pObject < pEnd; pObject++ )
	{
		vec3ObjectMax = ( *pObject )->GetMax( );
		vec3Max = CVector<3>::GetMin( { vec3Max, vec3ObjectMax } );
	}

	return vec3Max;
}

CVector<3> CRenderObject::GetDim( const CVector<3> &vec3Min, const CVector<3> &vec3Max )
{
	CVector<3> vec3Dim = vec3Min - vec3Max;
	vec3Dim[ 0 ] = std::abs( vec3Dim[ 0 ] );
	vec3Dim[ 1 ] = std::abs( vec3Dim[ 1 ] );
	vec3Dim[ 2 ] = std::abs( vec3Dim[ 2 ] );
	return vec3Dim;
}

CVector<3> CRenderObject::GetDim( const CRenderObject *const *pStart, const CRenderObject *const *pEnd )
{
	CVector<3> vec3Dim = GetMin( pStart, pEnd ) - GetMax( pStart, pEnd );
	vec3Dim[ 0 ] = std::abs( vec3Dim[ 0 ] );
	vec3Dim[ 1 ] = std::abs( vec3Dim[ 1 ] );
	vec3Dim[ 2 ] = std::abs( vec3Dim[ 2 ] );
	return vec3Dim;
}

CRenderBox CRenderObject::GetBox( const CRenderObject *const *pStart, const CRenderObject *const *pEnd )
{
	return CRenderBox( GetMin( pStart, pEnd ), GetMax( pStart, pEnd ) );
}

CRenderBox CRenderObject::GetBox( const std::initializer_list<const CRenderObject *> &list )
{
	return GetBox( ( const CRenderObject *const * ) &( *list.begin( ) ), ( const CRenderObject *const * ) &( *list.end( ) ) );
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