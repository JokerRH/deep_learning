#include "Render.h"
#include <algorithm>

CVector<3> CRenderObject::GetMin( const std::vector<const CRenderObject *> &vec )
{
	CVector<3> vec3Min( { 0 } );
	CVector<3> vec3ObjectMin( { 0 } );
	for( std::vector<const CRenderObject *>::const_iterator it = vec.begin( ); it < vec.end( ); it++ )
	{
		vec3ObjectMin = ( *it )->GetMin( );
		vec3Min[ 0 ] = std::min( vec3Min[ 0 ], vec3ObjectMin[ 0 ] );
		vec3Min[ 1 ] = std::min( vec3Min[ 1 ], vec3ObjectMin[ 1 ] );
		vec3Min[ 2 ] = std::min( vec3Min[ 2 ], vec3ObjectMin[ 2 ] );
	}
	
	return vec3Min;
}

CVector<3> CRenderObject::GetMax( const std::vector<const CRenderObject *> &vec )
{
	CVector<3> vec3Max( { 0 } );
	CVector<3> vec3ObjectMax( { 0 } );
	for( std::vector<const CRenderObject *>::const_iterator it = vec.begin( ); it < vec.end( ); it++ )
	{
		vec3ObjectMax = ( *it )->GetMin( );
		vec3Max[ 0 ] = std::max( vec3Max[ 0 ], vec3ObjectMax[ 0 ] );
		vec3Max[ 1 ] = std::max( vec3Max[ 1 ], vec3ObjectMax[ 1 ] );
		vec3Max[ 2 ] = std::max( vec3Max[ 2 ], vec3ObjectMax[ 2 ] );
	}
	
	return vec3Max;
}