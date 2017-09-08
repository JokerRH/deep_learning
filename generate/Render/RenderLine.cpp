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