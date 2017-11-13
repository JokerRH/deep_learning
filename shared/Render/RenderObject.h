#pragma once

#include "Vector.h"
#include <vector>

class CRenderBox;
class CRenderObject
{
public:
	static CVector<3> GetMin( const std::initializer_list<const CRenderObject *> &list );
	static CVector<3> GetMax( const std::initializer_list<const CRenderObject *> &list );
	static CVector<3> GetDim( const CVector<3> &vec3Min, const CVector<3> &vec3Max );
	static CVector<3> GetDim( const std::initializer_list<const CRenderObject *> &list );
	static CRenderBox GetBox( const std::initializer_list<const CRenderObject *> &list );

	virtual ~CRenderObject( void );
	virtual CVector<3> GetMin( void ) const = 0;
	virtual CVector<3> GetMax( void ) const = 0;
	CVector<3> GetDim( void ) const;
	CRenderBox GetBox( void ) const;
	
	virtual std::wstring ToString( unsigned int uPrecision = 2 ) const = 0;
};