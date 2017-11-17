#include "RenderObject.h"
#include "RenderBox.h"
#include <algorithm>
#include <math.h>

CVector<3> CRenderObject::GetMin( const std::initializer_list<const CRenderObject *> &list )
{
	CVector<3> vec3Min( { 0 } );
	CVector<3> vec3ObjectMin( { 0 } );
	for( const CRenderObject *pObject : list )
	{
		vec3ObjectMin = pObject->GetMin( );
		vec3Min = CVector<3>::GetMin( { vec3Min, vec3ObjectMin } );
	}

	return vec3Min;
}

CVector<3> CRenderObject::GetMax( const std::initializer_list<const CRenderObject *> &list )
{
	CVector<3> vec3Max( { 0 } );
	CVector<3> vec3ObjectMax( { 0 } );
	for( const CRenderObject *pObject : list )
	{
		vec3ObjectMax = pObject->GetMax( );
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