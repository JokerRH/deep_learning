#pragma once

#include "Vector.h"

class CLine
{
public:
	CLine( const CVector<3> &vec3Origin, const CVector<3> &vec3Dir ) :
		m_vec3Origin( vec3Origin ),
		m_vec3Dir( vec3Dir )
	{

	}

	inline CVector<3> operator()( const double &other ) const
	{
		return m_vec3Origin + m_vec3Dir * other;
	}

	CVector<2> PointOfShortestDistance( const CLine &other );

private:
	CVector<3> m_vec3Origin;
	CVector<3> m_vec3Dir;
};