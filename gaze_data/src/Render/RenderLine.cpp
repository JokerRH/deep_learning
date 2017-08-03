#include "RenderLine.h"

CVector<3> CRenderLine::GetMin( void ) const
{
	return CVector<3>(
	{
		std::min( { m_avec3Points[ 0 ][ 0 ], m_avec3Points[ 1 ][ 0 ] } ),
		std::min( { m_avec3Points[ 0 ][ 1 ], m_avec3Points[ 1 ][ 1 ] } ),
		std::min( { m_avec3Points[ 0 ][ 2 ], m_avec3Points[ 1 ][ 2 ] } )
	} );
}

CVector<3> CRenderLine::GetMax( void ) const
{
	return CVector<3>(
	{
		std::max( { m_avec3Points[ 0 ][ 0 ], m_avec3Points[ 1 ][ 0 ] } ),
		std::max( { m_avec3Points[ 0 ][ 1 ], m_avec3Points[ 1 ][ 1 ] } ),
		std::max( { m_avec3Points[ 0 ][ 2 ], m_avec3Points[ 1 ][ 2 ] } )
	} );
}

void CRenderLine::Transform( const CMatrix<3, 3> &mat )
{
	m_avec3Points[ 0 ] = mat * m_avec3Points[ 0 ];
	m_avec3Points[ 1 ] = mat * m_avec3Points[ 1 ];
}

void CRenderLine::Shift( const CVector<3> &vec3 )
{
	m_avec3Points[ 0 ] += vec3;
	m_avec3Points[ 1 ] += vec3;
}