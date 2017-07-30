#include "Line.h"
#include "Matrix.h"

CVector<2> CLine::PointOfShortestDistance( const CLine &other )
{
	double d = other.m_vec3Dir * m_vec3Dir;
	CMatrix<2, 2> mat( { d, -m_vec3Dir.Abs2( ), other.m_vec3Dir.Abs2( ), -d } );
	CVector<3> vec3OriginDif( m_vec3Origin - other.m_vec3Origin );
	CVector<2> vec2Const( { vec3OriginDif * m_vec3Dir, vec3OriginDif * other.m_vec3Dir } );
	return mat.Inverse( ) * vec2Const;
}